// Standard headers
#include <stdio.h>
#include <stdlib.h>

// Custom headers
#include "gtrm_fun.h"

int main(int argc, char* argv[]) {

  if (argc != 3) {
    exit(-1);
  }
  double c1 = atof(argv[1]);
  double c2 = atof(argv[2]);
  gtrm_init();

  _rm_application_h apps[_RM_MAX_APPLICATIONS];
  int i = 0;
  int prev_num_apps = 0;

  while(1) {
    int num_apps = update_applications(apps);

    if (num_apps > 0) {
      compute_virtual_platforms(apps, num_apps, i, num_apps != prev_num_apps, c1, c2);
      rescale_virtual_platforms(apps, num_apps);
      rmset_performance_multiplier(apps, num_apps);

      write_log(apps, num_apps);

      apply_virtual_platforms(apps, num_apps);

      prev_num_apps = num_apps;
      ++i;
    }

  }

  unreachable();
}
