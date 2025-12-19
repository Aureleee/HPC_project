#include "header.h"

#include <stdlib.h>

/* block partition with the remainder handled on first ranks */
void mpi_partition(int n, MPI_Comm comm, int *start, int *count)
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

/* counts/displs matching the same base+rem logic */
void mpi_counts_displs(int n, MPI_Comm comm, int *counts, int *displs)
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

/* everyone gets the full vector back, recvbuf must be n doubles */
void mpi_allgatherv_double(const double *sendbuf, int sendcount,
                           double *recvbuf, int n, MPI_Comm comm)
{
    int size;

    MPI_Comm_size(comm, &size);

    {
        int *counts = (int *)malloc(sizeof(int) * (size_t)size);
        int *displs = (int *)malloc(sizeof(int) * (size_t)size);

        if (!counts || !displs)
        {
            free(counts);
            free(displs);
            MPI_Abort(comm, 2);
        }

        mpi_counts_displs(n, comm, counts, displs);
        MPI_Allgatherv(sendbuf, sendcount, MPI_DOUBLE,
                       recvbuf, counts, displs, MPI_DOUBLE, comm);

        free(counts);
        free(displs);
    }
}

/* gather on root only, recvbuf valid only on root */
void mpi_gatherv_double_root(const double *sendbuf, int sendcount,
                             double *recvbuf, int n, int root, MPI_Comm comm)
{
    int rank;
    int size;

    MPI_Comm_rank(comm, &rank);
    MPI_Comm_size(comm, &size);

    if (rank == root)
    {
        int *counts = (int *)malloc(sizeof(int) * (size_t)size);
        int *displs = (int *)malloc(sizeof(int) * (size_t)size);

        if (!counts || !displs)
        {
            free(counts);
            free(displs);
            MPI_Abort(comm, 2);
        }

        mpi_counts_displs(n, comm, counts, displs);
        MPI_Gatherv(sendbuf, sendcount, MPI_DOUBLE,
                recvbuf, counts, displs, MPI_DOUBLE, root, comm);

        free(counts);
        free(displs);
        return;
    }

    MPI_Gatherv(sendbuf, sendcount, MPI_DOUBLE,
                NULL, NULL, NULL, MPI_DOUBLE, root, comm);
}
