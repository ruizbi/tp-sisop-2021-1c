/*
 * bitacoras.c
 *
 *  Created on: 8 jun. 2021
 *      Author: utnso
 */
#include "bitacoras.h"
//string y id
char* buscarBitacora(int id) {
	char* numId = string_itoa(id);
	char * bitaTripulante = direccionTripulante(numId);
	if (!existeArchivo(bitaTripulante)) {
		char* respuesta = string_new();
		string_append(&respuesta,"No se encuentra la Bitacora");
		free(bitaTripulante);
		free(numId);
		return respuesta;
	}
	t_config* bitacoraAux = config_create(bitaTripulante);
	int sizeBita = config_get_int_value(bitacoraAux, "SIZE");
	int cantidadBloques = calculoBloques(sizeBita);
	char** blocks = config_get_array_value(bitacoraAux, "BLOCKS");
	char* bitacoraTripulante = string_new();

	int i = 0;
	while (i < cantidadBloques) {
		int* aux = malloc(sizeof(int));
		*aux = strtol(blocks[i], NULL, 10);
		char*info = buscarBloque(*aux);
		if (i + 1 < cantidadBloques) {
			string_append(&bitacoraTripulante, info);
		} else {
			int tamanio = sizeBita % configSB.tamanio_blocks;
			char* resto = string_substring(info, 0, tamanio);
			string_append(&bitacoraTripulante, resto);
			free(resto);
		}
		free(aux);
		free(info);
		i++;
	}
	free(numId);
	liberarBloques(blocks,cantidadBloques);
	free(blocks);
	free(bitaTripulante);
	config_destroy(bitacoraAux);
	return bitacoraTripulante;
}
char* direccionTripulante(char* id) {
	char* aux = directorioBitacoras();
	char * direccionAux = string_new();
	string_append(&direccionAux, aux);
	string_append(&direccionAux, "/Tripulante");
	string_append(&direccionAux, id);
	string_append(&direccionAux, ".ims");
	free(aux);
	return direccionAux;
}
char* archivoTripulante(char nombre[]) {
	char* aux = directorioBitacoras();
	char * direccionAux = malloc(strlen(aux) + strlen(nombre) + 2);
	strcpy(direccionAux, aux);
	strcat(direccionAux, "/");
	strcat(direccionAux, nombre);
	free(aux);
	return direccionAux;
}
void crearBitacora(char* bitacora) {

	Blocks*infoBloque = iniciarBloque(INIT_BITACORA);
	char * listaBloques = listaAString(infoBloque->blocks);
	FILE * Tripulante = fopen(bitacora, "w+");
	int size = infoBloque->size;
	fprintf(Tripulante, "SIZE=%d\n", size);
	fprintf(Tripulante, "BLOCKS=%s\n", listaBloques);
	fclose(Tripulante);
	list_destroy(infoBloque->blocks);
	free(infoBloque);
	free(listaBloques);

}
void escribirBitacora(int id, char* datoAccion) {
	sem_wait(&mutexBitacora);
	char* numId = string_itoa(id);
	char * bitacoras = direccionTripulante(numId);
	free(numId);
	if (!existeArchivo(bitacoras)) {
//		sem_wait(&mutexBitacora);
		crearBitacora(bitacoras);
//		sem_post(&mutexBitacora);
	}
//	sem_wait(&mutexBitacora);
	actualizarTripulante(bitacoras, datoAccion);

	free(bitacoras);
	sem_post(&mutexBitacora);

}

void actualizarTripulante(char* bitacoras, char* accion) {

	t_config* bitacoraAux = config_create(bitacoras);
	int sizeBita = config_get_int_value(bitacoraAux, "SIZE");
	char** blocks = config_get_array_value(bitacoraAux, "BLOCKS");
	t_list* listaBlocks = list_create(); int i = 0;
	int cantidadBlocks = calculoBloques(sizeBita);
	i = cantidadBlocks - 1; int aux;
	aux = strtol(blocks[i], NULL, 10);
	char*info = buscarBloque(aux);

	if (sizeBita % configSB.tamanio_blocks + strlen(accion)
			<= configSB.tamanio_blocks && !bloquelleno(info)) {
		char* datos = grabarDatosBloque(info, accion);
		insertarBloqueMapeo(datos, aux);
		free(datos);
		free(info);

	} else {
		int longitud = 0;
		//bloque lleno filtrar
		if (bloquelleno(info)) {
			longitud = strlen(accion);
			free(info);
		} else {
			longitud = strlen(accion) - configSB.tamanio_blocks
					+ sizeBita % configSB.tamanio_blocks;
			char* datos = grabarDatosBloque(info, accion);
			insertarBloqueMapeo(datos, aux);
			free(datos);
			free(info);
		}
		while (longitud > 0) {
			char* restoLlenado = string_substring_from(accion,
					strlen(accion) - longitud);
			int nroBloque = buscarPosicionLibre();
			char* numBlock = string_itoa(nroBloque);
			char*restoInfo = buscarBloque(nroBloque);
			char* restoDatos = grabarDatosBloque(restoInfo, restoLlenado);
			insertarBloqueMapeo(restoDatos, nroBloque);
			free(restoInfo);
			free(restoDatos);
			free(restoLlenado);

			list_add(listaBlocks, (void*) nroBloque);

			free(numBlock);
			longitud -= configSB.tamanio_blocks;
		}
	}
	char* bloques = agregarListaBlocks(blocks, cantidadBlocks, listaBlocks);
	config_set_value(bitacoraAux, "BLOCKS", bloques);
	list_destroy(listaBlocks);
	sizeBita += strlen(accion);
	char* nuevoSize = string_itoa(sizeBita);
	config_set_value(bitacoraAux, "SIZE", nuevoSize);
	config_save(bitacoraAux);
	config_destroy(bitacoraAux);
	free(nuevoSize);
	liberarBloques(blocks, cantidadBlocks);
	free(blocks);
	free(bloques);
}

void actualizarBitacora(char* bitacoras, char* accion) {
	t_config* bitacoraAux = config_create(bitacoras);
	int sizeBita = config_get_int_value(bitacoraAux, "SIZE");
	char** blocks = NULL;
	blocks = (char**) realloc(blocks, 10 * sizeof(char*));
	blocks = config_get_array_value(bitacoraAux, "BLOCKS");
	t_list* listaBlocks = malloc(sizeof(t_list));
	listaBlocks = list_create();
	int i = 0;

	int cantidadBlocks = calculoBloques(sizeBita);

	i = calculoBloques(sizeBita) - 1;
	int* aux = malloc(sizeof(int));
	*aux = strtol(blocks[i], NULL, 10);
	char*info = buscarBlock(*aux);

	if (sizeBita % configSB.tamanio_blocks + strlen(accion)
			<= configSB.tamanio_blocks) {
		char* datos = grabarDatosBlock(info, accion);
		insertarBlockMapeo(datos, *aux);
		free(datos);
	} else {
		int longitud = strlen(accion) - configSB.tamanio_blocks
				+ sizeBita % configSB.tamanio_blocks;
		char* datos = grabarDatosBlock(info, accion);
		insertarBlockMapeo(datos, *aux);
		free(datos);
		free(aux);
		free(info);
		while (longitud > 0) {
			char* numBlock = malloc(10);
			char* restoLlenado = malloc(longitud + 1);
			restoLlenado = string_substring_from(accion,
					strlen(accion) - longitud);
			int* aux = malloc(sizeof(int));
			*aux = buscarPosicionLibre();
			numBlock = string_itoa(*aux);
			char*restoInfo = buscarBlock(*aux);
			char* restoDatos = grabarDatosBlock(restoInfo, restoLlenado);
			insertarBlockMapeo(restoDatos, *aux);
			free(restoInfo);
			free(restoDatos);
			free(restoLlenado);

			list_add(listaBlocks, (void*) numBlock);

			free(numBlock);
		}

	}
	char* bloques = agregarListaBlocks(blocks, cantidadBlocks, listaBlocks);
	config_set_value(bitacoraAux, "BLOCKS", bloques);
	free(listaBlocks);
	sizeBita += strlen(accion);
	char* nuevoSize = malloc(10);
	nuevoSize = string_itoa(sizeBita);
	config_set_value(bitacoraAux, "SIZE", nuevoSize);
	config_save(bitacoraAux);
	config_destroy(bitacoraAux);
	free(nuevoSize);

	free(blocks);
	free(aux);
	free(info);
}

