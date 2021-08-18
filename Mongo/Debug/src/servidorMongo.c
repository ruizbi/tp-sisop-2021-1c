/*
 * conexiones.c
 *
 *  Created on: 19 jun. 2021
 *      Author: utnso
 */

#include "servidorMongo.h"
void crearServerMongo() {
	void (*fptr)(int);
	fptr = recibir_paquete_mongo2;
	crearServerConPuntero(PUERTO_MONGO_STORE, BACKLOG, fptr);
	pthread_exit(NULL);
}
void recibir_paquete_mongo2(int socket) {

//	t_paquete* paquete = malloc(sizeof(t_paquete));
//	paquete->buffer = malloc(sizeof(t_buffer));

	int loop = 1;

	while (loop) {

		t_paquete* paquete = malloc(sizeof(t_paquete));
			paquete->buffer = malloc(sizeof(t_buffer));

		int bytes = recv(socket, &(paquete->codigo_operacion), sizeof(uint8_t),
				0);

		if (bytes < 0) {
			printf("ERROR EN EL CODOP\n");
			loop = 0;
			break;
		} else if (bytes == 0) {
			printf("SE DESCONECTO EL CLIENTE\n");
			loop = 0;
			break;
		} else
			printf("CODOP: %d \n", paquete->codigo_operacion);

		bytes = recv(socket, &(paquete->buffer->size), sizeof(uint32_t), 0);

		if (bytes < 0) {
			printf("ERROR EN EL SIZE DEL BUFFER\n");
			loop = 0;
			break;
		} else if (bytes == 0) {
			printf("SE DESCONECTO EL CLIENTE\n");
			loop = 0;
			break;
		} else
			printf("BUFFER SIZE: %d \n", paquete->buffer->size);

		paquete->buffer->stream = malloc(paquete->buffer->size);

		bytes = recv(socket, paquete->buffer->stream, paquete->buffer->size, 0);

		if (bytes < 0) {
			printf("ERROR EN RECIBIR EL STREAM\n");
			loop = 0;
			break;
		} else if (bytes == 0) {
			printf("SE DESCONECTO EL CLIENTE\n");
			loop = 0;
			break;
		}

		switch (paquete->codigo_operacion) {

		t_buffer* buffer;
		t_desplazamiento* desplazamiento;
		t_sabotaje* sabotaje;
		t_solicitudes_varias* solicitud;
		t_mensaje_string* mensaje;
		t_tarea_io* tarea;
		char* accion;
	case DESPLAZAMIENTO:
		accion = string_new();
		desplazamiento = deserializar_desplazamiento(paquete->buffer);
		string_append_with_format(&accion, "Desplazamiento de %d|%d a %d|%d\n",
				desplazamiento->origen_x, desplazamiento->origen_y,
				desplazamiento->destino_x, desplazamiento->destino_y);
		escribirBitacora(desplazamiento->tid, accion);
		free(desplazamiento);
		free(accion);
		break;

	case EJECUTAR_SABOTAJE:
		solicitud = deserializar_solicitudes_varias(paquete->buffer);
		buffer = serializar_solicitudes_varias(0, 0);
		//sleep(10); // reemplazar VerificarFS
		repararMongo();
		enviar_paquete(buffer, EJECUTAR_SABOTAJE, socket);
		free(solicitud);
		break;

	case INICIAR_IO:
		tarea = deserializar_tarea_io(paquete->buffer);
//		printf("TAREA:%s PARAMETROS:%d TRIPULANTE:%d\n", tarea->string,
//				tarea->parametros, tarea->tid);
		iniciarTarea(tarea->string,tarea->parametros);
		buffer = serializar_solicitudes_varias(0, 0);
		enviar_paquete(buffer, FINALIZAR_IO, socket);
		free(tarea->string);
		free(tarea);
		break;

	case SOLICITAR_BITACORA:
		solicitud = deserializar_solicitudes_varias(paquete->buffer);
		//printf("SOLICITUD DE BITACORA TRIPULANTE %d\n", solicitud->tid);
		accion = buscarBitacora(solicitud->tid);
		buffer = serializar_mensaje_string(solicitud->pid, solicitud->tid,
				accion);
		enviar_paquete(buffer, SOLICITAR_BITACORA, socket);
		free(solicitud);
		free(accion);
		break;

	case INICIAR_SABOTAJE:
		accion = string_new();
		solicitud = deserializar_solicitudes_varias(paquete->buffer);
		string_append(&accion, "Atender un sabotaje\n");
		escribirBitacora(solicitud->tid, accion);
		//		printf("EL TRIPULANTE %d INICIO EL SABOTAJE\n", solicitud->tid);
		free(solicitud);
		free(accion);
		break;

	case FINALIZAR_SABOTAJE:
		accion = string_new();
		solicitud = deserializar_solicitudes_varias(paquete->buffer);
		string_append(&accion, "Resolucion del sabotaje\n");
		escribirBitacora(solicitud->tid, accion);
		//printf("EL FINALIZO EL SABOTAJE\n", solicitud->tid);
		free(solicitud);
		free(accion);
		break;

	case INICIAR_TAREA:
		mensaje = deserializar_mensaje_string(paquete->buffer);
		accion = string_new();
		string_append_with_format(&accion, "Inicio de tarea %s\n",
				mensaje->string);

//		accion = malloc(31 + mensaje->len_string);
//		strcpy(accion, "");
//		sprintf(accion, "EL TRIPULANTE INICIA LA TAREA %s", mensaje->string);
//		printf("%s\n", accion);
		escribirBitacora(mensaje->tid, accion);
		free(mensaje->string);
		free(mensaje);
		free(accion);
		break;

	case FINALIZAR_TAREA:
		accion = string_new();
		mensaje = deserializar_mensaje_string(paquete->buffer);
		string_append_with_format(&accion, "Fin de tarea %s\n",
				mensaje->string);
		escribirBitacora(mensaje->tid, accion);
		free(mensaje->string);
		free(mensaje);
		free(accion);
		break;
		}

		free(paquete->buffer->stream);
		free(paquete->buffer);
		free(paquete);
	}

//	free(paquete->buffer->stream);
//	free(paquete->buffer);
//	free(paquete);
	close(socket);
}


void handler_sabotaje() {

	pthread_t hilo;
	if(pthread_create(&hilo, NULL, (void*) servidorSabotaje, NULL) < 0)
		perror("ERROR: NO SE PUDO CREAR EL HILO DE SABOTAJE POR SIGNAL");
	pthread_join(hilo, NULL);
}

void servidorSabotaje() {

	if(list_is_empty(lista_sabotajes)) {
		perror("ERROR: NO HAY MAS SABOTAJES PENDIENTES");
	}
	else {

		int cliente = crearCliente(IP_MONGO_STORE, PUERTO_SABOTAJE);

		t_sabotaje* sabotaje = list_remove(lista_sabotajes, 0);

		t_buffer* buffer = serializar_sabotaje(sabotaje->posicion_x, sabotaje->posicion_y);
		enviar_paquete(buffer, SABOTAJE, cliente);
		free(sabotaje);

		int loop = 1;

		t_paquete* paquete = malloc(sizeof(t_paquete));
		paquete->buffer = malloc(sizeof(t_buffer));

		while(loop) {

//			t_paquete* paquete = malloc(sizeof(t_paquete));
//			paquete->buffer = malloc(sizeof(t_buffer));

			int bytes = recv(cliente, &(paquete->codigo_operacion), sizeof(uint8_t), 0);

			if(bytes < 0) {
				printf("ERROR EN EL CODOP\n");
				loop = 0;
				break;
			}
			else if(bytes == 0) {
				printf("SE DESCONECTO EL CLIENTE\n");
				loop = 0;
				break;
			}
			else
				printf("CODOP: %d \n", paquete->codigo_operacion);

			bytes = recv(cliente, &(paquete->buffer->size), sizeof(uint32_t), 0);

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

			bytes = recv(cliente, paquete->buffer->stream, paquete->buffer->size, 0);

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

			t_solicitudes_varias* solicitud;
			t_desplazamiento* desplazamiento;
			t_buffer* buffer;
			char* accion;

				case EJECUTAR_SABOTAJE:
					solicitud = deserializar_solicitudes_varias(paquete->buffer);
					buffer = serializar_solicitudes_varias(0, 0);
					//sleep(10); // reemplazar VerificarFS
//					sem_wait(&mutexBitacora);
					repararMongo();
//					sem_post(&mutexBitacora);
					enviar_paquete(buffer, EJECUTAR_SABOTAJE, cliente);
					free(solicitud);
				break;

				case INICIAR_SABOTAJE:
					accion = string_new();
					solicitud = deserializar_solicitudes_varias(paquete->buffer);
					string_append(&accion, "Atender un sabotaje\n");
					escribirBitacora(solicitud->tid, accion);
					//		printf("EL TRIPULANTE %d INICIO EL SABOTAJE\n", solicitud->tid);
					free(solicitud);
					free(accion);
					break;

				case FINALIZAR_SABOTAJE:
					accion = string_new();
					solicitud = deserializar_solicitudes_varias(paquete->buffer);
					string_append(&accion, "Resolucion del sabotaje\n");
					escribirBitacora(solicitud->tid, accion);
					//printf("EL FINALIZO EL SABOTAJE\n", solicitud->tid);
					free(solicitud);
					free(accion);
					break;

				case DESPLAZAMIENTO:
					accion = string_new();
					desplazamiento = deserializar_desplazamiento(paquete->buffer);
					string_append_with_format(&accion, "Desplazamiento de %d|%d a %d|%d\n",
							desplazamiento->origen_x, desplazamiento->origen_y,
							desplazamiento->destino_x, desplazamiento->destino_y);
					escribirBitacora(desplazamiento->tid, accion);
					free(desplazamiento);
					free(accion);
					break;
			}

		}

		free(paquete->buffer->stream);
		free(paquete->buffer);
		free(paquete);

		close(cliente);
	}

}

