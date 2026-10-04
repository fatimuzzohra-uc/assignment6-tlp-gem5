#include <cstdio>
#include <cstdlib>
#include <gem5/m5ops.h>

const int N = 1000;

static double X[N];
static double Y[N];

int main(int argc, char **argv)
{
    // Arguments:
    // argv[1] = core ID
    // argv[2] = total number of cores
    if (argc != 3)
    {
        printf("Usage: daxpy_tlp <core_id> <num_cores>\n");
        return 1;
    }

    const int core_id = atoi(argv[1]);
    const int num_cores = atoi(argv[2]);

    if (core_id < 0 || core_id >= num_cores)
    {
        printf("Invalid core ID %d for %d cores\n",
               core_id, num_cores);
        return 1;
    }

    const double alpha = 0.5;

    // Divide the vector among the cores.
    const int chunk = N / num_cores;
    const int start = core_id * chunk;
    const int end = (core_id == num_cores - 1)
                        ? N
                        : start + chunk;

    // Initialize only this core's assigned portion.
    for (int i = start; i < end; ++i)
    {
        X[i] = 1.0 + (i % 100) * 0.01;
        Y[i] = 2.0 + (i % 100) * 0.01;
    }

    // Begin measured DAXPY region.
    m5_dump_reset_stats(0, 0);

    // DAXPY: Y = alpha * X + Y
    for (int i = start; i < end; ++i)
    {
        Y[i] = alpha * X[i] + Y[i];
    }

    // End measured DAXPY region.
    m5_dump_reset_stats(0, 0);

    // Verify this core's assigned portion.
    double partial_sum = 0.0;

    for (int i = start; i < end; ++i)
    {
        partial_sum += Y[i];
    }

    printf("Core %d/%d completed elements %d-%d, partial checksum: %lf\n",
           core_id,
           num_cores,
           start,
           end - 1,
           partial_sum);

    return 0;
}
