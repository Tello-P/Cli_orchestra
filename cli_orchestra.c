#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define HISTORY_FILE "/home/tello/.zsh_history"
#define HISTORY_FILE_DATE "/home/tello/.zsh_history_date"
#define MAX_LONGITUD 500


int longitud_historial()
{
	FILE *f = fopen(HISTORY_FILE_DATE, "r");
    	if (f == NULL)
	{
        	perror("Archivo no encontrado");
        	exit(1);
	}

	int MAX_COMANDOS = 0;
	char buffer[MAX_LONGITUD];

	while (fgets(buffer,sizeof(buffer),f))
	{
		MAX_COMANDOS++;
	}
	fclose(f);
	return MAX_COMANDOS;
}


void obtener_comandos(const int MAX_COMANDOS, char comandos_date[MAX_COMANDOS][MAX_LONGITUD],int *num_comandos_date)
{
	/* Obtiene desde el fichero HISTORY_FILE_DATE
	* todos los comandos almacenados en 
	* formato string, en orden de uso, de mas
	* antiguo a mas reciente.
	*/

	FILE *f = fopen(HISTORY_FILE_DATE, "r");
    	if (f == NULL)
	{
        	perror("Archivo no encontrado");
        	exit(1);
    	}

    	*num_comandos_date = 0;  

    	while (fgets(comandos_date[*num_comandos_date], MAX_LONGITUD, f) != NULL)
	{
        	comandos_date[*num_comandos_date][strcspn(comandos_date[*num_comandos_date], "\n")] = '\0';
        
        	(*num_comandos_date)++;  
	
        	if (*num_comandos_date >= MAX_COMANDOS)
		{
        	   	break;
        	}
    	}

	   fclose(f);
}


int main()
{

	// Obtener la longitud del historial
	int MAX_COMANDOS = longitud_historial();

	// Obtener el historial
	char array_comandos[MAX_COMANDOS][MAX_LONGITUD];
	int num_comandos = 0;
	obtener_comandos(MAX_COMANDOS, array_comandos, &num_comandos);

	for (int i=0; i<num_comandos; i++)
	{
		printf("%s\n",array_comandos[i]);
	}

	return 0;
}
