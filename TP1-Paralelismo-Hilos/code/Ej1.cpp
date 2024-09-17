#include <iostream>
#include <iomanip>
#include <cmath>
#include <thread>
#include <vector>
#include <sys/time.h>

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
  long long number;
  std::cout << "Ingrese un número mayor a 1500000: ";
  std::cin >> number;
  number = number * 1000000000000;
  unsigned int num_threads;
  std::cout << "Ingrese el número de hilos: ";
  std::cin >> num_threads;

  std::vector<std::thread> threads;
  std::vector<long double> results(num_threads, 0.0);

  // Declarar variables de tiempo
  struct timeval time1, time2;

  // Iniciar la medición de tiempo
  gettimeofday(&time1, NULL);

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

  // Terminar la medición de tiempo
  gettimeofday(&time2, NULL);

  // Calcular el tiempo de ejecución
  double execution_time = double(time2.tv_sec - time1.tv_sec) +
                          double(time2.tv_usec - time1.tv_usec) / 1000000;

  std::cout << "ln(" << number << ") = " << std::setprecision(15) << final_result << std::endl;
  std::cout << "Tiempo de ejecución: " << execution_time << " segundos" << std::endl;

  return 0;
}
