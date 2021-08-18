/*
 * store.h
 *
 *  Created on: 20 abr. 2021
 *      Author: utnso
 */

#ifndef POLUS_H_
#define POLUS_H_
#include<stdio.h>
#include<stdlib.h>
#include<commons/log.h>
#include<commons/string.h>
#include<commons/config.h>
#include <commons/collections/list.h>
#include <commons/bitarray.h>
#include<readline/readline.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include <signal.h>
#include <stdint.h>
#include <fcntl.h>

#include "mongoUtils.h"
typedef struct {
	uint32_t tamanio_blocks;
	uint32_t cantidad_blocks;
	uint32_t tamanio_total;
} ConfigFS;

typedef struct {
	uint32_t blockSize;
	uint32_t blocks;
	t_bitarray * bitmap;
}Superbloque;

typedef struct {
int size;
void * datos;
}SuperbloqueIMS;

char* datosBloques;
SuperbloqueIMS *datosSB;
void* superbloque;
Superbloque* datosSuperbloque;
ConfigFS configSB;
t_log* logMongo;
t_config* configMongo;
int tamanioBitarray;
int tiempoSync;
char* bitarrayGlobal;
void iniciarPolus();

void abrirDirectorioFiles();

void abrirDirectorioSuperBloque();
void abrirDirectorioBlocks();
char* directorio(char* direccion);
short existeArchivo(char *path);
void crearSuperBloqueBitmap(char* path);

//Agrega un nodo hijo al nodo padre
//verifica si existe el directorio de la ruta
bool existeDirectorio(char* path);
//verifica si las cadenas son iguales
bool sonIguales(char* a, char* b);
void iniciarMapeo();
char* directorioBitacoras();
char* directorioFiles();
char* direccionTarea(char* nombre);
char* direccionTareaMD5(char* nombre);
char* archivoTarea(char* nombre);
char* obtenerPosicionSabotaje(int nro);
#endif /* POLUS_H_ */
