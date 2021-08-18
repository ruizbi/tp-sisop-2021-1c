
#include "./discordiador.h"

int main(int argc, char *argv[]) {

	iniciarConfigs();
	iniciarColas();

	log_info(PATH_LOG, "INICIO DE EJECUCION");
	sem_init(&sem_detener_sabotaje, 0, 0);
	sem_init(&sem_block_execute, 0, 0);
	sem_init(&sem_detener_planificacion, 0, 0);
	sem_init(&sem_consumir, 0, 0);
	sem_init(&mutex_bitacora, 0, 0);
	sem_init(&consumir_block, 0, 0);
	sem_init(&sem_multitarea, 0, GRADO_MULTITAREA);

	int cliente_ram = crearCliente(IP_MI_RAM_HQ, PUERTO_MI_RAM_HQ);
	//mensajeIniciarPatota(1, "plantas.txt", cliente_ram);
	//mensajeIniciarTripulantes(1, 3, "1|1 2|3", cliente_ram);
	//mensajeCambiarEstado(2, "E", cliente_ram);
	mensajeExpulsarTripulante(1);
	/*
	pthread_t hilo_servidor_sabotaje;
	if(pthread_create(&hilo_servidor_sabotaje, NULL, (void*) servidorSabotaje, NULL) < 0)
		perror("ERROR: NO SE PUDO CREAR UN HILO DEL SERVER SABOTAJE");
	pthread_detach(hilo_servidor_sabotaje);

//	pthread_t hilo_servidor_sabotaje_sin_conexion;
//	if(pthread_create( &hilo_servidor_sabotaje_sin_conexion, NULL, (void*) sabotajeIniciado_sinConexion, NULL) < 0)
//		perror("ERROR: NO SE PUDO CREAR UN HILO DEL SERVER SABOTAJE");
//	pthread_detach(hilo_servidor_sabotaje_sin_conexion);

	pthread_t hilo_Bloqueante;
	if(pthread_create(&hilo_Bloqueante, NULL, (void*) desbloquearTripulante, NULL) < 0)
		perror("ERROR: NO SE PUDO CREAR UN HILO DE DESBLOQUEADO");
	pthread_detach(hilo_Bloqueante);

	int loop = 1;
	char* consoleLineAux;
	char* consoleLine;

	centinela_patota = 1;
	centinela_emergencia = 0;
	centinela_planificacion = 0;
	cantidadArgumentos = 0;
	tripulanteNumero = 0;
	patotaNumero = 0;

	listaTripulantes = list_create();

	while(loop) {

		t_list* argumentos = list_create();

		if( argc == 1 ){
			consoleLineAux = readline(">");
			consoleLine = (char*) malloc((strlen(consoleLineAux)+1) * sizeof(char));
			strcpy(consoleLine, consoleLineAux);
			char* token;
			token = (char*) strtok(consoleLine, " ");

			while (token != NULL) {
				list_add(argumentos, (void*) token);
				token = (char*) strtok(NULL, " ");
			}
			cantidadArgumentos = argumentos->elements_count;
		}

		char* proceso = (char *) list_get(argumentos, 0);

		// INICIAR_PATOTA CANTIDAD_TRIPULANTES PATH_TAREAS [POSICIONES]
		if((strcmp(proceso, "INICIAR_PATOTA") == 0) && (cantidadArgumentos >= 3)) {
			iniciarPatota(argumentos);
		}

		else if((strcmp(proceso, "LISTAR_TRIPULANTES") == 0) && (cantidadArgumentos == 1)) {
			listarTripulantes(listaTripulantes);
		}

		else if((strcmp(proceso, "EXPULSAR_TRIPULANTE") == 0) && (cantidadArgumentos == 2)) {
			int tid = atoi((char*) list_get(argumentos, 1));
			if(buscarTripulante(listaTripulantes, tid)) {
				t_tcb* tripulante = devolverTripulante(tid, listaTripulantes);
				t_pcb* patota = (t_pcb*) tripulante->puntero_pcb;
				listaTripulantes = expulsarTripulante(tripulante->tid, listaTripulantes);
				logger_expulsarTripulante(PATH_LOG, tripulante->tid);
				mensajeExpulsarTripulante(tripulante->tid);
				if(!existeDePatota(patota->pid, listaTripulantes))
					logger_finalizarPatota(PATH_LOG, patota->pid);
			}
			else {
				perror("ERROR: NO SE ENCONTRO EL TRIPULANTE");
			}
		}

		else if((strcmp(proceso, "INICIAR_PLANIFICACION") == 0) && (cantidadArgumentos == 1)) {
			if(centinela_planificacion) {
				if(!queue_is_empty(colaNew))
					deNew_a_ready();
				reactivarPlanificacion();
			}
			else {
				if(pthread_create( &planificacion, NULL, (void*) iniciarPlanificacion, NULL) < 0)
					perror("ERROR: NO SEPUDO CREAR EL HILO DE PLANIFICACION");
				pthread_detach(planificacion);
			}
		}

		else if((strcmp(proceso, "PAUSAR_PLANIFICACION") == 0) && (cantidadArgumentos == 1)) {
			centinela_planificacion = 1;
		}

		else if((strcmp(proceso, "OBTENER_BITACORA") == 0) && (cantidadArgumentos == 2)) {
			int tripulante = atoi((char*) list_get(argumentos, 1));
			if(buscarTripulante(listaTripulantes, tripulante)) {
				mensajeSolicitarBitacora(tripulante);
				sem_wait(&mutex_bitacora);
			}
		}

		else if((strcmp(proceso, "SABOTAJE") == 0) && (cantidadArgumentos == 1)) {
			centinela_emergencia = 1;
		}

		else if((strcmp(proceso, "EXIT") == 0) && (cantidadArgumentos == 1)) {
			loop = 0;
			if(!list_is_empty(colaExit)) {
				vaciarListaRemoviendo(colaExit);
			}
		}

		else {
			perror("ERROR: INSTRUCCION INVALIDA");
		}

		free(consoleLine);
		free(consoleLineAux);
		list_destroy(argumentos);
	}

//	pthread_join(planificacion, NULL);

	sem_destroy(&sem_block_execute);
	sem_destroy(&sem_detener_planificacion);
	sem_destroy(&sem_consumir);
	sem_destroy(&mutex_bitacora);
	sem_destroy(&consumir_block);
	sem_destroy(&sem_multitarea);

	list_destroy(listaTripulantes);

//	pthread_cancel(hilo_servidor_sabotaje_sin_conexion);
	pthread_cancel(hilo_servidor_sabotaje);
	pthread_cancel(hilo_Bloqueante);

	close(SERVIDOR_SABOTAJE);
	terminarConfig();
	finalizarColas();
	*/
	return 0;

}

void listarTripulantes(t_list* lista) {

	t_link_element* elemListaPatota = lista->head;
	t_tcb* unTripulante;
	char* estadoActual = temporal_get_string_time("%d/%m/%y %H:%M:%S");

	printf("--------------------------------------------------------------------\n");
	printf("Estado de la Nave: %s\n", estadoActual);
	for(int i=1; i<=lista->elements_count; i++) {
		unTripulante = (t_tcb*) elemListaPatota->data;
		char* estadoString = transformarEstado(unTripulante->estado);
		t_pcb* auxPCB = (t_pcb*) unTripulante->puntero_pcb;
		printf("Tripulante: %d\tPatota: %d\tStatus: %s \n", unTripulante->tid, auxPCB->pid, estadoString);
		elemListaPatota = elemListaPatota->next;
	}
	printf("--------------------------------------------------------------------\n");

	free(estadoActual);
}

void mensajeExpulsarTripulante(uint32_t tid) {

	int cliente_mongo = crearCliente(IP_I_MONGO_STORE, PUERTO_I_MONGO_STORE);
	t_buffer* buffer = serializar_solicitudes_varias(tid, 0);
	enviar_paquete(buffer, EXPULSAR_TRIPULANTE, cliente_mongo);
	close(cliente_mongo);
}

void mensajeCambiarEstado(uint32_t tid, char estado, int socket) {

	t_buffer* buffer = serializar_estado(tid, estado);
	enviar_paquete(buffer, CAMBIAR_ESTADO, socket);
}

void mensajeSolicitarBitacora(uint32_t tid) {

	int cliente_mongo = crearCliente(IP_I_MONGO_STORE, PUERTO_I_MONGO_STORE);
	t_buffer* buffer = serializar_solicitudes_varias(tid, 0);
	enviar_paquete(buffer, SOLICITAR_BITACORA, cliente_mongo);
	t_paquete* paquete = malloc(sizeof(t_paquete));
	paquete->buffer = malloc(sizeof(t_buffer));
	recv(cliente_mongo, &(paquete->codigo_operacion), sizeof(uint8_t), 0);
	recv(cliente_mongo, &(paquete->buffer->size), sizeof(uint32_t), 0);
	paquete->buffer->stream = malloc(paquete->buffer->size);
	recv(cliente_mongo, paquete->buffer->stream, paquete->buffer->size, 0);
	t_mensaje_string* mensaje = deserializar_mensaje_string(paquete->buffer);
	printf("%s\n", mensaje->string);
	free(mensaje->string);
	free(mensaje);
	free(paquete->buffer->stream);
	free(paquete->buffer);
	free(paquete);
	close(cliente_mongo);
	sem_post(&mutex_bitacora);
}

void mensajeTareaBloqueante(char* nombreTarea, uint32_t tid, uint32_t parametro, int socket) {

	t_buffer* buffer = serializar_tarea_io(tid, parametro, nombreTarea);
	enviar_paquete(buffer, INICIAR_IO, socket);
	t_paquete* paquete = malloc(sizeof(t_paquete));
	paquete->buffer = malloc(sizeof(t_buffer));
	recv(socket, &(paquete->codigo_operacion), sizeof(uint8_t), 0);
	recv(socket, &(paquete->buffer->size), sizeof(uint32_t), 0);
	paquete->buffer->stream = malloc(paquete->buffer->size);
	recv(socket, paquete->buffer->stream, paquete->buffer->size, 0);
	t_solicitudes_varias* mensaje = deserializar_solicitudes_varias(paquete->buffer);
	free(mensaje);
	free(paquete->buffer->stream);
	free(paquete->buffer);
	free(paquete);
}

void mensajeInicioTarea(char* nombreTarea, uint32_t tid, int socket) {

	t_buffer* buffer = serializar_mensaje_string(0, tid, nombreTarea);
	enviar_paquete(buffer, INICIAR_TAREA, socket);
}

void mensajeEjecutarSabotaje(int socket) {

	t_buffer* buffer = serializar_solicitudes_varias(0, 0);
	enviar_paquete(buffer, EJECUTAR_SABOTAJE, socket);
	t_paquete* paquete = malloc(sizeof(t_paquete));
	paquete->buffer = malloc(sizeof(t_buffer));
	recv(socket, &(paquete->codigo_operacion), sizeof(uint8_t), 0);
	recv(socket, &(paquete->buffer->size), sizeof(uint32_t), 0);
	paquete->buffer->stream = malloc(paquete->buffer->size);
	recv(socket, paquete->buffer->stream, paquete->buffer->size, 0);
	t_solicitudes_varias* mensaje = deserializar_solicitudes_varias(paquete->buffer);
	free(mensaje);
	free(paquete->buffer->stream);
	free(paquete->buffer);
	free(paquete);
}

void mensajeIniciarSabotaje(uint32_t tid, int socket) {
	t_buffer* buffer = serializar_solicitudes_varias(tid, 0);
	enviar_paquete(buffer, INICIAR_SABOTAJE, socket);
}

void mensajeFinalizarSabotaje(uint32_t tid, int socket) {
	t_buffer* buffer = serializar_solicitudes_varias(tid, 0);
	enviar_paquete(buffer, FINALIZAR_SABOTAJE, socket);
}

void mensajeFinTarea(char* nombreTarea, uint32_t tid, int socket) {

	t_buffer* buffer = serializar_mensaje_string(0, tid, nombreTarea);
	enviar_paquete(buffer, FINALIZAR_TAREA, socket);
}

void mensajeDesplazamiento(uint32_t tid, uint32_t origenX, uint32_t origenY, uint32_t destinoX, uint32_t destinoY, int socket) {

	t_buffer* buffer = serializar_desplazamiento(origenX, origenY, destinoX, destinoY, tid);
	enviar_paquete(buffer, DESPLAZAMIENTO, socket);
}

void mensajeIniciarTripulantes(uint32_t pid, uint32_t cantidad, char* posiciones, int socket) {

	t_buffer* buffer = serializar_iniciar_tripulantes(pid, cantidad, posiciones);
	enviar_paquete(buffer, INICIAR_TRIPULANTES, socket);
}

tarea_t* mensajeSolicitarTarea(uint32_t tid, uint32_t pid, int socket) {

	t_buffer* buffer = serializar_solicitudes_varias(tid, pid);
	enviar_paquete(buffer, SOLICITAR_TAREA, socket);
	t_paquete* paquete = malloc(sizeof(t_paquete));
	paquete->buffer = malloc(sizeof(t_buffer));
	recv(socket, &(paquete->codigo_operacion), sizeof(uint8_t), 0);
	recv(socket, &(paquete->buffer->size), sizeof(uint32_t), 0);
	paquete->buffer->stream = malloc(paquete->buffer->size);
	recv(socket, paquete->buffer->stream, paquete->buffer->size, 0);
	t_mensaje_string* mensaje = deserializar_mensaje_string(paquete->buffer);
	tarea_t* tarea = deString_aTarea(mensaje->string);
	free(mensaje->string);
	free(mensaje);
	free(paquete->buffer->stream);
	free(paquete->buffer);
	free(paquete);
	return tarea;
}

void mensajeIniciarPatota(uint32_t pid, char* path, int socket) {
	char* tareas = leer_archivo_tareas(path);
	t_buffer* buffer = serializar_mensaje_string(pid, 0, tareas);
	enviar_paquete(buffer, INICIAR_PATOTA, socket);
	free(tareas);
}

void mensajePosicionTripulante(uint32_t tid, uint32_t posicionX, uint32_t posicionY, int socket) {

	t_buffer* buffer = serializar_desplazamiento(0, 0, posicionX, posicionY, tid);
	enviar_paquete(buffer, MODIFICAR_POSICION, socket);
}

void reactivarPlanificacion() {

	for(int i=0; i<(GRADO_MULTITAREA+1); i++) {
		sem_post(&sem_detener_planificacion);
	}
	centinela_planificacion = 0;
}

char* transformarEstado(char estado) {

	if(estado == 'N')
		return "NEW";
	else if(estado == 'B')
		return "BLOCK I/O";
	else if(estado == 'R')
		return "READY";
	else if(estado == 'E')
		return "EXEC";
	else if(estado == 'F')
		return "EXIT";
	else if(estado == 'X')
		return "BLOCK EMERGENCY";
	else if(estado == 'Y')
		return "EXEC EMERGENCY";
	return "ERROR";
}

void servidorSabotaje() {


	void (*fptr)(int);
	fptr = recibir_paquete_sabotaje;
	crearServerConPuntero(PUERTO_SABOTAJE, BACKLOG, fptr);

//	unsigned int leng;
//	struct sockaddr_in cliente;
//	int socketServer;
//
//	socketServer = crearSocketEscucha(PUERTO_SABOTAJE, BACKLOG);
//	leng = sizeof(cliente);
//
//	while(1) {
//		int socketConexion = accept(socketServer, (struct sockaddr *)&cliente, &leng);
//
//		if( socketConexion < 0 ) {
//			perror("ERROR: Fallo el accept del servidor.");
//			close(socketConexion);
//		}
//		else {
//			pthread_t hilo;
//			if(pthread_create( &hilo, NULL, (void*) recibir_paquete_sabotaje, (void*) socketConexion))
//				perror("ERROR: NO SE PUDO CREAR EL HILO PARA EL CLIENTE");
//		}
//	}

}

void recibir_paquete_sabotaje(int socket) {

	t_paquete* paquete = malloc(sizeof(t_paquete));
	paquete->buffer = malloc(sizeof(t_buffer));

	int bytes = recv(socket, &(paquete->codigo_operacion), sizeof(uint8_t), 0);

	if(bytes < 0)
		printf("ERROR EN EL CODOP\n");
	else if(bytes == 0)
		printf("SE DESCONECTO EL CLIENTE\n");

	else {

		printf("CODOP: %d \n", paquete->codigo_operacion);
		bytes = recv(socket, &(paquete->buffer->size), sizeof(uint32_t), 0);

		if(bytes < 0)
				printf("ERROR EN EL SIZE DEL BUFFER\n");
		else if(bytes == 0)
				printf("SE DESCONECTO EL CLIENTE\n");

		else {

			printf("BUFFER SIZE: %d \n", paquete->buffer->size);
			paquete->buffer->stream = malloc(paquete->buffer->size);
			bytes = recv(socket, paquete->buffer->stream, paquete->buffer->size, 0);

			if(bytes < 0)
				printf("ERROR EN RECIBIR EL STREAM\n");
			else if(bytes == 0)
				printf("SE DESCONECTO EL CLIENTE\n");

			else {
				t_sabotaje* sabotaje = deserializar_sabotaje(paquete->buffer);
				sabotajeIniciado(sabotaje, socket);
				sem_post(&sem_detener_sabotaje);
			}
			free(paquete->buffer->stream);
		}
	}

	free(paquete->buffer);
	free(paquete);
	close(socket);
}

void iniciarConfigs() {

	PATH_CONFIG = config_create( PATH_CONFIG_DISCORDIADOR );
	PATH_LOG = log_create(PATH_LOG_DISCORDIADOR, "DISCORDIADOR", 0, LOG_LEVEL_INFO);
	IP_MI_RAM_HQ = config_get_string_value( PATH_CONFIG, "IP_MI_RAM_HQ" );
	IP_I_MONGO_STORE = config_get_string_value( PATH_CONFIG, "IP_I_MONGO_STORE" );
	ALGORITMO = config_get_string_value( PATH_CONFIG, "ALGORITMO");
	PUERTO_MI_RAM_HQ = config_get_int_value( PATH_CONFIG, "PUERTO_MI_RAM_HQ" );
	PUERTO_I_MONGO_STORE = config_get_int_value( PATH_CONFIG, "PUERTO_I_MONGO_STORE" );
	RETARDO_CICLO_CPU = config_get_int_value( PATH_CONFIG, "RETARDO_CICLO_CPU");
	QUANTUM = config_get_int_value( PATH_CONFIG, "QUANTUM");
	GRADO_MULTITAREA = config_get_int_value( PATH_CONFIG, "GRADO_MULTITAREA");
	DURACION_SABOTAJE = config_get_int_value( PATH_CONFIG, "DURACION_SABOTAJE");
}

void terminarConfig() {

	free(ALGORITMO);
	free(IP_I_MONGO_STORE);
	free(IP_MI_RAM_HQ);
	config_destroy(PATH_CONFIG);
	log_destroy(PATH_LOG);
}

t_pcb* nuevaPatota(uint32_t pid, char* tareas) {

	t_pcb* nuevaPatota = malloc(sizeof(uint32_t) * 2);
	nuevaPatota->pid = pid;
	nuevaPatota->puntero_tareas = (uint32_t) tareas;
	return nuevaPatota;
}

t_list* expulsarTripulante(int id, t_list* lista) {

	bool filtrarTripulante(void* elemento) {
		t_tcb* tripulante = (t_tcb *) elemento;
		return (tripulante->tid != id);
	}

	lista = list_filter(lista, filtrarTripulante);
	return lista;
}

t_tcb* nuevoTripulante(uint32_t tid, uint32_t posicionX, uint32_t posicionY, t_pcb* patota, tarea_t* tarea) {

	t_tcb* newTripulante = malloc((sizeof(uint32_t) * 5 ) + sizeof(char));

	newTripulante->tid = tid;
	newTripulante->pos_x = posicionX;
	newTripulante->pos_y = posicionY;
	newTripulante->estado = 'N';
	newTripulante->proximaInstruccion = (uint32_t) tarea;
	newTripulante->puntero_pcb = (uint32_t) patota;
	return newTripulante;
}

bool buscarTripulante(t_list* listaTripulantes, int id) {

	bool findTripulante(void* elemento) {
		t_tcb* tripulante = (t_tcb *) elemento;
		return (tripulante->tid == id);
	}

	if(list_find(listaTripulantes, findTripulante))
		return true;
	return false;
}


int existe_ruta(char* filename){

	struct stat buffer;
	char* ruta = malloc(10 + strlen(filename));
	sprintf(ruta, "./tareas/%s", filename);
	if(!stat(ruta,&buffer)) {
		free(ruta);
		return 1;
	}
	free(ruta);
	return 0;
}

//int existe_ruta(char* nombre_archivo) {
//
//	FILE* f;
//	char* ruta = malloc(10 + strlen(nombre_archivo));
//	sprintf(ruta, "./tareas/%s", nombre_archivo);
//
//	if((f = fopen(ruta,"rt")) != NULL) {
//		free(ruta);
//		fclose(f);
//		return 1;
//	}
//	free(ruta);
//	return 0;
//}


void iniciarPatota(t_list* argumentos) {

	char* pathTareas = (char*) list_get(argumentos, 2);
	if(existe_ruta(pathTareas)) {
		int cliente_ram = crearCliente(IP_MI_RAM_HQ, PUERTO_MI_RAM_HQ);
		int cliente_mongo = crearCliente(IP_I_MONGO_STORE, PUERTO_I_MONGO_STORE);

		char* posiciones = malloc(100);
		strcpy(posiciones, "");
		patotaNumero++;
		int cantidadTripulantes = atoi((char*) list_get(argumentos, 1));
		t_pcb* unaPatota = nuevaPatota(patotaNumero, pathTareas);
		loggear_inicioPatota(PATH_LOG, unaPatota->pid, pathTareas);
		mensajeIniciarPatota(unaPatota->pid, (char*) unaPatota->puntero_tareas, cliente_ram);

		for(int i=3; i<(cantidadTripulantes + 3); i++) {
			if(i < cantidadArgumentos) {
				char* ubicacionTripulante = (char*) list_get(argumentos, i);
				strcat(posiciones, ubicacionTripulante);
				strcat(posiciones, " ");
			}
			else {
				strcat(posiciones, "0|0");
				strcat(posiciones, " ");
			}
		}
		mensajeIniciarTripulantes(unaPatota->pid, cantidadTripulantes, posiciones, cliente_ram);
		free(posiciones);


		for(int i=3; i<(cantidadTripulantes + 3); i++) {
			tripulanteNumero++;
			t_tcb* unTripulante;
			tarea_t* tarea = mensajeSolicitarTarea(tripulanteNumero, unaPatota->pid, cliente_ram);

			// PILOTO PRUEBAS
//			tarea_t* tarea;
//			if(patotaNumero == 3) {
//				tarea = deString_aTarea("SALTAR_CUERDA 8;1;5;5");
//			}
//			else if(patotaNumero == 2 ) {
//				tarea = deString_aTarea("CONSUMIR_OXIGENO 1;4;3;3");
//			}
//			else if (patotaNumero == 1) {
//				tarea = deString_aTarea("GENERAR_OXIGENO 2;6;5;4");
//			}
//			else {
//				tarea = deString_aTarea("PINTAR_PAREDES 8;2;3;9");
//			}
			// FIN PILOTO PRUEBAS

			if(i < cantidadArgumentos) {
				char* ubicacionTripulante = (char*) list_get(argumentos, i);
//				strcat(posiciones, "");
//				strcat(posiciones, ubicacionTripulante);
				char* ubicacionX = (char*) strtok(ubicacionTripulante, "|");
				char* ubicacionY = (char*) strtok(NULL, " ");
				int ubicacionX_int = atoi(ubicacionX);
				int ubicacionY_int = atoi(ubicacionY);
				unTripulante = nuevoTripulante(tripulanteNumero, ubicacionX_int, ubicacionY_int, unaPatota, tarea);
				list_add(listaTripulantes, (void*) unTripulante);
				pthread_mutex_lock(&mutex_colaNew);
				queue_push(colaNew, (void*) unTripulante);
				pthread_mutex_unlock(&mutex_colaNew);
			}
			else {
				unTripulante = nuevoTripulante(tripulanteNumero, 0, 0, unaPatota, tarea);
				list_add(listaTripulantes, (void*) unTripulante);
				pthread_mutex_lock(&mutex_colaNew);
				queue_push(colaNew, (void*) unTripulante);
				pthread_mutex_unlock(&mutex_colaNew);
			}

			mensajeInicioTarea(tarea->nombreTarea, unTripulante->tid, cliente_mongo);
			loggear_crearTripulante(PATH_LOG, unTripulante->tid, unTripulante->pos_x, unTripulante->pos_y, unaPatota->pid);
			logger_iniciarTarea(PATH_LOG, unTripulante->tid, tarea->nombreTarea);
		}
//		mensajeIniciarTripulantes(unaPatota->pid, cantidadTripulantes, posiciones, cliente_ram);
//		free(posiciones);
		close(cliente_ram);
		close(cliente_mongo);
	}
	else
		perror("ERROR: EL NOMBRE DE TAREAS ES INCORRECTO");
}

void iniciarColas() {

	colaNew = queue_create();
	colaExit = list_create();
	colaReady = list_create();
	colaExec = list_create();
	colaBlock = list_create();
	colaBlockEmergency = list_create();
}

void finalizarColas() {

	list_destroy(colaExit);
	queue_destroy(colaNew);
	list_destroy(colaReady);
	list_destroy(colaExec);
	list_destroy(colaBlock);
	list_destroy(colaBlockEmergency);
}

tarea_t* nuevaTarea(char* nombreTarea, uint32_t parametro, uint32_t posicionX, uint32_t posicionY, uint32_t duracion) {

	int len_nombreTarea = strlen(nombreTarea) + 1;
//	tarea_t* tarea = (tarea_t*) malloc((sizeof(uint32_t)*4) + len_nombreTarea);
	tarea_t* tarea = (tarea_t*) malloc((sizeof(tarea_t)));
	tarea->nombreTarea = malloc(len_nombreTarea);
//	tarea->nombreTarea = nombreTarea;
	strcpy(tarea->nombreTarea, nombreTarea);
	tarea->duracion = duracion;
	tarea->parametro = parametro;
	tarea->pos_x = posicionX;
	tarea->pos_y = posicionY;

	return tarea;
}

tarea_t* deString_aTarea(char* unString) {

	int len_string = strlen(unString) + 1;
	char* auxString = (char*) malloc(len_string + 1);
	strcpy(auxString, unString);
	char* token;
	t_list* argumentos = list_create();
	token = (char*) strtok(auxString, " ");

	while (token != NULL) {
		list_add(argumentos, (void*) token);
		token = (char*) strtok(NULL, ";");
	}

	tarea_t* unaTareaX = nuevaTarea(((char*) list_get(argumentos, 0)), atoi((char*) list_get(argumentos, 1)), atoi((char*) list_get(argumentos, 2)), atoi((char*) list_get(argumentos, 3)), atoi((char*) list_get(argumentos, 4)));

	list_destroy(argumentos);
	free(auxString);
	free(token);
	return unaTareaX;
}

void deNew_a_ready() {

	t_tcb* unTripulante;
	int cliente_ram = crearCliente(IP_MI_RAM_HQ, PUERTO_MI_RAM_HQ);

	while(!queue_is_empty(colaNew)) {

		pthread_mutex_lock(&mutex_colaNew);
		unTripulante = queue_pop(colaNew);
		pthread_mutex_unlock(&mutex_colaNew);
		cambiarEstado('R', unTripulante, cliente_ram);
		pthread_mutex_lock(&mutex_colaReady);
		list_add(colaReady, (void*) unTripulante);
		pthread_mutex_unlock(&mutex_colaReady);
		sem_post(&sem_consumir);

	}
	close(cliente_ram);
}

void cambiarEstado(char estado, t_tcb* tripulante, int socket) {

	tripulante->estado = estado;
	mensajeCambiarEstado(tripulante->tid, estado, socket);
	logger_cambiarEstado(PATH_LOG, tripulante->tid, tripulante->estado);
}

void vaciarListaRemoviendo(t_list* lista) {
	t_tcb* unTripulante;
	t_pcb* patota;
	while(!list_is_empty(lista)) {
		unTripulante = list_remove(lista, 0);
		patota = (t_pcb*) unTripulante->puntero_pcb;
		tarea_t* tarea = (tarea_t*) unTripulante->proximaInstruccion;
		if(!existeDePatota(patota->pid, colaExit))
			free(patota);
		free(unTripulante);
		free(tarea->nombreTarea);
		free(tarea);
	}
}

void listaTripulantes_sinExit() {
	bool distintoEstado(void* elemento){
		t_tcb* tripulante = elemento;
		return tripulante->estado != 'F';
	}
	listaTripulantes = list_filter(listaTripulantes, distintoEstado);
}

void pruebaPlani() {

	int loop_Planificacion = 1;
	t_tcb* unTripulante;

	int cliente_ram = crearCliente(IP_MI_RAM_HQ, PUERTO_MI_RAM_HQ);
	int cliente_mongo = crearCliente(IP_I_MONGO_STORE, PUERTO_I_MONGO_STORE);

	while(loop_Planificacion) {

		if(list_is_empty(colaReady) && list_is_empty(colaBlock) && list_is_empty(colaExec) && list_is_empty(colaBlockEmergency)) {
			loop_Planificacion = 0;
		}
		else {

			sem_wait(&sem_multitarea);
			sem_wait(&sem_consumir);
			pthread_mutex_lock(&mutex_colaReady);
//			unTripulante = proximoTripulante();
			unTripulante = list_remove(colaReady, 0);
			pthread_mutex_unlock(&mutex_colaReady);

			if(buscarTripulante(listaTripulantes ,unTripulante->tid)){

				pthread_mutex_lock(&mutex_colaExecute);
				list_add(colaExec, (void*) unTripulante);
				pthread_mutex_unlock(&mutex_colaExecute);

				tarea_t* tarea = (tarea_t*) unTripulante->proximaInstruccion;

				if(strcmp(ALGORITMO, "FIFO") == 0) {
					ejecutarTareaFIFO(unTripulante, tarea, cliente_ram, cliente_mongo);
				}
				else if(strcmp(ALGORITMO, "RR") == 0) {
					ejecutarTareaRR(unTripulante, tarea, cliente_ram, cliente_mongo);
				}

				if(!centinela_emergencia)
					sem_post(&sem_multitarea);

//				if(centinela_emergencia)
//					sem_post(&sem_block_execute);
//				else
//					sem_post(&sem_multitarea);
			}
			else {
				t_pcb* auxPatota = (t_pcb*) unTripulante->puntero_pcb;
				if(!existeDePatota(auxPatota->pid, listaTripulantes)) {
					free(auxPatota);
				}
				free(unTripulante);
				sem_post(&sem_multitarea);
			}
		}
	}

	close(cliente_mongo);
	close(cliente_ram);
}

void iniciarPlanificacion() {

	deNew_a_ready();

	if(!list_is_empty(colaExit)) {
		listaTripulantes_sinExit();
		vaciarListaRemoviendo(colaExit);
	}

	pthread_t hilosPlanificacion[GRADO_MULTITAREA];

	for(int i=0; i<GRADO_MULTITAREA;i++) {
		pthread_create(&hilosPlanificacion[i], NULL, (void*) pruebaPlani, NULL);
	}
	for(int i=0; i<GRADO_MULTITAREA;i++) {
		pthread_detach(hilosPlanificacion[i]);
	}

}

bool esBloqueante(tarea_t* tarea) {

	if((strcmp(tarea->nombreTarea, "DESCARTAR_BASURA") == 0) || (strcmp(tarea->nombreTarea, "GENERAR_BASURA") == 0)) {
		return true;
	}
	else if((strcmp(tarea->nombreTarea, "CONSUMIR_COMIDA") == 0) || (strcmp(tarea->nombreTarea, "GENERAR_COMIDA") == 0)) {
		return true;
	}
	else if((strcmp(tarea->nombreTarea, "CONSUMIR_OXIGENO") == 0) || (strcmp(tarea->nombreTarea, "GENERAR_OXIGENO") == 0)) {
		return true;
	}
	return false;
}

void ejecutarTareaRR(t_tcb* tripulante, tarea_t* tarea, int cliente_ram, int cliente_mongo) {

	t_pcb* auxPatota = (t_pcb*) tripulante->puntero_pcb;
	int centinela_posicionOK = 0;
	cambiarEstado('E', tripulante, cliente_ram);

	for(int i=1; i<=QUANTUM; i++) {
		if(buscarTripulante(listaTripulantes, tripulante->tid) && !centinela_emergencia) {
			if(centinela_planificacion)
				sem_wait(&sem_detener_planificacion);

			if(!((tripulante->pos_x == tarea->pos_x) && (tripulante->pos_y == tarea->pos_y))){
				quantumMovimiento(tripulante, tarea, cliente_mongo);
				mensajePosicionTripulante(tripulante->tid, tripulante->pos_x, tripulante->pos_y, cliente_ram);
			}
			else if(tarea->duracion != 0 && !esBloqueante(tarea)) {
				quantumTarea(tripulante, tarea);
			}
			else if((i<QUANTUM) && (tripulante->pos_x == tarea->pos_x) && (tripulante->pos_y == tarea->pos_y) && esBloqueante(tarea)) {
				centinela_posicionOK = 1;
			}
			else {
				break;
			}
		}
		else {
			break;
		}
	}

	if(buscarTripulante(listaTripulantes, tripulante->tid)) {

		pthread_mutex_lock(&mutex_colaExecute);
		tripulante = devolverRemoviendoTripulante(tripulante->tid, colaExec);
		pthread_mutex_unlock(&mutex_colaExecute);

		if(centinela_emergencia) {
			cambiarEstado('X', tripulante, cliente_ram);
			pthread_mutex_lock(&mutex_colaBlockEmergency);
			list_add(colaBlockEmergency, (void*) tripulante);
			pthread_mutex_unlock(&mutex_colaBlockEmergency);

			sem_post(&sem_block_execute);

		}
		else if(tarea->duracion == 0 && !esBloqueante(tarea)) {
			logger_finalizarTarea(PATH_LOG, tripulante->tid, tarea->nombreTarea);
			mensajeFinTarea(tarea->nombreTarea, tripulante->tid, cliente_mongo);
			free(tarea->nombreTarea);
			free(tarea);
			tarea_t* nueva_tarea = mensajeSolicitarTarea(tripulante->tid, auxPatota->pid, cliente_ram);
			tripulante->proximaInstruccion = (uint32_t) nueva_tarea;
			if(strcmp(nueva_tarea->nombreTarea, "FIN") == 0) {
				cambiarEstado('F', tripulante, cliente_ram);
				pthread_mutex_lock(&mutex_colaExecute);
				list_add(colaExit, (void*) tripulante);
				pthread_mutex_unlock(&mutex_colaExecute);
			}
			else {
				logger_iniciarTarea(PATH_LOG, tripulante->tid, nueva_tarea->nombreTarea);
				mensajeInicioTarea(nueva_tarea->nombreTarea, tripulante->tid, cliente_mongo);
				cambiarEstado('R', tripulante, cliente_ram);
				pthread_mutex_lock(&mutex_colaReady);
				list_add(colaReady, (void*) tripulante);
				pthread_mutex_unlock(&mutex_colaReady);
				sem_post(&sem_consumir);
			}
		}
		else if(esBloqueante(tarea) && centinela_posicionOK){
			mensajeTareaBloqueante(tarea->nombreTarea, tripulante->tid, tarea->parametro, cliente_mongo);
			cambiarEstado('B', tripulante, cliente_ram);
			pthread_mutex_lock(&mutex_colaBlock);
			list_add(colaBlock, (void*) tripulante);
			pthread_mutex_unlock(&mutex_colaBlock);
			sem_post(&consumir_block);
		}
		else {
			cambiarEstado('R', tripulante, cliente_ram);
			pthread_mutex_lock(&mutex_colaReady);
			list_add(colaReady, (void*) tripulante);
			pthread_mutex_unlock(&mutex_colaReady);
			sem_post(&sem_consumir);
		}
	}
	else {
		if(!existeDePatota(auxPatota->pid, listaTripulantes)) {
			free(auxPatota);
		}
		free(tripulante);
	}

}

void ejecutarTareaFIFO(t_tcb* tripulante, tarea_t* tarea, int cliente_ram, int cliente_mongo) {

	bool ejecutar = 1;
	t_pcb* auxPatota = (t_pcb*) tripulante->puntero_pcb;
	cambiarEstado('E', tripulante, cliente_ram);

	while(ejecutar) {
		if(buscarTripulante(listaTripulantes, tripulante->tid) && !centinela_emergencia) {
			if(centinela_planificacion)
				sem_wait(&sem_detener_planificacion);

			if(!((tripulante->pos_x == tarea->pos_x) && (tripulante->pos_y == tarea->pos_y))){
				quantumMovimiento(tripulante, tarea, cliente_mongo);
				mensajePosicionTripulante(tripulante->tid, tripulante->pos_x, tripulante->pos_y, cliente_ram);
			}

			else if(tarea->duracion != 0 && !esBloqueante(tarea)) {
				quantumTarea(tripulante, tarea);
			}
			else
				ejecutar = 0;
		}
		else
			ejecutar = 0;
	}

	if(buscarTripulante(listaTripulantes, tripulante->tid)) {

		pthread_mutex_lock(&mutex_colaExecute);
		tripulante = devolverRemoviendoTripulante(tripulante->tid, colaExec);
		pthread_mutex_unlock(&mutex_colaExecute);

		if(centinela_emergencia) {
			cambiarEstado('X', tripulante, cliente_ram);
			pthread_mutex_lock(&mutex_colaBlockEmergency);
			list_add(colaBlockEmergency, (void*) tripulante);
			pthread_mutex_unlock(&mutex_colaBlockEmergency);

			sem_post(&sem_block_execute);

		}
		else if(esBloqueante(tarea)) {
			mensajeTareaBloqueante(tarea->nombreTarea, tripulante->tid, tarea->parametro, cliente_mongo);
			cambiarEstado('B', tripulante, cliente_ram);
			pthread_mutex_lock(&mutex_colaBlock);
			list_add(colaBlock, (void*) tripulante);
			pthread_mutex_unlock(&mutex_colaBlock);
			sem_post(&consumir_block);
		}
		else {
			mensajeFinTarea(tarea->nombreTarea, tripulante->tid, cliente_mongo);
			logger_finalizarTarea(PATH_LOG, tripulante->tid, tarea->nombreTarea);
			free(tarea->nombreTarea);
			free(tarea);
			tarea_t* nueva_tarea = mensajeSolicitarTarea(tripulante->tid, auxPatota->pid, cliente_ram);
			tripulante->proximaInstruccion = (uint32_t) nueva_tarea;
			if(strcmp(nueva_tarea->nombreTarea, "FIN") == 0) {
				cambiarEstado('F', tripulante, cliente_ram);
				pthread_mutex_lock(&mutex_colaExecute);
				list_add(colaExit, (void*) tripulante);
				pthread_mutex_unlock(&mutex_colaExecute);
			}
			else {
				logger_iniciarTarea(PATH_LOG, tripulante->tid, nueva_tarea->nombreTarea);
				mensajeInicioTarea(nueva_tarea->nombreTarea, tripulante->tid, cliente_mongo);
				cambiarEstado('R', tripulante, cliente_ram);
				pthread_mutex_lock(&mutex_colaReady);
				list_add(colaReady, (void*) tripulante);
				pthread_mutex_unlock(&mutex_colaReady);
				sem_post(&sem_consumir);
			}
		}
	}
	else {
		if(!existeDePatota(auxPatota->pid, listaTripulantes)) {
			free(auxPatota);
		}
		free(tripulante);
	}

}

void quantumMovimiento(t_tcb* tripulante, tarea_t* tarea, int mongo) {

	if( tripulante->pos_x < tarea->pos_x ) {

		mensajeDesplazamiento(tripulante->tid, tripulante->pos_x, tripulante->pos_y, (tripulante->pos_x + 1), tripulante->pos_y, mongo);
		tripulante->pos_x += 1;
		sleep(RETARDO_CICLO_CPU);
	}
	else if( tripulante->pos_x > tarea->pos_x ) {

		mensajeDesplazamiento(tripulante->tid, tripulante->pos_x, tripulante->pos_y, (tripulante->pos_x - 1), tripulante->pos_y, mongo);
		tripulante->pos_x -= 1;
		sleep(RETARDO_CICLO_CPU);
	}
	else if( tripulante->pos_y < tarea->pos_y ) {

		mensajeDesplazamiento(tripulante->tid, tripulante->pos_x, tripulante->pos_y, tripulante->pos_x, (tripulante->pos_y + 1), mongo);
		tripulante->pos_y += 1;
		sleep(RETARDO_CICLO_CPU);
	}
	else if( tripulante->pos_y > tarea->pos_y ) {

		mensajeDesplazamiento(tripulante->tid, tripulante->pos_x, tripulante->pos_y, tripulante->pos_x, (tripulante->pos_y - 1), mongo);
		tripulante->pos_y -= 1;
		sleep(RETARDO_CICLO_CPU);
	}

	logger_posicionTripulante(PATH_LOG, tripulante->tid, tripulante->pos_x, tripulante->pos_y);
}

void quantumTarea(t_tcb* tripulante, tarea_t* tarea) {

	logger_ejecucionTarea(PATH_LOG, tripulante->tid, tarea->duracion, tarea->nombreTarea);
	tarea->duracion -= 1;
	sleep(RETARDO_CICLO_CPU);
	tripulante->proximaInstruccion = (uint32_t) tarea;
}

void desbloquearTripulante() {

	int cliente_mongo = crearCliente(IP_I_MONGO_STORE, PUERTO_I_MONGO_STORE);
	int cliente_ram = crearCliente(IP_MI_RAM_HQ, PUERTO_MI_RAM_HQ);

	while(1) {

		pthread_mutex_lock(&mutex_ejecutarBloqueados);
		sem_wait(&consumir_block);
		pthread_mutex_lock(&mutex_colaBlock);
		t_tcb* tripulante = list_remove(colaBlock, 0);
		pthread_mutex_unlock(&mutex_colaBlock);

		tarea_t* tarea = (tarea_t*) tripulante->proximaInstruccion;
		t_pcb* patota = (t_pcb*) tripulante->puntero_pcb;
		logger_iniciarTareaIO(PATH_LOG, tripulante->tid, tarea->nombreTarea, tarea->parametro);
//		mensajeTareaBloqueante(tarea->nombreTarea, tripulante->tid, tarea->parametro, cliente_mongo);
		for(int i=0; i<tarea->duracion; i++) {
			if(centinela_emergencia)
				sem_wait(&sem_detener_sabotaje);
			if(centinela_planificacion)
				sem_wait(&sem_detener_planificacion);
			sleep(1);
		}
		mensajeFinTarea(tarea->nombreTarea, tripulante->tid, cliente_mongo);
		logger_finalizarTarea(PATH_LOG, tripulante->tid, tarea->nombreTarea);
		free(tarea->nombreTarea);
		free(tarea);
		tarea_t* nueva_tarea = mensajeSolicitarTarea(tripulante->tid, patota->pid, cliente_ram);
		tripulante->proximaInstruccion = (uint32_t) nueva_tarea;

		if(centinela_emergencia) {

			cambiarEstado('X', tripulante, cliente_ram);
			pthread_mutex_lock(&mutex_colaBlockEmergency);
			list_add(colaBlockEmergency, (void*) tripulante);
			pthread_mutex_unlock(&mutex_colaBlockEmergency);
		}
		else if(strcmp(nueva_tarea->nombreTarea, "F") == 0) {

			cambiarEstado('F', tripulante, cliente_ram);
			pthread_mutex_lock(&mutex_colaExecute);
			list_add(colaExit, (void*) tripulante);
			pthread_mutex_unlock(&mutex_colaExecute);
		}
		else {
			logger_iniciarTarea(PATH_LOG, tripulante->tid, nueva_tarea->nombreTarea);
			mensajeInicioTarea(nueva_tarea->nombreTarea, tripulante->tid, cliente_mongo);
			cambiarEstado('R', tripulante, cliente_ram);
			pthread_mutex_lock(&mutex_colaReady);
			list_add(colaReady, (void*) tripulante);
			pthread_mutex_unlock(&mutex_colaReady);
			sem_post(&sem_consumir);
		}

		pthread_mutex_unlock(&mutex_ejecutarBloqueados);
	}

	log_info(PATH_LOG, "FIN HILO BLOQUEADOS");
	close(cliente_mongo);
	close(cliente_ram);
}

t_tcb* devolverTripulante(int tid, t_list* lista) {

	bool findTripulante(void* elemento) {
		t_tcb* tripulante = (t_tcb *) elemento;
		return (tripulante->tid == tid);
	}

	t_tcb* tripulante = list_find(lista ,findTripulante);

	return tripulante;
}

t_tcb* devolverRemoviendoTripulante(int tid, t_list* lista) {

	bool findTripulante(void* elemento) {
		t_tcb* tripulante = (t_tcb *) elemento;
		return (tripulante->tid == tid);
	}

	t_tcb* tripulante = list_remove_by_condition(lista ,findTripulante);

	return tripulante;
}

t_tcb* proximoTripulante() {

	t_tcb* tripulante;
	int loop = 1;

	while(loop) {

		if(centinela_patota <= patotaNumero) {

			if(existeDePatota(centinela_patota, colaReady)) {

				tripulante = devolverRemoviendoTripulantePID(centinela_patota, colaReady);
				if(buscarTripulante(listaTripulantes, tripulante->tid)) {
					centinela_patota ++;
				}
				loop = 0;
			}
			else {
				centinela_patota ++;
			}
		}
		else {
			centinela_patota = 1;
		}
	}

	return tripulante;
}

t_tcb* devolverRemoviendoTripulantePID(int pid, t_list* lista) {

	bool findTripulante(void* elemento) {
		t_tcb* tripulante = (t_tcb *) elemento;
		t_pcb* patota = (t_pcb*) tripulante->puntero_pcb;
		return (patota->pid == pid);
	}
	t_tcb* tripulante = list_remove_by_condition(lista ,findTripulante);

	return tripulante;
}

bool existeDePatota(int pid, t_list* lista) {

	bool findTripulante(void* elemento) {
		t_tcb* tripulante = (t_tcb *) elemento;
		t_pcb* patota = (t_pcb*) tripulante->puntero_pcb;
		return (patota->pid == pid);
	}

	if(list_find(lista, findTripulante))
		return true;

	return false;
}

void deReady_a_blockEmergencia() {

	t_tcb* unTripulante;

	int cliente_ram = crearCliente(IP_MI_RAM_HQ, PUERTO_MI_RAM_HQ);

	pthread_mutex_lock(&mutex_colaReady);
	ordenarPorTID(colaReady);
	pthread_mutex_unlock(&mutex_colaReady);

	while(!list_is_empty(colaReady)) {

		pthread_mutex_lock(&mutex_colaReady);
		unTripulante = (t_tcb*) list_remove(colaReady, 0);
		pthread_mutex_unlock(&mutex_colaReady);
		sem_wait(&sem_consumir);
		cambiarEstado('X', unTripulante, cliente_ram);
		pthread_mutex_lock(&mutex_colaBlockEmergency);
		list_add(colaBlockEmergency, (void*) unTripulante);
		pthread_mutex_unlock(&mutex_colaBlockEmergency);
	}

	close(cliente_ram);
}

void deBlockEmergencia_a_ready() {

	t_tcb* unTripulante;
	int cliente_ram = crearCliente(IP_MI_RAM_HQ, PUERTO_MI_RAM_HQ);

	while(!list_is_empty(colaBlockEmergency)) {

		pthread_mutex_lock(&mutex_colaBlockEmergency);
		unTripulante = (t_tcb*) list_remove(colaBlockEmergency, 0);
		pthread_mutex_unlock(&mutex_colaBlockEmergency);

		cambiarEstado('R', unTripulante, cliente_ram);

		pthread_mutex_lock(&mutex_colaReady);
		list_add(colaReady, (void*) unTripulante);
		pthread_mutex_unlock(&mutex_colaReady);
		sem_post(&sem_consumir);
	}
	close(cliente_ram);
}

void sabotajeIniciado_sinConexion() {

	while(1) {

		while(!list_is_empty(colaExec) || !centinela_emergencia)
			sleep(1);
//		sleep(5);

		for(int i=0; i<GRADO_MULTITAREA; i++) {
			sem_wait(&sem_block_execute);
		}

		int cliente_ram = crearCliente(IP_MI_RAM_HQ, PUERTO_MI_RAM_HQ);
		int cliente_mongo = crearCliente(IP_I_MONGO_STORE, PUERTO_I_MONGO_STORE);

		int posicion_x = 9;
		int posicion_y = 11;

		pthread_mutex_lock(&mutex_colaBlockEmergency);
		ordenarPorTID(colaBlockEmergency);
		pthread_mutex_unlock(&mutex_colaBlockEmergency);
		deReady_a_blockEmergencia();

		pthread_mutex_lock(&mutex_colaBlockEmergency);
		t_tcb* tripulante = retornarMasCercano(posicion_x, posicion_y);
		pthread_mutex_unlock(&mutex_colaBlockEmergency);

		cambiarEstado('Y', tripulante, cliente_ram);
		mensajeIniciarSabotaje(tripulante->tid, cliente_mongo);
		logger_ejecutarSabotaje(PATH_LOG, tripulante->tid, posicion_x, posicion_y);
		while(!((tripulante->pos_x == posicion_x) && (tripulante->pos_y == posicion_y))) {

			if( tripulante->pos_x < posicion_x ) {

				mensajeDesplazamiento(tripulante->tid, tripulante->pos_x, tripulante->pos_y, (tripulante->pos_x + 1), tripulante->pos_y, cliente_mongo);
				tripulante->pos_x += 1;
				sleep(RETARDO_CICLO_CPU);
			}
			else if( tripulante->pos_x > posicion_x ) {

				mensajeDesplazamiento(tripulante->tid, tripulante->pos_x, tripulante->pos_y, (tripulante->pos_x - 1), tripulante->pos_y, cliente_mongo);
				tripulante->pos_x -= 1;
				sleep(RETARDO_CICLO_CPU);
			}
			else if( tripulante->pos_y < posicion_y ) {

				mensajeDesplazamiento(tripulante->tid, tripulante->pos_x, tripulante->pos_y, tripulante->pos_x, (tripulante->pos_y + 1), cliente_mongo);
				tripulante->pos_y += 1;
				sleep(RETARDO_CICLO_CPU);
			}
			else if( tripulante->pos_y > posicion_y ) {

				mensajeDesplazamiento(tripulante->tid, tripulante->pos_x, tripulante->pos_y, tripulante->pos_x, (tripulante->pos_y - 1), cliente_mongo);
				tripulante->pos_y -= 1;
				sleep(RETARDO_CICLO_CPU);
			}
			mensajePosicionTripulante(tripulante->tid, tripulante->pos_x, tripulante->pos_y, cliente_ram);
			logger_posicionTripulante(PATH_LOG, tripulante->tid, tripulante->pos_x, tripulante->pos_y);
		}
		sleep(DURACION_SABOTAJE);
		logger_finalizarSabotaje(PATH_LOG, tripulante->tid);
		mensajeFinalizarSabotaje(tripulante->tid, cliente_mongo);

		pthread_mutex_lock(&mutex_colaBlockEmergency);
		list_add(colaBlockEmergency, (void*) tripulante);
		pthread_mutex_unlock(&mutex_colaBlockEmergency);

		centinela_emergencia = 0;
		deBlockEmergencia_a_ready();
		reactivarMultitarea();

		close(cliente_mongo);
		close(cliente_ram);
	}
}

void sabotajeIniciado(t_sabotaje* ubicacion, int socket_sabotaje) {

	centinela_emergencia = 1;

	while(!list_is_empty(colaExec) || !centinela_emergencia)
		sleep(1);
	sleep(3);

	for(int i=0; i<GRADO_MULTITAREA; i++) {
		sem_wait(&sem_block_execute);
	}

	int cliente_ram = crearCliente(IP_MI_RAM_HQ, PUERTO_MI_RAM_HQ);

	pthread_mutex_lock(&mutex_colaBlockEmergency);
	ordenarPorTID(colaBlockEmergency);
	pthread_mutex_unlock(&mutex_colaBlockEmergency);
	deReady_a_blockEmergencia();

	pthread_mutex_lock(&mutex_colaBlockEmergency);
	t_tcb* tripulante = retornarMasCercano(ubicacion->posicion_x, ubicacion->posicion_y);
	pthread_mutex_unlock(&mutex_colaBlockEmergency);

	cambiarEstado('Y', tripulante, cliente_ram);
	mensajeIniciarSabotaje(tripulante->tid, socket_sabotaje);
	logger_ejecutarSabotaje(PATH_LOG, tripulante->tid, ubicacion->posicion_x, ubicacion->posicion_y);
	while(!((tripulante->pos_x == ubicacion->posicion_x) && (tripulante->pos_y == ubicacion->posicion_y))) {

		if( tripulante->pos_x < ubicacion->posicion_x ) {

			mensajeDesplazamiento(tripulante->tid, tripulante->pos_x, tripulante->pos_y, (tripulante->pos_x + 1), tripulante->pos_y, socket_sabotaje);
			tripulante->pos_x += 1;
			sleep(RETARDO_CICLO_CPU);
		}
		else if( tripulante->pos_x > ubicacion->posicion_x ) {

			mensajeDesplazamiento(tripulante->tid, tripulante->pos_x, tripulante->pos_y, (tripulante->pos_x - 1), tripulante->pos_y, socket_sabotaje);
			tripulante->pos_x -= 1;
			sleep(RETARDO_CICLO_CPU);
		}
		else if( tripulante->pos_y < ubicacion->posicion_y ) {

			mensajeDesplazamiento(tripulante->tid, tripulante->pos_x, tripulante->pos_y, tripulante->pos_x, (tripulante->pos_y + 1), socket_sabotaje);
			tripulante->pos_y += 1;
			sleep(RETARDO_CICLO_CPU);
		}
		else if( tripulante->pos_y > ubicacion->posicion_y ) {

			mensajeDesplazamiento(tripulante->tid, tripulante->pos_x, tripulante->pos_y, tripulante->pos_x, (tripulante->pos_y - 1), socket_sabotaje);
			tripulante->pos_y -= 1;
			sleep(RETARDO_CICLO_CPU);
		}
		mensajePosicionTripulante(tripulante->tid, tripulante->pos_x, tripulante->pos_y, cliente_ram);
		logger_posicionTripulante(PATH_LOG, tripulante->tid, tripulante->pos_x, tripulante->pos_y);
	}
	mensajeEjecutarSabotaje(socket_sabotaje);
	sleep(DURACION_SABOTAJE);
	logger_finalizarSabotaje(PATH_LOG, tripulante->tid);
	mensajeFinalizarSabotaje(tripulante->tid, socket_sabotaje);
	pthread_mutex_lock(&mutex_colaBlockEmergency);
	list_add(colaBlockEmergency, (void*) tripulante);
	pthread_mutex_unlock(&mutex_colaBlockEmergency);

	centinela_emergencia = 0;
	deBlockEmergencia_a_ready();
	reactivarMultitarea();

	free(ubicacion);
	close(cliente_ram);
}

void reactivarMultitarea() {

	for(int i=0; i<GRADO_MULTITAREA; i++) {
		sem_post(&sem_multitarea);
	}
}

t_tcb* retornarMasCercano(int sabotaje_x, int sabotaje_y) {

	t_link_element* elementoLista = colaBlockEmergency->head;
	t_tcb* tripulante;
	t_tcb* auxTripulante;
	int distanciaMenor = 100;

	for(int i=0; i<colaBlockEmergency->elements_count; i++){

		auxTripulante = (t_tcb*) elementoLista->data;
		int distancia = calcularDistancia(auxTripulante->pos_x, auxTripulante->pos_y, sabotaje_x, sabotaje_y);

		if(distancia < distanciaMenor) {

			distanciaMenor = distancia;
			tripulante = auxTripulante;
		}

		elementoLista = elementoLista->next;
	}

	tripulante = devolverRemoviendoTripulante(tripulante->tid, colaBlockEmergency);
	return tripulante;
}

double calcularDistancia(int x1, int y1, int x2, int y2) {

    return sqrt(pow(x1 - x2, 2) + pow(y1 - y2, 2));
}

void ordenarPorTID(t_list* lista) {

	bool ordenar(void* elem1, void* elem2) {
		t_tcb* tripulante1 = (t_tcb*) elem1;
		t_tcb* tripulante2 = (t_tcb*) elem2;
		return tripulante1->tid < tripulante2->tid;
	}

	list_sort(lista, ordenar);
}

char* leer_archivo_tareas(char* path){

	char* ruta = malloc(10 + strlen(path));
	sprintf(ruta, "./tareas/%s", path);

	FILE* archivo = fopen(ruta,"rt");
	if(archivo == NULL){
		perror("ERROR: NO EXISTE LA RUTA INDICADA");
		free(ruta);
		return "";
	}

	fseek(archivo,0L,SEEK_END);
	int size = ftell(archivo);
	fseek(archivo,0L,SEEK_SET);
	char* string_a_devolver = malloc(size+1);
	strcpy(string_a_devolver, "");

	char* token = malloc(60);
	int cont = 0;

	while(!feof(archivo)) {

		if(cont == 1)
			strcat(string_a_devolver, " ");
		else if(cont == 2){
			strcat(string_a_devolver, "|");
			cont = 0;
		}
		cont++;
		fscanf(archivo,"%s\n",token);
		strcat(string_a_devolver, token);
	}

	free(ruta);
	free(token);
	fclose(archivo);
	return string_a_devolver;
}



