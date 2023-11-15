#include <stdlib.h>
#include <time.h>
#include <stdio.h>

// A002182 - OEIS
#define I 1260
#define J 1680
#define K 2520

void prepare(int (*c)[I][J], int (*a)[I][K], int (*b)[K][J]) {
	for (int i = 0; i < I; i++)
		for (int k = 0; k < K; k++)
			(*a)[i][k] = i == k ? 1 : 0; // identity matrix with extra 0s

	for (int k = 0; k < K; k++)
		for (int j = 0; j < J; j++)
			(*b)[k][j] = k == j ? 1 : 0; // identity matrix with extra 0s


	for (int i = 0; i < I; i++)
		for (int j = 0; j < J; j++)
			(*c)[i][j] = 0; // zero matrix to store result
}

#ifdef RUN_NAIVE

void compute(int (*c)[I][J], const int (*a)[I][K], const int (*b)[K][J]) {
	for (int i = 0; i < I; i++)
	{
		for (int j = 0; j < J; j++)
		{
			int sum = 0;
			for (int k = 0; k < K; k++)
			{
				sum += (*a)[i][k] * (*b)[k][j];
			}
			(*c)[i][j] = sum;
		}
	}
}

#else

void compute(int (*c)[I][J], const int (*a)[I][K], const int (*b)[K][J]) {
#ifdef TRANSPOSE_A
	int (*a_t)[K][I] = malloc(sizeof(int[K][I]));

	// copy a into a_t
	for (int i = 0; i < I; i++)
		for (int k = 0; k < K; k++)
			(*a_t)[k][i] = (*a)[i][k];

	#define access_a(i, k) (*a_t)[k][i]
#else
	#define access_a(i, k) (*a)[i][k]
#endif

#ifdef TRANSPOSE_B
	int (*b_t)[J][K] = malloc(sizeof(int[J][K]));

	// copy b into b_t
	for (int k = 0; k < K; k++)
		for (int j = 0; j < J; j++)
			(*b_t)[j][k] = (*b)[k][j];

	#define access_b(k, j) (*b_t)[j][k]
#else
	#define access_b(k, j) (*b)[k][j]
#endif

	for (int tile_i = 0; tile_i < I; tile_i += I_BLOCK_SIZE)
	{
		for (int tile_j = 0; tile_j < J; tile_j += J_BLOCK_SIZE)
		{
			for (int product_part = 0; product_part < K; product_part += K_BLOCK_SIZE)
			{
				for (int i = tile_i; i < tile_i + I_BLOCK_SIZE; i++)
				{
					for (int j = tile_j; j < tile_j + J_BLOCK_SIZE; j++)
					{
						// computes one part of the dot product

						int part_sum = (*c)[i][j];

						for (int k = product_part; k < product_part + K_BLOCK_SIZE; k++)
						{
							part_sum += access_a(i, k) * access_b(k, j);
						}

						(*c)[i][j] = part_sum;
					}
				}
			}
		}
	}
}

#endif

int main(int argc, const char* argv[])
{
#ifndef RUN_NAIVE
	if (I % I_BLOCK_SIZE != 0 || J % J_BLOCK_SIZE != 0 || K % K_BLOCK_SIZE != 0)
	{
		printf("Error: block sizes do not divide matrix dimensions\n");
		return 1;
	}
#endif

	int (*c)[I][J] = malloc(sizeof(int[I][J]));
	int (*a)[I][K] = malloc(sizeof(int[I][K]));
	int (*b)[K][J] = malloc(sizeof(int[K][J]));

	prepare(c, a, b);

	clock_t start = clock();
	compute(c, a, b);
	clock_t end = clock();

	printf("%lf s\n", (double)(end - start) / CLOCKS_PER_SEC);

	int check_sum = 0;
	int expected = I;
	if (J < expected) expected = J;


	for (int i = 0; i < I; i++)
		for (int j = 0; j < J; j++)
			check_sum += (*c)[i][j];

	if (check_sum != expected)
	{
		fprintf(stderr, "Error: check sum is not %d\n", expected);
		return 1;
	}

	return 0;
}
