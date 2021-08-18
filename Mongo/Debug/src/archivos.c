/*
 * archivos.c
 *
 *  Created on: 30 abr. 2021
 *      Author: utnso
 */

#include "archivos.h"

int calculoBloques(int size) {
	if (size % configSB.tamanio_blocks == 0) {
		return size / configSB.tamanio_blocks;
	} else {
		return size / configSB.tamanio_blocks + 1;
	}
}
bool bloquelleno(char* bloque) {

	if (bloque[strlen(bloque) - 1] != ' ') {
		return 1;
	} else {
		return 0;
	}

}

Blocks* iniciarBloque(char* escritura) {
	Blocks* info = asignarBloques(strlen(escritura));

	int i = 0;
	int nroBloque;
	char* aux = string_new();
	string_append(&aux, escritura);
//	int tiempoSync = config_get_int_value(configMongo, "TIEMPO_SINCRONIZACION");
	while (i < info->block_count) {
		nroBloque = (int) list_get(info->blocks, i);
		char* bloque = buscarBloque(nroBloque);
		char* datos = grabarDatosBloque(bloque, aux);
		insertarBloqueMapeo(datos, nroBloque);
		i++;
		int resto = strlen(escritura) - configSB.tamanio_blocks * i;
		if (resto > 0) {
			char* sobre = string_substring_from(escritura,
					i * configSB.tamanio_blocks);
			string_append(&aux, sobre);
			free(sobre);
		}
		free(datos);
		free(bloque);

	}
	free(aux);

	return info;
}
Blocks* iniciarBlock(char* registro) {

	Blocks* info = asignarBloques(strlen(registro));
	int i = 0;
	int nroBloque;
//	int tiempoSync = config_get_int_value(configMongo, "TIEMPO_SINCRONIZACION");
	while (i < info->block_count) {

		nroBloque = (int) list_get(info->blocks, i);
		char* bloque = buscarBlock(nroBloque);
		char* datos = grabarDatosBlock(bloque, registro);
		insertarBlockMapeo(datos, nroBloque);
		i++;
		free(datos);
		free(bloque);
	}

	return info;
}

char* grabarDatosBloque(char* bloque, char* grabar) {
	char* aux = malloc(configSB.tamanio_blocks + 1);
	int i, j;
	int len = strlen(bloque);
	for (i = 0, j = 0; i < len && j < strlen(grabar); i++) {
		if (bloque[i] == ' '
				&& (bloque[i + 1] == ' ' || i == strlen(bloque) - 1)) {
			bloque[i] = grabar[j];
			j++;
		}
	}
	strcpy(aux, bloque);
	return aux;
}
char* grabarDatosBlock(char* bloque, char* grabar) {
	char* aux = malloc(65);
	char* bloqueLleno = malloc(65);
	//int i, j;
	int len = strlen(bloque);
//	for (i = len, j = 0; i < configSB.tamanio_blocks && j < strlen(grabar);
//			i++) {
//		bloque[i] = grabar[j];
//		j++;
//	}
	strcpy(aux, bloque);
	int corte = configSB.tamanio_blocks - len - strlen(grabar);
	if (corte >= 0) {
		bloqueLleno = string_substring_from(grabar, 0);
	} else {
		bloqueLleno = string_substring(grabar, 0, strlen(grabar) + corte);
	}
	strcat(aux, bloqueLleno);

	return aux;
}

char* borrarDatosBloque(char* bloque, char* grabar) {
	char* aux = malloc(configSB.tamanio_blocks + 1);
	int i, j;
	int len = strlen(bloque);
	for (i = len - 1, j = 0; i >= 0 && j < strlen(grabar); i--) {
		if (bloque[i] != ' ') {
			bloque[i] = grabar[j];
			j++;
		} else {
			if (bloque[i - 1] != ' ' && j > 0) {
				bloque[i] = grabar[j];
				j++;

			}
		}
	}
	strcpy(aux, bloque);
	return aux;
}
char* borrarDatosBlock(char* bloque, int cantidad) {
	char* aux = malloc(65);
	int i, j;
	int len = strlen(bloque);
	for (i = len - 1, j = 0; i >= 0 && j < cantidad; i--) {
		bloque[i] = '\0';
		j++;
	}
	strcpy(aux, bloque);
	return aux;
}
void insertarBloqueMapeo(char* bloque, int nroBloque) {
	//Insertar semaforo
	sem_wait(&mutexBlocks);
	memcpy(mapeo, datosBloques, configSB.tamanio_total);
	int pos = nroBloque * configSB.tamanio_blocks;
	for (int i = pos, j = 0; i < pos + configSB.tamanio_blocks; i++, j++) {
		mapeo[i] = bloque[j];
	}
	memcpy(datosBloques, mapeo, configSB.tamanio_total);
	msync(datosBloques, filesize, tiempoSync);
	sem_post(&mutexBlocks);
	//lock semaforo
}
void insertarBlockMapeo(char* bloque, int nroBloque) {
	int posBloque = configSB.tamanio_blocks * nroBloque;
	int offset = 0;
	if (offset != posBloque) {
		memcpy(datosBloques + offset, mapeo + offset, posBloque);
		offset += posBloque;
	}
	memcpy(datosBloques + offset, bloque, configSB.tamanio_blocks);
	offset += configSB.tamanio_blocks;
	if (offset < filesize) {
		memcpy(datosBloques + offset, mapeo + offset, filesize - offset);
	}
//	for (int i = posBloque, j = 0; i < posBloque + 64 && j<strlen(bloque); i++, j++) {
//		mapeo[i] = bloque[j];
//	}
//	memcpy(datosBloques, mapeo, filesize);
	memcpy(datosBloques + posBloque, bloque, 64);
	msync(datosBloques, filesize, MS_SYNC);
}
char* buscarBloque(int nroBloque) {
	char* aux = string_substring(mapeo, nroBloque * configSB.tamanio_blocks,
			configSB.tamanio_blocks);
	//string_append(&aux, "\0");
	return aux;
}
char* buscarBlock(int nroBloque) {
	int offset = configSB.tamanio_blocks * nroBloque;
	char* aux = malloc(65);
	memcpy(aux, datosBloques + offset, configSB.tamanio_blocks);
	return aux;
}

char* eliminarListaBlocks(char** bloques, int cantBloques, int eliminados) {
	char* blocks = malloc(20);
	strcpy(blocks, "[");
	int i;
	int max = cantBloques - eliminados;
	for (i = 0; i < max; i++) {
		strcat(blocks, bloques[i]);
		if (i + 1 < max) {
			strcat(blocks, ",");
		}
	}
	strcat(blocks, "]");
	blocks[strlen(blocks)] = '\0';
	return blocks;

}
char* agregarListaBlocks(char** bloques, int cantBloques, t_list * lista) {

	char* blocks = string_new();
	int sizeList = list_size(lista);
	string_append(&blocks, "[");
	for (int i = 0; i < cantBloques; i++) {
		string_append(&blocks, bloques[i]);
		if (i + 1 < cantBloques) {
			string_append(&blocks, ",");
		}
	}
	for (int i = 0; i < sizeList; i++) {
		string_append(&blocks, ",");
		int nro = (int) list_get(lista, i);
		char* nroBloque = string_itoa(nro);
		string_append(&blocks, nroBloque);

		free(nroBloque);
	}
	string_append(&blocks, "]");
	blocks[strlen(blocks)] = '\0';
	return blocks;

}

Blocks* asignarBloques(int tamanioEscritura) {
	Blocks * aux = malloc(sizeof(Blocks));
	t_list* listaBlock = list_create();
	char* bloques = string_new();
	aux->block_count = (uint32_t) calculoBloques(tamanioEscritura);
	int i = 0;
	int nroBloque = 0;
	string_append(&bloques, "[");
	while (i < aux->block_count) {
		nroBloque = buscarPosicionLibre();
		char* numBlock = string_itoa(nroBloque);
		list_add(listaBlock, (void*) nroBloque);
		string_append(&bloques, numBlock);
		if (i + 1 < aux->block_count) {
			string_append(&bloques, ",");
		}
		i++;
		free(numBlock);
	}
	string_append(&bloques, "]");
	bloques[strlen(bloques)] = '\0';
	aux->size = tamanioEscritura;
	aux->blocks = listaBlock;
	free(bloques);
	return aux;
}

int buscarPosicionLibre() {
	//Insertar semaforo
	sem_wait(&mutexBitmap);
	int i;
	for (i = 0;
			bitarray_test_bit(datosSuperbloque->bitmap, i)
					&& i < (datosSuperbloque->bitmap->size) * 8; i++) {
	}
	if (i >= (datosSuperbloque->bitmap->size) * 8) {
		return -1;
	}
	bitarray_set_bit(datosSuperbloque->bitmap, i);
	int offset = 0;
	memcpy(superbloque + offset, &datosSuperbloque->blockSize,
			sizeof(uint32_t));
	offset += sizeof(uint32_t);
	memcpy(superbloque + offset, &datosSuperbloque->blocks, sizeof(uint32_t));
	offset += sizeof(uint32_t);
	memcpy(superbloque + offset, bitarrayGlobal, configSB.cantidad_blocks / 8);
	offset += strlen(bitarrayGlobal);
	msync(superbloque, offset, tiempoSync);
	sem_post(&mutexBitmap);
	//lock semaforo
	return i;
}
void liberarPosicion(int pos) {
	bitarray_clean_bit(datosSuperbloque->bitmap, pos);

	int offset = 0;
	memcpy(superbloque + offset, &datosSuperbloque->blockSize,
			sizeof(uint32_t));
	offset += sizeof(uint32_t);
	memcpy(superbloque + offset, &datosSuperbloque->blocks, sizeof(uint32_t));
	offset += sizeof(uint32_t);
	memcpy(superbloque + offset, bitarrayGlobal, configSB.cantidad_blocks / 8);
	offset += strlen(bitarrayGlobal);
	msync(superbloque, offset, MS_SYNC);
}

char* listaAString(t_list* datos) {
	int sizeList = list_size(datos);
//	char* lista = malloc(2 * sizeList + 2);
	char* lista = string_new();
//	strcpy(lista, "[");
	string_append(&lista, "[");
	for (int i = 0; i < sizeList; i++) {
		char* aux = string_itoa((int) list_get(datos, i));
//		strcat(lista, aux);
		string_append(&lista, aux);
		if (i + 1 < sizeList) {
			string_append(&lista, ",");
//			strcat(lista, ",");
		}
		free(aux);
	}
//	strcat(lista, "]");
	string_append(&lista, "]");
//	lista[strlen(lista)] = '\0';
	return lista;
}

//t_list* cargarLista(char *datos) {
//	char**valores = NULL;
//	int valor;
//	valores = (char**) realloc(valores, 10 * sizeof(char*));
//	valores = string_get_string_as_array(datos);
//	t_list *aux = list_create();
//	int i = 0;
//	while (valores[i] != NULL) {
//		valor =strtol(valores[i], NULL, 10);
//		list_add(aux,valor);
//		i++;
//	}
//	free(valores);
//	return aux;
//}
