/*
 * conexiones.c
 *
 *  Created on: 2 mar. 2019
 *      Author: utnso
 */

#include "utilsRam.h"

void recibir_paquete2(int socket) {

	t_paquete* paquete = malloc(sizeof(t_paquete));
	paquete->buffer = malloc(sizeof(t_buffer));

	int loop = 1;

	while(loop) {

		int bytes = recv(socket, &(paquete->codigo_operacion), sizeof(uint8_t), 0);

		if(bytes < 0) {
			printf("ERROR EN EL CODOP\n");
			loop = 0;
			break;
		}
		else if(bytes == 0) {
			loop = 0;
			break;
		}
		else
			printf("CODOP: %d \n", paquete->codigo_operacion);

		bytes = recv(socket, &(paquete->buffer->size), sizeof(uint32_t), 0);

		if(bytes < 0) {
			printf("ERROR EN EL SIZE DEL BUFFER\n");
			loop = 0;
			break;
		}
		else if(bytes == 0) {
			printf("SE DESCONECTO EL CLIENTE\n");
			loop = 0;
			break;
		}
		else
			printf("BUFFER SIZE: %d \n", paquete->buffer->size);

		paquete->buffer->stream = malloc(paquete->buffer->size);

		bytes = recv(socket, paquete->buffer->stream, paquete->buffer->size, 0);

		if(bytes < 0) {
			printf("ERROR EN RECIBIR EL STREAM\n");
			loop = 0;
			break;
		}
		else if(bytes == 0) {
			printf("SE DESCONECTO EL CLIENTE\n");
			loop = 0;
			break;
		}

		switch(paquete->codigo_operacion) {

		t_buffer* buffer;
		t_desplazamiento* desplazamiento;
		t_solicitudes_varias* solicitud;
		t_estado* estado;
		t_mensaje_string* mensaje;
		t_iniciar_patota* tripulantes;

			case MODIFICAR_POSICION:
				desplazamiento = deserializar_desplazamiento(paquete->buffer);
				modificarPosicionTripulante2(desplazamiento->tid, desplazamiento->destino_x, desplazamiento->destino_y);
				free(desplazamiento);
				break;

			case SOLICITAR_TAREA:
				solicitud = deserializar_solicitudes_varias(paquete->buffer);
				char* tarea = solicitarTarea(solicitud->pid,solicitud->tid);
				buffer = serializar_mensaje_string(solicitud->pid, solicitud->tid, tarea);
				enviar_paquete(buffer, SOLICITAR_TAREA, socket);
				free(solicitud);
				break;

			case CAMBIAR_ESTADO:
				estado = deserializar_estado(paquete->buffer);
				actualizarTripulante2(estado->tid, estado->estado);
				free(estado);
				break;

			case EXPULSAR_TRIPULANTE:
				solicitud = deserializar_solicitudes_varias(paquete->buffer);
				eliminar_tripulante_de_memoria2(solicitud->tid);
				free(solicitud);
				break;

			case INICIAR_TRIPULANTES:
				tripulantes= deserializar_iniciar_tripulantes(paquete->buffer);
				iniciarTripulante2(tripulantes->pid,tripulantes->cantidad_tripulantes,tripulantes->posiciones);
				free(tripulantes->posiciones);
				free(tripulantes);
				break;

			case INICIAR_PATOTA:
				mensaje = deserializar_mensaje_string(paquete->buffer);
				crear_patota2(mensaje->pid,mensaje->string);
				free(mensaje->string);
				free(mensaje);
				break;

//			case DUMP:
//				dump_memoria();
//			break;
		}
	}

	free(paquete->buffer->stream);
	free(paquete->buffer);
	free(paquete);
	close(socket);
}

//void process_request(int cod_op, int cliente_fd) {
//	int size;
//	void* msg = recibir_mensaje(cliente_fd, &size);
//	char* respuesta = string_new();
//		switch (cod_op) {
//		case INICIAR_PATOTA:
//			procesarInicioPatota((char*) msg);
//			free(msg);
//		break;
//		case INICIAR_TRIPULANTES:
//			procesarIniciarTripulante((char*) msg);
//			free(msg);
//		break;
//		case MODIFICAR_POSICION:
//			procesarModificarPosicion((char*) msg);
//			free(msg);
//		break;
//		case CAMBIAR_ESTADO:
//			procesarActualizarTripulante((char*) msg);
//			free(msg);
//		break;
//		case SOLICITAR_TAREA:
//			respuesta =procesarSolicitarTarea((char*) msg);
//			devolver_mensaje((void*)respuesta, string_length(respuesta), cliente_fd,SOLICITAR_TAREA);
//			free(msg);
//		break;
//		case EXPULSAR_TRIPULANTE:
//			procesarExpulsarTripulante((char*) msg);
//			free(msg);
//		break;
////		case DUMP:
////			dump_memoria();
////		break;
//		case -1:
//			pthread_exit(NULL);
//		}
//}

void terminar_programa(t_log* logger, t_config* config)
{
	log_destroy(logger);
	config_destroy(config);
	//Y por ultimo, para cerrar, hay que liberar lo que utilizamos (conexion, log y config) con las funciones de las commons y del TP mencionadas en el enunciado
}

char* procesarInicioPatota(char* string){
	//<PID> <tareas>
	char** parametros = string_n_split(string,2," ");
	volatile uint32_t pid  =(uint32_t) atoi(parametros[0]);
	char* tareas = crear_patota(pid,parametros[1]);
	return tareas;
}

char* procesarExpulsarTripulante(char* string){
	//<TID>
	volatile uint32_t tid = (uint32_t)atoi(string);

	return eliminar_tripulante_de_memoria(tid);
}

char* procesarModificarPosicion(char* string){
	//<TID> <X|Y>
	char** parametros = string_n_split(string,2," ");
	volatile uint32_t tid =(uint32_t)atoi(parametros[0]);
	return modificarPosicionTripulante(tid,parametros[1]);
}

char* procesarIniciarTripulante(char* string){
	//<PID> <cantidad> <X|Y X|Y (sin 0|0's)>
	char** parametros = string_n_split(string,3," ");
	volatile uint32_t pid= (uint32_t)atoi(parametros[0]);
	volatile uint32_t cantidad= (uint32_t)atoi(parametros[1]);
	char* posiciones = parametros[2];
	return iniciarTripulante(pid,cantidad,posiciones);
}

char* procesarSolicitarTarea(char* string){
	//<PID> <TID>
	char** parametros = string_n_split(string,2," ");
	volatile uint32_t pid= (uint32_t)atoi(parametros[0]);
		volatile uint32_t tid= (uint32_t)atoi(parametros[1]);
	return solicitarTarea(pid,tid);
}

char* procesarActualizarTripulante(char* string){
	//<TID> <status>
	char** parametros = string_n_split(string,2," ");
	volatile uint32_t tid = (uint32_t)atoi(parametros[0]);
	char status = *parametros[1];
	return actualizarTripulante(tid,status);
}
