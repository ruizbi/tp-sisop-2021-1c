/*
 * conexiones.h
 *
 *  Created on: 19 jun. 2021
 *      Author: utnso
 */

#ifndef SERVIDORMONGO_H_
#define SERVIDORMONGO_H_

#include <inttypes.h>
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
#include<commons/string.h>
#include "bitacoras.h"
#include <utils.h>
#include "i_mongo_store.h"
#include "./sabotajes.h"

#define BACKLOG 10
void recibir_paquete_mongo2(int socket);
void crearServerMongo();
void handler_sabotaje();
void servidorSabotaje();

#endif /* SERVIDORMONGO_H_ */
