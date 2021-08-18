/*
 * store.c
 *  Created on: 20 abr. 2021
 *      Author: utnso
 */

#include "polus.h"

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>
#include <sys/types.h>
#include <commons/string.h>
#include <sys/mman.h>

void iniciarPolus() {

	if (!existeDirectorio(ptomje))
		mkdir(ptomje, 0700);

	abrirDirectorioFiles();
	abrirDirectorioSuperBloque();
	abrirDirectorioBlocks();

}
bool existeDirectorio(char* path) {
	struct stat buf;
	return stat(path, &buf) != -1;
}

void abrirDirectorioFiles() {

	char* aux = directorio("Files");
	char* bita = directorioBitacoras();
	if (!existeDirectorio(aux)) {
		log_info(logMongo, "Creando directorio Files");
		mkdir(aux, 0700);
		mkdir(bita, 0700);
		log_info(logMongo, "Creando directorio Bitacoras");
	} else {
		log_info(logMongo, "Configurando directorio Files");
		DIR* dir = opendir(aux);
		struct dirent* entry;

		if (dir == NULL) {
			log_error(logMongo,
					"No se pudo abrir correctamente el directorio files");
		} else {
			while ((entry = readdir(dir))) {
				if (!sonIguales(entry->d_name, ".")
						&& !sonIguales(entry->d_name, "..")) {
					log_info(logMongo, "Leyendo directorio %s", entry->d_name);
				}
			}

			closedir(dir);
		}
	}
	free(aux);
	free(bita);
}

bool sonIguales(char* a, char* b) {
	return string_equals_ignore_case(a, b);
}

char* directorioFiles() {
	return directorio("Files");
}

void abrirDirectorioSuperBloque() {

	char* aux = directorio("Superbloque.ims");
	configSB.cantidad_blocks = (uint32_t) config_get_int_value(configMongo,
			"BLOCKS");
	configSB.tamanio_blocks = (uint32_t) config_get_int_value(configMongo,
			"BLOCK_SIZE");
	configSB.tamanio_total = configSB.cantidad_blocks * configSB.tamanio_blocks;
	tiempoSync = config_get_int_value(configMongo, "TIEMPO_SINCRONIZACION");
	filesize = configSB.tamanio_total;
	tamanioSB = sizeof(uint32_t) * 2 + configSB.cantidad_blocks / 8;
	datosSuperbloque = malloc(sizeof(Superbloque));
	if (!existeArchivo(aux)) {
		log_info(logMongo, "Creando archivo SuperBloque.ims");

		crearSuperBloqueBitmap(aux);

	} else {
		log_info(logMongo, "Configurando archivo SuperBloque.ims");
		fd = open(aux, O_RDWR, 0700);
		bitarrayGlobal = calloc(configSB.cantidad_blocks / 8, sizeof(char));
		datosSuperbloque->bitmap = bitarray_create_with_mode(bitarrayGlobal,
				configSB.cantidad_blocks / 8, LSB_FIRST);
		superbloque = mmap(NULL, tamanioSB,
		PROT_READ | PROT_WRITE | PROT_EXEC,
		MAP_SHARED, fd, 0);
		int offset = 0;

		memcpy(&datosSuperbloque->blockSize, superbloque + offset,
				sizeof(uint32_t));
		offset += sizeof(uint32_t);
		memcpy(&datosSuperbloque->blocks, superbloque + offset,
				sizeof(uint32_t));
		offset += sizeof(uint32_t);
		memcpy(bitarrayGlobal, superbloque + offset,
				configSB.cantidad_blocks / 8);
	}
	free(aux);
}
char* directorio(char* direccion) {
	char* aux = string_new();
	string_append(&aux, ptomje);
	string_append(&aux, "/");
	string_append(&aux, direccion);
	return aux;
}
short existeArchivo(char *path) {
	int fd = open(path, O_RDONLY);
	if (fd < 0)
		return 0;
	close(fd);
	return 1;
}
void crearSuperBloqueBitmap(char* path) {

	log_info(logMongo, "Creando bitmap");

	fd = open(path, O_RDWR | O_CREAT, 0700);

	bitarrayGlobal = calloc(configSB.cantidad_blocks / 8, sizeof(char));
	datosSuperbloque->bitmap = bitarray_create_with_mode(bitarrayGlobal,
			configSB.cantidad_blocks / 8, LSB_FIRST);
	datosSuperbloque->blockSize = config_get_int_value(configMongo,
			"BLOCK_SIZE");
	datosSuperbloque->blocks = config_get_int_value(configMongo, "BLOCKS");
	ftruncate(fd, tamanioSB);
	superbloque = mmap(NULL, tamanioSB,
	PROT_READ | PROT_WRITE | PROT_EXEC,
	MAP_SHARED, fd, 0);
	int offset = 0;
	memcpy(superbloque + offset, &datosSuperbloque->blockSize,
			sizeof(uint32_t));
	offset += sizeof(uint32_t);
	memcpy(superbloque + offset, &datosSuperbloque->blocks, sizeof(uint32_t));
	offset += sizeof(uint32_t);
	memcpy(superbloque + offset, bitarrayGlobal, configSB.cantidad_blocks / 8);
	msync(superbloque, tamanioSB, MS_SYNC);
}
void abrirDirectorioBlocks() {
	char* direccion = directorio("Blocks.ims");
	if (!existeArchivo(direccion)) {
		log_info(logMongo, "Creando archivo blocks.ims");
		fd = open(direccion, O_RDWR | O_CREAT, 0700);
		ftruncate(fd, filesize);
		datosBloques = mmap(NULL, filesize,
		PROT_READ | PROT_WRITE | PROT_EXEC,
		MAP_SHARED, fd, 0);
		memset(datosBloques, ' ', filesize);
		msync(datosBloques, filesize, MS_SYNC);
		mapeo = malloc(filesize);
		memcpy(mapeo, datosBloques, filesize);
		//mapeo[filesize] = '\0';
	} else {
		log_info(logMongo, "Configurando archivo blocks.ims");
		fd = open(direccion, O_RDWR);
		datosBloques = mmap(NULL, filesize,
		PROT_READ | PROT_WRITE | PROT_EXEC,
		MAP_SHARED, fd, 0);
		mapeo = malloc(filesize);
		memcpy(mapeo, datosBloques, filesize);
		//mapeo[filesize] = '\0';
	}
	free(direccion);
}

char* directorioBitacoras() {
	char* aux = directorioFiles();
	char * direccionAux = string_new();
	string_append(&direccionAux, aux);
	string_append(&direccionAux, "/");
	string_append(&direccionAux, "Bitacoras");
	free(aux);
	return direccionAux;
}
char* direccionTarea(char* nombre) {
	char* aux = directorioFiles();
	char * direccionAux = string_new();
	string_append(&direccionAux, aux);
	string_append(&direccionAux, "/");
	string_append(&direccionAux, nombre);
	string_append(&direccionAux, ".ims");
	free(aux);
	return direccionAux;
}
char* direccionTareaMD5(char* nombre) {
	char* aux = directorioFiles();
	char * direccionAux = string_new();
	string_append(&direccionAux, aux);
	string_append(&direccionAux, "/");
	string_append(&direccionAux, nombre);
	string_append(&direccionAux, ".ims.MD5");
	free(aux);
	return direccionAux;
}
char* archivoTarea(char* nombre) {
	char* aux = directorioFiles();
	char * direccionAux = malloc(strlen(aux) + strlen(nombre) + 2);
	strcpy(direccionAux, aux);
	strcat(direccionAux, "/");
	strcat(direccionAux, nombre);
	free(aux);
	return direccionAux;
}
char* obtenerPosicionSabotaje(int nro){
	char** posiciones = config_get_array_value(configMongo,"POSICIONES_SABOTAJES");
	char* posicion = string_new();
	string_append(&posicion,posiciones[nro]);
	int cantidad = cantidadBloquesFalsos(posiciones);
	liberarBloques(posiciones,cantidad);
	free(posiciones);
	return posicion;
}

void iniciarMapeo() {
	char* direccion = directorio("Blocks.ims");// direccion del archivo a mapear
	fd = open(direccion, O_RDWR | O_CREAT, 0700); // abriendo el archivo
	ftruncate(fd, filesize); // darle tamaño al archivo(filesize el tamaño del archivo)
	datosBloques = mmap(NULL, filesize,
	PROT_READ | PROT_WRITE | PROT_EXEC,
	MAP_SHARED, fd, 0);	//Siempre NULL el 1er parametro
	memset(datosBloques, ' ', filesize); // Esto llena el archivo con x caracteres(no se si te sirva)
	msync(datosBloques, filesize, MS_SYNC); //Esto sincroniza los datos actualizados a causa del memset
	mapeo = malloc(filesize); // En mi caso cree otra variable para hacer los cambios (del mismo tipo)
	memcpy(mapeo, datosBloques, filesize); // Con esto copio los datos de mi mapeo original a la variable a la
	// cual le hare cambios


	//Aparte
	/* con esto copiamos los datos a una variable que podemos manejar a nuestro antojo */
	memcpy(mapeo, datosBloques, strlen(datosBloques) + 1);

	/* con esto hacemos los cambios de datos nuevos al archivo mapeado*/
	memcpy(datosBloques, mapeo, strlen(mapeo) + 1);
	/* con esto hacemos actualizamos los cambios de datos nuevos */
	int tiempoSync = config_get_int_value(configMongo, "TIEMPO_SINCRONIZACION");
	msync(datosBloques, filesize, tiempoSync);

	/* Desmapeamos el archivo de la memoria */
	int rc = munmap(datosBloques, filesize);
	/* Cerramos el archivo */
	close(fd);
}
