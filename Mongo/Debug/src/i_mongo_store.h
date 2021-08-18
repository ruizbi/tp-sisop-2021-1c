#ifndef I_MONGO_STORE_H_
#define I_MONGO_STORE_H_
#include <stdio.h>
#include <stdlib.h>
#include <commons/config.h>
#include <commons/log.h>
#include <commons/string.h>
#include <sys/mman.h>
#include "polus.h"
#include "archivos.h"
#include "bitacoras.h"
#include "tareas.h"
#include "mongoUtils.h"
#include "servidorMongo.h"
#include <pthread.h>
#include <conexiones.h>
#include <signal.h>
#include <unistd.h>

#define PUERTO_SABOTAJE 5059

sem_t mutex_basura;
sem_t mutex_comida;
sem_t mutex_oxigeno;

t_list* lista_sabotajes;
int PUERTO_MONGO_STORE;
char* IP_MONGO_STORE;
char* IP_SABOTAJE;
void leer_config(char* nombre);
void iniciar_logger(char* nombre);
void iniciar_log(char* nombre);
void cargar_Sabotajes();
sem_t mutexBitmap;
sem_t mutexBlocks;
sem_t mutexBitacora;
sem_t mutexSabotaje;
sem_t mutexTareas;
#endif /* I_MONGO_STORE_H_ */
