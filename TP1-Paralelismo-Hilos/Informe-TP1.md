# Arquitectura Distribuida

# Trabajo Practico 1 - Paralelismo a nivel de hilos

## Información del Sistema

Para poder tener una imagen y entender el contexto sobre el cual se va a trabajar, se presentan las caracteristicas de la computadora la cual ejecutara los siguientes programas;

### Arquitectura

- **Tipo**: x86_64
- **Modo(s) de operación de CPU**: 32-bit, 64-bit
- **Tamaños de dirección**: 48 bits físicos, 48 bits virtuales

### Detalles del CPU

- **Total de CPU(s)**: 16
- **ID del vendedor**: AuthenticAMD
- **Nombre del modelo**: AMD Ryzen 7 5700U con gráficos Radeon
  - **Hilos por núcleo**: 2
  - **Núcleos por socket**: 8
  - **Socket(s)**: 1
  - **Stepping**: 1
  - **Máxima frecuencia de CPU**: 4372,0000 MHz
  - **Mínima frecuencia de CPU**: 400,0000 MHz
  - **BogoMIPS**: 3593.13

## Serie de Taylor del Logaritmo Natural

Se realizo un programa que calcule el logaritmo natural mediente serie de Tayler, empleando diez millones de terminos de dicha serie. El objetivo es observa

![alt text](ej1-h1-2.png)
![alt text](ej1-h1.png)

![alt text](ej1-h20-2.png)
![alt text](ej1-h20.png)

Se logra ver un leve aumento en todos los porcentaje de uso de los CPUS para la implementacion con hilos; pero, para la implementacion con un hilo, se ve
1500000000
