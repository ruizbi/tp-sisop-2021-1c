
#include "./utils.h"

/*
 * MENSAJERIA
 */

void enviar_paquete(t_buffer* buffer, uint8_t codop, int socket) {

	t_paquete* paquete = malloc(sizeof(t_paquete));
	paquete->codigo_operacion = codop;
	paquete->buffer = buffer;

	int offset = 0;
	int tamanio_total_buffer = buffer->size + sizeof(uint8_t) + sizeof(uint32_t);
	void* a_enviar = malloc(tamanio_total_buffer);

	memcpy(a_enviar+offset, &(paquete->codigo_operacion), sizeof(uint8_t));
	offset+=sizeof(uint8_t);
	memcpy(a_enviar+offset, &(paquete->buffer->size), sizeof(uint32_t));
	offset+=sizeof(uint32_t);
	memcpy(a_enviar+offset, paquete->buffer->stream, paquete->buffer->size);

	send(socket, a_enviar, tamanio_total_buffer, 0);

	free(a_enviar);
	free(paquete->buffer->stream);
	free(paquete->buffer);
	free(paquete);
}

void recibir_paquete(int socket) {

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
			printf("SE DESCONECTO EL CLIENTE\n");
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
		t_sabotaje* sabotaje;
		t_solicitudes_varias* solicitud;
		t_estado* estado;
		t_mensaje_string* mensaje;
		t_tarea_io* tarea;
		t_iniciar_patota* tripulantes;

			case MODIFICAR_POSICION:
				desplazamiento = deserializar_desplazamiento(paquete->buffer);
				printf("DESTINO:%d|%d TRIPULANTE:%d\n", desplazamiento->destino_x, desplazamiento->destino_y, desplazamiento->tid);
				free(desplazamiento);
				break;

			case DESPLAZAMIENTO:
				desplazamiento = deserializar_desplazamiento(paquete->buffer);
				printf("ORIGEN:%d|%d DESTINO:%d|%d TRIPULANTE:%d\n", desplazamiento->origen_x, desplazamiento->origen_y, desplazamiento->destino_x, desplazamiento->destino_y, desplazamiento->tid);
				free(desplazamiento);
				break;

			case SABOTAJE:
				sabotaje = deserializar_sabotaje(paquete->buffer);
				printf("SABOTAJE EN %d|%d\n", sabotaje->posicion_x, sabotaje->posicion_y);
				sleep(5);
				buffer = serializar_solicitudes_varias(0, 0);
				enviar_paquete(buffer, EJECUTAR_SABOTAJE, socket);
				free(sabotaje);
				break;

			case SOLICITAR_TAREA:
				solicitud = deserializar_solicitudes_varias(paquete->buffer);
				printf("SOLICITUD TRIPULANTE %d DE PATOTA %d\n", solicitud->tid, solicitud->pid);
				buffer = serializar_mensaje_string(solicitud->pid, solicitud->tid, "EXIT 4;4;4;4");
				enviar_paquete(buffer, SOLICITAR_TAREA, socket);
				free(solicitud);
				break;

			case CAMBIAR_ESTADO:
				estado = deserializar_estado(paquete->buffer);
				printf("TRIPULANTE %d CAMBIO A %c\n", estado->tid, estado->estado);
				free(estado);
				break;

			case INICIAR_IO:
				tarea = deserializar_tarea_io(paquete->buffer);
				printf("TAREA:%s PARAMETROS:%d TRIPULANTE:%d\n", tarea->string, tarea->parametros, tarea->tid);
				buffer = serializar_solicitudes_varias(0, 0);
				enviar_paquete(buffer, FINALIZAR_IO, socket);
				free(tarea->string);
				free(tarea);
				break;

			case SOLICITAR_BITACORA:
				solicitud = deserializar_solicitudes_varias(paquete->buffer);
				printf("SOLICITUD DE BITACORA TRIPULANTE %d\n", solicitud->tid);
				buffer = serializar_mensaje_string(solicitud->pid, solicitud->tid, "ESTO ES UNA BITACORA");
				enviar_paquete(buffer, SOLICITAR_BITACORA, socket);
				free(solicitud);
				break;

			case EXPULSAR_TRIPULANTE:
				solicitud = deserializar_solicitudes_varias(paquete->buffer);
				printf("SE EXPULSO AL TRIPULANTE %d\n", solicitud->tid);
				free(solicitud);
				break;


			case INICIAR_TAREA:
				mensaje = deserializar_mensaje_string(paquete->buffer);
				printf("EL TRIPULANTE %d INICIA LA TAREA %s\n", mensaje->tid, mensaje->string);
				free(mensaje->string);
				free(mensaje);
				break;

			case FINALIZAR_TAREA:
				mensaje = deserializar_mensaje_string(paquete->buffer);
				printf("EL TRIPULANTE %d TERMINO LA TAERA %s\n", mensaje->tid, mensaje->string);
				free(mensaje->string);
				free(mensaje);
				break;

			case INICIAR_TRIPULANTES:
				tripulantes= deserializar_iniciar_tripulantes(paquete->buffer);
				printf("SE INICIAN %d TRIPULANTES DE LA PATOTA %d EN LAS POSICIONES: %s\n", tripulantes->cantidad_tripulantes, tripulantes->pid, tripulantes->posiciones);
				free(tripulantes->posiciones);
				free(tripulantes);
				break;

			case INICIAR_PATOTA:
				mensaje = deserializar_mensaje_string(paquete->buffer);
				printf("SE INICIA LA PATOTA %d CON LAS TAREAS: %s\n", mensaje->pid, mensaje->string);
				free(mensaje->string);
				free(mensaje);
				break;
		}
	}

	free(paquete->buffer->stream);
	free(paquete->buffer);
	free(paquete);
	close(socket);
}

/*
 * RELLENAR ESTRUCTURAS
 */

t_desplazamiento* fill_desplazamiento(uint32_t origen_x, uint32_t origen_y, uint32_t destino_x, uint32_t destino_y, uint32_t tid) {

	t_desplazamiento* desplazamiento = malloc(sizeof(uint32_t) * 5);
	desplazamiento->tid = tid;
	desplazamiento->origen_x = origen_x;
	desplazamiento->origen_y = origen_y;
	desplazamiento->destino_x = destino_x;
	desplazamiento->destino_y = destino_y;
	return desplazamiento;
}

t_sabotaje* fill_sabotaje(uint32_t posicion_x, uint32_t posicion_y) {

	t_sabotaje* sabotaje = malloc(sizeof(uint32_t) * 2);
	sabotaje->posicion_x = posicion_x;
	sabotaje->posicion_y = posicion_y;
	return sabotaje;
}

t_solicitudes_varias* fill_solicitudes_varias(uint32_t tid, uint32_t pid) {

	t_solicitudes_varias* solicitud = malloc(sizeof(uint32_t) * 2);
	solicitud->pid = pid;
	solicitud->tid = tid;
	return solicitud;
}

t_estado* fill_estado(uint32_t tid, char status) {

	t_estado* estado = malloc(sizeof(uint32_t) + sizeof(char));
	estado->tid = tid;
	estado->estado = status;
	return estado;
}

t_mensaje_string* fill_mensaje_string(uint32_t pid, uint32_t tid, char* mensaje_string) {

	int len_string = strlen(mensaje_string) + 1;
	t_mensaje_string* mensaje = malloc(sizeof(uint32_t)* 3 + len_string);
	mensaje->pid = pid;
	mensaje->tid = tid;
	mensaje->len_string = len_string;
	mensaje->string = mensaje_string;
	return mensaje;
}

t_tarea_io* fill_tarea_io(uint32_t tid, uint32_t parametros, char* nombre_tarea) {

	int len_string = strlen(nombre_tarea) + 1;
	t_tarea_io* tarea = malloc(sizeof(uint32_t)*3 + len_string);
	tarea->tid = tid;
	tarea->parametros = parametros;
	tarea->len_string = len_string;
	tarea->string = nombre_tarea;
	return tarea;
}

t_iniciar_patota* fill_iniciar_tripulantes(uint32_t pid, uint32_t cantidad_tripulantes, char* posiciones) {

	int len_string = strlen(posiciones) + 1;
	t_iniciar_patota* patota = malloc(sizeof(uint32_t)*3 + len_string);
	patota->pid = pid;
	patota->cantidad_tripulantes = cantidad_tripulantes;
	patota->len_posiciones = len_string;
	patota->posiciones = posiciones;
	return patota;
}
/*
 * SERIALIZAR ESTRUCTURAS
 */

t_buffer* serializar_desplazamiento(uint32_t origen_x, uint32_t origen_y, uint32_t destino_x, uint32_t destino_y, uint32_t tid) {

	t_desplazamiento* desplazamiento = fill_desplazamiento(origen_x, origen_y, destino_x, destino_y, tid);
	t_buffer* buffer = malloc(sizeof(t_buffer));
	buffer->size = sizeof(uint32_t) * 5;

	void* stream = malloc(buffer->size);
	int offset = 0;
	memcpy(stream+offset, &(desplazamiento->tid), sizeof(uint32_t));
	offset+=sizeof(uint32_t);
	memcpy(stream+offset, &(desplazamiento->origen_x), sizeof(uint32_t));
	offset+=sizeof(uint32_t);
	memcpy(stream+offset, &(desplazamiento->origen_y), sizeof(uint32_t));
	offset+=sizeof(uint32_t);
	memcpy(stream+offset, &(desplazamiento->destino_x), sizeof(uint32_t));
	offset+=sizeof(uint32_t);
	memcpy(stream+offset, &(desplazamiento->destino_y), sizeof(uint32_t));
	buffer->stream = stream;

	return buffer;
}

t_buffer* serializar_sabotaje(uint32_t posicion_x, uint32_t posicion_y) {

	t_sabotaje* sabotaje = fill_sabotaje(posicion_x, posicion_y);
	t_buffer* buffer = malloc(sizeof(t_buffer));
	buffer->size = sizeof(uint32_t) * 2;

	void* stream = malloc(buffer->size);
	int offset = 0;
	memcpy(stream+offset, &(sabotaje->posicion_x), sizeof(uint32_t));
	offset+=sizeof(uint32_t);
	memcpy(stream+offset, &(sabotaje->posicion_y), sizeof(uint32_t));
	buffer->stream = stream;

	return buffer;
}

t_buffer* serializar_solicitudes_varias(uint32_t tid, uint32_t pid) {

	t_solicitudes_varias* solicitud = fill_solicitudes_varias(tid, pid);
	t_buffer* buffer = malloc(sizeof(t_buffer));
	buffer->size = sizeof(uint32_t) * 2;

	void* stream = malloc(buffer->size);
	int offset = 0;
	memcpy(stream+offset, &(solicitud->tid), sizeof(uint32_t));
	offset+=sizeof(uint32_t);
	memcpy(stream+offset, &(solicitud->pid), sizeof(uint32_t));
	buffer->stream = stream;

	return buffer;
}

t_buffer* serializar_estado(uint32_t tid, char status) {

	t_estado* estado = fill_estado(tid, status);
	t_buffer* buffer = malloc(sizeof(t_buffer));
	buffer->size = sizeof(uint32_t) + sizeof(char);

	void* stream = malloc(buffer->size);
	int offset = 0;
	memcpy(stream+offset, &(estado->tid), sizeof(uint32_t));
	offset+=sizeof(uint32_t);
	memcpy(stream+offset, &(estado->estado), sizeof(char));
	buffer->stream = stream;

	return buffer;
}

t_buffer* serializar_mensaje_string(uint32_t pid, uint32_t tid, char* mensaje_string) {

	t_mensaje_string* mensaje = fill_mensaje_string(pid, tid, mensaje_string);
	t_buffer* buffer = malloc(sizeof(t_buffer));
	buffer->size = sizeof(uint32_t) * 3 + strlen(mensaje_string) + 1;
	void* stream = malloc(buffer->size);
	int offset = 0;

	memcpy(stream+offset, &(mensaje->pid), sizeof(uint32_t));
	offset+=sizeof(uint32_t);
	memcpy(stream+offset, &(mensaje->tid), sizeof(uint32_t));
	offset+=sizeof(uint32_t);
	memcpy(stream+offset, &(mensaje->len_string), sizeof(uint32_t));
	offset+=sizeof(uint32_t);
	memcpy(stream+offset, mensaje->string, (strlen(mensaje->string) + 1));
	buffer->stream = stream;

	return buffer;
}

t_buffer* serializar_tarea_io(uint32_t tid, uint32_t parametros, char* nombre_tarea) {

	t_tarea_io* tare = fill_tarea_io(tid, parametros, nombre_tarea);
	t_buffer* buffer = malloc(sizeof(t_buffer));
	buffer->size = sizeof(uint32_t) * 3 + strlen(nombre_tarea) + 1;
	void* stream = malloc(buffer->size);
	int offset = 0;

	memcpy(stream+offset, &(tare->tid), sizeof(uint32_t));
	offset+=sizeof(uint32_t);
	memcpy(stream+offset, &(tare->parametros), sizeof(uint32_t));
	offset+=sizeof(uint32_t);
	memcpy(stream+offset, &(tare->len_string), sizeof(uint32_t));
	offset+=sizeof(uint32_t);
	memcpy(stream+offset, tare->string, (strlen(tare->string) + 1));
	buffer->stream = stream;

	return buffer;
}

t_buffer* serializar_iniciar_tripulantes(uint32_t pid, uint32_t cantidad_tripulantes, char* posiciones) {

	t_iniciar_patota* patota = fill_iniciar_tripulantes(pid, cantidad_tripulantes, posiciones);
	t_buffer* buffer = malloc(sizeof(t_buffer));
	buffer->size = sizeof(uint32_t) * 3 + strlen(posiciones) + 1;
	void* stream = malloc(buffer->size);
	int offset = 0;

	memcpy(stream+offset, &(patota->pid), sizeof(uint32_t));
	offset+=sizeof(uint32_t);
	memcpy(stream+offset, &(patota->cantidad_tripulantes), sizeof(uint32_t));
	offset+=sizeof(uint32_t);
	memcpy(stream+offset, &(patota->len_posiciones), sizeof(uint32_t));
	offset+=sizeof(uint32_t);
	memcpy(stream+offset, patota->posiciones, (strlen(patota->posiciones) + 1));
	buffer->stream = stream;

	return buffer;
}
/*
 * DESERIALIZAR ESTRUCTURAS
 */

t_desplazamiento* deserializar_desplazamiento(t_buffer* buffer) {

	t_desplazamiento* desplazamiento = malloc(sizeof(uint32_t) * 5);
	void* stream = buffer->stream;

	memcpy(&(desplazamiento->tid), stream, sizeof(uint32_t));
	stream+=sizeof(uint32_t);
	memcpy(&(desplazamiento->origen_x), stream, sizeof(uint32_t));
	stream+=sizeof(uint32_t);
	memcpy(&(desplazamiento->origen_y), stream, sizeof(uint32_t));
	stream+=sizeof(uint32_t);
	memcpy(&(desplazamiento->destino_x), stream, sizeof(uint32_t));
	stream+=sizeof(uint32_t);
	memcpy(&(desplazamiento->destino_y), stream, sizeof(uint32_t));

	return desplazamiento;
}

t_sabotaje* deserializar_sabotaje(t_buffer* buffer) {

	t_sabotaje* sabotaje = malloc(sizeof(uint32_t) * 2);
	void* stream = buffer->stream;

	memcpy(&(sabotaje->posicion_x), stream, sizeof(uint32_t));
	stream+=sizeof(uint32_t);
	memcpy(&(sabotaje->posicion_y), stream, sizeof(uint32_t));

	return sabotaje;
}

t_solicitudes_varias* deserializar_solicitudes_varias(t_buffer* buffer) {

	t_solicitudes_varias* solicitud = malloc(sizeof(uint32_t) * 2);
	void* stream = buffer->stream;

	memcpy(&(solicitud->tid), stream, sizeof(uint32_t));
	stream+=sizeof(uint32_t);
	memcpy(&(solicitud->pid), stream, sizeof(uint32_t));

	return solicitud;
}

t_estado* deserializar_estado(t_buffer* buffer) {

	t_estado* estado = malloc(sizeof(uint32_t) + sizeof(char));
	void* stream = buffer->stream;

	memcpy(&(estado->tid), stream, sizeof(uint32_t));
	stream+=sizeof(uint32_t);
	memcpy(&(estado->estado), stream, sizeof(char));

	return estado;
}

t_mensaje_string* deserializar_mensaje_string(t_buffer* buffer) {

	t_mensaje_string* mensaje = malloc(sizeof(t_mensaje_string));
	void* stream = buffer->stream;

	memcpy(&(mensaje->pid), stream, sizeof(uint32_t));
	stream+=sizeof(uint32_t);
	memcpy(&(mensaje->tid), stream, sizeof(uint32_t));
	stream+=sizeof(uint32_t);
	memcpy(&(mensaje->len_string), stream, sizeof(uint32_t));
	stream+=sizeof(uint32_t);
	mensaje->string = malloc(mensaje->len_string);
	memcpy(mensaje->string, stream, mensaje->len_string);

	return mensaje;
}

t_tarea_io* deserializar_tarea_io(t_buffer* buffer) {

	t_tarea_io* tarea = malloc(sizeof(t_tarea_io));
	void* stream = buffer->stream;

	memcpy(&(tarea->tid), stream, sizeof(uint32_t));
	stream+=sizeof(uint32_t);
	memcpy(&(tarea->parametros), stream, sizeof(uint32_t));
	stream+=sizeof(uint32_t);
	memcpy(&(tarea->len_string), stream, sizeof(uint32_t));
	stream+=sizeof(uint32_t);
	tarea->string = malloc(tarea->len_string);
	memcpy(tarea->string, stream, tarea->len_string);

	return tarea;
}

t_iniciar_patota* deserializar_iniciar_tripulantes(t_buffer* buffer) {
	t_iniciar_patota* patota = malloc(sizeof(t_tarea_io));
	void* stream = buffer->stream;

	memcpy(&(patota->pid), stream, sizeof(uint32_t));
	stream+=sizeof(uint32_t);
	memcpy(&(patota->cantidad_tripulantes), stream, sizeof(uint32_t));
	stream+=sizeof(uint32_t);
	memcpy(&(patota->len_posiciones), stream, sizeof(uint32_t));
	stream+=sizeof(uint32_t);
	patota->posiciones = malloc(patota->len_posiciones);
	memcpy(patota->posiciones, stream, patota->len_posiciones);

	return patota;
}

