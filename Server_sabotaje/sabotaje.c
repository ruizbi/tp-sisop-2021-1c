
#include "./sabotaje.h"

int main() {

	// 10) SIGUSR1
	signal(SIGUSR1, &handler_sabotaje);

	while(1) {
		sleep(10);
	}
	return 0;
}

void handler_sabotaje() {

	pthread_t hilo;
	if(pthread_create(&hilo, NULL, (void*) servidorSabotaje, NULL) < 0)
		perror("ERROR: NO SE PUDO CREAR EL HILO DE SABOTAJE POR SIGNAL");
	pthread_join(hilo, NULL);
}

void servidorSabotaje() {

	int cliente = crearCliente(IP_DISCORDIADOR, PUERTO_SABOTAJE);

	/*
	 * -----------
	 * OBTENGO LA POSICION DEL SABOTAJE
	 * -----------
	 */

	t_buffer* buffer = serializar_sabotaje(9, 11);
	enviar_paquete(buffer, SABOTAJE, cliente);
	int loop = 1;

	t_paquete* paquete = malloc(sizeof(t_paquete));
	paquete->buffer = malloc(sizeof(t_buffer));

	while(loop) {

		t_paquete* paquete = malloc(sizeof(t_paquete));
		paquete->buffer = malloc(sizeof(t_buffer));

		int bytes = recv(cliente, &(paquete->codigo_operacion), sizeof(uint8_t), 0);

		if(bytes < 0) {
			printf("ERROR EN EL CODOP\n");
			loop = 0;
			break;
		}
		else if(bytes == 0) {
			printf("SE DESCONECTO EL CLIENTE\n");
			loop = 0;
			break;
		}
		else
			printf("CODOP: %d \n", paquete->codigo_operacion);

		bytes = recv(cliente, &(paquete->buffer->size), sizeof(uint32_t), 0);

		if(bytes < 0) {
			printf("ERROR EN EL SIZE DEL BUFFER\n");
			loop = 0;
			break;
		}
		else if(bytes == 0) {
			printf("SE DESCONECTO EL CLIENTE\n");
			loop = 0;
			break;
		}
		else
			printf("BUFFER SIZE: %d \n", paquete->buffer->size);

		paquete->buffer->stream = malloc(paquete->buffer->size);

		bytes = recv(cliente, paquete->buffer->stream, paquete->buffer->size, 0);

		if(bytes < 0) {
			printf("ERROR EN RECIBIR EL STREAM\n");
			loop = 0;
			break;
		}
		else if(bytes == 0) {
			printf("SE DESCONECTO EL CLIENTE\n");
			loop = 0;
			break;
		}

		switch(paquete->codigo_operacion) {

		t_solicitudes_varias* solicitud;
		t_desplazamiento* desplazamiento;
		t_buffer* buffer;

			case EJECUTAR_SABOTAJE:
				solicitud = deserializar_solicitudes_varias(paquete->buffer);
				// EJECUTO LO CORRESPONDIENTE AL SABOTAJE
				buffer = serializar_solicitudes_varias(0, 0);
				enviar_paquete(buffer, EJECUTAR_SABOTAJE, cliente);
				free(solicitud);
				break;

			case INICIAR_SABOTAJE:
				solicitud = deserializar_solicitudes_varias(paquete->buffer);
				printf("EL TRIPULANTE %d INICIO EL SABOTAJE\n", solicitud->tid);
				free(solicitud);
				break;

			case FINALIZAR_SABOTAJE:
				solicitud = deserializar_solicitudes_varias(paquete->buffer);
				printf("EL TRIPULANTE %d FINALIZO EL SABOTAJE\n", solicitud->tid);
				free(solicitud);
				loop = 0;
				break;

			case DESPLAZAMIENTO:
				desplazamiento = deserializar_desplazamiento(paquete->buffer);
				printf("ORIGEN:%d|%d DESTINO:%d|%d TRIPULANTE:%d\n", desplazamiento->origen_x, desplazamiento->origen_y, desplazamiento->destino_x, desplazamiento->destino_y, desplazamiento->tid);
				free(desplazamiento);
				break;
		}

	}

	free(paquete->buffer->stream);
	free(paquete->buffer);
	free(paquete);

	close(cliente);

}
