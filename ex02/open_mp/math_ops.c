#include "header.h"

/* same logic as mpi version, just no MPI. */
void omp_compute_chunk(const t_vector *lower, const t_vector *diag,
					   const t_vector *upper, const t_vector *b,
					   t_vector *y, int start, int count)
{
	int i;
	int n;

	n = diag->size;
	i = 0;
	while (i < count)
	{
		int g = start + i;
		double sum = diag->vec[g] * b->vec[g];

		if (g > 0)
			sum += lower->vec[g - 1] * b->vec[g - 1];
		if (g < n - 1)
			sum += upper->vec[g] * b->vec[g + 1];
		y->vec[g] = sum;
		i++;
	}
}

void tridiag_matvec_serial(const t_vector *lower, const t_vector *diag,
					   const t_vector *upper, const t_vector *b,
					   t_vector *y)
{
	int n;
	int i;

	n = diag->size;
	i = 0;
	while (i < n)
	{
		double sum = diag->vec[i] * b->vec[i];

		if (i > 0)
			sum += lower->vec[i - 1] * b->vec[i - 1];
		if (i < n - 1)
			sum += upper->vec[i] * b->vec[i + 1];
		y->vec[i] = sum;
		i++;
	}
}
