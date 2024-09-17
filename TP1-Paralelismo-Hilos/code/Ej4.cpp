#include <iostream>
#include <vector>
#include <cmath>
#include <thread>
#include <mutex>
#include <sys/time.h>
#include <algorithm>

std::mutex prime_mutex; // Mutex para proteger el acceso a la lista de números primos

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
      std::lock_guard<std::mutex> guard(prime_mutex); // Mutex para evitar condiciones de carrera
      primes.push_back(i);
    }
  }
}

// Función que genera los números primos hasta sqrt(N) usando un solo hilo
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

int main()
{
  long long N;
  int num_threads;
  std::cout << "Ingrese el valor de N: ";
  std::cin >> N;
  std::cout << "Ingrese el número de hilos a usar: ";
  std::cin >> num_threads;

  std::vector<long long> primes;
  std::vector<long long> base_primes;

  timeval time1, time2;
  gettimeofday(&time1, NULL);

  // Buscar los primos base hasta sqrt(N)
  find_base_primes(sqrt(N), base_primes);

  // Crear hilos para buscar números primos en paralelo
  std::vector<std::thread> threads;
  long long range_size = N / num_threads;

  for (int i = 0; i < num_threads; ++i)
  {
    long long start = i * range_size + 2;
    long long end = (i == num_threads - 1) ? N : (i + 1) * range_size + 1;
    threads.emplace_back(find_primes_in_range, start, end, std::cref(base_primes), std::ref(primes));
  }

  // Esperar a que todos los hilos terminen
  for (auto &t : threads)
  {
    t.join();
  }

  gettimeofday(&time2, NULL);
  double execution_time = get_time_difference(time1, time2);

  // Ordenar el vector de números primos
  std::sort(primes.begin(), primes.end());

  // Mostrar los 10 mayores números primos
  std::cout << "Los 10 mayores números primos son:" << std::endl;
  for (int i = std::max(0, int(primes.size()) - 10); i < primes.size(); ++i)
  {
    std::cout << primes[i] << " ";
  }
  std::cout << std::endl;

  // Mostrar la cantidad total de números primos
  std::cout << "Cantidad total de números primos menores que " << N << ": " << primes.size() << std::endl;
  std::cout << "Tiempo de ejecución: " << execution_time << " segundos" << std::endl;

  return 0;
}
