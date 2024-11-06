#include <iostream>
#include <iomanip>
#include <cmath>
#include <mpi.h>
#include <sys/time.h>
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

int main(int argc, char *argv[])
{
    MPI_Init(&argc, &argv);
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    long long number;
    if (rank == 0) {
        std::cout << "Ingrese un número mayor a 1500000: ";
        std::cin >> number;
        number = number * 1000000000000;
    }

    // Enviar el número a todos los procesos
    MPI_Bcast(&number, 1, MPI_LONG_LONG, 0, MPI_COMM_WORLD);

    unsigned int num_threads = size; // Número de procesos
    std::vector<long double> results(num_threads, 0.0);

    // Declarar variables de tiempo
    struct timeval time1, time2;

    // Iniciar la medición de tiempo
    gettimeofday(&time1, NULL);

    // Dividir el trabajo entre los procesos
    unsigned int terms_per_process = terms / size;
    unsigned int start_term = rank * terms_per_process + 1;
    unsigned int end_term = (rank == size - 1) ? terms : start_term + terms_per_process - 2;

    // Calcular la parte de la serie de Taylor
    taylor_ln_partial(number, start_term, end_term, results[rank]);

    // Reunir los resultados en el proceso 0
    long double final_result = 0.0;
    MPI_Reduce(&results[rank], &final_result, 1, MPI_LONG_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

    // Terminar la medición de tiempo
    gettimeofday(&time2, NULL);

    // Calcular el tiempo de ejecución
    double execution_time = double(time2.tv_sec - time1.tv_sec) +
                            double(time2.tv_usec - time1.tv_usec) / 1000000;

    // Mostrar resultados solo en el proceso 0
    if (rank == 0) {
        std::cout << "ln(" << number << ") = " << std::setprecision(15) << final_result << std::endl;
        std::cout << "Tiempo de ejecución: " << execution_time << " segundos" << std::endl;
    }

    MPI_Finalize();
    return 0;
}
