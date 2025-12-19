#include "header.h"

#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/* args: n lower diag upper display
   b is random so no arg for that */
static void parse_args(int argc, char **argv,
					   int *n, double *lv, double *dv, double *uv, int *display)
{
	*n = 50;
	*lv = -2.0;
	*dv = 1.0;
	*uv = 2.0;
	*display = 0;

	if (argc >= 5)
	{
		*n = atoi(argv[1]);
		*lv = atof(argv[2]);
		*dv = atof(argv[3]);
		*uv = atof(argv[4]);
	}
	if (argc >= 6)
		*display = atoi(argv[5]);

	if (*n <= 0)
		*n = 50;
}

static double checksum_sum(const double *x, int n)
{
	double s;
	int i;

	s = 0.0;
	i = 0;
	while (i < n)
	{
		s += x[i];
		i++;
	}
	return (s);
}

int main(int argc, char **argv)
{
	int rank;
	int size;
	int n;
	double lv;
	double dv;
	double uv;
	int display;

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);

	parse_args(argc, argv, &n, &lv, &dv, &uv, &display);

	/* A is tridiagonal stored as 3 vectors. lower/upper are n-1 */
	t_vector lower;
	t_vector diag;
	t_vector upper;

	if (!vector_alloc(&diag, n) || !vector_alloc(&lower, n - 1) || !vector_alloc(&upper, n - 1))
		MPI_Abort(MPI_COMM_WORLD, 1);
	vector_fill_constant(&diag, dv);
	if (n > 1)
	{
		vector_fill_constant(&lower, lv);
		vector_fill_constant(&upper, uv);
	}

	/* partition for b chunk */
	int start;
	int count;
	mpi_partition(n, MPI_COMM_WORLD, &start, &count);

	t_vector b_local;
	t_vector b_full;
	if (!vector_alloc(&b_local, count) || !vector_alloc(&b_full, n))
		MPI_Abort(MPI_COMM_WORLD, 1);

	unsigned int seed = (unsigned int)time(NULL) ^ (unsigned int)(rank * 1337u);
	vector_fill_random(&b_local, &seed);

	/* MPI is not always happy with NULL even when count=0 */
	double dummy = 0.0;
	double *b_send = (count > 0) ? b_local.vec : &dummy;

	/* every rank receives full b (required) */
	mpi_allgatherv_double(b_send, count, b_full.vec, n, MPI_COMM_WORLD);

	/* serial baseline on rank 0 only (same data b_full) */
	double t_serial = 0.0;
	double t_mpi_max = 0.0;

	t_vector y_serial;
	t_vector y_root;
	if (rank == 0)
	{
		if (!vector_alloc(&y_serial, n) || !vector_alloc(&y_root, n))
			MPI_Abort(MPI_COMM_WORLD, 1);
	}

	/* all ranks go here, sinon deadlock */
	MPI_Barrier(MPI_COMM_WORLD);
	if (rank == 0)
	{
		double ts0 = MPI_Wtime();
		tridiag_matvec_serial(&lower, &diag, &upper, &b_full, &y_serial);
		double ts1 = MPI_Wtime();
		t_serial = ts1 - ts0;
	}
	MPI_Barrier(MPI_COMM_WORLD);

	/* mpi compute: local chunk into y_local then gather on root */
	double *y_local = NULL;
	if (count > 0)
	{
		y_local = (double *)malloc(sizeof(double) * (size_t)count);
		if (!y_local)
			MPI_Abort(MPI_COMM_WORLD, 1);
	}
	double *y_send = (count > 0) ? y_local : &dummy;

	MPI_Barrier(MPI_COMM_WORLD);
	double t0 = MPI_Wtime();
	if (count > 0)
		mpi_compute_chunk(&lower, &diag, &upper, &b_full, y_local, start, count);
	mpi_gatherv_double_root(y_send, count,
							(rank == 0) ? y_root.vec : NULL,
							n, 0, MPI_COMM_WORLD);
	MPI_Barrier(MPI_COMM_WORLD);
	double t1 = MPI_Wtime();

	double t_local = t1 - t0;
	MPI_Reduce(&t_local, &t_mpi_max, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);

	if (rank == 0)
	{
		double su = 0.0;
		if (t_mpi_max > 0.0)
			su = t_serial / t_mpi_max;

		printf("n=%d ranks=%d\n", n, size);
		printf("time serial: %.6f s\n", t_serial);
		printf("time mpi (max): %.6f s\n", t_mpi_max);
		printf("speedup: %.3f\n", su);

		/* just a quick check, pas besoin d etre exact */
		printf("checksum serial: %.17g\n", checksum_sum(y_serial.vec, n));
		printf("checksum mpi   : %.17g\n", checksum_sum(y_root.vec, n));

		if (display)
		{
			vector_print("b", &b_full, 10);
			vector_print("y", &y_root, 10);
		}
		vector_free(&y_serial);
		vector_free(&y_root);
	}

	free(y_local);
	vector_free(&b_local);
	vector_free(&b_full);
	vector_free(&lower);
	vector_free(&diag);
	vector_free(&upper);

	MPI_Finalize();
	return (0);
}