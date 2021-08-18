/*
 * memoriaHQ.h
 *
 *  Created on: 23 abr. 2021
 *      Author: utnso
 */

#ifndef MEMORIAHQ_H_
#define MEMORIAHQ_H_

#include<stdio.h>
#include<stdlib.h>
#include<commons/log.h>
#include<commons/string.h>
#include<commons/config.h>
#include<readline/readline.h>
#include<string.h>
#include <nivel-gui/nivel-gui.h>
#include <nivel-gui/tad_nivel.h>
#include<pthread.h>
#include<commons/temporal.h>
#include<commons/memory.h>
#include<commons/bitarray.h>
#include<sys/mman.h>
#include<fcntl.h>
#include<unistd.h>
#include "utilsRam.h"
#include <utils.h>
#include <conexiones.h>
#include <signal.h>
#include <unistd.h>
#define BACKLOG 10

#define ASSERT_CREATE(nivel, id, err)                                                   \
    if(err) {                                                                           \
        nivel_destruir(nivel);                                                          \
        nivel_gui_terminar();                                                           \
        fprintf(stderr, "Error al crear '%c': %s\n", id, nivel_gui_string_error(err));  \
        return EXIT_FAILURE;                                                            \
    }

NIVEL* nivel;

int cols;
int rows;
int err;

t_log* iniciar_logger(void);
t_config* leer_config(void);

char* ALGORITMO_MEMORIA;
char* TIPO_AJUSTE;


uint32_t contador_segmento;
uint32_t contador_id_tripulantes;
void* BLOQUE_MEMORIA;
int TAMANIO_MEMORIA;
int TAMANIO_MEMORIA_SECUNDARIA;
int TAMANIO_PAGINA;
int CANTIDAD_PAGINAS;
int CANTIDAD_PAGINAS_SECUNDARIA;
char* PATH_SWAP;
t_list* tabla_segmentos_general;
t_list* lista_tabla_segmentos;
//int archivo_memoria_secundaria;
int archivo_memoria_secundaria;
void* BLOQUE_MEMORIA_SECUNDARIA;
char* ALGORITMO_REEMPLAZO;
//espacio para semaforos
pthread_mutex_t mutex_contador_segmentos = PTHREAD_MUTEX_INITIALIZER;//
pthread_mutex_t mutex_tabla_segmento_general = PTHREAD_MUTEX_INITIALIZER;//
pthread_mutex_t mutex_lista_tabla_segmentos = PTHREAD_MUTEX_INITIALIZER;//
pthread_mutex_t mutex_bloque_memoria = PTHREAD_MUTEX_INITIALIZER; //
pthread_mutex_t mutex_logger = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_mapa = PTHREAD_MUTEX_INITIALIZER;//
pthread_mutex_t mutex_contador_id_tripulantes = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_dump = PTHREAD_MUTEX_INITIALIZER;


//paginacion
pthread_mutex_t mutex_lista_tabla_paginas = PTHREAD_MUTEX_INITIALIZER; //
pthread_mutex_t mutex_lista_paginas_principal_general = PTHREAD_MUTEX_INITIALIZER;//
pthread_mutex_t mutex_lista_paginas_secundaria_general = PTHREAD_MUTEX_INITIALIZER;//
pthread_mutex_t mutex_bloque_memoria_secundario = PTHREAD_MUTEX_INITIALIZER;//



//paginacion
int POSICION_ACTUAL_FRAME;
t_list* lista_tabla_paginas;
t_list* lista_paginas_principal_general;
t_list* lista_paginas_secundaria_general;
void aplicar_paginacion_patota(t_pcb* pcb);
void crear_tabla_paginas_pcb(uint32_t pid,void* pcb_serializado );
void* guardarEnFrame(uint32_t numero_frame,void* pcb_serializado,t_pagina* pagina, int* puedeRecorrer, int* tamanio);
t_pagina* crear_pagina(uint32_t numeroPagina, uint32_t numeroFrame);
t_frame* crear_frame_libre(uint32_t numero);
void actualizar_frame(t_frame* frame);
void guardar_body_de_paginas_en_memoria(t_tabla_paginas* tabla_pagina,void* body,int tamanio);
t_tabla_paginas* buscar_tabla_paginas(uint32_t pid);
void aplicar_paginacion_tareas(char* tareas,uint32_t pid, int tamanio);
void aplicar_paginacion_tripulante(t_tcb* tcb,uint32_t pid);
void cargar_frame_a_memoria_principal(t_pagina* pagina,t_frame* frame);
void eliminar_tripulante_de_tabla_paginas(uint32_t tid);
int existe_espacio_disponible_en_memoria(uint32_t peso);
void iniciar_memoria_secundaria(void);
void copiar_pagina(t_pagina* origen, t_pagina* destino);
int mover_pagina_a_memoria_secundaria_lru(void);
void guardar_pagina_en_memoria_secundaria(t_pagina* pagina_a_mover);
bool fechaPivoteEsMasAntiguoQueFechaDeFrame(int hora,int minuto,int segundo,int milisegundo,t_frame* frame);
void* obtenerStreamDeMemoriaPrincipal(t_pagina* pagina);
void guardar_stream_en_memoria_secundaria(t_pagina* pagina, void* stream);
void guardarEnFrameSecundario(uint32_t frame_secundario,void* stream, t_pagina* pagina,int* guardado);
void obtenerStreamDeArchivo(void);
void mover_frame_a_memoria_secundaria(t_frame* frame);
void limpiar_ultimas_n_paginas_lru(int cantidadPaginasALimpiar);
int buscar_frame__a_reemplazar_clock(void);
void* obtener_stream_de_pagina_en_memoria_principal(t_pagina* pagina,bool debeManeterseEnMemoriaPrincipal);
char* buscar_tareas_paginacion(uint32_t pid);
void chequear_paginas_en_memoria_principal(t_tabla_paginas* tabla, int inicio, int limite);
void* buscar_elemento_en_memoria_principal(t_tabla_paginas* tabla,t_tipo_estructura tipo_estructura,uint32_t tid,bool debeManetenerseEnMemoriaPrincipal);
t_tcb* buscar_tcb_paginacion(uint32_t tid);
void guardar_tripulante_paginacion(t_tcb* tcb);
t_tabla_paginas* obtener_tabla_paginas_de_tid(uint32_t tid);
void* obtener_stream_de_MS(t_pagina* pagina);
void guardar_stream_en_frame_de_MP(uint32_t nro_frame, void* stream,uint32_t tamanio);
void traer_pagina_a_memoria_principal(t_pagina* pagina);
void limpiar_ultimas_n_paginas_clock(int cantidadPaginasALimpiar);
int buscar_posicion_de_tcb_en_tabla(t_tabla_paginas* tabla, uint32_t tid);
void liberar_frame(t_frame* frame);
void procesar_string_paginacion(char* hora,FILE* archivo);
void verificar_tripulantes_de_tabla_paginas(uint32_t tid, t_tabla_paginas* tabla);
void liberar_en_uso_frames_de_tabla_paginas(t_tabla_paginas* tabla, int inicio,int limite);
t_pagina* buscar_pagina_que_use_frame(uint32_t nro_frame);
void eliminar_tabla_de_paginas(uint32_t pid);
mover_pagina_a_memoria_secundaria_clock(void);

void crear_patota2(uint32_t pid,char* tareas );
void eliminar_tripulante_de_memoria2(uint32_t tid);
void modificarPosicionTripulante2(uint32_t tid, uint32_t posicion_x, uint32_t posicion_y);
void iniciarTripulante2(uint32_t pid, uint32_t cantidad, char* posiciones);
void actualizarTripulante2(uint32_t tid, char estado);
void crearServerDiscordiador();

void iniciarListas(void);
void reservar_memoria(void);
void iniciar_mapa(void);
void mapear_tripulante(int posx,int posy,int pid);
char* crear_patota(uint32_t pid,char* pathTareas );
void hardcodeo_patota(void);
char* eliminar_tripulante(uint32_t tid);
bool buscar_y_eliminar_tripulante(uint32_t tid);
t_tcb* crear_estructura_tcb(uint32_t id,char* posiciones,uint32_t pcb);
t_pcb* crear_estructura_pcb(uint32_t pid,uint32_t direccionTareas);
void registrarTripulante(t_tcb* tripulante,uint32_t pid);
void registrarPatota(t_pcb* patota);
void registrarTareas(char* tareas,uint32_t pid);
t_segmento* crear_segmento(int inicio,int tamanio,bool esLibre);
t_tabla_segmento* crear_tabla_segmentos(t_segmento* segmento_patota);
void buscar_y_modificar_tabla_segmento(t_segmento* segmento);
void listar_en_tabla_segmentos(t_segmento* segmento);
t_segmento* crear_segmento_patota(t_pcb* patota);
char* modificarPosicionTripulante(uint32_t tid, char* posicion);
bool ocupar_segmento_libre(t_segmento* segmento);
void* serializar_patota(t_pcb* pcb);
void* serializar_tripulante(t_tcb* tcb);
void guardar_patota_en_memoria(t_pcb* pcb,uint32_t inicio);
void guardar_tripulante_en_memoria(t_tcb* tcb,uint32_t inicio);
void guardar_tareas_en_memoria(char* tareas,uint32_t inicio,uint32_t tamanio);
t_segmento* crear_segmento_tareas(char* tareas,uint32_t pid);
t_segmento* crear_segmento_tripulante(t_tcb* tripulante, uint32_t pid);
char* eliminar_tripulante_de_memoria(uint32_t tid);
t_tcb* buscar_tripulante_segmentacion(uint32_t tid);
t_tcb* obtener_tcb_de_memoria(uint32_t inicio, uint32_t tamanio);
void eliminar_segmento_de_tabla_segmento_de_patota(uint32_t numeroSegmento);
bool aplicar_segmentacion(t_segmento* segmento);
void eliminar_segmento(void* segmento);

char* iniciarTripulante(uint32_t pid, uint32_t tid, char* posiciones);
t_pcb* buscar_patota_segmentacion(uint32_t pid);
t_pcb* obtener_pcb_de_memoria(uint32_t inicio, uint32_t tamanio);
t_pcb* buscar_patota_de_memoria_con_pid(uint32_t pid);

char* solicitarTarea(uint32_t pid,uint32_t tid);
char* obtener_tareas_de_memoria(uint32_t inicio, uint32_t tamanio);
t_segmento* buscar_segmento_tareas_con_pid(uint32_t pid);
char* buscar_tareas_segmentacion(uint32_t pid);

void eliminar_tripulante_en_mapa(uint32_t tid);
void mover_tripulante_en_mapa(uint32_t tid,uint32_t pos_x,uint32_t pos_y);
void verificar_tripulantes_de_tabla_segmentos(uint32_t pid);

char* actualizarTripulante(uint32_t tid, char estado);

void compactacion(void);

void dump_memoria(void);
void dump_memoria_segmentacion(void);
void dump_memoria_paginacion(void);
void procesar_string_segmentacion(char* hora,FILE* archivo);
//PAGINACION

t_list* tabla_paginas_general;
t_list* lista_tabla_frames;

#endif /* MEMORIAHQ_H_ */
