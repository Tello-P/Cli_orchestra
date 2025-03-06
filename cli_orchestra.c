#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define HISTORY_FILE_DATE "/home/tello/.zsh_history_date"
#define MAX_LONGITUD 500

// Función para obtener la fecha actual en formato YYYY-MM-DD
void obtener_fecha_actual(char *fecha_actual)
{
	time_t t = time(NULL);
	struct tm tm = *localtime(&t);
	sprintf(fecha_actual, "%04d-%02d-%02d", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday);
}

// Función para contar el número de comandos del día actual
int longitud_historial(const char *fecha_actual)
{
	FILE *f = fopen(HISTORY_FILE_DATE, "r");
	if (f == NULL)
	{
		perror("Archivo no encontrado");
		exit(1);
	}

	int MAX_COMANDOS = 0;
	char buffer[MAX_LONGITUD];

	while (fgets(buffer, sizeof(buffer), f))
	{
		if (strncmp(buffer, fecha_actual, 10) == 0)
		{
			MAX_COMANDOS++;
		}
	}
	fclose(f);
	return MAX_COMANDOS;
}

// Función para obtener los comandos del día actual
void obtener_comandos(const char *fecha_actual, int MAX_COMANDOS, char comandos_date[MAX_COMANDOS][MAX_LONGITUD], int *num_comandos_date)
{
	FILE *f = fopen(HISTORY_FILE_DATE, "r");
	if (f == NULL)
	{
		perror("Archivo no encontrado");
		exit(1);
	}

	*num_comandos_date = 0;
	char buffer[MAX_LONGITUD];

	while (fgets(buffer, sizeof(buffer), f) != NULL)
	{
		if (strncmp(buffer, fecha_actual, 10) == 0)
		{
			strncpy(comandos_date[*num_comandos_date], buffer, MAX_LONGITUD);
			comandos_date[*num_comandos_date][strcspn(comandos_date[*num_comandos_date], "\n")] = '\0';
			(*num_comandos_date)++;
			if (*num_comandos_date >= MAX_COMANDOS)
			{
				break;
			}
		}
	}
	fclose(f);
}

// Función para extraer el timestamp de un comando
time_t obtener_timestamp(const char *linea)
{
	struct tm tm_time = {0};
	if (sscanf(linea, "%4d-%2d-%2d %2d:%2d:%2d",
				&tm_time.tm_year, &tm_time.tm_mon, &tm_time.tm_mday,
				&tm_time.tm_hour, &tm_time.tm_min, &tm_time.tm_sec) != 6)
	{
		return -1;
	}
	tm_time.tm_year -= 1900;
	tm_time.tm_mon -= 1;
	return mktime(&tm_time);
}

// Función para calcular la diferencia de tiempo entre comandos
void contador_tiempo_entre_comandos(float *array_tiempos, int num_comandos, const char array_comandos[num_comandos][MAX_LONGITUD])
{
	for (int i = 1; i < num_comandos; i++)
	{
		time_t t1 = obtener_timestamp(array_comandos[i - 1]);
		time_t t2 = obtener_timestamp(array_comandos[i]);
		if (t1 == -1 || t2 == -1)
		{
			array_tiempos[i - 1] = -1;
		}
		else
		{
			if (difftime(t2,t1) >= 60)
			{
				if (difftime(t2,t1) >= 3600)
					array_tiempos[i-1] = difftime(t2, t1) / 3600;
				else
					array_tiempos[i-1] = difftime(t2, t1) / 60;
			}
			else
				array_tiempos[i - 1] = difftime(t2, t1);
		}
	}
}

int main()
{
	char fecha_actual[11];
	obtener_fecha_actual(fecha_actual);

	int MAX_COMANDOS = longitud_historial(fecha_actual);
	if (MAX_COMANDOS == 0)
	{
		printf("No hay comandos registrados para hoy.\n");
		return 0;
	}

	char array_comandos[MAX_COMANDOS][MAX_LONGITUD];
	int num_comandos = 0;
	obtener_comandos(fecha_actual, MAX_COMANDOS, array_comandos, &num_comandos);

	float array_tiempos[num_comandos - 1];
	contador_tiempo_entre_comandos(array_tiempos, num_comandos, array_comandos);

	printf("\nDiferencias de tiempo entre comandos de hoy:\n");
	for (int i = 0; i < num_comandos - 1; i++)
	{
		if (array_tiempos[i] == -1)
		{
			printf("Error en el cálculo entre '%s' y '%s'\n", array_comandos[i], array_comandos[i + 1]);
		}
		else
		{
			printf("%.0f, ", array_tiempos[i]);
		}
	}

	return 0;
}

