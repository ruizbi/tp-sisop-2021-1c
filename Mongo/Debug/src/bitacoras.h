/*
 * bitacoras.h
 *
 *  Created on: 8 jun. 2021
 *      Author: utnso
 */

#ifndef BITACORAS_H_
#define BITACORAS_H_

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

#include <sys/mman.h>

#include "mongoUtils.h"
#define INIT_BITACORA "Inicio de bitacora\n"
#define INIT_SIZE 19


char* buscarBitacora(int id);
void crearBitacora(char*bitacora);
void escribirBitacora(int id, char* datoAccion);
char* direccionTripulante(char* id);
void actualizarTripulante(char* bitacoras, char* accion);
char* archivoTripulante(char nombre[]);
//segunda version
void actualizarBitacora(char* bitacoras, char* accion);
#endif /* BITACORAS_H_ */
