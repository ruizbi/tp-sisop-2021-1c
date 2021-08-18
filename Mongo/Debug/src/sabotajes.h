/*
 * sabotaje.h
 *
 *  Created on: 11 jun. 2021
 *      Author: utnso
 */

#ifndef SABOTAJES_H_
#define SABOTAJES_H_
#include <stdio.h>
#include <stdlib.h>
#include "polus.h"
#include "archivos.h"
#include <unistd.h>
#include <signal.h>
#include <stdint.h>
#include <fcntl.h>
#include <string.h>
#include <commons/string.h>
#include <pthread.h>
#include "mongoUtils.h"

void sabotearCantidadBloques(uint32_t cambio);
bool repararCantidadBloques();
void repararBitmap();
bool repararSizeTarea(char* nombre);
bool repararBlockCount();
bool repararBlocks();
void restaurarArchivo(char* tarea,char*nombre);
t_list * recolectarBloquesTareas(t_list * listaBloques);
t_list * recolectarBloquesBitacoras();
t_list* controlBloquesExitentes(char**bloques, int cantidad, char caracter,
		int size);
int contarCaracteres(char*info, char caracter);
void mostrarBits();
int cantidadBloquesFalsos(char** bloques);
void repararMongo();
bool repararSabotajeFiles(char* nombre);
#endif /* SABOTAJES_H_ */
