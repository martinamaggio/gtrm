#include <cg.h>

int main()
{
	cg_create("tests");
	cg_attach("tests", 0);
	cg_detach("tests", 0);
	cg_destroy("tests");
	return 0;
}

