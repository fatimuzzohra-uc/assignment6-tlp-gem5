#include <cstdio>
#include <gem5/m5ops.h>

const int N = 65536;

static double X[N];
static double Y[N];

int main()
{
    const double alpha = 0.5;

    // Initialize vectors outside the measured region.
    for (int i = 0; i < N; ++i)
    {
        X[i] = 1.0 + (i % 100) * 0.01;
        Y[i] = 2.0 + (i % 100) * 0.01;
    }

    // Begin measured DAXPY region.
    m5_dump_reset_stats(0, 0);

    // DAXPY: Y = alpha * X + Y
    for (int i = 0; i < N; ++i)
    {
        Y[i] = alpha * X[i] + Y[i];
    }

    // End measured DAXPY region.
    m5_dump_reset_stats(0, 0);

    // Verification is outside the measured region.
    double sum = 0.0;

    for (int i = 0; i < N; ++i)
    {
        sum += Y[i];
    }

    printf("DAXPY checksum: %lf\n", sum);

    return 0;
}
