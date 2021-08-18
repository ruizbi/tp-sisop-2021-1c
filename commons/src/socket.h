
#ifndef SRC_SOCKET_H_
#define SRC_SOCKET_H_

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include "../src/string.h"
#include <string.h>
#include <semaphore.h>
#include "collections/queue.h"
#include "./utils.h"
#include <pthread.h>

int crearSocket();
int crearSocketEscucha(int puerto, int backlog);
int crearCliente(char* ip, int puerto);

// Desarrolo del sv nuevo
int crearServer(int puerto, int backlog);
void recibirMensajes_server(int server);
void crearServer2(int puerto, int backlog );
int crearClienteConReconexion(char* ip, int puerto, int tiempo_reco);

#endif /* SRC_SOCKET_H_ */
