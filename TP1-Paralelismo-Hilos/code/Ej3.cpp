#include <iostream>
#include <vector>
#include <thread>
#include <cstdlib>
#include <sys/time.h>

// Función para medir el tiempo de ejecución
double get_time_difference(timeval start, timeval end)
{
  return double(end.tv_sec - start.tv_sec) + double(end.tv_usec - start.tv_usec) / 1000000;
}

// Función para inicializar una matriz con un valor fijo
void initialize_matrix_with_value(std::vector<std::vector<float>> &matrix, int N, float value)
{
  for (int i = 0; i < N; ++i)
  {
    for (int j = 0; j < N; ++j)
    {
      matrix[i][j] = value;
    }
  }
}

// Función para multiplicar un rango de filas de matrices en paralelo
void multiply_range(const std::vector<std::vector<float>> &A, const std::vector<std::vector<float>> &B, std::vector<std::vector<float>> &C, int start_row, int end_row, int N)
{
  for (int i = start_row; i < end_row; ++i)
  {
    for (int j = 0; j < N; ++j)
    {
      C[i][j] = 0.0f;
      for (int k = 0; k < N; ++k)
      {
        C[i][j] += A[i][k] * B[k][j];
      }
    }
  }
}

// Función para calcular la sumatoria de todos los elementos de la matriz
float sum_matrix(const std::vector<std::vector<float>> &matrix, int N)
{
  float sum = 0.0f;
  for (int i = 0; i < N; ++i)
  {
    for (int j = 0; j < N; ++j)
    {
      sum += matrix[i][j];
    }
  }
  return sum;
}

// Función para mostrar las esquinas de la matriz
void print_matrix_corners(const std::vector<std::vector<float>> &matrix, int N)
{
  std::cout << "Esquinas de la matriz:" << std::endl;
  std::cout << "Top-left: " << matrix[0][0] << std::endl;
  std::cout << "Top-right: " << matrix[0][N - 1] << std::endl;
  std::cout << "Bottom-left: " << matrix[N - 1][0] << std::endl;
  std::cout << "Bottom-right: " << matrix[N - 1][N - 1] << std::endl;
}

int main()
{
  int N, num_threads;
  std::cout << "Ingrese el valor de N para las matrices NxN: ";
  std::cin >> N;
  std::cout << "Ingrese el número de hilos a usar: ";
  std::cin >> num_threads;

  // Inicializar matrices A, B, y C
  std::vector<std::vector<float>> A(N, std::vector<float>(N));
  std::vector<std::vector<float>> B(N, std::vector<float>(N));
  std::vector<std::vector<float>> C(N, std::vector<float>(N));

  // Inicializar la matriz A con 0.1 y la matriz B con 0.2
  initialize_matrix_with_value(A, N, 0.1f);
  initialize_matrix_with_value(B, N, 0.2f);

  timeval time1, time2;
  gettimeofday(&time1, NULL);

  // Crear los hilos para multiplicar las matrices en paralelo
  std::vector<std::thread> threads;
  int rows_per_thread = N / num_threads;

  for (int i = 0; i < num_threads; ++i)
  {
    int start_row = i * rows_per_thread;
    int end_row = (i == num_threads - 1) ? N : start_row + rows_per_thread;
    threads.emplace_back(multiply_range, std::cref(A), std::cref(B), std::ref(C), start_row, end_row, N);
  }

  // Esperar a que todos los hilos terminen
  for (auto &t : threads)
  {
    t.join();
  }

  gettimeofday(&time2, NULL);
  double execution_time = get_time_difference(time1, time2);

  // Mostrar esquinas de las matrices
  std::cout << "Matriz A: " << std::endl;
  print_matrix_corners(A, N);

  std::cout << "Matriz B: " << std::endl;
  print_matrix_corners(B, N);

  std::cout << "Matriz C (Resultado): " << std::endl;
  print_matrix_corners(C, N);

  // Calcular la sumatoria de la matriz C
  float sum_result = sum_matrix(C, N);
  std::cout << "Sumatoria de todos los elementos de la matriz C: " << sum_result << std::endl;

  std::cout << "Tiempo de ejecución: " << execution_time << " segundos" << std::endl;

  return 0;
}
