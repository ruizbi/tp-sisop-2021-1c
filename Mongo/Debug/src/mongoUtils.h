#ifndef MONGOUTILS_H_
#define MONGOUTILS_H_

#include <stdio.h>
#include <stdlib.h>

#include <commons/config.h>
#include <commons/string.h>
#include <string.h>
#include "i_mongo_store.h"
#include <inttypes.h>

int getPuerto(char* key);
char* getIp(char* key);

//Variables para mapeo a memoria
char* mapeo;
size_t filesize;
int tamanioSB;
int fd;
int sb;
char*ptomje;

#endif /* MONGOUTILS_H_ */
