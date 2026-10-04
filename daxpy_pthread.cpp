#include <cstdio>
#include <cstdlib>
#include <pthread.h>
#include <gem5/m5ops.h>

const int N = 1000;

static double X[N];
static double Y[N];

struct ThreadArgs
{
    int thread_id;
    int num_threads;
};

void *daxpy_worker(void *arg)
{
    ThreadArgs *args = static_cast<ThreadArgs *>(arg);

    const int thread_id = args->thread_id;
    const int num_threads = args->num_threads;

    const int chunk = N / num_threads;
    const int start = thread_id * chunk;
    const int end = (thread_id == num_threads - 1)
                        ? N
                        : start + chunk;

    const double alpha = 0.5;

    for (int i = start; i < end; ++i)
    {
        Y[i] = alpha * X[i] + Y[i];
    }

    return nullptr;
}

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        printf("Usage: daxpy_pthread <num_threads>\n");
        return 1;
    }

    const int num_threads = atoi(argv[1]);

    if (num_threads <= 0 || num_threads > 8)
    {
        printf("Number of threads must be between 1 and 8\n");
        return 1;
    }

    // Initialize shared vectors before the measured region.
    for (int i = 0; i < N; ++i)
    {
        X[i] = 1.0 + (i % 100) * 0.01;
        Y[i] = 2.0 + (i % 100) * 0.01;
    }

    pthread_t threads[8];
    ThreadArgs args[8];

    // Begin measured parallel DAXPY region.
    m5_dump_reset_stats(0, 0);

    for (int t = 0; t < num_threads; ++t)
    {
        args[t].thread_id = t;
        args[t].num_threads = num_threads;

        if (pthread_create(&threads[t], nullptr,
                           daxpy_worker, &args[t]) != 0)
        {
            printf("pthread_create failed for thread %d\n", t);
            return 1;
        }
    }

    for (int t = 0; t < num_threads; ++t)
    {
        pthread_join(threads[t], nullptr);
    }

    // End measured parallel DAXPY region.
    m5_dump_reset_stats(0, 0);

    // Verify the shared result.
    double checksum = 0.0;

    for (int i = 0; i < N; ++i)
    {
        checksum += Y[i];
    }

    printf("DAXPY pthread checksum: %lf\n", checksum);
    printf("Threads used: %d\n", num_threads);

    return 0;
}
