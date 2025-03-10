#include <stdio.h>
#include <math.h>
#include <stdint.h>
#include <stdlib.h>

// Definición de estructuras
struct Note {
    double start_time; // Tiempo de inicio en segundos
    double duration;   // Duración en segundos
    double frequency;  // Frecuencia en Hz
};

struct Instrument {
    int num_notes;     // Número de notas
    struct Note* notes; // Arreglo de notas
};

// Prototipo de la función
float* generate_instrument_samples(struct Instrument instr, int total_samples);

// Función para escribir el archivo WAV
void write_wav(const char* filename, int16_t* data, int total_samples) {
    FILE* fp = fopen(filename, "wb");
    if (!fp) return;

    fwrite("RIFF", 1, 4, fp);
    uint32_t chunk_size = 36 + total_samples * 2;
    fwrite(&chunk_size, 4, 1, fp);
    fwrite("WAVEfmt ", 1, 8, fp);
    uint32_t subchunk1_size = 16;
    fwrite(&subchunk1_size, 4, 1, fp);
    uint16_t audio_format = 1; // PCM
    fwrite(&audio_format, 2, 1, fp);
    uint16_t num_channels = 1; // Mono
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

// Implementación de generate_instrument_samples
float* generate_instrument_samples(struct Instrument instr, int total_samples) {
    float* samples = calloc(total_samples, sizeof(float));
    for (int n = 0; n < instr.num_notes; n++) {
        int start = (int)(instr.notes[n].start_time * 44100);
        int end = start + (int)(instr.notes[n].duration * 44100);
        if (end > total_samples) end = total_samples;
        for (int i = start; i < end; i++) {
            double t = (double)(i - start) / 44100.0;
            samples[i] = sin(2 * M_PI * instr.notes[n].frequency * t);
        }
    }
    return samples;
}

int main() {
    // Definir partitura
    struct Note violin_notes[] = {
        {0.0, 1.0, 440.0}, {1.0, 1.0, 523.25}, {2.0, 1.0, 659.25}
    };
    struct Note clarinet_notes[] = {
        {0.5, 1.0, 329.63}, {1.5, 1.0, 392.0}, {2.5, 1.0, 493.88}
    };
    struct Instrument instruments[] = {
        {3, violin_notes}, {3, clarinet_notes}
    };
    int num_instruments = 2;
    int total_samples = (int)(3.5 * 44100);

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
    write_wav("orchestra.wav", wav_data, total_samples);

    // Liberar memoria
    free(final_samples);
    free(wav_data);
    return 0;
}
