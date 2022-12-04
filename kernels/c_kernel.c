#include <stdlib.h>

#define N 1260

int main(int argc, const char** argv)
{
    int n = BLOCK_SIZE * (N / BLOCK_SIZE);

    int *a = malloc(sizeof(int[N*N]));
    int *b = malloc(sizeof(int[N*N]));
    int *c = malloc(sizeof(int[N*N]));

    volatile int _init = 0;
    int init = _init;


    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < N; j++)
            a[i * N +j] = b[i * N + j] = _init;

        a[i * N + i] = b[i * N + i] = 1;
    }

    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++)
            c[i * N + j] = _init;

    int sum = 0;

    for (int j1 = 0; j1 < n; j1 += BLOCK_SIZE)
    {
        for (int k1 = 0; k1 < n; k1 += BLOCK_SIZE)
        {
            for (int i = 0; i < n; i++)
            {
                for (int j = j1; j < j1 + BLOCK_SIZE; j++)
                {
                    sum = c[i * N + j];

                    for (int k = k1; k < k1 + BLOCK_SIZE; k++)
                    {
                        sum += a[i * N + k] * b[k * N + j];
                    }

                    c[i * N + j] = sum;
                }
            }
        }
    }

    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++)
            init += c[i * N + j];
    
    _init = init;
    
    return !(_init == N);
}
