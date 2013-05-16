#ifndef _GTRM_FUNCTIONS
#define _GTRM_FUNCTIONS

  #include <sched.h>
  #include <signal.h>
  #include <stdlib.h>
  #include <jobsignaler.h>
  #include <unistd.h> 
  #include <sys/types.h>
  #include <math.h>

  #include "dl_syscalls.h"

  #define _DEBUG 1
  #define _EXIT_SUCCESS 0
  #define _RM_MAX_APPLICATIONS 100
  #define _RM_DEADLINE 1000000 // nsec
  #define _RM_RUNTIME    10000 // nsec
  #define _MIN_SINGLE_ASSIGNABLE 0.10
  #define _MAX_SINGLE_ASSIGNABLE 0.90
  #define _MAX_ASSIGNABLE (0.9000 * sysconf(_SC_NPROCESSORS_ONLN))

  #ifdef _DEBUG
  #define debug_info(M)         \
    ({                            \
       fprintf(stderr, "Info: "); \
       fprintf(stderr, M);      \
       fprintf(stderr, "\n");     \
    }) 
  #else
  #define debug_info(...)
  #endif // _DEBUG

  #define fatal(M)               \
    ({                             \
       fprintf(stderr, "Fatal: "); \
       fprintf(stderr, M);       \
       fprintf(stderr, "\n");      \
                                   \
       exit(EXIT_FAILURE);         \
    })

  #define unreachable() \
    fatal("unreachable");

  typedef struct {
    pid_t tid;
    float vp;
    float vp_old;
    float performance;
    float weight;
  } _rm_application_h;

  double get_epsilon(unsigned int iterations, unsigned int offset,
      double c1, double c2) {
    double base = (double) iterations - (double) offset;
    double value = c2 * base;
    value = value * (double)_RM_DEADLINE/1000000000.0; // period in seconds
    return (c1) / (1.0 + value);
  }

  void signal_callback_handler(int signum) {
    fprintf(stdout, "GTRM: Terminated\n");
    exit(_EXIT_SUCCESS);
  }

  void gtrm_init() {
    // Handle termination
    signal(SIGINT, signal_callback_handler);
    // Set SCHED_DEADLINE as scheduler with parameters
    struct sched_param2 param2;
    param2.sched_priority = 0;
    param2.sched_deadline = _RM_DEADLINE;
    param2.sched_period = _RM_DEADLINE;
    param2.sched_runtime = _RM_RUNTIME;
    pid_t rm_pid = getpid();
    sched_setscheduler2(rm_pid, SCHED_DEADLINE, &param2);
  }

  int apply_scheddeadline(pid_t tid, float vp) {
    struct sched_param2 param2;
    param2.sched_priority = 0;
    param2.sched_deadline = _RM_DEADLINE;
    param2.sched_period = _RM_DEADLINE;
    param2.sched_runtime = (uint64_t) floor(vp * param2.sched_period);
    pid_t rm_pid = getpid();
    int ret = sched_setscheduler2(tid, SCHED_DEADLINE, &param2);
    #ifdef _DEBUG
      if (ret != 0)
        fprintf(stderr, "Error in sched_setscheduler2 for pid %d, vp %f\n", tid, vp);
    #endif
    return ret;
  }

  int update_applications(_rm_application_h* apps) {
    int application_ids[_RM_MAX_APPLICATIONS];
    int num_applications = get_applications(application_ids);

    // Copy old structure
    _rm_application_h old_apps[_RM_MAX_APPLICATIONS];
    memcpy(old_apps, apps, sizeof(_rm_application_h)*_RM_MAX_APPLICATIONS);

    // Zerify new structure
    int id_counter;
    int old_counter;
    for(id_counter = 0; id_counter < _RM_MAX_APPLICATIONS; id_counter++) {
      apps[id_counter].tid = 0;
      apps[id_counter].vp = 0.0;
      apps[id_counter].vp_old = 0.0;
      apps[id_counter].performance = 0.0;
      apps[id_counter].weight = 0.0;
    }

    // Rebuild structure
    int found_in_old = 0; // maybe used in the future to raise reset
    for(id_counter = 0; id_counter < num_applications; id_counter++) {
      apps[id_counter].tid = application_ids[id_counter];
      apps[id_counter].vp = 0.0;
      for(old_counter = 0; old_counter < _RM_MAX_APPLICATIONS; old_counter++) {
        if (old_apps[old_counter].tid == application_ids[id_counter]) {
          // avoid nans
          if (apps[id_counter].vp_old == apps[id_counter].vp_old)  
            apps[id_counter].vp_old = old_apps[old_counter].vp;
          else
            apps[id_counter].vp_old = 0.0; 

          found_in_old++;
          break;
        }
      }
      _application_h* a =
        monitor_application_init((int) apps[id_counter].tid);
      if (a != NULL) {
        apps[id_counter].performance = 
          get_performance_number(a, -1); // -1 means all jobs
        apps[id_counter].weight = get_weight(a);
        monitor_application_stop(a);
      }
      else {
        apps[id_counter].performance = 0.0;
        apps[id_counter].weight = 0.5; 
      }
    } // if (found_in_old == num_applications) found all in old
    return num_applications;
  }

  void compute_virtual_platforms(_rm_application_h* apps, int num_applications,
    unsigned int iterations, unsigned int offset, double c1, double c2) {
    float epsilon = get_epsilon(iterations, offset, c1, c2);
    float sumlambdafi = 0.0;
    int id_counter;
    for(id_counter = 0; id_counter < num_applications; id_counter++) {
      sumlambdafi += (1 - apps[id_counter].weight) * apps[id_counter].performance;
    } // sumlambdafi has the correct value now
    for(id_counter = 0; id_counter < num_applications; id_counter++) {
      if (iterations == 0) {
        apps[id_counter].vp = (_MAX_ASSIGNABLE) / (float) num_applications;
      } 
      else {
        float vp_old = apps[id_counter].vp_old / (_MAX_ASSIGNABLE);
        apps[id_counter].vp = vp_old -
          epsilon * ((1 - apps[id_counter].weight) * apps[id_counter].performance
          - (sumlambdafi * vp_old)); // computed as sum to one
        apps[id_counter].vp = apps[id_counter].vp * _MAX_ASSIGNABLE; // scaled
      }
    }
  }

  void rescale_virtual_platforms(_rm_application_h* apps, int num_applications) {
    int id_counter;
    float total_sum_vp = 0.0;
    for(id_counter = 0; id_counter < num_applications; id_counter++) {
      if (apps[id_counter].vp > _MAX_SINGLE_ASSIGNABLE)
        apps[id_counter].vp = _MAX_SINGLE_ASSIGNABLE;
      if (apps[id_counter].vp < _MIN_SINGLE_ASSIGNABLE)
        apps[id_counter].vp = _MIN_SINGLE_ASSIGNABLE;
      total_sum_vp += apps[id_counter].vp; 
    }

    double totalsingle_high = num_applications * _MAX_SINGLE_ASSIGNABLE;
    double totalsingle_low = num_applications * _MIN_SINGLE_ASSIGNABLE;
      
    if (total_sum_vp > _MAX_ASSIGNABLE && total_sum_vp != totalsingle_low) {
      double alfa = (_MAX_ASSIGNABLE - totalsingle_low) / 
        (total_sum_vp - totalsingle_low); 
      for(id_counter = 0; id_counter < num_applications; id_counter++) {
        apps[id_counter].vp = _MIN_SINGLE_ASSIGNABLE + 
          alfa * (apps[id_counter].vp - _MIN_SINGLE_ASSIGNABLE);
      }
    }
    // If not all CPU is assigned (unless RM has given the maximum to
    // all the present applications)
    if (total_sum_vp < _MAX_ASSIGNABLE && total_sum_vp != totalsingle_high) {
      double alfa = (totalsingle_high - _MAX_ASSIGNABLE) / 
        (totalsingle_high - total_sum_vp); 
      for(id_counter = 0; id_counter < num_applications; id_counter++) {
        apps[id_counter].vp = _MAX_SINGLE_ASSIGNABLE - 
          alfa * (_MAX_SINGLE_ASSIGNABLE - apps[id_counter].vp);
      }
    }
  }

  void rmset_performance_multiplier(_rm_application_h* apps, int num_applications) {
    int id_counter;
    for(id_counter = 0; id_counter < num_applications; id_counter++) {
      double performance_multiplier = 1.0; // set performance multiplier
      if (apps[id_counter].vp_old != 0.0)
        performance_multiplier = (1 + apps[id_counter].performance) *
          apps[id_counter].vp/apps[id_counter].vp_old;
      _application_h* a =
        monitor_application_init((int) apps[id_counter].tid);
      if (a != NULL) {
        set_performance_multiplier(a, performance_multiplier);
        monitor_application_stop(a);
      }
    }
  }

  void apply_virtual_platforms(_rm_application_h* apps, int num_applications) {
    int id_counter;
    // Apply decrease
    for(id_counter = 0; id_counter < num_applications; id_counter++) {
      if (apps[id_counter].vp < apps[id_counter].vp_old)
        apply_scheddeadline(apps[id_counter].tid, apps[id_counter].vp);
    }
    // Apply increase
    for(id_counter = 0; id_counter < num_applications; id_counter++) {
      if (apps[id_counter].vp > apps[id_counter].vp_old)
        apply_scheddeadline(apps[id_counter].tid, apps[id_counter].vp);
    }
  }


  void write_log(_rm_application_h* apps, int num_applications) {
    if (num_applications > 0) {
      struct timespec time_info;
      int64_t current_time;
      clock_gettime(CLOCK_REALTIME, &time_info);
      current_time = (int64_t) time_info.tv_sec*1000000000 
        + (int64_t) time_info.tv_nsec;
      FILE* logfile = fopen("gtrm.log", "a+");
      int id_counter;
      for(id_counter = 0; id_counter < num_applications; id_counter++) {
        _application_h* a = monitor_application_init((int) apps[id_counter].tid);
        if (a!=NULL) {
          fprintf(logfile, "%lld, %d, %f, %f, %f\n", 
            current_time, apps[id_counter].tid, apps[id_counter].vp,
            apps[id_counter].performance, apps[id_counter].weight);
          monitor_application_stop(a);
        }
      }
      fclose(logfile);
    }
  }

#endif
