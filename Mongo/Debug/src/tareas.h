/*
 * tareas.h
 *
 *  Created on: 8 jun. 2021
 *      Author: utnso
 */

#ifndef TAREAS_H_
#define TAREAS_H_

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
#include <openssl/md5.h>
#include "mongoUtils.h"
#include "i_mongo_store.h"

void generarOxigeno(int cantidad);
void generarComida(int cantidad);
void consumirOxigeno(int cantidad);
void consumirComida(int cantidad);
void descargarBasura(int cantidad);
int tamanioTarea(char* tarea);
void generarBasura(int cantidad);
void crearArchivoTarea(char* tarea, char * llenadoCaracteres);
void escribirTarea(char*tarea, int cantidad, char tipo);
void aumentarCantidad(char* direccionTarea, char* llenado);
void disminuirCantidad(char * direccionTarea, char* consumido);
void consumirTarea(char* tarea, int cantidad);
void verificarTarea(char* tarea, int cantidad);
char* obtenerHash(char* direccion);
char* generarHash(char* direccion);
void insertarHash(char* direccion,char* out);
void liberarBloques(char** bloques,int cantidad);
void iniciarTarea(char* nombreTarea, int cantidad);
#endif /* TAREAS_H_ */
