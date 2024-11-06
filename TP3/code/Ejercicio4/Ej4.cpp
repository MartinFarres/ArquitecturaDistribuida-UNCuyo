#include <iostream>
#include <vector>
#include <cstdlib>
#include <sys/time.h>
#include <mpi.h>

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

// Función para multiplicar un rango de filas de matrices
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

int main(int argc, char *argv[])
{
  MPI_Init(&argc, &argv);

  int rank, size;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  int N;
  if (rank == 0)
  {
    std::cout << "Ingrese el valor de N para las matrices NxN: ";
    std::cin >> N;
  }

  // Compartir el valor de N con todos los procesos
  MPI_Bcast(&N, 1, MPI_INT, 0, MPI_COMM_WORLD);

  // Inicializar matrices A, B y C en el proceso raíz
  std::vector<std::vector<float>> A(N, std::vector<float>(N));
  std::vector<std::vector<float>> B(N, std::vector<float>(N));
  std::vector<std::vector<float>> C(N, std::vector<float>(N, 0.0f));

  if (rank == 0)
  {
    initialize_matrix_with_value(A, N, 0.1f);
    initialize_matrix_with_value(B, N, 0.2f);
  }

  timeval time1, time2;
  if (rank == 0)
  {
    gettimeofday(&time1, NULL);
  }

  // Enviar la matriz B a todos los procesos
  for (int i = 0; i < N; ++i)
  {
    MPI_Bcast(B[i].data(), N, MPI_FLOAT, 0, MPI_COMM_WORLD);
  }

  // Dividir las filas entre los procesos
  int rows_per_process = N / size;
  int start_row = rank * rows_per_process;
  int end_row = (rank == size - 1) ? N : start_row + rows_per_process;

  // Multiplicar la parte asignada de A por B y almacenar el resultado en la parte correspondiente de C
  std::vector<std::vector<float>> local_C(N, std::vector<float>(N, 0.0f));
  multiply_range(A, B, local_C, start_row, end_row, N);

  // Recopilar la matriz C completa en el proceso raíz
  for (int i = start_row; i < end_row; ++i)
  {
    MPI_Gather(local_C[i].data(), N, MPI_FLOAT, C[i].data(), N, MPI_FLOAT, 0, MPI_COMM_WORLD);
  }

  if (rank == 0)
  {
    gettimeofday(&time2, NULL);
    double execution_time = get_time_difference(time1, time2);

    // Mostrar esquinas de las matrices A, B y C
    std::cout << "Matriz A: " << std::endl;
    print_matrix_corners(A, N);

    std::cout << "Matriz B: " << std::endl;
    print_matrix_corners(B, N);

    std::cout << "Matriz C (Resultado): " << std::endl;
    print_matrix_corners(C, N);

    // Calcular y mostrar la sumatoria de todos los elementos de la matriz C
    float sum_result = sum_matrix(C, N);
    std::cout << "Sumatoria de todos los elementos de la matriz C: " << sum_result << std::endl;

    std::cout << "Tiempo de ejecución: " << execution_time << " segundos" << std::endl;
  }

  MPI_Finalize();
  return 0;
}
