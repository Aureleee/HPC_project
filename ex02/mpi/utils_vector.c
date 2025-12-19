/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils_vector.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aurele <aurele@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/19 00:27:45 by aurele            #+#    #+#             */
/*   Updated: 2025/12/19 13:53:55 by aurele           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include "header.h"

/* Compute the [start,count] chunk assigned to this MPI rank. */
static void	mpi_partition(int n, MPI_Comm comm, int *start, int *count)
{
    int rank;
    int size;
    int base;
    int rem;

    MPI_Comm_rank(comm, &rank);
    MPI_Comm_size(comm, &size);
    base = n / size;
    rem = n % size;
    *start = rank * base + (rank < rem ? rank : rem);
    *count = base + (rank < rem);
}

/* Compute the local mat-vec product for a contiguous chunk of rows. */
static void	mpi_compute_chunk(t_vector *lower, t_vector *diag, t_vector *upper,
            t_vector *b, t_vector *result, int start, int count)
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
        result->vec[g] = sum;
        i++;
    }
}

/* Build recvcounts/displs arrays for MPI_Gatherv on rank 0. */
static void	mpi_fill_counts(int n, MPI_Comm comm, int *counts, int *displs)
{
	int size;
	int base;
	int rem;
	int r;
	int disp;

	MPI_Comm_size(comm, &size);
	base = n / size;
	rem = n % size;
	disp = 0;
	r = 0;
	while (r < size)
	{
		counts[r] = base + (r < rem);
		displs[r] = disp;
		disp += counts[r];
		r++;
	}
}

/* Gather the distributed result vector back to rank 0. */
static void	mpi_gather_result(int n, int start, int count, double *buf, MPI_Comm comm)
{
	int rank;
	int size;

	MPI_Comm_rank(comm, &rank);
	MPI_Comm_size(comm, &size);
	if (rank == 0)
	{
		int counts[size];
		int displs[size];

		mpi_fill_counts(n, comm, counts, displs);
		MPI_Gatherv(MPI_IN_PLACE, count, MPI_DOUBLE,
			buf, counts, displs, MPI_DOUBLE, 0, comm);
		return ;
	}
	MPI_Gatherv(buf + start, count, MPI_DOUBLE,
		NULL, NULL, NULL, MPI_DOUBLE, 0, comm);
}

/* Allocate and fill a vector with a constant value. */
void init_vector(t_vector *vector, int size, double constant)
{
    vector->size = size;
    vector->vec = malloc(sizeof(double) * size);
    int i = 0;
    while (i < size)
    {
        vector->vec[i] = constant;
        i++;
    }  
}

/* Free a vector's storage and reset its pointer. */
void free_vector(t_vector *vector)
{
    free(vector->vec);
    vector->vec = NULL;
}

/* Compute y = A*b for a tridiagonal matrix A stored as (lower, diag, upper). */
void tridiag_matrix_vector_multiply(t_vector *lower, t_vector *diag, t_vector *upper,
                                    t_vector *b, t_vector *result)
{
    int n = diag->size;
    int i;

    for (i = 0; i < n; i++)
    {
        result->vec[i] = diag->vec[i] * b->vec[i];
        if (i > 0)
            result->vec[i] += lower->vec[i - 1] * b->vec[i - 1];
        if (i < n - 1)
            result->vec[i] += upper->vec[i] * b->vec[i + 1];
    }
}

/* Parallel MPI version of the tridiagonal mat-vec multiply (block distribution). */
void    mpi_tridiag_matrix_vector_multiply(t_vector *lower, t_vector *diag,
            t_vector *upper, t_vector *b, t_vector *result, MPI_Comm comm)
{
	int start;
	int count;
    int n;

    n = diag->size;
    mpi_partition(n, comm, &start, &count);
	mpi_compute_chunk(lower, diag, upper, b, result, start, count);
    mpi_gather_result(n, start, count, result->vec, comm);
}

/* Print the [start,count] distribution per rank in order (DISPLAY=2). */
void    mpi_print_work_distribution(int n, MPI_Comm comm)
{
	int rank;
	int size;
	int start;
	int count;
	int r;

	MPI_Comm_rank(comm, &rank);
	MPI_Comm_size(comm, &size);
	mpi_partition(n, comm, &start, &count);
	r = 0;
	while (r < size)
	{
		if (rank == r)
			printf("rank %d/%d: start=%d count=%d end=%d\n",
				rank, size, start, count, start + count - 1);
		MPI_Barrier(comm);
		r++;
	}
}
