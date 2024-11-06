#include <iostream>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include "direccion_IP.cpp"  // Incluye la definición de obtener_IP

using namespace std;

int main(int argc, char *argv[]) {
    if (argc != 2) {
        cout << "Uso: " << argv[0] << " <número de procesos>" << endl;
        return 1;
    }

    int n = atoi(argv[1]);
    char ip[40];
    obtener_IP(ip);

    for (int i = 0; i < n; i++) {
        pid_t pid = fork();
        if (pid < 0) {
            cerr << "Error al crear el proceso." << endl;
            return 1;
        }
        if (pid == 0) {
            // Proceso hijo
            cout << "Hola Mundo! soy el proceso " << i + 1 << " de " << n << " corriendo en la máquina "
                 << "IP= " << ip << endl;
            exit(0); // Termina el proceso hijo
        }
    }

    // Espera a que todos los procesos hijos terminen
    for (int i = 0; i < n; i++) {
        wait(NULL);
    }

    return 0;
}
