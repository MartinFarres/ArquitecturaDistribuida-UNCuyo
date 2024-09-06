#include <iostream>
#include <iomanip>
#include <cmath>
#include <chrono>
#include <thread>
#include <vector>

const unsigned int terms = 10000000; // 10 millones de términos

// Función que calcula la serie de Taylor en un rango de términos
void taylor_ln_partial(long double x, unsigned int start, unsigned int end, long double &result)
{
  long double term;
  long double ratio = (x - 1) / (x + 1);
  long double ratio_pow = ratio; // ratio^1

  for (unsigned int i = start; i <= end; i += 2)
  {
    term = ratio_pow / i;
    result += term;
    ratio_pow *= ratio * ratio;
  }
  result *= 2;
}

int main()
{
  long double number;
  std::cout << "Ingrese un número mayor a 1500000: ";
  std::cin >> number;

  if (number <= 1500000)
  {
    std::cerr << "El número debe ser mayor a 1500000." << std::endl;
    return 1;
  }

  unsigned int num_threads;
  std::cout << "Ingrese el número de hilos: ";
  std::cin >> num_threads;

  if (num_threads == 0 || num_threads > terms)
  {
    std::cerr << "Número de hilos inválido." << std::endl;
    return 1;
  }

  std::vector<std::thread> threads;
  std::vector<long double> results(num_threads, 0.0);

  auto start = std::chrono::high_resolution_clock::now();

  // Dividir el trabajo entre los hilos
  unsigned int terms_per_thread = terms / num_threads;
  for (unsigned int i = 0; i < num_threads; ++i)
  {
    unsigned int start_term = i * terms_per_thread + 1;
    unsigned int end_term = (i == num_threads - 1) ? terms : start_term + terms_per_thread - 2;
    threads.emplace_back(taylor_ln_partial, number, start_term, end_term, std::ref(results[i]));
  }

  for (auto &th : threads)
  {
    th.join();
  }

  long double final_result = 0.0;
  for (const auto &partial : results)
  {
    final_result += partial;
  }

  auto end = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double> elapsed = end - start;

  std::cout << "ln(" << number << ") = " << std::setprecision(15) << final_result << std::endl;
  std::cout << "Tiempo de ejecución: " << elapsed.count() << " segundos" << std::endl;

  return 0;
}
