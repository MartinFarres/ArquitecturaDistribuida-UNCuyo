#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <mpi.h>
#include <sys/time.h>

// Función para medir el tiempo de ejecución
double get_time_difference(timeval start, timeval end) {
    return double(end.tv_sec - start.tv_sec) + double(end.tv_usec - start.tv_usec) / 1000000;
}

// Función para contar ocurrencias de un patrón en el texto
int count_pattern(const std::string &text, const std::string &pattern) {
    int count = 0;
    size_t pos = text.find(pattern, 0);
    while (pos != std::string::npos) {
        count++;
        pos = text.find(pattern, pos + 1);
    }
    return count;
}

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // Solo el proceso 0 lee el archivo
    std::string text;
    std::vector<std::string> patterns;

    if (rank == 0) {
        // Leer el archivo texto.txt
        std::ifstream text_file("texto.txt");
        if (text_file.is_open()) {
            getline(text_file, text);
            text_file.close();
        } else {
            std::cerr << "Error al abrir el archivo texto.txt" << std::endl;
            MPI_Abort(MPI_COMM_WORLD, 1);
        }

        // Leer los patrones desde patrones.txt
        std::ifstream pattern_file("patrones.txt");
        std::string pattern;
        while (getline(pattern_file, pattern)) {
            patterns.push_back(pattern);
        }
        pattern_file.close();
    }

    // Declarar variables de tiempo
    struct timeval time1, time2;

    // Iniciar la medición de tiempo
    gettimeofday(&time1, NULL);

    // Enviar el texto y patrones a todos los procesos
    int text_size = text.size();
    MPI_Bcast(&text_size, 1, MPI_INT, 0, MPI_COMM_WORLD);
    if (rank != 0) {
        text.resize(text_size);
    }
    MPI_Bcast(&text[0], text_size, MPI_CHAR, 0, MPI_COMM_WORLD);

    // Enviar patrones
    int patterns_size = patterns.size();
    MPI_Bcast(&patterns_size, 1, MPI_INT, 0, MPI_COMM_WORLD);
    if (rank != 0) {
        patterns.resize(patterns_size);
    }
    for (int i = 0; i < patterns_size; ++i) {
        int pattern_length = patterns[i].size();
        MPI_Bcast(&pattern_length, 1, MPI_INT, 0, MPI_COMM_WORLD);
        if (rank != 0) {
            patterns[i].resize(pattern_length);
        }
        MPI_Bcast(&patterns[i][0], pattern_length, MPI_CHAR, 0, MPI_COMM_WORLD);
    }

    // Cada proceso cuenta un subconjunto de patrones
    int patterns_per_process = patterns_size / size;
    int start_index = rank * patterns_per_process;
    int end_index = (rank + 1 == size) ? patterns_size : start_index + patterns_per_process;

    std::vector<int> local_results(end_index - start_index, 0);
    for (int i = start_index; i < end_index; ++i) {
        local_results[i - start_index] = count_pattern(text, patterns[i]);
    }

    // Recopilar resultados en el proceso 0
    std::vector<int> global_results(patterns_size, 0);
    MPI_Gather(local_results.data(), local_results.size(), MPI_INT,
               global_results.data(), local_results.size(), MPI_INT,
               0, MPI_COMM_WORLD);
    

    // Terminar la medición de tiempo
    gettimeofday(&time2, NULL);

    // Calcular el tiempo de ejecución
    double execution_time = double(time2.tv_sec - time1.tv_sec) +
                            double(time2.tv_usec - time1.tv_usec) / 1000000;

    // Solo el proceso 0 imprime los resultados
    if (rank == 0) {
        for (size_t i = 0; i < patterns.size(); i++) {
            std::cout << "El patrón " << i << " aparece " << global_results[i] << " veces" << std::endl;
        }
        std::cout << "Tiempo de ejecución: " << execution_time << " segundos" << std::endl;
    }

    MPI_Finalize();
    return 0;
}
