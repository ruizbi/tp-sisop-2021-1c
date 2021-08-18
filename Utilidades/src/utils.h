
#ifndef UTILS_H_
#define UTILS_H_

#include <stdio.h>
#include <stdlib.h>
#include <commons/config.h>

#include "conexiones.h"

typedef enum {

	MODIFICAR_POSICION,
	DESPLAZAMIENTO,
	SABOTAJE,
	CAMBIAR_ESTADO,
	INICIAR_PATOTA,
	INICIAR_TAREA,
	INICIAR_TRIPULANTES,
	INICIAR_IO,
	EJECUTAR_SABOTAJE,
	FINALIZAR_TAREA,
	FINALIZAR_IO,
	SOLICITAR_TAREA,
	SOLICITAR_BITACORA,
	EXPULSAR_TRIPULANTE,
	INICIAR_SABOTAJE,
	FINALIZAR_SABOTAJE
} t_opcode;

typedef struct {

	uint32_t size;
	void* stream;
} t_buffer;

typedef struct {

	uint8_t codigo_operacion;
	t_buffer* buffer;
} t_paquete;

typedef struct {

	uint32_t tid;
	uint32_t origen_x;
	uint32_t origen_y;
	uint32_t destino_x;
	uint32_t destino_y;
} t_desplazamiento;

typedef struct {

	uint32_t posicion_x;
	uint32_t posicion_y;
} t_sabotaje;

typedef struct {

	uint32_t tid;
	uint32_t pid;
} t_solicitudes_varias;

typedef struct {

	uint32_t tid;
	char estado;
} t_estado;

typedef struct {

	uint32_t pid;
	uint32_t tid;
	uint32_t len_string;
	char* string;
} t_mensaje_string;

typedef struct {

	uint32_t parametros;
	uint32_t tid;
	uint32_t len_string;
	char* string;
} t_tarea_io;

typedef struct {

	uint32_t pid;
	uint32_t cantidad_tripulantes;
	uint32_t len_posiciones;
	char* posiciones;
} t_iniciar_patota;

t_desplazamiento* fill_desplazamiento(uint32_t origen_x, uint32_t origen_y, uint32_t destino_x, uint32_t destino_y, uint32_t tid);
t_sabotaje* fill_sabotaje(uint32_t posicion_x, uint32_t posicion_y);
t_solicitudes_varias* fill_solicitudes_varias(uint32_t tid, uint32_t pid);
t_estado* fill_estado(uint32_t tid, char status);
t_mensaje_string* fill_mensaje_string(uint32_t pid, uint32_t tid, char* mensaje_string);
t_tarea_io* fill_tarea_io(uint32_t tid, uint32_t parametros, char* nombre_tarea);
t_iniciar_patota* fill_iniciar_tripulantes(uint32_t pid, uint32_t cantidad_tripulantes, char* posiciones);

t_desplazamiento* deserializar_desplazamiento(t_buffer* buffer);
t_sabotaje* deserializar_sabotaje(t_buffer* buffer);
t_solicitudes_varias* deserializar_solicitudes_varias(t_buffer* buffer);
t_estado* deserializar_estado(t_buffer* buffer);
t_mensaje_string* deserializar_mensaje_string(t_buffer* buffer);
t_tarea_io* deserializar_tarea_io(t_buffer* buffer);
t_iniciar_patota* deserializar_iniciar_tripulantes(t_buffer* buffer);

t_buffer* serializar_desplazamiento(uint32_t origen_x, uint32_t origen_y, uint32_t destino_x, uint32_t destino_y, uint32_t tid);
t_buffer* serializar_sabotaje(uint32_t posicion_x, uint32_t posicion_y);
t_buffer* serializar_solicitudes_varias(uint32_t tid, uint32_t pid);
t_buffer* serializar_estado(uint32_t tid, char status);
t_buffer* serializar_mensaje_string(uint32_t pid, uint32_t tid, char* mensaje_string);
t_buffer* serializar_tarea_io(uint32_t tid, uint32_t parametros, char* nombre_tarea);
t_buffer* serializar_iniciar_tripulantes(uint32_t pid, uint32_t cantidad_tripulantes, char* posiciones);

void enviar_paquete(t_buffer* buffer, uint8_t codop, int socket);
void recibir_paquete(int socket);

#endif /* UTILS_H_ */
