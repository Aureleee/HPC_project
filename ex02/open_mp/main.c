#include "header.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/* args: n lower diag upper threads display */
static void parse_args(int argc, char **argv,
                   int *n, double *lv, double *dv, double *uv,
                   int *threads, int *display)
{
    *n = 50;
    *lv = -2.0;
    *dv = 1.0;
    *uv = 2.0;
    *threads = 4;
    *display = 0;

    if (argc >= 5)
    {
        *n = atoi(argv[1]);
        *lv = atof(argv[2]);
        *dv = atof(argv[3]);
        *uv = atof(argv[4]);
    }
    if (argc >= 6)
        *threads = atoi(argv[5]);
    if (argc >= 7)
        *display = atoi(argv[6]);
    if (*n <= 0)
        *n = 50;
    if (*threads <= 0)
        *threads = 1;
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
    int n;
    double lv;
    double dv;
    double uv;
    int threads;
    int display;

    parse_args(argc, argv, &n, &lv, &dv, &uv, &threads, &display);
    omp_set_num_threads(threads);

    /* A tridiag: diag size n, lower/upper size n-1 */
    t_vector diag;
    t_vector lower;
    t_vector upper;
    t_vector b;
    t_vector y_serial;
    t_vector y_omp;

    if (!vector_alloc(&diag, n) || !vector_alloc(&b, n)
        || !vector_alloc(&y_serial, n) || !vector_alloc(&y_omp, n)
        || !vector_alloc(&lower, (n > 1) ? (n - 1) : 0)
        || !vector_alloc(&upper, (n > 1) ? (n - 1) : 0))
        return (1);

    vector_fill_constant(&diag, dv);
    if (n > 1)
    {
        vector_fill_constant(&lower, lv);
        vector_fill_constant(&upper, uv);
    }

    unsigned int seed = (unsigned int)time(NULL) ^ 0xBADC0FFEu;
    vector_fill_random(&b, &seed);

    /* serial first */
    double t0 = omp_get_wtime();
    tridiag_matvec_serial(&lower, &diag, &upper, &b, &y_serial);
    double t1 = omp_get_wtime();
    double t_serial = t1 - t0;

    /* openmp */
    double t2 = omp_get_wtime();
    omp_tridiag_matvec(&lower, &diag, &upper, &b, &y_omp);
    double t3 = omp_get_wtime();
    double t_omp = t3 - t2;

    double su = (t_omp > 0.0) ? (t_serial / t_omp) : 0.0;

    printf("n=%d threads=%d\n", n, threads);
    printf("time serial: %.6f s\n", t_serial);
    printf("time omp   : %.6f s\n", t_omp);
    printf("speedup: %.3f\n", su);
    printf("checksum serial: %.17g\n", checksum_sum(y_serial.vec, n));
    printf("checksum omp   : %.17g\n", checksum_sum(y_omp.vec, n));

    if (display)
    {
        vector_print("b", &b, 10);
        vector_print("y", &y_omp, 10);
    }

    vector_free(&diag);
    vector_free(&lower);
    vector_free(&upper);
    vector_free(&b);
    vector_free(&y_serial);
    vector_free(&y_omp);
    return (0);
}