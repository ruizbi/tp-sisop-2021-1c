/*
 * conexiones.h
 *
 *  Created on: 2 mar. 2019
 *      Author: utnso
 */

#ifndef UTILSRAM_H_
#define UTILSRAM_H_

#include<stdio.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<unistd.h>
#include<netdb.h>
#include<commons/log.h>
#include<commons/config.h>
#include<commons/collections/list.h>
#include<string.h>
#include<pthread.h>
#include <commons/string.h>
#include <conexiones.h>
#include <utils.h>

//#include "./memoriaHQ.h"

typedef enum
{
	PCB,
	TCB,
	TAREAS
}t_tipo_estructura;

typedef struct
{
	uint32_t pid;
	uint32_t puntero_tareas;
}t_pcb;

typedef struct
{
	uint32_t tid;
	char estado;
	uint32_t pos_x;
	uint32_t pos_y;
	uint32_t prox_instruccion;
	uint32_t puntero_pcb;
}t_tcb;//A definir en el archivo de utils

pthread_t thread;

t_log* logger;
t_config* config;


typedef struct
{
	uint32_t id;
	uint32_t idHilo;
	uint32_t nro_segmento;
	uint32_t inicio;
	uint32_t tamanio;
	bool libre;
	bool esPatota;
	bool esTareas;
	bool terminado;
}t_segmento;

typedef struct
{
	uint32_t id_patota;
	t_list* lista_segmentos;
}t_tabla_segmento;

//paginacion
typedef struct
{
	uint32_t nro_pagina;
	uint32_t nro_frame;
	uint32_t tamanio;
	bool enMemoriaSecundaria;
	uint32_t nro_frame_secundario;
	uint32_t pid;
}t_pagina;

typedef struct
{
	uint32_t pid;
	uint32_t inicio_pcb;
	uint32_t inicio_tareas;
	uint32_t fin_tareas;
	uint32_t tamanio_tareas;
	uint32_t cantidad_tcbs_terminados;
	t_list* lista_paginas;
	t_list* lista_id_tripulantes;
}t_tabla_paginas;

typedef struct
{
	uint32_t nro_frame;
	int hora_modificada;
	int min_modificado;
	int seg_modificado;
	int mili_seg_modificado;
	bool libre;
	bool usado;
	bool en_uso;
}t_frame;


void* recibir_buffer(int*, int);

void iniciar_servidor(void);
void esperar_cliente(int);
void* recibir_mensaje(int socket_cliente, int* size);
int recibir_operacion(int);
void process_request(int cod_op, int cliente_fd);
void serve_client(int *socket);
void* serializar_paquete(t_paquete* paquete, int bytes);
void terminar_programa(t_log*, t_config*);

char* identificarMensaje(char* buffer);
char* crear_patota(uint32_t pid,char* pathTareas );
char* procesarInicioPatota(char* string);
char* procesarExpulsarTripulante(char* string);
char* eliminar_tripulante_de_memoria(uint32_t tid);
char* modificarPosicionTripulante(uint32_t tid, char* posicion);
char* procesarModificarPosicion(char* string);
char* procesarIniciarTripulante(char* string);
char* iniciarTripulante(uint32_t pid, uint32_t tid, char* posiciones);
char* procesarSolicitarTarea(char* string);
char* solicitarTarea(uint32_t pid,uint32_t tid);
char* procesarActualizarTripulante(char* string);
char* actualizarTripulante(uint32_t tid, char estado);
void recibir_paquete2(int socket);
void dump_memoria(void);

#endif /* UTILSRAM_H_ */
