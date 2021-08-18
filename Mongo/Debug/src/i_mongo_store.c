#include "i_mongo_store.h"

int main() {

	sem_init(&mutex_oxigeno, 0, 1);
	sem_init(&mutex_comida, 0, 1);
	sem_init(&mutex_basura, 0, 1);

	pthread_t hiloDiscord;
	sem_init(&mutexBitmap, 0, 1);
	sem_init(&mutexBlocks, 0, 1);
	sem_init(&mutexBitacora, 0, 1);
	//sem_init(&mutexSabotaje, 0, 1);
	sem_init(&mutexTareas, 0, 1);

	leer_config("./I_MONGO_STORE.config");
	cargar_Sabotajes();
	//configMongo = config_create("I_MONGO_STORE.config");
	//leer_config("Sabotaje.config");
	PUERTO_MONGO_STORE = config_get_int_value(configMongo, "PUERTO");
	IP_MONGO_STORE = config_get_string_value(configMongo, "IP_MONGO");

	iniciar_log("MongoStore");
	//logMongo = log_create("MongoStore.log", "MongoStore", true, LOG_LEVEL_INFO);
	//PUERTO_MONGO_STORE = getPuerto("PUERTO");
	ptomje = config_get_string_value(configMongo, "PUNTO_MONTAJE");
//	sem_wait(&mutexBitmap);
//	sem_post(&mutexBitmap);
//
//	sem_wait(&mutexBlocks);
//	sem_post(&mutexBlocks);

	if (pthread_create(&hiloDiscord, NULL, (void*) crearServerMongo,
	NULL) < 0) {
		perror("ERROR: NO SE PUDO CREAR UN HILO DEL DISCORDIADOR");
	}

	iniciarPolus();
	signal(SIGUSR1, &handler_sabotaje);
	//iniciarTarea("GENERAR_BASURA",20);
	//iniciarTarea("DESCARTAR_BASURA",0);
	//iniciarTarea("GENERAR_BASURA",20);
	//iniciarTarea("CONSUMIR_COMIDA",1);
	//iniciarTarea("GENERAR_COMIDA",64);
	//iniciarTarea("GENERAR_COMIDA",76);
	//iniciarTarea("CONSUMIR_COMIDA",64);
	//iniciarTarea("CONSUMIR_COMIDA",76);
	//generarOxigeno(55);
	//sabotearCantidadBloques(34);
	//consumirOxigeno(120);
	//generarOxigeno(65);
	//escribirBitacora(2,"3|30|5");
//	generarOxigeno(65);
	//escribirBitacora(2,"Iniciar tarea generarOxigeno");
//
	//generarComida(70);
	//consumirComida(70);
	//repararCantidadBloques();
	//repararBitmap();
	//repararSizeTarea("Oxigeno");
	//repararBlockCount("Oxigeno");
	//repararBlocks("Oxigeno");

//	log_info(logMongo,"Antes del sabotaje");
//	mostrarBits();
//	bitarray_clean_bit(datosSuperbloque->bitmap,1);
//	bitarray_set_bit(datosSuperbloque->bitmap,12);
//	log_info(logMongo,"Despues del sabotaje");
//	mostrarBits();
//	repararBitmap();
//	log_info(logMongo,"Reparado del sabotaje");
//	mostrarBits();
//	repararBitmap();

//	char* mds = generarHash("/home/utnso/workspace/Mongo/Polus/Files/Oxigeno.ims");
//	insertarHash("/home/utnso/workspace/Mongo/Polus/Files/Oxigeno.ims",mds);
//	log_info(logMongo,mds);
//	free(mds);
	//consumirOxigeno(65);
	//descargarBasura(0);
	//generarBasura(5);

	//char * bitac = directorioBitacoras();
	//char* tripu = malloc(strlen(bitac)+strlen("/Tripulante2.ims")+2);
	//char* mongol = malloc(strlen(bitac)+strlen("/Tripulante3.ims")+2);
	//strcpy(tripu,bitac);
	//strcpy(mongol,bitac);
	//strcat(tripu, "/Tripulante4.ims");
	//strcat(mongol, "/Tripulante5.ims");
	//asignarBloques(12);
//	crearBitacora("/home/utnso/workspace/Mongo/Polus/Files/Bitacoras/Tripulante1.ims");
	//crearBitacora(mongol);

//	char* resultado = buscarBitacora(2);
//	log_info(logMongo, resultado);
//	free(resultado);
//	if (resultado == NULL) {
//		log_info(logMongo, "Error no se encuentra bitacora");
//	} else {
//		log_info(logMongo, resultado);
//	}

	//free(resultado3);
	//free(bitac);
	//free(tripu);
	//free(mongol);
//	escribirBitacora(1,0,"3|30|5");
//	log_info(logMongo,"Escrita bitacora 1");
//	escribirBitacora(1,1,"Oxigeno");
//	log_info(logMongo,"Escrita bitacora 2");

//	char* bitac = buscarBitacora(1);
//	printf("Bitacora: %s",bitac);
//	char* bitacora2 = buscarBitacora(2);

	pthread_join(hiloDiscord, NULL);
	/* Cerramos el archivo mapeado */
	munmap(datosBloques, filesize);
	munmap(superbloque, tamanioSB);
	/* Cerramos el archivo */
//		close(fd);
	free(bitarrayGlobal);
	free(datosSuperbloque->bitmap);
	free(datosSuperbloque);
	free(mapeo);
	close(fd);
	close(sb);
	free(ptomje);
	free(IP_SABOTAJE);
	log_info(logMongo, "Se termino todillo");
	log_destroy(logMongo);
	sem_destroy(&mutexBitmap);
	sem_destroy(&mutexBlocks);
	sem_destroy(&mutexBitacora);
	sem_destroy(&mutexTareas);
}

void leer_config(char* nombre) {
	configMongo = config_create(nombre);
}
void iniciar_logger(char* nombre) {
	char* pathLog = malloc(sizeof(char) * strlen(nombre) + 4);
	strcat(pathLog, nombre);
	strcat(pathLog, ".log");
	logMongo = log_create(pathLog, nombre, true, LOG_LEVEL_INFO);
	free(pathLog);
}
void iniciar_log(char* nombre) {
	char* pathLog = string_new();
	string_append(&pathLog, nombre);
	string_append(&pathLog, ".log");
	logMongo = log_create(pathLog, nombre, true, LOG_LEVEL_INFO);
	free(pathLog);
}

void liberarVariablesGlobales() {

	munmap(datosBloques, filesize);
	munmap(superbloque, tamanioSB);
	/* Cerramos el archivo */
	//		close(fd);
	free(bitarrayGlobal);
	free(datosSuperbloque->bitmap);
	free(datosSuperbloque);
	free(mapeo);
	close(fd);
	close(sb);
	free(ptomje);
	log_info(logMongo, "Se termino todillo");
	log_destroy(logMongo);
}

void cargar_Sabotajes() {

	lista_sabotajes = list_create();
	char* sabotajes = config_get_string_value(configMongo,
			"POSICIONES_SABOTAJES");

	int contador = 0;
	char *p;
	p = sabotajes;
	int posicionX;
	int posicionY;

	while (*p != '\0') {
		if (isdigit(*p)) {
			contador++;
			if (contador == 1) {
				posicionX = (*p) - '0';
			} else if (contador == 2) {
				posicionY = (*p) - '0';
				t_sabotaje* sabotaje = fill_sabotaje(posicionX, posicionY);
				list_add(lista_sabotajes, (void*) sabotaje);
				contador = 0;
//				printf("%d|%d ", sabotaje->posicion_x, sabotaje->posicion_y);
			}
		} else if (*p == ']')
			break;
		p++;
	}

}

//Preguntas
/*
 1)md5 sabotajes Es necesario el md5 para reparar blocks ? Solo lo usaria para comparar el actual
 con el que voy a generar con los bloques saboteados. Es ese su fin? dado que ya tengo funciones que lo reparan
 pero me quedan con un orden diferente digamos
 Sin sabotaje [0,1,2]	size = 170
 con sabotaje [1,12,2,43232]
 reparado [1,0,2]	size = 170
 segun entiendo el md5 siempre me dara igual si tengo escrito en block.ims el mismo size de x caracter
 tipo ooooo md5 = ooooo md5

 2)cantidad de bloques sabotaje yo cada vez que inicio el FS a traves del config del Mongo y luego
 coloco en una variable global de una estructura  los datos del superbloque y trabajo mediante eso.
 Cuando hagan el sabotaje en cantidad de bloques segun el video debo arreglarlo mediante una division
 tamaño de archivo / size de bloque (que no fue saboteado), en mi caso yo tengo respaldado mis datos del superbloque
 en la variable global hace falta que haga esa division ?
 stat
 */
