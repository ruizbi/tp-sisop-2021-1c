
#include "./socket.h"

int crearSocket() {
	int nuevoSocket = socket(AF_INET, SOCK_STREAM, 0);
	if(nuevoSocket < 0)
		perror("ERROR: NO SE PUDO CREAR EL SOCKET");
	return nuevoSocket;
}

int crearCliente(char* ip, int puerto) {

	char* message = malloc(100);
	int socketCliente = crearSocket();
	struct sockaddr_in servaddr;
	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = inet_addr(ip);
	servaddr.sin_port = htons(puerto);

	if(connect(socketCliente, (struct sockaddr *)&servaddr, sizeof(servaddr)) != 0) {
		sprintf(message, "ERROR: NO SE PUDO CONECTAR A LA IP: %s PUERTO: %d", ip, puerto);
		perror(message);
	}

	free(message);
	return socketCliente;
}

int crearClienteConReconexion(char* ip, int puerto, int tiempo_reco) {

	char* message = malloc(100);
	int socketCliente = crearSocket();
	int loop = 1;
	struct sockaddr_in servaddr;
	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = inet_addr(ip);
	servaddr.sin_port = htons(puerto);

	while(loop) {
		if(connect(socketCliente, (struct sockaddr *)&servaddr, sizeof(servaddr)) != 0) {
			sprintf(message, "ERROR: NO SE PUDO CONECTAR A LA IP: %s PUERTO: %d", ip, puerto);
			perror(message);
			sleep(tiempo_reco);
		}
		else
			loop = 0;
	}

	free(message);
	return socketCliente;
}

int crearSocketEscucha(int puerto, int backlog) {

	struct sockaddr_in servaddr;
	int socketServer = crearSocket();
	char* message = malloc(100);

	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons((uint16_t) puerto);
	servaddr.sin_addr.s_addr = INADDR_ANY;

	if( bind(socketServer, (struct sockaddr *)&servaddr, sizeof(servaddr)) != 0 ) {
		sprintf(message, "ERROR: BIND DEL PUERTO: %d", puerto);
		perror(message);
	}

	if(listen(socketServer, backlog) != 0) {
		sprintf(message, "ERROR: LISTEN DEL SOCKET: %d PUERTO: %d", socketServer, puerto);
		perror(message);
	}

	free(message);
	return socketServer;

}

int crearServer(int puerto, int backlog) {

	unsigned int leng;
	struct sockaddr_in cliente;
	int socketConexion, socketServer;

	socketServer = crearSocketEscucha(puerto, backlog);
	leng = sizeof(cliente);

	socketConexion = accept(socketServer, (struct sockaddr *)&cliente, &leng);

	if( socketConexion < 0 )
		perror("ERROR: Fallo el accept del servidor.");

	return socketConexion;
}

void crearServer2(int puerto, int backlog) {

	unsigned int leng;
	struct sockaddr_in cliente;
	int socketServer;

	socketServer = crearSocketEscucha(puerto, backlog);
	leng = sizeof(cliente);

	while(1) {
		int socketConexion = accept(socketServer, (struct sockaddr *)&cliente, &leng);

		if( socketConexion < 0 ) {
			perror("ERROR: Fallo el accept del servidor.");
			close(socketConexion);
		}
		else {
			pthread_t hilo;
			if(pthread_create( &hilo, NULL, (void*) recibir_paquete, (void*) socketConexion))
				perror("ERROR: NO SE PUDO CREAR EL HILO PARA EL CLIENTE");
		}
	}
}

