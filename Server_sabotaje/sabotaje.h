
#ifndef SABOTAJE_H_
#define SABOTAJE_H_

#include <stdio.h>
#include <stdlib.h>
#include <conexiones.h>
#include <utils.h>
#include <unistd.h>
#include <signal.h>

#define PUERTO_SABOTAJE 5059
#define IP_DISCORDIADOR "127.0.0.1"

void handler_sabotaje();
void servidorSabotaje();

#endif /* SABOTAJE_H_ */
