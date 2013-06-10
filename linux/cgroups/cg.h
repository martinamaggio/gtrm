#ifndef _CG_H
#define _CG_H

#ifdef __cplusplus
extern "C" {
#endif

// management application programming interface
void cg_create(const char *group);
void cg_destroy(const char *group); 
void cg_attach(const char *group, int tid);
void cg_detach(const char *group, int tid);
void cg_detach_all(const char *group);

// actuation application programming interface
void cg_set_cpus_str(const char *group, const char *cpus);
void cg_set_cpus(const char *group, unsigned int first, unsigned int last);
void cg_set_cfs_period(const char *group, unsigned long period);
void cg_set_cfs_quota(const char *group, unsigned long quota);

#ifdef __cplusplus
}
#endif

#endif

