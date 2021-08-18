
#ifndef DISCORDIADOR_H_
#define DISCORDIADOR_H_

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <utils.h>
#include <commons/config.h>
#include <pthread.h>
#include <commons/collections/list.h>
#include <commons/collections/queue.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <string.h>
#include <commons/temporal.h>
#include <semaphore.h>
#include <commons/log.h>
#include<sys/stat.h>

#include <conexiones.h>
#include "logsDiscordiador.h"

#define PUERTO_SABOTAJE 5059
#define BACKLOG 10
#define PATH_CONFIG_DISCORDIADOR "discordiador.config"
#define PATH_LOG_DISCORDIADOR "discordiador.log"

typedef struct {

	char* nombreTarea;
	uint32_t parametro;
	uint32_t pos_x;
	uint32_t pos_y;
	uint32_t duracion;

} tarea_t;

typedef struct {

	uint32_t tid;
	uint32_t pos_x;
	uint32_t pos_y;
	uint32_t proximaInstruccion;
	uint32_t puntero_pcb;
	char estado;

} t_tcb;

typedef struct {

	uint32_t pos_x;
	uint32_t pos_y;

} t_pos_sabotaje;

typedef struct {

	uint32_t pid;
	uint32_t puntero_tareas;

} t_pcb;

t_config* PATH_CONFIG;
t_log* PATH_LOG;

int SERVIDOR_SABOTAJE;
int PUERTO_MI_RAM_HQ;
int PUERTO_I_MONGO_STORE;
int RETARDO_CICLO_CPU;
int DURACION_SABOTAJE;
int QUANTUM;
int GRADO_MULTITAREA;
int cantidadArgumentos;
int tripulanteNumero;
int patotaNumero;
int centinela_planificacion;
int centinela_emergencia;
int centinela_patota;

char* ALGORITMO;
char* IP_I_MONGO_STORE;
char* IP_MI_RAM_HQ;

t_queue* colaNew;

t_list* colaExit;
t_list* colaReady;
t_list* colaExec;
t_list* colaBlock;
t_list* colaBlockEmergency;
t_list* listaTripulantes;

pthread_mutex_t mutex_colaBlockEmergency = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_ejecutarBloqueados = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_colaNew = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_colaReady = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_colaExecute = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_colaBlock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_colaExit = PTHREAD_MUTEX_INITIALIZER;

sem_t sem_detener_sabotaje;
sem_t sem_consumir;
sem_t sem_detener_planificacion;
sem_t sem_multitarea;
sem_t mutex_bitacora;
sem_t consumir_block;
sem_t sem_block_execute;

pthread_t planificacion;
pthread_t hilo_mongo;

void recibirMongo();
void escucharMONGO();
void quantumMovimiento(t_tcb* tripulante, tarea_t* tarea, int mongo);
void quantumTarea(t_tcb* tripulante, tarea_t* tarea);
void deNew_a_ready();
void iniciarTarea(t_tcb* tripulante);
void iniciarPlanificacion();
void iniciarColas();
void finalizarColas();
void terminarConfig();
void iniciarPatota(t_list* argumentos);
void iniciarConfigs();
void iniciarServerSabotaje();
void reactivarPlanificacion();
void listarTripulantes(t_list* lista);
void mensajeCambiarEstado(uint32_t tid, char estado, int socket);
void mensajePosicionTripulante(uint32_t tid, uint32_t posicionX, uint32_t posicionY, int socket);
void mensajeIniciarPatota(uint32_t pid, char* path, int socket);
void mensajeIniciarTripulantes(uint32_t pid, uint32_t cantidad, char* posiciones, int socket);
void mensajeInicioTarea(char* nombreTarea, uint32_t tid, int socket);
void mensajeFinTarea(char* nombreTarea, uint32_t tid, int socket);
void mensajeTareaBloqueante(char* nombreTarea, uint32_t tid, uint32_t parametro, int socket);
void mensajeDesplazamiento(uint32_t tid, uint32_t origenX, uint32_t origenY, uint32_t destinoX, uint32_t destinoY, int socket);
void mensajeSolicitarBitacora(uint32_t tid);
void mensajeExpulsarTripulante(uint32_t tid);
void mensajeFinalizarSabotaje(uint32_t tid, int socket);
void mensajeIniciarSabotaje(uint32_t tid, int socket);
void mensajeEjecutarSabotaje(int socket);
//void ejecutarTareaRR(t_tcb* tripulante, tarea_t* tarea);
//void ejecutarTareaFIFO(t_tcb* tripulante, tarea_t* tarea);
void ejecutarTareaRR(t_tcb* tripulante, tarea_t* tarea, int cliente_ram, int cliente_mongo);
void ejecutarTareaFIFO(t_tcb* tripulante, tarea_t* tarea, int cliente_ram, int cliente_mongo);
void cambiarEstado(char estado, t_tcb* tripulante, int socket);
void deReady_a_blockEmergencia();
void deBlockEmergencia_a_ready();
void reactivarMultitarea();
void sabotajeIniciado(t_sabotaje* sabotaje, int socket_mongo);
void sabotajeIniciado_sinConexion();
void desbloquearTripulante();
void ordenarPorTID(t_list* lista);
void listaTripulantes_sinExit();
void vaciarListaRemoviendo(t_list* lista);

char* transformarEstado(char estado);

double calcularDistancia(int x1, int y1, int x2, int y2);
int existe_ruta(char* filename);

bool buscarTripulante(t_list* listaTripulantes, int id);
bool esBloqueante(tarea_t* tarea);
bool existeDePatota(int pid, t_list* lista);

t_tcb* nuevoTripulante(uint32_t tid, uint32_t posicionX, uint32_t posicionY, t_pcb* patota, tarea_t* tarea);
t_tcb* devolverTripulante(int tid, t_list* lista);
t_tcb* devolverRemoviendoTripulante(int tid, t_list* lista);
t_tcb* proximoTripulante();
t_tcb* devolverRemoviendoTripulantePID(int pid, t_list* lista);
t_tcb* retornarMasCercano(int sabotaje_x, int sabotaje_y);

t_pcb* nuevaPatota(uint32_t pid, char* tareas);

tarea_t* mensajeSolicitarTarea(uint32_t tid, uint32_t pid, int socket);
tarea_t* deString_aTarea( char* tarea );
tarea_t* nuevaTarea(char* nombreTarea,  uint32_t parametro, uint32_t posicionX, uint32_t posicionY, uint32_t duracion);

t_list* expulsarTripulante(int id, t_list* lista);

char* leer_archivo_tareas(char* path);
void servidorSabotaje();
void recibir_paquete_sabotaje(int socket);

#endif /* DISCORDIADOR_H_ */
