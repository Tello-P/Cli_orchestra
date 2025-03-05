#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Escala de Do mayor: C, D, E, F, G, A, B
// Para más detalles sobre melodías y teoría musical: https://es.wikipedia.org/wiki/Teor%C3%ADa_musical
int main() {
    const char *escala[] = {"C", "D", "E", "F", "G", "A", "B"};
    int numNotas = sizeof(escala) / sizeof(escala[0]);
    int longitudMelodia = 16; // Número de notas de la melodía

    srand(time(NULL));

    // Selecciona una nota de inicio aleatoria
    int indiceActual = rand() % numNotas;
    printf("Melodía con movimientos suaves en Do mayor:\n");
    printf("%s ", escala[indiceActual]);

    for (int i = 1; i < longitudMelodia; i++) {
        // Permite cambiar la nota en -1, 0 o +1 grados de la escala
        int cambio = (rand() % 3) - 1; // resultado: -1, 0 o 1
        int nuevoIndice = indiceActual + cambio;

        // Evitar que se salga de los límites de la escala
        if (nuevoIndice < 0)
            nuevoIndice = 0;
        if (nuevoIndice >= numNotas)
            nuevoIndice = numNotas - 1;

        printf("%s ", escala[nuevoIndice]);
        indiceActual = nuevoIndice;
    }
    printf("\n");

    return 0;
}

