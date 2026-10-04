#include <cstdio>
#include <cstdlib>
#include <gem5/m5ops.h>

const int TOTAL_N = 1000;

int main(int argc, char **argv)
{
    if (argc != 3)
    {
        printf("Usage: daxpy_core <core_id> <num_cores>\n");
        return 1;
    }

    const int core_id = atoi(argv[1]);
    const int num_cores = atoi(argv[2]);

    if (core_id < 0 || core_id >= num_cores)
    {
        printf("Invalid core configuration\n");
        return 1;
    }

    const int chunk = TOTAL_N / num_cores;
    const int start = core_id * chunk;
    const int end = (core_id == num_cores - 1)
                        ? TOTAL_N
                        : start + chunk;

    const int local_n = end - start;

    double *X = new double[local_n];
    double *Y = new double[local_n];

    const double alpha = 0.5;

    // Initialize this core's workload.
    for (int i = 0; i < local_n; ++i)
    {
        X[i] = 1.0 + ((start + i) % 100) * 0.01;
        Y[i] = 2.0 + ((start + i) % 100) * 0.01;
    }

    // Measure only the DAXPY computation.
    m5_dump_reset_stats(0, 0);

    for (int i = 0; i < local_n; ++i)
    {
        Y[i] = alpha * X[i] + Y[i];
    }

    m5_dump_reset_stats(0, 0);

    double checksum = 0.0;

    for (int i = 0; i < local_n; ++i)
    {
        checksum += Y[i];
    }

    printf(
        "Core %d/%d processed elements %d-%d, checksum: %lf\n",
        core_id,
        num_cores,
        start,
        end - 1,
        checksum
    );

    delete[] X;
    delete[] Y;

    return 0;
}
