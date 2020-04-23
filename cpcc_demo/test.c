#include <stdio.h>
#include <stdlib.h>
#include "calc.h"

int main(int argc, char* argv[])
{
	int a = 1, b = 2;
	swap(&a,&b);
	printf("%d %d\n",a,b);
	return EXIT_SUCCESS;
}
