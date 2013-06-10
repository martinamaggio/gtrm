#include <cg.h>

#define GROUP "tests"

int main()
{
	cg_create(GROUP);
	cg_attach(GROUP, 0);
	cg_set_cpus(GROUP, 2, 3);
	cg_set_cfs_period(GROUP, 200000);
	cg_set_cfs_quota(GROUP, 200000);
	cg_detach(GROUP, 0);
	cg_destroy(GROUP);
	return 0;
}

