
#ifndef SRC_CONEXIONES_H_
#define SRC_CONEXIONES_H_

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <string.h>
#include <semaphore.h>
#include "utils.h"
#include <pthread.h>

int crearSocket();
int crearSocketEscucha(int puerto, int backlog);
int crearCliente(char* ip, int puerto);
int crearClienteConReconexion(char* ip, int puerto, int tiempo_reco);
void crearServer(int puerto, int backlog);

void crearServerConPuntero(int puerto, int backlog, void (*frecibir)(int));

#endif /* SRC_CONEXIONES_H_ */
