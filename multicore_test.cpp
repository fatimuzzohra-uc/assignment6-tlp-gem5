#include <cstdio>

int main(int argc, char **argv)
{
    int cpu_id = (argc > 1) ? argv[1][0] - '0' : -1;

    printf("Multicore test process started: CPU %d\n", cpu_id);

    volatile double result = 0.0;

    for (int i = 0; i < 100; ++i)
    {
        result += i * 0.5;
    }

    printf("CPU %d result = %f\n", cpu_id, result);

    return 0;
}
