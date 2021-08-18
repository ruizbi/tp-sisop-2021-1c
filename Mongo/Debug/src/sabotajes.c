/*
 * sabotaje.c
 *
 *  Created on: 11 jun. 2021
 *      Author: utnso
 */

#include "sabotajes.h"
void repararMongo() {
	char* direccionOxigeno = direccionTarea("Oxigeno");
	char* direccionComida = direccionTarea("Comida");
	char* direccionBasura = direccionTarea("Basura");
	bool seReparo = false;
	seReparo = repararCantidadBloques();

	if (existeArchivo(direccionOxigeno) && !seReparo) {
		seReparo = repararSabotajeFiles("Oxigeno");
	}
	if (existeArchivo(direccionComida) && !seReparo) {
		seReparo = repararSabotajeFiles("Comida");
	}
	if (existeArchivo(direccionBasura) && !seReparo) {
		seReparo = repararSabotajeFiles("Basura");
	}
	if (!seReparo) {
		repararBitmap(); // si se sabotea size blocks y block count falla repararBitmap
	}
	free(direccionOxigeno);
	free(direccionComida);
	free(direccionBasura);

}
bool repararSabotajeFiles(char* nombre) {

	bool seReparo = false;

	if(strcmp(nombre, "Oxigeno") == 0) {
		sem_wait(&mutex_oxigeno);
	}
	else if(strcmp(nombre, "Basura") == 0) {
		sem_wait(&mutex_basura);
	}
	else if(strcmp(nombre, "Comida") == 0) {
		sem_wait(&mutex_comida);
	}

	if (!seReparo) {
		seReparo = repararBlocks(nombre); // se hace repararSize dentro
		//si se sabotea blocks y *block count falla repararSize
	}
	if (!seReparo) {
		seReparo = repararBlockCount(nombre);
	}

	if(strcmp(nombre, "Oxigeno") == 0) {
		sem_post(&mutex_oxigeno);
	}
	else if(strcmp(nombre, "Basura") == 0) {
		sem_post(&mutex_basura);
	}
	else if(strcmp(nombre, "Comida") == 0) {
		sem_post(&mutex_comida);
	}

	return seReparo;
}
void sabotearCantidadBloques(uint32_t cambio) {
	int offset = 0;
	memcpy(superbloque + offset, &datosSuperbloque->blockSize,
			sizeof(uint32_t));
	offset += sizeof(uint32_t);
	memcpy(superbloque + offset, &cambio, sizeof(uint32_t));
	offset += sizeof(uint32_t);
	memcpy(superbloque + offset, bitarrayGlobal, configSB.cantidad_blocks / 8);
	offset += strlen(bitarrayGlobal);
	msync(superbloque, offset, MS_SYNC);
}
//Esta ok
bool repararCantidadBloques() {
	bool seReparo = false;
	struct stat bk;
	char* direccion = directorio("Blocks.ims");
	stat(direccion, &bk);
	uint32_t cantidadBloquesReal = bk.st_size / datosSuperbloque->blockSize;
	free(direccion);
	uint32_t cambio;
	int offset = 0;
	memcpy(&cambio, superbloque + 4, sizeof(uint32_t));
	char* nro2 = string_itoa(cambio);
	log_info(logMongo, nro2);

	char* nro = string_itoa(cantidadBloquesReal);
	log_info(logMongo, nro);

	if (strcasecmp(nro2, nro) != 0) {
		seReparo = true;
	}
	free(nro);
	free(nro2);
	memcpy(superbloque + offset, &datosSuperbloque->blockSize,
			sizeof(uint32_t));
	offset += sizeof(uint32_t);
	memcpy(superbloque + offset, &cantidadBloquesReal, sizeof(uint32_t));
	offset += sizeof(uint32_t);
	memcpy(superbloque + offset, bitarrayGlobal, configSB.cantidad_blocks / 8);
	offset += strlen(bitarrayGlobal);
	msync(superbloque, offset, MS_SYNC);
	return seReparo;
}

void mostrarBits() {
	for (int i = 0; i < configSB.cantidad_blocks; i++) {
		if (bitarray_test_bit(datosSuperbloque->bitmap, i)) {
			char* bit = string_new();
			string_append_with_format(&bit, "%d", i);
			log_info(logMongo, bit);
			free(bit);
		}
	}
}
//Esta ok , hay error en valgrind en lista
void repararBitmap() {
	//tomar de cada archivo sus bloques y comparar con bitarray_test_bit() con un for recorrer el bitmap;
	//setear el valor/limpiar el bit segun si contradice a los bloques obtenidos con bitarray_set_bit()/bitarray_clean_bit()
	//crear una funcion que almacene los bloques obtenidos de las bitacoras y tareas X
	//crear una funcion que recolecte los bloques de todas las bitacoras X
	//crear una funcion que recolecte los bloques de todas las tareas X
	t_list * bloques = recolectarBloquesBitacoras();
	bloques = recolectarBloquesTareas(bloques);

	bool ordenMenorMayor(t_list* bloque1, t_list* bloque2) {
		return bloque1 < bloque2;
	}
	list_sort(bloques, (void*) ordenMenorMayor);

	int j = 0;
	for (int i = 0; i < configSB.cantidad_blocks; i++) {
		int x = (int) list_get(bloques, j);
		if (!bitarray_test_bit(datosSuperbloque->bitmap, i) && i == x) {
			bitarray_set_bit(datosSuperbloque->bitmap, i);
			j++;
		} else {
			if (i != x) {
				bitarray_clean_bit(datosSuperbloque->bitmap, i);
			} else {
				j++;
			}
		}
	}

	list_destroy(bloques);

}

t_list * recolectarBloquesBitacoras() {
	char* dirBitacoras = directorioBitacoras();

	t_list* listaBloques = list_create();
	DIR* dir = opendir(dirBitacoras);
	struct dirent* entry;
	int size, cantidad;
	while ((entry = readdir(dir))) {
		if (!sonIguales(entry->d_name, ".")
				&& !sonIguales(entry->d_name, "..")) {
			char* bitacora = archivoTripulante(entry->d_name);
			t_config * tripulante = config_create(bitacora);
//			char** bloques = NULL;

			size = config_get_int_value(tripulante, "SIZE");
			cantidad = calculoBloques(size);
//			bloques = (char**) realloc(bloques, cantidad * sizeof(char*));
			char** bloques = config_get_array_value(tripulante, "BLOCKS");
			for (int i = 0; i < cantidad; i++) {
				int nro = strtol(bloques[i], NULL, 10);
				list_add(listaBloques, (void*) nro);
			}
			liberarBloques(bloques, cantidad);
			free(bitacora);
			free(bloques);
			config_destroy(tripulante);
		}
	}
	closedir(dir);
	free(dirBitacoras);
	return listaBloques;
}
t_list * recolectarBloquesTareas(t_list * listaBloques) {
	char* dirFiles = directorioFiles();
	DIR* dir = opendir(dirFiles);
	struct dirent* entry;
	int cantidad;
	while ((entry = readdir(dir))) {
		if (!sonIguales(entry->d_name, ".") && !sonIguales(entry->d_name, "..")
				&& !sonIguales(entry->d_name, "Bitacoras")) {
			char* tarea = archivoTarea(entry->d_name);
			t_config * configTarea = config_create(tarea);
			char** bloques = config_get_array_value(configTarea, "BLOCKS");
			cantidad = config_get_int_value(configTarea, "BLOCK_COUNT");

			for (int i = 0; i < cantidad; i++) {
				int nro = strtol(bloques[i], NULL, 10);
				list_add(listaBloques, (void*) nro);
			}
			free(tarea);
			liberarBloques(bloques, cantidad);
			free(bloques);
			config_destroy(configTarea);
		}
	}
	closedir(dir);
	free(dirFiles);
	return listaBloques;
}
//Esta ok
bool repararSizeTarea(char* nombre) {
	bool seReparo = false;
	char* tarea = direccionTarea(nombre);
	t_config * configTarea = config_create(tarea);
	char** bloques = config_get_array_value(configTarea, "BLOCKS");
	int cantidad = cantidadBloquesFalsos(bloques);
	int size = config_get_int_value(configTarea, "SIZE");
	int nuevoSize = (cantidad - 1) * configSB.tamanio_blocks;
	int nro = strtol(bloques[cantidad - 1], NULL, 10);
	char*info = buscarBloque(nro);
	nuevoSize += contarCaracteres(info, nombre[0]);
	char*sizeNuevo = string_itoa(nuevoSize);
	if (nuevoSize != size) {
		seReparo = true;
	}
	config_set_value(configTarea, "SIZE", sizeNuevo);
	//tomar los bloques del archivo(solo tareas? o tambien bitacoras) y recorrer su contenido sumando la cantidad escrita
	config_save(configTarea);
	config_destroy(configTarea);
	free(tarea);
	liberarBloques(bloques, cantidad);
	free(bloques);
	free(info);
	free(sizeNuevo);
	return seReparo;
}

int contarCaracteres(char*info, char caracter) {
	int cantidad = 0;
	for (int i = 0; i < configSB.tamanio_blocks; i++) {
		if (info[i] == caracter) {
			cantidad++;
		}
	}
	return cantidad;
}
//Esta ok
bool repararBlockCount(char* nombre) {
	bool seReparo = false;
//tomar los bloques del archivo de la tarea, contarlos y arreglar el block count
	char* tarea = direccionTarea(nombre);
	t_config * configTarea = config_create(tarea);
	int size = config_get_int_value(configTarea, "SIZE");
	int cantidad = config_get_int_value(configTarea, "BLOCK_COUNT");
	int cantidadReal = calculoBloques(size);
	if (cantidad != cantidadReal) {
		seReparo = true;
	}
	char*cantidadBloques = string_itoa(cantidadReal);
	config_set_value(configTarea, "BLOCK_COUNT", cantidadBloques);
	config_save(configTarea);
	config_destroy(configTarea);
	free(tarea);
	free(cantidadBloques);
	return seReparo;
}
int cantidadBloquesFalsos(char** bloques) {
	int i;
	for (i = 0; bloques[i] != NULL; i++) {
	}
	return i;
}
bool repararBlocks(char*nombre) {
	bool seReparo = false;
	char* tarea = direccionTarea(nombre);
	char* hashActual = obtenerHash(tarea);
	char * hash = generarHash(tarea);
	if (strcasecmp(hashActual, hash) != 0) {
		restaurarArchivo(tarea, nombre);
		free(tarea);
		free(hash);
		free(hashActual);
		seReparo = true;
		return seReparo;
	} else {
		seReparo = repararSizeTarea(nombre);
		free(tarea);
		free(hash);
		free(hashActual);
		return seReparo;
	}

}

void restaurarArchivo(char* tarea, char* nombre) {
//tomar el size del archivo tarea e ir controlando/llenando los bloques ya que pueden agregar,cambiar valores
//Ver si algún bloque se pasa de la cantidad máxima (por ejemplo les ponemos el bloque 1024 y en el FS solo tienen 512 bloques)
//Validar que los bloques estén ocupados en el bitmap (ya que capaz les seteamos un bloque vacío)
//Ver si el orden de los bloques esta bien y para eso tienen que validar con el MD5 para ver que el orden de los bloque este ok (posiblemente solo falle si cambian el último bloque)
	t_config * configTarea = config_create(tarea);
	int size = config_get_int_value(configTarea, "SIZE");
	char** bloques = config_get_array_value(configTarea, "BLOCKS");
	int cantidadFalse = cantidadBloquesFalsos(bloques);

	t_list * listaBloques = controlBloquesExitentes(bloques, cantidadFalse,
			nombre[0], size);
	bool ordenMayorMenor(t_list* bloque1, t_list* bloque2) {
		char* info = buscarBloque((int) bloque1);
		char* info2 = buscarBloque((int) bloque2);
		int cantidad1 = contarCaracteres(info, nombre[0]);
		int cantidad2 = contarCaracteres(info2, nombre[0]);
		free(info);
		free(info2);
		return cantidad1 >= cantidad2;
	}
	list_sort(listaBloques, (void*) ordenMayorMenor);
	//
	liberarBloques(bloques, cantidadFalse);
	free(bloques);
	char * blocks = listaAString(listaBloques);
	config_set_value(configTarea, "BLOCKS", blocks);
	config_save(configTarea);

	free(blocks);
	//free(listaBloques);
	list_destroy(listaBloques);
	config_destroy(configTarea);

	//[1,3,4] ->[3,1,4] o [1,3,4,2012] o [1,3,4,40] o [1,3] no contemplar posicion de
	//bloques de llenado iguales tipo [1,3,4] ->[3,1,4]

	//	char** bloques = NULL;
	//	bloques = (char**) realloc(bloques, 10 * sizeof(char*));
	//	bloques = config_get_array_value(configTarea, "BLOCKS");
	//liberar lo de adentro
	//free(bloques);
	//falta una funcion que controle si faltan bloques
	//funcion que ordene los bloques a su posicion original
}
t_list* controlBloquesExitentes(char**bloques, int cantidad, char caracter,
		int size) {
	t_list * bloquesReparados = list_create();
	int nroBloque;
	int tamanio = 0;
	//Control de bloques existentes(verifica que existan y contengan el caracter)
	for (int i = 0; i < cantidad; i++) {
		nroBloque = strtol(bloques[i], NULL, 10);
		char* info = buscarBloque(nroBloque);
		if (nroBloque < configSB.cantidad_blocks && info[0] == caracter) {
			list_add(bloquesReparados, (void*) nroBloque);
			tamanio += contarCaracteres(info, caracter);
		}
		free(info);
	}
	//Control de bloques perdidos (verifica en bitmap bloques que no estan en el archivo)
	if (tamanio != size) {
		for (int i = 0; i < configSB.cantidad_blocks; i++) {
			if (bitarray_test_bit(datosSuperbloque->bitmap, i)) {
				char* info = buscarBloque(i);
				if ((info[0] == caracter) && (info[1] == caracter || info[1] == ' ')) {
					bool estaAsignado(void* element) {
						return ((int) element == i);
					}
					if (!list_any_satisfy(bloquesReparados, estaAsignado)) {
						list_add(bloquesReparados, (void*) i);
					}
				}
				free(info);
			}
		}
	}

	return bloquesReparados;
}
