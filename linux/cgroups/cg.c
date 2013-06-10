#include <cg.h>

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "params.h"
#include "utils.h"

void cg_create(const char *group)
{
	assert(group != NULL);

	char cg[PATH_SIZE_MAX];
	snprintf(cg, PATH_SIZE_MAX, MOUNT_POINT "/%s", group);

	// XXX: if mkdir fails it is possible the cgroup already exists. This
	// allows avoiding an additional function call to stat(2). -siro
	int retval = mkdir(cg, S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH);
	assert(retval == 0);

	char cg_cpus[PATH_SIZE_MAX];
	snprintf(cg_cpus, PATH_SIZE_MAX, MOUNT_POINT "/cpuset.cpus");
	FILE *f = fopen(cg_cpus, "r");
	assert(f != NULL);
	char *cpus = NULL;
	int n = fscanf(f, "%m[0-9-]", &cpus);
	assert(n == 1 && cpus != NULL);
	fclose(f);

	snprintf(cg_cpus, PATH_SIZE_MAX, "%s/cpuset.cpus", cg);
	f = fopen(cg_cpus, "w");
	assert(f != NULL);
	setvbuf(f, NULL, _IONBF, 0);
	fprintf(f, "%s", cpus);
	free(cpus);
	fclose(f);

	char cg_mems[PATH_SIZE_MAX];
	snprintf(cg_mems, PATH_SIZE_MAX, "%s/cpuset.mems", cg);
	f = fopen(cg_mems, "w");
	assert(f != NULL);
	setvbuf(f, NULL, _IONBF, 0);
	fprintf(f, "0");
	fclose(f);
}

void cg_destroy(const char *group)
{
	assert(group != NULL);

	cg_detach_all(group);

	char cg[PATH_SIZE_MAX];
	snprintf(cg, PATH_SIZE_MAX, MOUNT_POINT "/%s", group);

	int retval = rmdir(cg);
	assert(retval == 0);
}

void cg_attach(const char *group, int tid)
{
	assert(group != NULL);
	assert(tid >= 0);

	if (tid == 0)
		tid = gettid();

	char cg_tasks[PATH_SIZE_MAX];
	snprintf(cg_tasks, PATH_SIZE_MAX, MOUNT_POINT "/%s/tasks", group);
	FILE *f = fopen(cg_tasks, "w");
	assert(f != NULL);
	setvbuf(f, NULL, _IONBF, 0);
	fprintf(f, "%d", (int) tid);
	fclose(f);
}

void cg_detach(const char *group, int tid)
{
	assert(group != NULL);
	assert(tid >= 0);

	if (tid == 0)
		tid = gettid();

	// FIXME: double-check to guarantee the actually TID exists. -siro

	char cg_tasks[PATH_SIZE_MAX];
	snprintf(cg_tasks, PATH_SIZE_MAX, MOUNT_POINT "/tasks");
	FILE *f = fopen(cg_tasks, "w");
	assert(f != NULL);
	setvbuf(f, NULL, _IONBF, 0);
	fprintf(f, "%d", (int) tid);
	fclose(f);
}

void cg_detach_all(const char *group)
{
	assert(group != NULL);

	char cg_tasks[PATH_SIZE_MAX];
	snprintf(cg_tasks, PATH_SIZE_MAX, MOUNT_POINT "/%s/tasks", group);
	FILE *f = fopen(cg_tasks, "rm");
	assert(f != NULL);
	char *tasks = NULL;
	int n = fscanf(f, "%m[0-9\n]", &tasks);
	fclose(f);
	if ( n == -1 && tasks == NULL)
		return;
	assert(n == 1 && tasks != NULL);

	snprintf(cg_tasks, PATH_SIZE_MAX, MOUNT_POINT "/tasks");
	f = fopen(cg_tasks, "w");
	assert(f);
	setvbuf(f, NULL, _IONBF, 0);
	char *tasks_context;
	char *task = strtok_r(tasks, "\n", &tasks_context); 
	while (task != NULL) {
		fprintf(f, "%s", task);
		task = strtok_r(NULL, "\n", &tasks_context);
	}
	fclose(f);
	free(tasks);
}

void cg_set_cpus_str(const char *group, const char *cpus)
{
	assert(group != NULL);
	assert(cpus != NULL);

	char cg_cpus[PATH_SIZE_MAX];
	snprintf(cg_cpus, PATH_SIZE_MAX, MOUNT_POINT "/%s/cpuset.cpus", group);
	FILE *f = fopen(cg_cpus, "w");
	assert(f != NULL);
	setvbuf(f, NULL, _IONBF, 0);
	fprintf(f, "%s", cpus);
	fclose(f);
}

void cg_set_cpus(const char *group, unsigned int first, unsigned int last)
{
	assert(group != NULL);
	assert(first <= last);

	char cg_cpus[PATH_SIZE_MAX];
	snprintf(cg_cpus, PATH_SIZE_MAX, MOUNT_POINT "/%s/cpuset.cpus", group);
	FILE *f = fopen(cg_cpus, "w");
	assert(f != NULL);
	setvbuf(f, NULL, _IONBF, 0);
	fprintf(f, "%u-%u", first, last);
	fclose(f);
}

void cg_set_cfs_period(const char *group, unsigned long period)
{
	assert(group != NULL);

	char cg_cfs_period[PATH_SIZE_MAX];
	snprintf(cg_cfs_period, PATH_SIZE_MAX, MOUNT_POINT "/%s/cpu.cfs_period_us", group);
	FILE *f = fopen(cg_cfs_period, "w");
	assert(f != NULL);
	setvbuf(f, NULL, _IONBF, 0);
	fprintf(f, "%lu", period);
	fclose(f);
}

void cg_set_cfs_quota(const char *group, unsigned long quota)
{
	assert(group != NULL);

	char cg_cfs_quota[PATH_SIZE_MAX];
	snprintf(cg_cfs_quota, PATH_SIZE_MAX, MOUNT_POINT "/%s/cpu.cfs_quota_us", group);
	FILE *f = fopen(cg_cfs_quota, "w");
	assert(f != NULL);
	setvbuf(f, NULL, _IONBF, 0);
	fprintf(f, "%lu", quota);
	fclose(f);
}

