/*
 * tareas.c
 *
 *  Created on: 8 jun. 2021
 *      Author: utnso
 */

#include "tareas.h"

void iniciarTarea(char* nombreTarea, int cantidad) {

	if (!strcasecmp(nombreTarea, "GENERAR_OXIGENO")) {
		generarOxigeno(cantidad);
	}
	if (!strcasecmp(nombreTarea, "CONSUMIR_OXIGENO")) {
		consumirOxigeno(cantidad);
	}
	if (!strcasecmp(nombreTarea, "GENERAR_COMIDA")) {
		generarComida(cantidad);
	}
	if (!strcasecmp(nombreTarea, "CONSUMIR_COMIDA")) {
		consumirComida(cantidad);
	}
	if (!strcasecmp(nombreTarea, "GENERAR_BASURA")) {
		generarBasura(cantidad);
	}
	if (!strcasecmp(nombreTarea, "DESCARTAR_BASURA")) {
		descargarBasura(cantidad);
	}

}
void generarOxigeno(int cantidad) {
	verificarTarea("Oxigeno", cantidad);
}
void generarComida(int cantidad) {
	verificarTarea("Comida", cantidad);
}

void consumirOxigeno(int cantidad) {
	consumirTarea("Oxigeno", cantidad);
}

void consumirComida(int cantidad) {
	consumirTarea("Comida", cantidad);
}
void descargarBasura(int cantidad) {
	int tamanio = tamanioTarea("Basura");
	consumirTarea("Basura", cantidad + tamanio);
}
int tamanioTarea(char* tarea) {
	int tamanio = 0;
	char* direccion = direccionTarea(tarea);
	if (!existeArchivo(direccion)) {
		free(direccion);
		return tamanio;
	} else {
		t_config * infoTarea = config_create(direccion);
		tamanio = config_get_int_value(infoTarea, "SIZE");
		config_destroy(infoTarea);
		free(direccion);
		return tamanio;
	}

}
void generarBasura(int cantidad) {
	verificarTarea("Basura", cantidad);
}
void crearArchivoTarea(char* tarea, char * llenadoCaracteres) {

	Blocks*infoBloque = iniciarBloque(llenadoCaracteres);
	char * lista = listaAString(infoBloque->blocks);
	FILE* archivoTarea = fopen(tarea, "w+");
	int size = (int) infoBloque->size;
	int cantidadBloques = (int) infoBloque->block_count;
	int md5 = 0;
	fprintf(archivoTarea, "SIZE=%d\n", size);
	fprintf(archivoTarea, "BLOCK_COUNT=%d\n", cantidadBloques);
	fprintf(archivoTarea, "BLOCKS=%s\n", lista);
	fprintf(archivoTarea, "CARACTER_LLENADO=%c\n", llenadoCaracteres[0]);
	fprintf(archivoTarea, "MD5=%d", md5);

	list_destroy(infoBloque->blocks);
	free(infoBloque);
	free(lista);
	fclose(archivoTarea);

}

char* generarHash(char* direccion) {
	t_config* configTarea = config_create(direccion);
	char** bloques = config_get_array_value(configTarea, "BLOCKS");
	char* stringMD5 = string_new();
	if (bloques[0] == NULL) {
		string_append(&stringMD5, "0");
		free(bloques);
		config_destroy(configTarea);
		return stringMD5;
	}
	int cantidad = cantidadBloquesFalsos(bloques);

	for (int i = 0; i < cantidad; i++) {
		int nro = strtol(bloques[i], NULL, 10);
		char* block = buscarBloque(nro);
		string_append(&stringMD5, block);
		free(block);
	}
	MD5_CTX m;
	unsigned char dig[16];
	char* out = malloc(33);
	MD5_Init(&m);
	MD5_Update(&m, stringMD5, strlen(stringMD5));
	MD5_Final(dig, &m);

	for (int i = 0; i < 16; ++i) {
		snprintf(&(out[i * 2]), 16 * 2, "%02x", (unsigned int) dig[i]);
	}

	liberarBloques(bloques, cantidad);
	free(bloques);
	free(stringMD5);
	config_destroy(configTarea);
	return out;
}
char* obtenerHash(char* direccion) {
	t_config* configTarea = config_create(direccion);
	char* hash = config_get_string_value(configTarea, "MD5");
	char* out = string_new();
	string_append(&out, hash);
	config_destroy(configTarea);
	return out;
}
void insertarHash(char* direccion, char* out) {
	t_config* configTarea = config_create(direccion);
	char* hash = config_get_string_value(configTarea, "MD5");
	if (strcasecmp(hash, out)) {
		config_set_value(configTarea, "MD5", out);
		config_save(configTarea);
	}
	config_destroy(configTarea);
}

void verificarTarea(char* tarea, int cantidad) {

	char* direccion = direccionTarea(tarea);
	char* llenadoCaracteres = string_repeat(tarea[0], cantidad);

	if(strcmp(tarea, "Oxigeno") == 0) {
		sem_wait(&mutex_oxigeno);
	}
	else if(strcmp(tarea, "Basura") == 0) {
		sem_wait(&mutex_basura);
	}
	else if(strcmp(tarea, "Comida") == 0) {
		sem_wait(&mutex_comida);
	}

	if (!existeArchivo(direccion)) {
		crearArchivoTarea(direccion, llenadoCaracteres);
	} else {
		aumentarCantidad(direccion, llenadoCaracteres);  // nos quedamos aca
	}



	char* hash = generarHash(direccion);
	insertarHash(direccion, hash);
	free(hash);
	free(direccion);
	free(llenadoCaracteres);
	msync(datosBloques, filesize, tiempoSync);

	if(strcmp(tarea, "Oxigeno") == 0) {
		sem_post(&mutex_oxigeno);
	}
	else if(strcmp(tarea, "Basura") == 0) {
		sem_post(&mutex_basura);
	}
	else if(strcmp(tarea, "Comida") == 0) {
		sem_post(&mutex_comida);
	}

}
void liberarBloques(char** bloques, int cantidad) {
	for (int i = cantidad - 1; i >= 0; i--) {
		free(bloques[i]);
	}
}

void consumirTarea(char* tarea, int cantidad) {
	char* direccion = direccionTarea(tarea);
	char* consumoCaracteres = string_repeat(' ', cantidad);

	if(strcmp(tarea, "Oxigeno") == 0) {
		sem_wait(&mutex_oxigeno);
	}
	else if(strcmp(tarea, "Basura") == 0) {
		sem_wait(&mutex_basura);
	}
	else if(strcmp(tarea, "Comida") == 0) {
		sem_wait(&mutex_comida);
	}

	if (!existeArchivo(direccion)) {
		char* tareaInfo = string_new();
		string_append(&tareaInfo, "No existe el archivo ");
		string_append(&tareaInfo, tarea);
		string_append(&tareaInfo, ".ims");
		log_info(logMongo, tareaInfo);
		free(tareaInfo);

	} else {
		if (tamanioTarea(tarea) != 0) {
			sem_wait(&mutexTareas);
			disminuirCantidad(direccion, consumoCaracteres);
			if (tarea[0] == 'B') {
				remove(direccion);
			} else {
				char* hash = generarHash(direccion);
				insertarHash(direccion, hash);
				free(hash);
			}
			msync(datosBloques, filesize, tiempoSync);
			sem_post(&mutexTareas);
		} else {
			if (tarea[0] != 'B') {
				log_info(logMongo,
						"Se quisieron eliminar mas caracteres de los existentes");

			}
		}

	}

	free(direccion);
	free(consumoCaracteres);


	if(strcmp(tarea, "Oxigeno") == 0) {
		sem_post(&mutex_oxigeno);
	}
	else if(strcmp(tarea, "Basura") == 0) {
		sem_post(&mutex_basura);
	}
	else if(strcmp(tarea, "Comida") == 0) {
		sem_post(&mutex_comida);
	}

}
void disminuirCantidad(char * direccionTarea, char* consumido) {
	//verificar esta funcion
	t_config* configTarea = config_create(direccionTarea);
	int size = config_get_int_value(configTarea, "SIZE");
	char** blocks = config_get_array_value(configTarea, "BLOCKS");
	int tamanio = strlen(consumido);
	int cantidadBlocks = config_get_int_value(configTarea, "BLOCK_COUNT");
	char* caracter = config_get_string_value(configTarea, "CARACTER_LLENADO");
	int aux;
	aux = strtol(blocks[cantidadBlocks - 1], NULL, 10);
	char*info = buscarBloque(aux);
	int cantidadEliminada = 0;
	//Error en resto 0 y tamanio de borrado 64/128 etc
	if (size % (int) configSB.tamanio_blocks - tamanio > 0) {
		char* datos = borrarDatosBloque(info, consumido);
		insertarBloqueMapeo(datos, aux);
		liberarBloques(blocks, cantidadBlocks);
		free(blocks);
		free(datos);
		free(info);
	} else {

		int longitud = strlen(consumido) - size % (int) configSB.tamanio_blocks;
		if(bloquelleno(info)){
			longitud -=configSB.tamanio_blocks;
		}
		char* datos = borrarDatosBloque(info, consumido);
		insertarBloqueMapeo(datos, aux);
		liberarPosicion(aux);
		cantidadEliminada++;
		free(datos);
		free(info);
		while (longitud > 0 && cantidadBlocks - cantidadEliminada > 0) {
			aux = strtol(blocks[cantidadBlocks - cantidadEliminada - 1], NULL,
					10);
			char*info = buscarBloque(aux);
			char* restoConsumido = string_substring_until(consumido, longitud);
			char* datos = borrarDatosBloque(info, restoConsumido);
			insertarBloqueMapeo(datos, aux);

			if (longitud >= configSB.tamanio_blocks) {
				cantidadEliminada++;
				liberarPosicion(aux);
			}
			longitud -= configSB.tamanio_blocks;
			free(info);
			free(datos);
			free(restoConsumido);
		}

		char* listaBlocks = eliminarListaBlocks(blocks, cantidadBlocks,
				cantidadEliminada);
		liberarBloques(blocks, cantidadBlocks);
		free(blocks);
		cantidadBlocks -= cantidadEliminada;
		char* totalBlocks = string_itoa(cantidadBlocks);
		config_set_value(configTarea, "BLOCKS", listaBlocks);
		config_set_value(configTarea, "BLOCK_COUNT", totalBlocks);
		free(listaBlocks);
		free(totalBlocks);
	}
	if (size < strlen(consumido) && caracter[0] != 'B') {
		log_info(logMongo,
				"Se quisieron eliminar mas caracteres de los existentes");
		size = 0;
	} else {
		size -= strlen(consumido);
	}

	char* nuevoSize = string_itoa(size);
	config_set_value(configTarea, "SIZE", nuevoSize);
	config_save(configTarea);
	config_destroy(configTarea);
	free(nuevoSize);
}

void aumentarCantidad(char* direccionTarea, char* llenado) {
	t_config* configTarea = config_create(direccionTarea);
	t_list* listaBlocks = list_create();
	char** blocks = config_get_array_value(configTarea, "BLOCKS");
	int size = config_get_int_value(configTarea, "SIZE");
	int cantidadBlocks = cantidadBloquesFalsos(blocks);
	int cantidadBloques;
	cantidadBloques = cantidadBlocks;
	int aux;
	if (cantidadBlocks == 0) {
		aux = buscarPosicionLibre();
		list_add(listaBlocks, (void*) aux);
		cantidadBloques++;
	} else {
		aux = strtol(blocks[cantidadBlocks - 1], NULL, 10);
	}
	char*info = buscarBloque(aux);

	if ((size % configSB.tamanio_blocks + strlen(llenado)
			<= configSB.tamanio_blocks) && !bloquelleno(info)) {
		char* datos = grabarDatosBloque(info, llenado);
		insertarBloqueMapeo(datos, aux);
		free(datos);
		free(info);
	} else {
		int longitud = 0;
		//bloque lleno filtrar
		if (bloquelleno(info)) {
			longitud = strlen(llenado);
			free(info);
		} else {
			char* datos = grabarDatosBloque(info, llenado);
			insertarBloqueMapeo(datos, aux);
			free(datos);
			free(info);
			longitud = strlen(llenado) - configSB.tamanio_blocks
					+ size % configSB.tamanio_blocks;
		}

		while (longitud > 0) {
			char* restoLlenado = string_substring_from(llenado,
					strlen(llenado) - longitud);
			aux = buscarPosicionLibre();
			char* numBlock = string_itoa(aux);
			char*restoInfo = buscarBloque(aux);
			char* restoDatos = grabarDatosBloque(restoInfo, restoLlenado);
			insertarBloqueMapeo(restoDatos, aux);
			free(restoInfo);
			free(restoDatos);
			free(restoLlenado);
			list_add(listaBlocks, (void*)aux);
			cantidadBloques++;
			longitud -= configSB.tamanio_blocks;
			free(numBlock);
		}
	}
	char* totalBlocks = string_itoa(cantidadBloques);
	config_set_value(configTarea, "BLOCK_COUNT", totalBlocks);

	char* bloques = agregarListaBlocks(blocks, cantidadBlocks, listaBlocks);
	config_set_value(configTarea, "BLOCKS", bloques);

	size += strlen(llenado);
	char* nuevoSize = string_itoa(size);
	config_set_value(configTarea, "SIZE", nuevoSize);
	config_save(configTarea);
	config_destroy(configTarea);
	free(nuevoSize);
	free(totalBlocks);
	list_destroy(listaBlocks);
	liberarBloques(blocks, cantidadBlocks);
	free(blocks);
	free(bloques);
}
