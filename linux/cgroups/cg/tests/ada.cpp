#include <array>
#include <pthread.h>

#include <cg.h>

static pthread_barrier_t b;

void *t(void *);

int main()
{
	pthread_barrier_init(&b, NULL, 4);

	std::array<pthread_t, 3> ts;
	for (int i = 0; i < 3; i++)
		pthread_create(&ts[i], NULL, t, NULL);

	pthread_barrier_wait(&b);

	cg_create("tests");
	cg_detach_all("tests");
	cg_destroy("tests");

	pthread_barrier_destroy(&b);

	for (int i = 0; i < 3; i++)
		pthread_join(ts[i], NULL);

	pthread_barrier_destroy(&b);

	return 0;
}

void *t(void *)
{
	pthread_barrier_wait(&b);
	cg_attach("tests", 0);
	pthread_barrier_wait(&b);
	return NULL;
}

