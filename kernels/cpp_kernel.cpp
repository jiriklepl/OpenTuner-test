#include <array>
#include <vector>
#include <chrono>
#include <iostream>

#define I 1280
#define J 1280
#define K 1280

void prepare(auto &c, auto &a, auto &b) {
	for (int i = 0; i < I; i++)
		for (int k = 0; k < K; k++)
			a[i][k] = i == k ? 1 : 0; // identity matrix with extra 0s

	for (int k = 0; k < K; k++)
		for (int j = 0; j < J; j++)
			b[k][j] = k == j ? 1 : 0; // identity matrix with extra 0s


	for (int i = 0; i < I; i++)
		for (int j = 0; j < J; j++)
			c[i][j] = 0; // zero matrix to store result
}

void compute(auto &c, const auto &a, const auto &b) {
#ifdef TRANSPOSE_A
	auto a_t = std::vector<std::array<int, I>>(K);

	// copy a into a_t
	for (int i = 0; i < I; i++)
		for (int k = 0; k < K; k++)
			a_t[k][i] = a[i][k];

	#define access_a(i, k) a_t[k][i]
#else
	#define access_a(i, k) a[i][k]
#endif

#ifdef TRANSPOSE_B
	auto b_t = std::vector<std::array<int, K>>(J);

	// copy b into b_t
	for (int k = 0; k < K; k++)
		for (int j = 0; j < J; j++)
			b_t[j][k] = b[k][j];

	#define access_b(k, j) b_t[j][k]
#else
	#define access_b(k, j) b[k][j]
#endif

	for (int tile_i = 0; tile_i < I; tile_i += I_BLOCK_SIZE)
	{
		for (int tile_j = 0; tile_j < J; tile_j += J_BLOCK_SIZE)
		{
			for (int product_part = 0; product_part < K; product_part += K_BLOCK_SIZE)
			{
				for (int i = tile_i; i < tile_i + I_BLOCK_SIZE; i++)
				{
					if (i >= I)
						break;

					for (int j = tile_j; j < tile_j + J_BLOCK_SIZE; j++)
					{
						// computes one part of the dot product

						if (j >= J)
							break;

						int part_sum = c[i][j];

						for (int k = product_part; k < product_part + K_BLOCK_SIZE; k++)
						{
							if (k >= K)
								break;

							part_sum += access_a(i, k) * access_b(k, j);
						}

						c[i][j] = part_sum;
					}
				}
			}
		}
	}
}

int main(int argc, const char** argv)
{
	if (I % I_BLOCK_SIZE != 0 || J % J_BLOCK_SIZE != 0 || K % K_BLOCK_SIZE != 0)
	{
		std::cerr << "Error: block sizes do not divide matrix dimensions\n";
		return 1;
	}

	auto c = std::vector<std::array<int, J>>(I);
	auto a = std::vector<std::array<int, K>>(I);
	auto b = std::vector<std::array<int, J>>(K);

	prepare(c, a, b);

	auto start = std::chrono::steady_clock::now();
	compute(c, a, b);
	auto end = std::chrono::steady_clock::now();

	std::cout << "Time: " << end - start << "\n";

	int check_sum = 0;
	for (int i = 0; i < I; i++)
		for (int j = 0; j < J; j++)
			check_sum += c[i][j];

	if (check_sum != std::min(I, J))
	{
		std::cerr << "Error: check sum is not" << std::min(I, J) << "\n";
		return 1;
	}
	
	std::cout << "Check sum: " << check_sum << "\n";
	return 0;
}
