#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

// Definición de una nota musical
typedef struct {
    char *nombre;
    double frecuencia;
} Nota;

// Escala de Do mayor con sus frecuencias (Hz)
Nota escala[] = {
    {"C", 261.63},
    {"D", 293.66},
    {"E", 329.63},
    {"F", 349.23},
    {"G", 392.00},
    {"A", 440.00},
    {"B", 493.88}
};

#define NUM_NOTAS (sizeof(escala)/sizeof(escala[0]))

int main() {
    // Simulación de intervalos de tiempo (en segundos) obtenidos del historial de comandos
    double intervalos[] = {0.5, 1.0, 0.75, 1.5, 0.25, 1.2, 0.8, 1.0};
    int numIntervalos = sizeof(intervalos) / sizeof(intervalos[0]);

    // Inicializa la semilla para números aleatorios
    srand(time(NULL));

    printf("Generando melodía basada en intervalos de comandos:\n");

    for (int i = 0; i < numIntervalos; i++) {
        // Selecciona una nota aleatoria de la escala
        int indice = rand() % NUM_NOTAS;
        Nota nota = escala[indice];

        // Mapea el intervalo al tiempo de duración de la nota
        double duracion = intervalos[i];
        
        // Construye el comando para reproducir la nota con SoX.
        // Ejemplo: play -n synth 0.5 sine 261.63
        char comando[256];
        sprintf(comando, "play -n synth %f sine %f", duracion, nota.frecuencia);
        printf("Reproduciendo nota %s con duración %f s: %s\n", nota.nombre, duracion, comando);

        // Ejecuta el comando para reproducir la nota
        system(comando);

        // Se podría agregar una pausa corta entre notas, si se desea
        // usleep(100000); // 100 ms
    }

    return 0;
}

