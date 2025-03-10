#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <math.h>
#include <stdint.h>

#define HISTORY_FILE_DATE "/home/tello/.zsh_history_date"
#define MAX_LONGITUD 500

// Estructuras para la orquesta
struct Note {
    double start_time;
    double duration;
    double frequency;
    float amplitude; // Nueva propiedad para controlar el volumen
};

struct Instrument {
    int num_notes;
    struct Note* notes;
};

// Escala de Do mayor
typedef struct {
    char *nombre;
    double frecuencia;
} NotaMusical;

NotaMusical escala[] = {
    {"C", 261.63}, {"D", 293.66}, {"E", 329.63}, {"F", 349.23},
    {"G", 392.00}, {"A", 440.00}, {"B", 493.88}
};
#define NUM_NOTAS (sizeof(escala)/sizeof(escala[0]))

// Prototipos
float* generate_instrument_samples(struct Instrument instr, int total_samples);
void write_wav(const char* filename, int16_t* data, int total_samples);
void obtener_fecha_actual(char *fecha_actual);
int longitud_historial(const char *fecha_actual);
void obtener_comandos(const char *fecha_actual, int MAX_COMANDOS, char comandos_date[MAX_COMANDOS][MAX_LONGITUD], int *num_comandos_date);
time_t obtener_timestamp(const char *linea);
void contador_tiempo_entre_comandos(double *array_tiempos, int num_comandos, const char array_comandos[num_comandos][MAX_LONGITUD]);

// Generar samples con amplitud
float* generate_instrument_samples(struct Instrument instr, int total_samples) {
    float* samples = calloc(total_samples, sizeof(float));
    for (int n = 0; n < instr.num_notes; n++) {
        int start = (int)(instr.notes[n].start_time * 44100);
        int end = start + (int)(instr.notes[n].duration * 44100);
        if (end > total_samples) end = total_samples;
        for (int i = start; i < end; i++) {
            double t = (double)(i - start) / 44100.0;
            samples[i] += instr.notes[n].amplitude * sin(2 * M_PI * instr.notes[n].frequency * t);
        }
    }
    return samples;
}

// Escribir archivo WAV
void write_wav(const char* filename, int16_t* data, int total_samples) {
    FILE* fp = fopen(filename, "wb");
    if (!fp) return;
    fwrite("RIFF", 1, 4, fp);
    uint32_t chunk_size = 36 + total_samples * 2;
    fwrite(&chunk_size, 4, 1, fp);
    fwrite("WAVEfmt ", 1, 8, fp);
    uint32_t subchunk1_size = 16;
    fwrite(&subchunk1_size, 4, 1, fp);
    uint16_t audio_format = 1;
    fwrite(&audio_format, 2, 1, fp);
    uint16_t num_channels = 1;
    fwrite(&num_channels, 2, 1, fp);
    uint32_t sample_rate = 44100;
    fwrite(&sample_rate, 4, 1, fp);
    uint32_t byte_rate = sample_rate * num_channels * 16 / 8;
    fwrite(&byte_rate, 4, 1, fp);
    uint16_t block_align = num_channels * 16 / 8;
    fwrite(&block_align, 2, 1, fp);
    uint16_t bits_per_sample = 16;
    fwrite(&bits_per_sample, 2, 1, fp);
    fwrite("data", 1, 4, fp);
    uint32_t subchunk2_size = total_samples * num_channels * 16 / 8;
    fwrite(&subchunk2_size, 4, 1, fp);
    fwrite(data, 2, total_samples, fp);
    fclose(fp);
}

// Funciones del historial
void obtener_fecha_actual(char *fecha_actual) {
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    sprintf(fecha_actual, "%04d-%02d-%02d", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday);
}

int longitud_historial(const char *fecha_actual) {
    FILE *f = fopen(HISTORY_FILE_DATE, "r");
    if (f == NULL) {
        perror("Archivo no encontrado");
        exit(1);
    }
    int MAX_COMANDOS = 0;
    char buffer[MAX_LONGITUD];
    while (fgets(buffer, sizeof(buffer), f)) {
        if (strncmp(buffer, fecha_actual, 10) == 0) {
            MAX_COMANDOS++;
        }
    }
    fclose(f);
    return MAX_COMANDOS;
}

void obtener_comandos(const char *fecha_actual, int MAX_COMANDOS, char comandos_date[MAX_COMANDOS][MAX_LONGITUD], int *num_comandos_date) {
    FILE *f = fopen(HISTORY_FILE_DATE, "r");
    if (f == NULL) {
        perror("Archivo no encontrado");
        exit(1);
    }
    *num_comandos_date = 0;
    char buffer[MAX_LONGITUD];
    while (fgets(buffer, sizeof(buffer), f) != NULL) {
        if (strncmp(buffer, fecha_actual, 10) == 0) {
            strncpy(comandos_date[*num_comandos_date], buffer, MAX_LONGITUD);
            comandos_date[*num_comandos_date][strcspn(comandos_date[*num_comandos_date], "\n")] = '\0';
            (*num_comandos_date)++;
            if (*num_comandos_date >= MAX_COMANDOS) break;
        }
    }
    fclose(f);
}

time_t obtener_timestamp(const char *linea) {
    struct tm tm_time = {0};
    if (sscanf(linea, "%4d-%2d-%2d %2d:%2d:%2d",
               &tm_time.tm_year, &tm_time.tm_mon, &tm_time.tm_mday,
               &tm_time.tm_hour, &tm_time.tm_min, &tm_time.tm_sec) != 6) {
        return -1;
    }
    tm_time.tm_year -= 1900;
    tm_time.tm_mon -= 1;
    return mktime(&tm_time);
}

void contador_tiempo_entre_comandos(double *array_tiempos, int num_comandos, const char array_comandos[num_comandos][MAX_LONGITUD]) {
    for (int i = 1; i < num_comandos; i++) {
        time_t t1 = obtener_timestamp(array_comandos[i - 1]);
        time_t t2 = obtener_timestamp(array_comandos[i]);
        if (t1 == -1 || t2 == -1) {
            array_tiempos[i - 1] = -1;
        } else {
            double diff = difftime(t2, t1);
            if (diff >= 10) {
                if (diff >= 100) {
                    if (diff >= 1000) {
                        if (diff >= 10000) array_tiempos[i-1] = diff * 0.0001;
                        else array_tiempos[i-1] = diff * 0.001;
                    } else array_tiempos[i-1] = diff * 0.01;
                } else array_tiempos[i-1] = diff * 0.1;
            } else array_tiempos[i-1] = diff;
        }
    }
    for (int i = 0; i < num_comandos - 1; i++) {
        array_tiempos[i] = array_tiempos[i] / 2.0;
    }
}

int main() {
    // Obtener datos del historial
    char fecha_actual[11];
    obtener_fecha_actual(fecha_actual);
    int MAX_COMANDOS = longitud_historial(fecha_actual);
    if (MAX_COMANDOS == 0) {
        printf("No hay comandos registrados para hoy.\n");
        return 0;
    }

    char array_comandos[MAX_COMANDOS][MAX_LONGITUD];
    int num_comandos = 0;
    obtener_comandos(fecha_actual, MAX_COMANDOS, array_comandos, &num_comandos);

    double array_tiempos[num_comandos - 1];
    contador_tiempo_entre_comandos(array_tiempos, num_comandos, array_comandos);

    // Crear partitura variada
    int num_notes = num_comandos - 1;
    struct Note *violin_notes = malloc(num_notes * sizeof(struct Note));
    struct Note *clarinet_notes = malloc(num_notes * sizeof(struct Note));
    struct Note *bass_notes = malloc(num_notes * sizeof(struct Note)); // Nuevo instrumento
    double current_time = 0.0;

    for (int i = 0; i < num_notes; i++) {
        double duration = (array_tiempos[i] != -1) ? array_tiempos[i] : 0.5;
        if (duration > 2.0) duration = 2.0; // Limitar duración máxima

        // Violín: Melodía rápida basada en la longitud del comando
        int cmd_len = strlen(array_comandos[i]) - 19; // Restar timestamp
        int note_idx = (cmd_len % NUM_NOTAS); // Elegir nota según longitud
        violin_notes[i].start_time = current_time;
        violin_notes[i].duration = duration * 0.5; // Notas más cortas
        violin_notes[i].frequency = escala[note_idx].frecuencia;
        violin_notes[i].amplitude = (cmd_len > 10) ? 0.8 : 0.4; // Más fuerte si comando largo

        // Clarinete: Armonía sostenida con variación
        clarinet_notes[i].start_time = current_time + 0.2; // Retraso
        clarinet_notes[i].duration = duration; // Notas más largas
        clarinet_notes[i].frequency = escala[(note_idx + 2) % NUM_NOTAS].frecuencia * 0.5; // Octava más baja
        clarinet_notes[i].amplitude = 0.6;

        // Contrabajo: Tono grave intermitente
        bass_notes[i].start_time = current_time;
        bass_notes[i].duration = duration * 1.5;
        bass_notes[i].frequency = escala[0].frecuencia * 0.5; // Do grave
        bass_notes[i].amplitude = (i % 2 == 0) ? 0.3 : 0.0; // Toca cada dos notas

        current_time += duration * 0.7; // Añadir pausas entre notas
    }

    struct Instrument instruments[] = {
        {num_notes, violin_notes},
        {num_notes, clarinet_notes},
        {num_notes, bass_notes}
    };
    int num_instruments = 3;
    int total_samples = (int)(current_time * 44100) + 44100; // Margen extra

    // Generar samples
    float* final_samples = calloc(total_samples, sizeof(float));
    for (int instr = 0; instr < num_instruments; instr++) {
        float* instr_samples = generate_instrument_samples(instruments[instr], total_samples);
        for (int i = 0; i < total_samples; i++) {
            final_samples[i] += instr_samples[i];
        }
        free(instr_samples);
    }

    // Normalizar
    float max_abs = 0.0;
    for (int i = 0; i < total_samples; i++) {
        if (fabs(final_samples[i]) > max_abs) max_abs = fabs(final_samples[i]);
    }
    if (max_abs > 0) {
        for (int i = 0; i < total_samples; i++) {
            final_samples[i] /= max_abs;
        }
    }

    // Convertir a 16-bit
    int16_t* wav_data = malloc(total_samples * sizeof(int16_t));
    for (int i = 0; i < total_samples; i++) {
        wav_data[i] = (int16_t)(final_samples[i] * 32767);
    }

    // Guardar archivo
    write_wav("varied_orchestra.wav", wav_data, total_samples);

    // Liberar memoria
    free(violin_notes);
    free(clarinet_notes);
    free(bass_notes);
    free(final_samples);
    free(wav_data);

    printf("Archivo 'varied_orchestra.wav' generado con éxito.\n");
    return 0;
}
