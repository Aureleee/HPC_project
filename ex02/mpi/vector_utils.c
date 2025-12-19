
#include "header.h"

#include <stdio.h>
#include <stdlib.h>

/* basic vector helpers, rien de fancy */

int vector_alloc(t_vector *v, int n)
{
	v->size = n;
	v->vec = NULL;
	if (n < 0)
		return (0);
	if (n == 0)
		return (1);
	v->vec = (double *)malloc(sizeof(double) * (size_t)n);
	if (!v->vec)
		return (0);
	return (1);
}

void vector_free(t_vector *v)
{
	free(v->vec);
	v->vec = NULL;
	v->size = 0;
}

void vector_fill_constant(t_vector *v, double x)
{
	int i;

	i = 0;
	while (i < v->size)
	{
		v->vec[i] = x;
		i++;
	}
}

/* rand_r is ok here. just want a random-ish vector */
void vector_fill_random(t_vector *v, unsigned int *seed)
{
	int i;

	i = 0;
	while (i < v->size)
	{
		unsigned int r = (unsigned int)rand_r(seed);
		double u = (double)r / (double)RAND_MAX;

		v->vec[i] = 2.0 * u - 1.0;
		i++;
	}
}

void vector_print(const char *name, const t_vector *v, int max_elems)
{
	int i;
	int n;

	if (!v || !v->vec)
		return;
	n = v->size;
	if (max_elems > 0 && max_elems < n)
		n = max_elems;

	if (name)
		printf("%s ", name);
	printf("(n=%d): ", v->size);
	i = 0;
	while (i < n)
	{
		printf("%.6f ", v->vec[i]);
		i++;
	}
	if (n < v->size)
		printf("...\n");
	else
		printf("\n");
}
