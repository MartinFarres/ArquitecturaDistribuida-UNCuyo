#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <thread>
#include <sys/time.h>

// Función para medir el tiempo de ejecución
double get_time_difference(timeval start, timeval end)
{
  return double(end.tv_sec - start.tv_sec) + double(end.tv_usec - start.tv_usec) / 1000000;
}

// Función para contar ocurrencias de un patrón en el texto
int count_pattern(const std::string &text, const std::string &pattern)
{
  int count = 0;
  size_t pos = text.find(pattern, 0);
  while (pos != std::string::npos)
  {
    count++;
    pos = text.find(pattern, pos + 1);
  }
  return count;
}

// Función que ejecutará cada hilo para buscar un patrón
void search_pattern(const std::string &text, const std::string &pattern, int index, int &result)
{
  result = count_pattern(text, pattern);
}

int main()
{
  // Solicitar la cantidad de hilos al usuario
  int num_threads;
  std::cout << "Ingrese la cantidad de hilos (1 a 32): ";
  std::cin >> num_threads;

  // Validar el número de hilos
  if (num_threads < 1 || num_threads > 32)
  {
    std::cerr << "Número de hilos no válido. Debe estar entre 1 y 32." << std::endl;
    return 1;
  }

  // Leer el archivo texto.txt
  std::ifstream text_file("texto.txt");
  std::string text;
  if (text_file.is_open())
  {
    getline(text_file, text);
    text_file.close();
  }
  else
  {
    std::cerr << "Error al abrir el archivo texto.txt" << std::endl;
    return 1;
  }

  // Leer los patrones desde patrones.txt
  std::ifstream pattern_file("patrones.txt");
  std::vector<std::string> patterns;
  std::string pattern;
  while (getline(pattern_file, pattern))
  {
    patterns.push_back(pattern);
  }
  pattern_file.close();

  timeval time1, time2;
  gettimeofday(&time1, NULL);

  // Si num_threads es 1, hacer la búsqueda sin multihilos
  if (num_threads == 1)
  {
    for (size_t i = 0; i < patterns.size(); i++)
    {
      int count = count_pattern(text, patterns[i]);
      std::cout << "El patrón " << i << " aparece " << count << " veces" << std::endl;
    }
  }
  else
  {
    // Crear hilos para buscar patrones
    std::vector<std::thread> threads(num_threads);
    std::vector<int> results(patterns.size(), 0);

    for (size_t i = 0; i < num_threads; ++i)
    {
      threads[i] = std::thread([&](int idx)
                               { search_pattern(text, patterns[idx], idx, results[idx]); }, i);
    }

    // Esperar a que todos los hilos terminen
    for (auto &t : threads)
    {
      t.join();
    }

    // Mostrar resultados
    for (size_t i = 0; i < patterns.size(); ++i)
    {
      std::cout << "El patrón " << i << " aparece " << results[i] << " veces" << std::endl;
    }
  }

  gettimeofday(&time2, NULL);
  double execution_time = get_time_difference(time1, time2);
  std::cout << "Tiempo de ejecución con " << num_threads << " hilo(s): " << execution_time << " segundos" << std::endl;

  return 0;
}
