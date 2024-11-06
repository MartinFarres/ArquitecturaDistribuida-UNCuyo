#include <iostream>
#include <vector>
#include <cmath>
#include <sys/time.h>
#include <algorithm>
#include <mpi.h>

// Función para medir el tiempo de ejecución
double get_time_difference(timeval start, timeval end)
{
  return double(end.tv_sec - start.tv_sec) + double(end.tv_usec - start.tv_usec) / 1000000;
}

// Función que busca números primos en un rango específico
void find_primes_in_range(long long start, long long end, const std::vector<long long> &base_primes, std::vector<long long> &primes)
{
  std::vector<bool> is_prime(end - start + 1, true);

  for (long long prime : base_primes)
  {
    long long first_multiple = std::max(prime * prime, (start + prime - 1) / prime * prime);
    for (long long j = first_multiple; j <= end; j += prime)
    {
      is_prime[j - start] = false;
    }
  }

  // Guardar los números primos en el rango actual
  for (long long i = start; i <= end; ++i)
  {
    if (is_prime[i - start])
    {
      primes.push_back(i);
    }
  }
}

// Función que genera los números primos hasta sqrt(N) usando un solo proceso
void find_base_primes(long long N, std::vector<long long> &base_primes)
{
  std::vector<bool> is_prime(N + 1, true);
  is_prime[0] = is_prime[1] = false;

  for (long long i = 2; i <= sqrt(N); ++i)
  {
    if (is_prime[i])
    {
      for (long long j = i * i; j <= N; j += i)
      {
        is_prime[j] = false;
      }
      base_primes.push_back(i);
    }
  }
}

int main(int argc, char *argv[])
{
  MPI_Init(&argc, &argv);

  int world_size, world_rank;
  MPI_Comm_size(MPI_COMM_WORLD, &world_size);
  MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

  long long N;
  if (world_rank == 0)
  {
    std::cout << "Ingrese el valor de N: ";
    std::cin >> N;
  }

  // Broadcast del valor de N a todos los procesos
  MPI_Bcast(&N, 1, MPI_LONG_LONG, 0, MPI_COMM_WORLD);

  std::vector<long long> base_primes;
  if (world_rank == 0)
  {
    find_base_primes(sqrt(N), base_primes);
  }

  // Broadcast de los primos base a todos los procesos
  int base_primes_size = base_primes.size();
  MPI_Bcast(&base_primes_size, 1, MPI_INT, 0, MPI_COMM_WORLD);
  if (world_rank != 0)
    base_primes.resize(base_primes_size);
  MPI_Bcast(base_primes.data(), base_primes_size, MPI_LONG_LONG, 0, MPI_COMM_WORLD);

  // Cada proceso encuentra los primos en su rango asignado
  long long range_size = N / world_size;
  long long start = world_rank * range_size + 2;
  long long end = (world_rank == world_size - 1) ? N : (world_rank + 1) * range_size + 1;

  std::vector<long long> local_primes;
  find_primes_in_range(start, end, base_primes, local_primes);

  // Recolección de los números primos encontrados en cada proceso en el proceso 0
  std::vector<long long> global_primes;
  if (world_rank == 0)
  {
    global_primes.reserve(N / 10); // Reserva aproximada para optimizar la memoria
  }

  // Gather
  int local_size = local_primes.size();
  std::vector<int> all_sizes(world_size);
  MPI_Gather(&local_size, 1, MPI_INT, all_sizes.data(), 1, MPI_INT, 0, MPI_COMM_WORLD);

  if (world_rank == 0)
  {
    int total_size = 0;
    for (int size : all_sizes)
      total_size += size;
    global_primes.resize(total_size);
  }

  std::vector<int> displacements(world_size, 0);
  if (world_rank == 0)
  {
    displacements[0] = 0;
    for (int i = 1; i < world_size; ++i)
    {
      displacements[i] = displacements[i - 1] + all_sizes[i - 1];
    }
  }

    MPI_Gatherv(local_primes.data(), local_size, MPI_LONG_LONG, global_primes.data(), all_sizes.data(), displacements
