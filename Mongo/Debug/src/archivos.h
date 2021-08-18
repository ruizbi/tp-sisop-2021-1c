/*
 * archivos.h
 *
 *  Created on: 30 abr. 2021
 *      Author: utnso
 */

#ifndef ARCHIVOS_H_
#define ARCHIVOS_H_
#include <stdio.h>
#include <stdlib.h>
#include "polus.h"
#include <unistd.h>
#include <signal.h>
#include <stdint.h>
#include <fcntl.h>
#include <string.h>
#include <commons/string.h>

#include <sys/mman.h>
#include "mongoUtils.h"


typedef struct {
	uint32_t size;
	t_list* blocks;
	uint32_t block_count;
}Blocks;


Blocks* asignarBloques(int cantidad);
Blocks* iniciarBloque(char* registro);
char* buscarBloque(int nroBloque);
void insertarBloqueMapeo(char* bloque, int nroBloque);

t_list* cargarLista(char *datos);
char* listaAString(t_list* datos);
int buscarPosicionLibre();
char* grabarDatosBloque(char* bloque,char *grabar);
int calculoBloques(int size);
char* agregarListaBlocks(char** bloques, int cantBloques, t_list * lista);
char* borrarDatosBloque(char* bloque, char* grabar);
void liberarPosicion(int pos);
char* eliminarListaBlocks(char** bloques, int cantBloques, int eliminados);
bool bloquelleno(char* bloque);

//Segunda version
char* borrarDatosBlock(char* bloque, int cantidad);
char* grabarDatosBlock(char* bloque, char* grabar);
void insertarBlockMapeo(char* bloque, int nroBloque);
char* buscarBlock(int nroBloque);
Blocks* iniciarBlock(char* registro);
void liberarLista(t_list* lista);
#endif /* ARCHIVOS_H_ */
