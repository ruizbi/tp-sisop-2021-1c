/*
 * memoriaHQ.c
 *
 *  Created on: 23 abr. 2021
 *      Author: utnso
 */
#include "memoriaHQ.h"

void signal_catch(int n) {
	switch (n) {
		case SIGUSR1:
			compactacion();
		break;
		case SIGUSR2:
			dump_memoria();
		break;
	}
}

int main(void)
{
	contador_segmento=0;
	contador_id_tripulantes = 1;
	POSICION_ACTUAL_FRAME = 0;
	logger = iniciar_logger();
	config =leer_config();
	ALGORITMO_MEMORIA = string_new();
	TIPO_AJUSTE = string_new();
	ALGORITMO_MEMORIA = config_get_string_value(config,"ESQUEMA_MEMORIA");
	TIPO_AJUSTE = config_get_string_value(config,"AJUSTE");
	iniciarListas();
	reservar_memoria();
	iniciar_mapa();
	signal(SIGUSR1, signal_catch);
	signal(SIGUSR2,signal_catch);
	/*
	crear_patota2(1,"GENERAR_COMIDA 4;2;3;1|GENERAR_BASURA 12;2;3;5|DESCARTAR_BASURA 3;1;7;5|CONSUMIR_OXIGENO 120;2;3;1");
	iniciarTripulante2(1, 3,"5|3 6|6");
	eliminar_tripulante_de_memoria2(2);
	*/

	pthread_t hilo_escucha_discordiador;
	if(pthread_create(&hilo_escucha_discordiador, NULL, crearServerDiscordiador, NULL) < 0)
		perror("ERROR: NO SE PUDO CREAR EL HILO DE SV DISCORDIADOR");


	pthread_join(hilo_escucha_discordiador,NULL);

	return EXIT_SUCCESS;
}

void crearServerDiscordiador() {

//	char* PUERTO = config_get_string_value( config, PUERTO);
	void (*fptr)(int);
	fptr = recibir_paquete2;
	crearServerConPuntero(5002, BACKLOG, fptr);
}


void iniciarListas(){
		tabla_segmentos_general= list_create();
		lista_tabla_segmentos = list_create();
		lista_tabla_paginas = list_create();

		lista_paginas_principal_general = list_create();
		lista_paginas_secundaria_general = list_create();
}

t_log* iniciar_logger(void){
return log_create("memoriaHQ.log", "MemoriaHQ", true, LOG_LEVEL_INFO);
}


t_config* leer_config(void) //cambiar ruta a ubicacion del clone del repo
{
return config_create("memoriaHQ.config");
}


void reservar_memoria(void){
	log_info(logger,"reservando memoria ...");
	char* memoria = config_get_string_value(config,"TAMANIO_MEMORIA");
	TAMANIO_MEMORIA = atoi(memoria) /8;
	BLOQUE_MEMORIA = malloc(TAMANIO_MEMORIA);
	if(strcmp(ALGORITMO_MEMORIA,"SEGMENTACION")== 0){
		t_segmento* segmento = crear_segmento(0,TAMANIO_MEMORIA,true);
			contador_segmento++;
			list_add(tabla_segmentos_general,segmento);
			log_info(logger,"Se reserva %d bytes de espacio de memoria",TAMANIO_MEMORIA);
	}else{
		TAMANIO_PAGINA = atoi(config_get_string_value(config,"TAMANIO_PAGINA")) /8;
		CANTIDAD_PAGINAS = (TAMANIO_MEMORIA/ TAMANIO_PAGINA);
		//inicio la lista de memoria principal y secundaria con paginas vacias
		t_frame* frame = NULL;
		for(int i=0;i<CANTIDAD_PAGINAS;i++){
			frame = crear_frame_libre(i);
			list_add(lista_paginas_principal_general,frame);
		}

		iniciar_memoria_secundaria();
		for(int j=0;j<CANTIDAD_PAGINAS_SECUNDARIA;j++){
			frame = crear_frame_libre(j);
			list_add(lista_paginas_secundaria_general,frame);
		}


		log_info(logger,"Se crea la tabla de paginas de la memoria principal");
		log_info(logger,"Se reserva %d bytes de espacio de memoria",TAMANIO_MEMORIA);
	}
	free(memoria);
}

void iniciar_mapa(void){
	/*
	nivel_gui_inicializar();			   //
	nivel_gui_get_area_nivel(&cols, &rows);//
	nivel = nivel_crear("Test Chamber 04");// Seteo del mapa, ocupa toda la consola, ante mas logs, se solapan en las ultimas 3 lineas (tapan los datos del mapa)
	nivel_gui_dibujar(nivel);
	*/
}

t_tcb* crear_estructura_tcb(uint32_t id,char* posiciones,uint32_t pcb){

	char** posicion = string_split(posiciones,"|");
	volatile uint32_t posicion_x = (uint32_t)atoi(posicion[0]);
	volatile uint32_t posicion_y = (uint32_t)atoi(posicion[1]);

	t_tcb* tcb = malloc(sizeof(t_tcb));
	tcb->tid = id;
	tcb->estado='N';
	tcb->pos_x = posicion_x;
	tcb->pos_y = posicion_y;
	tcb->prox_instruccion = 0;
	tcb->puntero_pcb =pcb;

	free(posicion);
	log_info(logger,"Se crea nuevo TCB: id: %d , pos_x: %d , pos_y: %d",tcb->tid,tcb->pos_x,tcb->pos_y);
	return tcb;
}

t_pcb* crear_estructura_pcb(uint32_t pid,uint32_t direccionTareas){

	t_pcb* patota = malloc(sizeof(t_pcb));
	patota->pid = pid;
	patota->puntero_tareas =direccionTareas;
	log_info(logger,"Se crea estructura PCB: id: %d , puntero_tareas: %d",patota->pid,patota->puntero_tareas);
	return patota;
}

void crear_patota2(uint32_t pid,char* tareas ){
	t_pcb* patota = crear_estructura_pcb(pid,(uint32_t)tareas);
	registrarPatota(patota);
	registrarTareas(tareas,patota->pid);
	free(patota);
	free(tareas);
}

char* crear_patota(uint32_t pid,char* tareas ){
	t_pcb* patota = crear_estructura_pcb(pid,(uint32_t)tareas);
	registrarPatota(patota);
	registrarTareas(tareas,patota->pid);
	free(patota);
	return tareas;
}

void mapear_tripulante(int posx,int posy,int tid){
	/*
	pthread_mutex_lock(&mutex_mapa);
	err = personaje_crear(nivel,tid+'0' , posx, posy);//crea tripulante en posicion hardcodeada en el mapa
		ASSERT_CREATE(nivel, tid+'0' , err);//no des bola a este error // @suppress("Unused return value")
		nivel_gui_dibujar(nivel); //refresca la ultima version de datos en el mapa
	pthread_mutex_unlock(&mutex_mapa);
	*/
}

void registrarPatota(t_pcb* patota){
	//creo tabla segmento de patota
			if(strcmp(ALGORITMO_MEMORIA,"SEGMENTACION")== 0){
				t_segmento* segmento = crear_segmento_patota(patota);
				bool resultado =aplicar_segmentacion(segmento);
				//el segmento queda con nuevo inicio
				if(resultado){
					guardar_patota_en_memoria(patota,segmento->inicio);
					log_info(logger, "PATOTA %d REGISTRADA",patota->pid);
				}else{
					log_info("MEMORIA INSUFICIENTE PARA ALMACENAR LA PATOTA %d",patota->puntero_tareas);
					free(segmento);
				}
			}else{
				aplicar_paginacion_patota(patota);
			}
}

void aplicar_paginacion_patota(t_pcb* pcb){
	void* pcb_serializado = serializar_patota(pcb);
	crear_tabla_paginas_pcb(pcb->pid,pcb_serializado);
}

void aplicar_paginacion_tareas(char* tareas,uint32_t pid,int tamanio){
	void* tarea_serializada =(void*)tareas;
	pthread_mutex_lock(&mutex_lista_tabla_paginas);
	t_tabla_paginas* tabla_paginas_encontrada=buscar_tabla_paginas(pid);
	pthread_mutex_unlock(&mutex_lista_tabla_paginas);
	tabla_paginas_encontrada->inicio_tareas = tabla_paginas_encontrada->lista_paginas->elements_count;
	tabla_paginas_encontrada->tamanio_tareas = tamanio;
	guardar_body_de_paginas_en_memoria(tabla_paginas_encontrada,tarea_serializada,tamanio);
	tabla_paginas_encontrada->fin_tareas = tabla_paginas_encontrada->lista_paginas->elements_count ;

}

void aplicar_paginacion_tripulante(t_tcb* tcb,uint32_t pid){

	log_info(logger,"Se inicia el registro de tripulante %d con PID %d",tcb->tid,pid);
	void* tripulante_serializado = serializar_tripulante(tcb);
	pthread_mutex_lock(&mutex_lista_tabla_paginas);
	t_tabla_paginas* tabla_paginas_encontrada = buscar_tabla_paginas(pid);
	pthread_mutex_unlock(&mutex_lista_tabla_paginas);
	list_add(tabla_paginas_encontrada->lista_id_tripulantes,(uint32_t)tcb->tid);
	guardar_body_de_paginas_en_memoria(tabla_paginas_encontrada,tripulante_serializado,sizeof(t_tcb));
}

t_tabla_paginas* buscar_tabla_paginas(uint32_t pid){
	bool buscar_tabla_paginas_con_pid(void* tabla_paginas){
		t_tabla_paginas* tabla_paginas_a_buscar = (t_tabla_paginas*) tabla_paginas;
		return tabla_paginas_a_buscar->pid == pid;
	}
	return (t_tabla_paginas*) list_find(lista_tabla_paginas,(void*)buscar_tabla_paginas_con_pid);


}

void crear_tabla_paginas_pcb(uint32_t pid,void* pcb_serializado ){
	log_info(logger,"Se crea la tabla de paginas para el proceso %d",pid);
	t_tabla_paginas* tabla_pagina = malloc(sizeof(t_tabla_paginas));
	tabla_pagina->pid = pid;
	tabla_pagina->lista_paginas = list_create();
	tabla_pagina->lista_id_tripulantes = list_create();
	tabla_pagina->inicio_pcb = tabla_pagina->lista_paginas->elements_count;
	tabla_pagina->cantidad_tcbs_terminados = 0;
	guardar_body_de_paginas_en_memoria(tabla_pagina,pcb_serializado,sizeof(t_pcb));
	pthread_mutex_lock(&mutex_lista_tabla_paginas);
	list_add(lista_tabla_paginas,tabla_pagina);
	pthread_mutex_unlock(&mutex_lista_tabla_paginas);
}

void guardarTodasLasPaginasEnMemoria(t_tabla_paginas* tabla_pagina,void* body,int tamanio ){
	int puedeRecorrer = 1;
	int nro_pagina = tabla_pagina->lista_paginas->elements_count;
	int i=0;
	t_frame* frame_a_ocupar = NULL;
	pthread_mutex_lock(&mutex_lista_paginas_principal_general);
	while(puedeRecorrer && i<lista_paginas_principal_general->elements_count){
		frame_a_ocupar = (t_frame*) list_get(lista_paginas_principal_general,i);
		if(frame_a_ocupar->libre){
		log_info(logger,"Se encuentra frame libre nro: %d",i);
		t_pagina* pagina =  crear_pagina(nro_pagina++,i);


		pagina->pid = tabla_pagina->pid;
		body = guardarEnFrame(i,body,pagina,&puedeRecorrer,&tamanio);
		//actualizo el frame en general
		frame_a_ocupar->libre = false;
		actualizar_frame(frame_a_ocupar);
		frame_a_ocupar->usado = true;
		//pagina_a_remplazar ya se encuentra guardada en la lista

		//agrego a la tabla general que representa la MP

		//agrego la pagina guardada en su lista de paginas
		list_add(tabla_pagina->lista_paginas,pagina);
		}
		i++;
	}
	pthread_mutex_unlock(&mutex_lista_paginas_principal_general);
}


void guardarTodasLasPaginasEnMemoriaYaExistente(t_tabla_paginas* tabla_pagina,void* body, int inicio, int limite, int tamanio){
	int puedeRecorrer = 1;
	int i=inicio;
	t_frame* frame_a_ocupar = NULL;
	while(puedeRecorrer && i<limite){
		t_pagina* pagina = (t_pagina*)list_get(tabla_pagina->lista_paginas,i);
		pthread_mutex_lock(&mutex_lista_paginas_principal_general);
		frame_a_ocupar = (t_frame*) list_get(lista_paginas_principal_general,pagina->nro_frame);
		log_info(logger,"Se reescribe el frame libre nro: %d",frame_a_ocupar->nro_frame);

		body = guardarEnFrame(i,body,pagina,&puedeRecorrer,&tamanio);
		//actualizo el frame en general
		actualizar_frame(frame_a_ocupar);
		frame_a_ocupar->usado = true;
		frame_a_ocupar->en_uso = false;
		//pagina_a_remplazar ya se encuentra guardada en la lista
		pthread_mutex_unlock(&mutex_lista_paginas_principal_general);
		i++;
	}
}

void guardar_body_de_paginas_en_memoria(t_tabla_paginas* tabla_pagina,void* body,int tamanio){

	int cantidadDePaginasNecesarias =  existe_espacio_disponible_en_memoria(tamanio);
	if(cantidadDePaginasNecesarias == 0){
	//si cantidadDePaginasNecesarias es 0, se insertara en memoria principal
		guardarTodasLasPaginasEnMemoria(tabla_pagina,body,tamanio);
	}else{
		//cantidadDePaginasNecesarias devuelve el nro de paginas faltantes a reemplazar para aplicar memoria virtual
		//guardo las paginas que si puedo insertar en MM principal
			//aplico algoritmo de reemplazo
		if(strcmp(ALGORITMO_REEMPLAZO,"LRU")== 0){
			//aplico lru
			limpiar_ultimas_n_paginas_lru(cantidadDePaginasNecesarias);
			guardarTodasLasPaginasEnMemoria(tabla_pagina,body,tamanio);
		}else{
			//aplico clock
			//metodo clock para liberar la cantidad de paginas faltantes para almacenar todo el body
			limpiar_ultimas_n_paginas_clock(cantidadDePaginasNecesarias);
			guardarTodasLasPaginasEnMemoria(tabla_pagina,body,tamanio);
		}


	}
}

void limpiar_ultimas_n_paginas_lru(int cantidadPaginasALimpiar){
	for(int i=0;i<cantidadPaginasALimpiar;i++){
		mover_pagina_a_memoria_secundaria_lru();

	}
}

void limpiar_ultimas_n_paginas_clock(int cantidadPaginasALimpiar){
	for(int i=0;i<cantidadPaginasALimpiar;i++){
		mover_pagina_a_memoria_secundaria_clock();
	}
}

int mover_pagina_a_memoria_secundaria_clock(){
	int indice_frame =buscar_frame__a_reemplazar_clock();
	pthread_mutex_lock(&mutex_lista_paginas_principal_general);
	t_frame* frame_a_buscar = (t_frame*) list_get(lista_paginas_principal_general,indice_frame);

	//la voy a tener que alojar en memoria secundaria y cambiar su referencia de la tabla de paginas de la patota a la que pertence
	mover_frame_a_memoria_secundaria(frame_a_buscar);
	//una vez guardado la pagina, la dejo en libre, lo demas datos internos no importan
	frame_a_buscar->libre = true;
	pthread_mutex_unlock(&mutex_lista_paginas_principal_general);
	return indice_frame;
}

int mover_pagina_a_memoria_secundaria_lru(){
	//obtengo primer elemento y le saco el horario
	pthread_mutex_lock(&mutex_lista_paginas_principal_general);
	t_frame* frame_a_buscar = (t_frame*)list_get(lista_paginas_principal_general,0);
	int hora = 0;
	int minuto = 0;
	int segundo = 0;
	int milisegundo = 0;
	int indice =0;
	bool primerElmentoEncontrado = false;
	for(int i=0;i<lista_paginas_principal_general->elements_count;i++){
		frame_a_buscar =(t_frame*) list_get(lista_paginas_principal_general,i);
		if( !frame_a_buscar->libre && !frame_a_buscar->en_uso){
			if(!primerElmentoEncontrado){
				indice = i;
				hora = frame_a_buscar->hora_modificada;
				minuto = frame_a_buscar->min_modificado;
				segundo = frame_a_buscar->seg_modificado;
				milisegundo = frame_a_buscar->mili_seg_modificado;
				primerElmentoEncontrado = true;
			}else{
				if(!fechaPivoteEsMasAntiguoQueFechaDeFrame(hora,minuto,segundo,milisegundo,frame_a_buscar)){
					//encontre una fecha mas chica
					indice = i;
					hora = frame_a_buscar->hora_modificada;
					minuto = frame_a_buscar->min_modificado;
					segundo = frame_a_buscar->seg_modificado;
					milisegundo = frame_a_buscar->mili_seg_modificado;
				}
			}
		}
	}
	log_info(logger,"Se encontro la pagina mas antigua: nro_frame %d",indice);
	//al final se consigue el indice con la fecha mas chica (la mas antigua)
	frame_a_buscar = (t_frame*)list_get(lista_paginas_principal_general,indice);
	//la voy a tener que alojar en memoria secundaria y cambiar su referencia de la tabla de paginas de la patota a la que pertence
	mover_frame_a_memoria_secundaria(frame_a_buscar);
	//una vez guardado la pagina, la dejo en libre, lo demas datos internos no importan
	frame_a_buscar->libre = true;
	pthread_mutex_unlock(&mutex_lista_paginas_principal_general);

	return indice;
}

void mover_frame_a_memoria_secundaria(t_frame* frame){
	bool contiene_frame(void* pagina_stream){
			t_pagina* pagina_a_buscar = (t_pagina*) pagina_stream;
			return pagina_a_buscar->nro_frame ==  frame->nro_frame && !pagina_a_buscar->enMemoriaSecundaria;
		}

	bool buscar_tabla_paginas_con_frame(void* tabla_paginas){
			t_tabla_paginas* tabla_paginas_a_buscar = (t_tabla_paginas*) tabla_paginas;
			return list_any_satisfy(tabla_paginas_a_buscar->lista_paginas,(void*) contiene_frame);
		}
		//busco la tabla y la pagina finalmente para mover
		pthread_mutex_lock(&mutex_lista_tabla_paginas);
		t_tabla_paginas* tabla_paginas_encontrada = (t_tabla_paginas*)list_find(lista_tabla_paginas,(void*)buscar_tabla_paginas_con_frame);
		pthread_mutex_unlock(&mutex_lista_tabla_paginas);
		t_pagina* pagina_a_mover = (t_pagina*)list_find(tabla_paginas_encontrada->lista_paginas,(void*)contiene_frame);

		guardar_pagina_en_memoria_secundaria(pagina_a_mover);
}

void guardar_pagina_en_memoria_secundaria(t_pagina* pagina_a_mover){
	//voy a obtener el stream que contiene la pagina y lo guardo en MS
	void* stream = obtenerStreamDeMemoriaPrincipal(pagina_a_mover);
	guardar_stream_en_memoria_secundaria(pagina_a_mover,stream);

}


void* obtenerStreamDeMemoriaPrincipal(t_pagina* pagina){
	void * stream = malloc(pagina->tamanio);
	int inicio = pagina->nro_frame * TAMANIO_PAGINA;
	memcpy(stream, BLOQUE_MEMORIA + inicio, pagina->tamanio);
	return stream;
}

void guardar_stream_en_memoria_secundaria(t_pagina* pagina, void* stream){
	t_frame* frame_a_buscar =NULL;
	int guardado = 0;
	int i=0;
	pthread_mutex_lock(&mutex_lista_paginas_secundaria_general);
	while(i<lista_paginas_secundaria_general->elements_count && guardado == 0){
		frame_a_buscar =(t_frame*) list_get(lista_paginas_secundaria_general,i);
		if(frame_a_buscar->libre){
			guardarEnFrameSecundario(i,stream, pagina,&guardado);

			//una vez guardado en MS, actualizo la ubicacion
			pagina->enMemoriaSecundaria = true;
			frame_a_buscar->libre = false;
		}
		i++;
	}
	pthread_mutex_unlock(&mutex_lista_paginas_secundaria_general);

}

bool fechaPivoteEsMasAntiguoQueFechaDeFrame(int hora,int minuto,int segundo,int milisegundo,t_frame* frame){ // @suppress("No return")
	if(hora > frame->hora_modificada){
		return false;
	}
	if(hora < frame->hora_modificada){
		return true;
	}
	if(hora == frame->hora_modificada){
		if(minuto > frame->min_modificado){
			return false;
		}
		if(minuto < frame->min_modificado){
			return true;
		}
		if(minuto == frame->min_modificado){
			if(segundo > frame->seg_modificado){
				return false;
			}
			if(segundo < frame->seg_modificado){
				return true;
			}
			if(segundo == frame->seg_modificado){
				if(milisegundo > frame->mili_seg_modificado){
					return false;
				}
				if(milisegundo < frame->mili_seg_modificado){
					return true;
				}
			}
		}
	}
	return false;
}

void guardarEnFrameSecundario(uint32_t frame_secundario,void* stream, t_pagina* pagina,int* guardado){

			int offset = frame_secundario * TAMANIO_PAGINA;
			pagina->nro_frame_secundario = frame_secundario;
			//BLOQUE_MEMORIA_SECUNDARIA = mmap(NULL, TAMANIO_MEMORIA_SECUNDARIA,PROT_WRITE,MAP_SHARED,archivo_memoria_secundaria, 0);
			pthread_mutex_lock(&mutex_bloque_memoria_secundario);
			memcpy(BLOQUE_MEMORIA_SECUNDARIA+offset,stream, pagina->tamanio);
			pthread_mutex_unlock(&mutex_bloque_memoria_secundario);
			//msync(BLOQUE_MEMORIA_SECUNDARIA, TAMANIO_MEMORIA_SECUNDARIA, MS_SYNC); //sincronizar
			*guardado = 1;
			log_info(logger,"Se guardo el frame %d de MP al frame %d de MS",pagina->nro_frame,pagina->nro_frame_secundario);

}

void* guardarEnFrame(uint32_t numero_frame,void* stream,t_pagina* pagina, int* puedeRecorrer,int* tamanio){
	uint32_t offset = numero_frame * TAMANIO_PAGINA;
	void* auxiliar;


	if(*tamanio <= TAMANIO_PAGINA){
		pagina->tamanio = *tamanio;
		memcpy(BLOQUE_MEMORIA + offset, stream, *tamanio);
		*puedeRecorrer = 0;
		log_info(logger,"Se guardo en memoria el frame %d ,offset %d , tamanio %d",numero_frame,offset,*tamanio);
	}else{
		pagina->tamanio = TAMANIO_PAGINA;
		memcpy(BLOQUE_MEMORIA + offset, stream, TAMANIO_PAGINA);
		int pesoRestante = *tamanio - TAMANIO_PAGINA;
		auxiliar = malloc(pesoRestante);
		memcpy(auxiliar,stream+TAMANIO_PAGINA,pesoRestante);
		log_info(logger,"Se guardo en memoria el frame %d ,offset %d , tamanio %d",numero_frame,TAMANIO_PAGINA,*tamanio);
		*tamanio = pesoRestante;
	}
	return auxiliar;
}


void copiar_pagina(t_pagina* origen, t_pagina* destino){
	destino->enMemoriaSecundaria = origen->enMemoriaSecundaria;
	destino->nro_frame = origen->nro_frame;
	destino->nro_frame_secundario = origen->nro_frame_secundario;
	destino->nro_pagina = origen->nro_pagina;
	destino->tamanio = origen->tamanio;
	destino->pid = origen->pid;
}

t_pagina* crear_pagina(uint32_t numeroPagina, uint32_t numeroFrame){
	t_pagina* pagina = malloc(sizeof(t_pagina));
	pagina->nro_frame = numeroFrame;
	pagina->nro_pagina = numeroPagina;
	pagina->nro_frame_secundario = 99999;
	pagina->enMemoriaSecundaria = false;
	pagina->tamanio = 99999;
	pagina->pid = 99999;
	pagina->tamanio= 99999;
	return pagina;
}

t_frame* crear_frame_libre(uint32_t numero){
	usleep(10000);
	t_frame* frame = malloc(sizeof(t_frame));
	frame->nro_frame = numero;
	char* hora = temporal_get_string_time("%H:%M:%S:%MS");
	char** hora_separada = string_split(hora,":");
	frame->hora_modificada = atoi(hora_separada[0]);
	frame->min_modificado = atoi(hora_separada[1]);
	frame->seg_modificado = atoi(hora_separada[2]);
	frame->mili_seg_modificado = atoi(hora_separada[3]);
	frame->usado = false;
	frame->libre = true;
	free(hora_separada);
	free(hora);
	return frame;
}

void actualizar_frame(t_frame* frame){
	//actualiza el estado del frame tanto en mecanismo de LRU como CLOCK
	usleep(10000);
	char* hora = temporal_get_string_time("%H:%M:%S:%MS");
	char** hora_separada = string_split(hora,":");
	frame->hora_modificada = atoi(hora_separada[0]);
	frame->min_modificado = atoi(hora_separada[1]);
	frame->seg_modificado = atoi(hora_separada[2]);
	frame->mili_seg_modificado = atoi(hora_separada[3]);
	free(hora_separada);
	free(hora);
}

void registrarTareas(char* tareas,uint32_t pid){
	log_info(logger,"Se inicia el registro de la tarea %s de PID %d",tareas,pid);
	if(strcmp(ALGORITMO_MEMORIA,"SEGMENTACION")== 0){
		t_segmento* segmento =crear_segmento_tareas(tareas,pid);
		bool resultado =aplicar_segmentacion(segmento);
		if(resultado){
			guardar_tareas_en_memoria(tareas,segmento->inicio,segmento->tamanio);
			log_info(logger, "tareas del PID %d registrado",pid);
		}else{
			log_info("MEMORIA INSUFICIENTE PARA ALMACENAR LAS TAREAS del PID %d",pid);
		}
	}else{
		aplicar_paginacion_tareas(tareas,pid,string_length(tareas)+1);
	}
	free(tareas);
}


t_segmento* crear_segmento_patota(t_pcb* patota){
	t_segmento* segmento = malloc(sizeof(t_segmento));
	segmento->tamanio = sizeof(t_pcb);
	segmento->libre = false;
	segmento->esPatota = true;
	segmento->esTareas = false;
	segmento->id = patota->pid;
	segmento->nro_segmento=0;
	segmento->idHilo = 0;
	segmento->terminado = false;

	return segmento;
}

t_segmento* crear_segmento_tripulante(t_tcb* tripulante, uint32_t pid){
	t_segmento* segmento = malloc(sizeof(t_segmento));
	segmento->tamanio = sizeof(t_tcb);
	segmento->libre = false;
	segmento->esPatota = false;
	segmento->esTareas = false;
	segmento->id = pid;

	segmento->nro_segmento=0;
	segmento->terminado = false;

	segmento->idHilo = tripulante->tid;

	return segmento;
}

t_segmento* crear_segmento(int inicio,int tamanio,bool esLibre){
	t_segmento* segmento = malloc(sizeof(t_segmento));
	segmento->tamanio = tamanio;
	segmento->libre = esLibre;
	segmento->inicio = inicio;
	segmento->esPatota= false;
	segmento->id = 0;
	segmento->esTareas = false;
	segmento->nro_segmento=0;
	segmento->idHilo = 0;
	segmento->terminado = false;
	return segmento;
}

t_segmento* crear_segmento_tareas(char* tareas,uint32_t pid){
	t_segmento* segmento = malloc(sizeof(t_segmento));
	segmento->tamanio = string_length(tareas)+1;
	segmento->libre = false;
	segmento->esPatota = false;
	segmento->esTareas = true;
	segmento->id = pid;
	segmento->idHilo = 0;
	segmento->nro_segmento= 0;
	segmento->terminado = false;
	return segmento;
}

bool aplicar_segmentacion(t_segmento* segmento){
	pthread_mutex_lock(&mutex_tabla_segmento_general);
	bool resultado = ocupar_segmento_libre(segmento);
	if(!resultado){
		compactacion();
		resultado = ocupar_segmento_libre(segmento);
	}
	pthread_mutex_unlock(&mutex_tabla_segmento_general);

	return resultado;
}

void eliminar_segmento(void* segmento){
	t_segmento* segmento_a_borrar =(t_segmento*)segmento;
	free(segmento_a_borrar);
}

bool ocupar_segmento_libre(t_segmento* segmento){
	bool almacenado = false;
	t_segmento* segmentoDeTabla= NULL;
	//segmentoDeTabla = malloc(sizeof(t_segmento));

	int i=0;
	int cantidadSegmentos = list_size(tabla_segmentos_general);
	if(strcmp(TIPO_AJUSTE,"PRIMER_AJUSTE")== 0){
		//implementacion del primerAjuste

		while(i<cantidadSegmentos && !almacenado){
			segmentoDeTabla = (t_segmento*)list_get(tabla_segmentos_general,i);
			if(segmentoDeTabla->libre == true && segmentoDeTabla->tamanio >= segmento->tamanio  ){
				//procedo a ocupar el segmento libre y dividir lo sobrante en otro segmento libre
				//edito el segmento libre a ocupar
				log_info(logger,"Se encontró segmento libre: nro: %d , inicio: %d , tamanio: %d",segmentoDeTabla->nro_segmento, segmentoDeTabla->inicio,segmentoDeTabla->tamanio);
				int tamanioSobrante = segmentoDeTabla->tamanio -segmento->tamanio ;
				segmento->inicio = segmentoDeTabla->inicio;
				segmento->nro_segmento = segmentoDeTabla->nro_segmento;
				int nuevoInicio = segmentoDeTabla->inicio + segmento->tamanio;
				//lo persisto en la tabla de segmentos, reemplazando el segmento original libre
				list_remove(tabla_segmentos_general,i);
				list_add_in_index(tabla_segmentos_general,i,segmento);
				log_info(logger,"Se guarda en la tabla general el segmento nro %d , inicio %d, tamanio %d",segmento->nro_segmento,segmento->inicio,segmento->tamanio);
				//guardo el segmento en la tabla correspondiente
				listar_en_tabla_segmentos(segmento);

				almacenado = true;
				//verifico si sobra espacio, en caso afirmativo, creo el nuevo segmentoLibre
				if(tamanioSobrante != 0){
				//con el tamanioSobrante y nuevoInicio cre un nuevo segmento libre
				pthread_mutex_lock(&mutex_contador_segmentos);
				segmentoDeTabla->nro_segmento = contador_segmento++;
				pthread_mutex_unlock(&mutex_contador_segmentos);
				segmentoDeTabla->inicio = nuevoInicio;
				segmentoDeTabla->tamanio = tamanioSobrante;
				//t_segmento* segmentoLibre =crear_segmento(nuevoInicio,tamanioSobrante,true);
				list_add_in_index(tabla_segmentos_general,i+1,segmentoDeTabla);
				}

			}
			//free(segmentoDeTabla);
			i++;
			}//FIN LOOP

	}else{
		//implementacion del mejor ajuste
		int indiceDeSegmentoConMejorAjuste = -1;
		int	tamanioDeSegmentoConMejorAjuste;
		bool esPrimerSegmentoLibre = false;
		while(i<cantidadSegmentos && !almacenado){
			segmentoDeTabla = (t_segmento*)list_get(tabla_segmentos_general,i);
			//obtengo el primer segmento libre que servira como pivote para encontrar el que mejor se ajuste
			if(segmentoDeTabla->libre && segmentoDeTabla->tamanio >= segmento->tamanio && !esPrimerSegmentoLibre){
				indiceDeSegmentoConMejorAjuste = i;
				tamanioDeSegmentoConMejorAjuste = segmentoDeTabla->tamanio;
				esPrimerSegmentoLibre = true;
			}
			if(segmentoDeTabla->libre && segmentoDeTabla->tamanio >= segmento->tamanio && esPrimerSegmentoLibre){
				//verficamos los siguientes segmentos libres para ajustar
				if(tamanioDeSegmentoConMejorAjuste > segmentoDeTabla->tamanio){
					indiceDeSegmentoConMejorAjuste = i;
					tamanioDeSegmentoConMejorAjuste = segmentoDeTabla->tamanio;
				}
			}
			//free(segmentoDeTabla);
			i++;
		}//fin loop
		//verifico si se encontro al menos un segmento libre para operar
		if(indiceDeSegmentoConMejorAjuste != -1){
			//busco el segmentoLibre con el indice
			segmentoDeTabla = list_get(tabla_segmentos_general,indiceDeSegmentoConMejorAjuste);
			log_info(logger,"Se encontró segmento libre: nro: %d , inicio: %d , tamanio: %d",segmentoDeTabla->nro_segmento, segmentoDeTabla->inicio,segmentoDeTabla->tamanio);
			//procedo a ocupar el segmento libre y dividir lo sobrante en otro segmento libre
			//edito el segmento libre a ocupar
			int tamanioSobrante = segmentoDeTabla->tamanio - segmento->tamanio;
			segmento->inicio = segmentoDeTabla->inicio;
			segmento->nro_segmento = segmentoDeTabla->nro_segmento;
			int nuevoInicio = segmentoDeTabla->inicio + segmento->tamanio;
			//lo persisto en la tabla de segmentos, reemplazando el segmento original libre
			list_remove(tabla_segmentos_general,indiceDeSegmentoConMejorAjuste);
			list_add_in_index(tabla_segmentos_general,indiceDeSegmentoConMejorAjuste,segmento);
			log_info(logger,"Se guarda en la tabla general el segmento nro %d , inicio %d, tamanio %d",segmento->nro_segmento,segmento->inicio,segmento->tamanio);
			listar_en_tabla_segmentos(segmento);
			//guardo el segmento en la tabla correspondiente
			almacenado= true;
			//verifico si sobra espacio, en caso afirmativo, creo el nuevo segmentoLibre
			if(tamanioSobrante != 0){
			//con el tamanioSobrante y nuevoInicio cre un nuevo segmento libre
			//t_segmento* segmentoLibre =crear_segmento(nuevoInicio,tamanioSobrante,true);
				pthread_mutex_lock(&mutex_contador_segmentos);
				segmentoDeTabla->nro_segmento = contador_segmento++;
				pthread_mutex_unlock(&mutex_contador_segmentos);
				segmentoDeTabla->inicio = nuevoInicio;
				segmentoDeTabla->tamanio = tamanioSobrante;
			list_add_in_index(tabla_segmentos_general,indiceDeSegmentoConMejorAjuste+1,segmentoDeTabla);
			//free(segmentoDeTabla);
			}
			//no se encontro ningun espacio libre
		}
	}
	if(!almacenado){
		log_info(logger,"No se pudo almacenar el segmento");
	}
	return almacenado;
}

void compactacion(){
	log_info(logger,"INICIANDO COMPACTACION....");
	void* MEMORIA_AUXILIAR = malloc(TAMANIO_MEMORIA);
	int offset = 0;
	for(int i=0;i<tabla_segmentos_general->elements_count;i++){
		t_segmento* segmento = (t_segmento*) list_get(tabla_segmentos_general,i);
		//es patota
		if(segmento->esPatota && !segmento->libre){
			t_pcb* pcb = obtener_pcb_de_memoria(segmento->inicio,segmento->tamanio);
			segmento->inicio = offset;
			memcpy(MEMORIA_AUXILIAR + offset, serializar_patota(pcb), sizeof(t_pcb));
			log_info(logger,"Se guardo la patota %d (segmento nro %d) en MEMORIA_AUXILIAR, su offset fue %d, tamanio %d",pcb->pid,segmento->nro_segmento,offset,segmento->tamanio);
			offset = offset + sizeof(t_pcb);
			free(pcb);
		}
		//es tareas
		if(segmento->esTareas && !segmento->libre){
			char* tareas = obtener_tareas_de_memoria(segmento->inicio,segmento->tamanio);
			segmento->inicio = offset;
			memcpy(MEMORIA_AUXILIAR+ offset,(void*) tareas,segmento->tamanio );
			log_info(logger,"Se guardo las tareas %s (segmento nro %d) en MEMORIA_AUXILIAR, su offset fue %d, tamanio %d",tareas,segmento->nro_segmento,offset,segmento->tamanio );
			offset = offset + segmento->tamanio;
			free(tareas);
		}
		//es tripulante
		if(!segmento->esPatota && !segmento->esTareas && !segmento->libre){
			t_tcb* tcb = obtener_tcb_de_memoria(segmento->inicio,segmento->tamanio);
			segmento->inicio = offset;
			memcpy(MEMORIA_AUXILIAR + offset, serializar_tripulante(tcb), sizeof(t_tcb));
			log_info(logger,"Se guardo el tripulante %d (segmento nro %d)en MEMORIA_AUXILIAR, su offset fue %d , tamanio %d",tcb->tid,segmento->nro_segmento,offset,segmento->tamanio );
			offset = offset +sizeof(t_tcb);
			free(tcb);
		}
	}// fin loop
	//borro los libres
	bool esLibre(void* segmento){
		t_segmento* segmento_a_buscar = (t_segmento*) segmento;
		return segmento_a_buscar->libre;
	}
	list_remove_by_condition(tabla_segmentos_general,(void*)esLibre);
	pthread_mutex_lock(&mutex_bloque_memoria);
	free(BLOQUE_MEMORIA);
	BLOQUE_MEMORIA = MEMORIA_AUXILIAR;
	pthread_mutex_unlock(&mutex_bloque_memoria);
	//se guarda el segmento libre al final con la memoria sobrante total
	t_segmento* segmento_libre = crear_segmento(offset,TAMANIO_MEMORIA - offset,true);
	pthread_mutex_lock(&mutex_contador_segmentos);
	segmento_libre->nro_segmento = contador_segmento++;
	pthread_mutex_unlock(&mutex_contador_segmentos);
	list_add(tabla_segmentos_general,segmento_libre);



	log_info(logger,"COMPACTACION FINALIZADA");
}

void listar_en_tabla_segmentos(t_segmento* segmento){
	if(segmento->esPatota){
		t_tabla_segmento* tabla = crear_tabla_segmentos(segmento);
		log_info(logger,"Se crea la tabla de segmentos con PID %d el segmento nro %d , inicio %d, tamanio %d",segmento->id,segmento->nro_segmento,segmento->inicio,segmento->tamanio);
		pthread_mutex_lock(&mutex_lista_tabla_segmentos);
		list_add(lista_tabla_segmentos,tabla);// al ejecutar segmentacion, se traba acá por exepcion de malloc, sigo revisando
		pthread_mutex_unlock(&mutex_lista_tabla_segmentos);
	}else{
		log_info(logger,"Se busca tabla segmentos del PID %d",segmento->id);
		buscar_y_modificar_tabla_segmento(segmento);
	}
}

void guardar_patota_en_memoria(t_pcb* pcb,uint32_t inicio){
	void* pcbSerializado = serializar_patota(pcb);
	pthread_mutex_lock(&mutex_bloque_memoria);
	memcpy(BLOQUE_MEMORIA + inicio, pcbSerializado, sizeof(t_pcb));
	pthread_mutex_unlock(&mutex_bloque_memoria);
	free(pcbSerializado);
	log_info(logger,"Se guardo la patota %d en BLOQUE_MEMORIA, su offset fue %d , tamanio %d",pcb->pid,inicio,sizeof(t_pcb));
}


void guardar_tareas_en_memoria(char* tareas,uint32_t inicio,uint32_t tamanio){
	void* tareasSerializadas = (void*) tareas;
	pthread_mutex_lock(&mutex_bloque_memoria);
	memcpy(BLOQUE_MEMORIA+ inicio,tareasSerializadas,tamanio );
	pthread_mutex_unlock(&mutex_bloque_memoria);
	free(tareasSerializadas);
	log_info(logger,"Se guardo las tareas %s en BLOQUE_MEMORIA, su offset fue %d , tamanio %d",tareas,inicio,tamanio);
}

void guardar_tripulante_en_memoria(t_tcb* tcb, uint32_t inicio){
	void* tcbSerializado = serializar_tripulante(tcb);
	pthread_mutex_lock(&mutex_bloque_memoria);
	memcpy(BLOQUE_MEMORIA + inicio, tcbSerializado, sizeof(t_tcb));

	pthread_mutex_unlock(&mutex_bloque_memoria);
	log_info(logger,"Se guardo el tripulante %d en BLOQUE_MEMORIA, su offset fue %d , tamanio %d",tcb->tid,inicio,sizeof(t_tcb));
	free(tcbSerializado);
}


void* serializar_patota(t_pcb* pcb){

	void* stream = malloc(sizeof(t_pcb));
	int offset = 0;
	memcpy(stream + offset, &pcb->pid, sizeof(uint32_t));
	offset += sizeof(uint32_t);
	memcpy(stream + offset, &pcb->puntero_tareas, sizeof(uint32_t));
	return stream;
}

void* serializar_tripulante(t_tcb* tcb){
	void* stream = malloc(sizeof(t_tcb));
	int offset = 0;
	memcpy(stream + offset, &tcb->tid, sizeof(uint32_t));
	offset += sizeof(uint32_t);
	memcpy(stream + offset, &tcb->estado, sizeof(char));
	offset += sizeof(char);
	memcpy(stream + offset, &tcb->pos_x, sizeof(uint32_t));
	offset += sizeof(uint32_t);
	memcpy(stream + offset, &tcb->pos_y, sizeof(uint32_t));
	offset += sizeof(uint32_t);
	memcpy(stream + offset, &tcb->prox_instruccion, sizeof(uint32_t));
	offset += sizeof(uint32_t);
	memcpy(stream + offset, &tcb->puntero_pcb, sizeof(uint32_t));
	offset += sizeof(uint32_t);
	return stream;
}

t_pcb* deserializar_patota(void* stream){
		int offset = 0;
		t_pcb* pcb = malloc(sizeof(t_pcb));
		memcpy(&pcb->pid, stream + offset, sizeof(uint32_t));
		offset += sizeof(uint32_t);
		memcpy(&pcb->puntero_tareas, stream + offset , sizeof(uint32_t));

		return pcb;
}

t_tcb* deserializar_tripulante(void* stream){
	int offset = 0;
			t_tcb* tcb = malloc(sizeof(t_tcb));
			memcpy(&tcb->tid, stream + offset, sizeof(uint32_t));
			offset += sizeof(uint32_t);
			memcpy(&tcb->estado, stream + offset , sizeof(char));
			offset += sizeof(char);
			memcpy(&tcb->pos_x, stream + offset, sizeof(uint32_t));
			offset += sizeof(uint32_t);
			memcpy(&tcb->pos_y, stream + offset, sizeof(uint32_t));
			offset += sizeof(uint32_t);
			memcpy(&tcb->prox_instruccion, stream + offset, sizeof(uint32_t));
			offset += sizeof(uint32_t);
			memcpy(&tcb->puntero_pcb, stream + offset, sizeof(uint32_t));
			offset += sizeof(uint32_t);
			return tcb;
}


void buscar_y_modificar_tabla_segmento(t_segmento* segmento){
	//busco la tabla de segmento
	bool buscar_tabla_con_pid(void* tabla_segmento){
		t_tabla_segmento* tabla_segmento_a_buscar = (t_tabla_segmento*) tabla_segmento;
									return tabla_segmento_a_buscar->id_patota == segmento->id;
								}
	pthread_mutex_lock(&mutex_lista_tabla_segmentos);
	t_tabla_segmento* tabla_encontrada =(t_tabla_segmento*) list_find(lista_tabla_segmentos,(void*)buscar_tabla_con_pid);
	if(tabla_encontrada != NULL){
		list_add(tabla_encontrada->lista_segmentos,segmento);
		list_remove_by_condition(lista_tabla_segmentos,(void*)buscar_tabla_con_pid);
		list_add(lista_tabla_segmentos,tabla_encontrada);
		log_info(logger,"Se guarda en la tabla de segmento de la patota %d, el segmento nro %d , inicio %d, tamanio %d",tabla_encontrada->id_patota,segmento->nro_segmento,segmento->inicio,segmento->tamanio);
	}
	pthread_mutex_unlock(&mutex_lista_tabla_segmentos);


}


t_tabla_segmento* crear_tabla_segmentos(t_segmento* segmento_patota){
	t_tabla_segmento* tabla_segmento = malloc(sizeof(t_tabla_segmento));
	tabla_segmento->id_patota = segmento_patota->id;
	tabla_segmento->lista_segmentos = list_create();
	list_add(tabla_segmento->lista_segmentos,segmento_patota);
return tabla_segmento;
}


void registrarTripulante(t_tcb* tripulante,uint32_t pid){
	log_info(logger, "REGISTRANDO TRIPULANTE %d ...",tripulante->tid);
		if(strcmp(ALGORITMO_MEMORIA,"SEGMENTACION")== 0){
			//procedo a guardarlo en un segmento
			t_segmento* segmento = crear_segmento_tripulante(tripulante,pid);
			bool resultado = aplicar_segmentacion(segmento);
			if(resultado){
				guardar_tripulante_en_memoria(tripulante, segmento->inicio);
				log_info(logger, "TRIPULANTE %d REGISTRADO",tripulante->tid);
			}else{
				log_info("MEMORIA INSUFICIENTE PARA ALMACENAR EL TRIPULANTE %d del PID %d",tripulante->tid,pid);
			}
		}else{
			aplicar_paginacion_tripulante(tripulante, pid);
			log_info(logger, "tripulante %d registrado",tripulante->tid);
		}
		free(tripulante);
}


t_segmento* buscar_segmento_tid_de_tabla_general(uint32_t tid){
	bool buscar_tripulante_con_tid(void* tripulante){
											t_segmento* tripulante_a_buscar = (t_segmento*) tripulante;
											return tripulante_a_buscar->idHilo == tid;
										}
	pthread_mutex_lock(&mutex_tabla_segmento_general);
		t_segmento* segmento= (t_segmento*) list_find(tabla_segmentos_general,(void*)buscar_tripulante_con_tid);
		pthread_mutex_unlock(&mutex_tabla_segmento_general);
		return segmento;
}

t_tcb* buscar_tripulante_segmentacion(uint32_t tid){
	t_segmento* segmento_tcb_encontrado =buscar_segmento_tid_de_tabla_general(tid);
	return obtener_tcb_de_memoria(segmento_tcb_encontrado->inicio,segmento_tcb_encontrado->tamanio);


}
t_pcb* buscar_patota_de_memoria_con_pid(uint32_t pid){
	if(strcmp(ALGORITMO_MEMORIA,"SEGMENTACION")== 0){
		return buscar_patota_segmentacion( pid);
	}else{
		return buscar_pcb_paginacion(pid);
	}
}

t_pcb* buscar_patota_segmentacion(uint32_t pid){
	bool buscar_tabla_segmentos_con_pid(void* tabla_segmento){
		t_tabla_segmento* tabla_a_buscar = (t_tabla_segmento*) tabla_segmento;
		return tabla_a_buscar->id_patota == pid;
	}

	bool buscar_segmento_patota_con_pid(void* segmento){
		t_segmento* segmento_a_buscar = (t_segmento*) segmento;
		return segmento_a_buscar->id == pid && segmento_a_buscar->esPatota;
	}
	pthread_mutex_lock(&mutex_lista_tabla_segmentos);
	t_tabla_segmento* tabla_segmento_encontrada = (t_tabla_segmento*) list_find(lista_tabla_segmentos,(void*)buscar_tabla_segmentos_con_pid);
	t_segmento* segmento_patota = (t_segmento*) list_find(tabla_segmento_encontrada->lista_segmentos,(void*)buscar_segmento_patota_con_pid);
	pthread_mutex_unlock(&mutex_lista_tabla_segmentos);
	//obtengo el pcb de memoria
	return obtener_pcb_de_memoria(segmento_patota->inicio,segmento_patota->tamanio);


}

char* buscar_tareas(uint32_t pid){
	if(strcmp(ALGORITMO_MEMORIA,"SEGMENTACION")== 0){
	return	buscar_tareas_segmentacion(pid);
	}else{
		return buscar_tareas_paginacion(pid);
	}
}

char* buscar_tareas_segmentacion(uint32_t pid){

	t_segmento * segmento_encontrado = buscar_segmento_tareas_con_pid( pid);
	return obtener_tareas_de_memoria(segmento_encontrado->inicio,segmento_encontrado->tamanio);
}

t_segmento* buscar_segmento_tareas_con_pid(uint32_t pid){
	bool buscar_tabla_segmentos_con_pid(void* tabla_segmento){
				t_tabla_segmento* tabla_a_buscar = (t_tabla_segmento*) tabla_segmento;
				return tabla_a_buscar->id_patota == pid;
			}

		bool buscar_segmento_tareas_con_pid(void* segmento){
			t_segmento* segmento_a_buscar = (t_segmento*) segmento;
			return segmento_a_buscar->esTareas && segmento_a_buscar->id == pid;
		}
		pthread_mutex_lock(&mutex_lista_tabla_segmentos);
		t_tabla_segmento* tabla_segmento_encontrada = (t_tabla_segmento*) list_find(lista_tabla_segmentos,(void*)buscar_tabla_segmentos_con_pid);
		pthread_mutex_unlock(&mutex_lista_tabla_segmentos);
		return (t_segmento*) list_find(tabla_segmento_encontrada->lista_segmentos,(void*)buscar_segmento_tareas_con_pid);
}


char* obtener_tareas_de_memoria(uint32_t inicio, uint32_t tamanio){
	void * stream = malloc(tamanio);
	pthread_mutex_lock(&mutex_bloque_memoria);
	memcpy(stream, BLOQUE_MEMORIA + inicio, tamanio);
	pthread_mutex_unlock(&mutex_bloque_memoria);
	return (char*) stream;
}

t_tcb* obtener_tcb_de_memoria(uint32_t inicio, uint32_t tamanio){
	void * stream = malloc(tamanio);
	pthread_mutex_lock(&mutex_bloque_memoria);
	memcpy(stream, BLOQUE_MEMORIA + inicio, tamanio);
	pthread_mutex_unlock(&mutex_bloque_memoria);
	t_tcb* tcb = deserializar_tripulante(stream);
	free(stream);
	return tcb;
}

t_pcb* obtener_pcb_de_memoria(uint32_t inicio, uint32_t tamanio){
	void * stream = malloc(tamanio);
	pthread_mutex_lock(&mutex_bloque_memoria);
	memcpy(stream, BLOQUE_MEMORIA + inicio, tamanio);
	pthread_mutex_unlock(&mutex_bloque_memoria);
	t_pcb* pcb = deserializar_patota(stream);
	free(stream);
	return pcb;
}


void eliminar_segmento_de_tabla_segmento_de_patota(uint32_t numeroSegmento){
	//busco dentro de la tabla si su lista de segmentos contiene al segmento
	bool buscar_segmento_en_tabla_segmento(void* segmento){
		t_segmento* segmento_a_buscar = (t_segmento*) segmento;
		return segmento_a_buscar->nro_segmento == numeroSegmento;
	}

	//busco la tabla de segmentos donde pertenece
	bool buscar_tabla_segmento_con_nro(void* tabla_segmento){
		t_tabla_segmento* tabla_segmento_a_buscar = (t_tabla_segmento*) tabla_segmento;
		return list_any_satisfy(tabla_segmento_a_buscar->lista_segmentos,(void*) buscar_segmento_en_tabla_segmento);
	}
	pthread_mutex_lock(&mutex_lista_tabla_segmentos);
	t_tabla_segmento* tabla_encontrada =(t_tabla_segmento*) list_find(lista_tabla_segmentos,(void*)buscar_tabla_segmento_con_nro);
	if(tabla_encontrada != NULL){
		list_remove_by_condition(tabla_encontrada->lista_segmentos,(void*)buscar_segmento_en_tabla_segmento);

		log_info(logger, "Se vacio el segmento %d de la tabla de segmentos del proceso %d",numeroSegmento,tabla_encontrada->id_patota);
	}
	pthread_mutex_unlock(&mutex_lista_tabla_segmentos);


}

void eliminar_segmento_tripulante_de_tabla_general(t_segmento* segmento_tripulante){
	pthread_mutex_lock(&mutex_tabla_segmento_general);
	for(int i=0;i<tabla_segmentos_general->elements_count;i++){
		t_segmento* segmento_a_buscar = list_get(tabla_segmentos_general,i);
		if(segmento_a_buscar->nro_segmento == segmento_tripulante->nro_segmento){
			segmento_a_buscar->libre = true;
			segmento_a_buscar->esPatota = false;
			segmento_a_buscar->esTareas = false;
			segmento_a_buscar->id = 0;
			segmento_a_buscar->idHilo = 0;
			log_info(logger,"El segmento %d se ha vaciado en la tabla general",segmento_tripulante->nro_segmento);
		}
	}
	pthread_mutex_unlock(&mutex_tabla_segmento_general);
}

void eliminar_tripulante_de_memoria2(uint32_t tid){
	//buscamos el tcb del uid;
	if(strcmp(ALGORITMO_MEMORIA,"SEGMENTACION")== 0){
		t_segmento* segmento_tripulante = buscar_segmento_tid_de_tabla_general(tid);
		eliminar_segmento_tripulante_de_tabla_general(segmento_tripulante);
		eliminar_segmento_de_tabla_segmento_de_patota(segmento_tripulante->nro_segmento);

		eliminar_tripulante_en_mapa(tid);
	}else{

		eliminar_tripulante_de_tabla_paginas(tid);
		eliminar_tripulante_en_mapa(tid);
	}
}

char* eliminar_tripulante_de_memoria(uint32_t tid){
	//buscamos el tcb del uid;
	if(strcmp(ALGORITMO_MEMORIA,"SEGMENTACION")== 0){
		t_segmento* segmento_tripulante = buscar_segmento_tid_de_tabla_general(tid);
		eliminar_segmento_tripulante_de_tabla_general(segmento_tripulante);
		eliminar_segmento_de_tabla_segmento_de_patota(segmento_tripulante->nro_segmento);

		eliminar_tripulante_en_mapa(tid);
		char* mensaje = string_new();
					string_append(&mensaje, " Se elimina el tripulante ");
					string_append(&mensaje, string_itoa(tid));
					return mensaje;
	}else{

		eliminar_tripulante_de_tabla_paginas(tid);
		eliminar_tripulante_en_mapa(tid);
		char* mensaje = string_new();
							string_append(&mensaje, " Se elimina el tripulante ");
							string_append(&mensaje, string_itoa(tid));
							return mensaje;
	}
}

void eliminar_tripulante_de_tabla_paginas(uint32_t tid){
	//no es necesario buscarlo en memoria y traerlo, solo hay que borrar su registro en tabla


	void eliminar_paginas(void* pagina){
			t_pagina* pagina_a_borrar = (t_pagina*) pagina;
			free(pagina_a_borrar);
		}
	/*
	void eliminar_id_tripulante(void* id_tripulante){
			uint32_t id_tripulante_a_borrar = (uint32_t) id_tripulante;
			free(id_tripulante_a_borrar);
		}*/

	bool buscar_id_tripulante(void* id){
		uint32_t id_a_buscar = (uint32_t) id;
		return id_a_buscar ==tid;
	}

	/*
	void eliminar_id_tripulante(void* id_tripulante){
			uint32_t id_tripulante_a_borrar = (uint32_t) id_tripulante;
			free(id_tripulante_a_borrar);
		}*/

	t_tabla_paginas* tabla_pagina_encontrada =obtener_tabla_paginas_de_tid(tid);
	int posicion_tripulante = buscar_posicion_de_tcb_en_tabla(tabla_pagina_encontrada,tid);
	int inicio = tabla_pagina_encontrada->fin_tareas + (posicion_tripulante *3);
	int limite = inicio + 3;


	t_pagina* pagina_a_borrar = NULL;
	t_frame* frame_a_liberar = NULL;
	for(int i=inicio;i<limite;i++){
		pagina_a_borrar = (t_pagina*)list_get(tabla_pagina_encontrada->lista_paginas,i);
		if(pagina_a_borrar->enMemoriaSecundaria){
			pthread_mutex_lock(&mutex_lista_paginas_secundaria_general);
			frame_a_liberar = (t_frame*) list_get(lista_paginas_secundaria_general,pagina_a_borrar->nro_frame_secundario);
			liberar_frame(frame_a_liberar);
			log_info(logger,"Se libera el frame %d de MS",frame_a_liberar->nro_frame);
			pthread_mutex_unlock(&mutex_lista_paginas_secundaria_general);
		}else{
			pthread_mutex_lock(&mutex_lista_paginas_principal_general);
			frame_a_liberar = (t_frame*) list_get(lista_paginas_principal_general,pagina_a_borrar->nro_frame);
			liberar_frame(frame_a_liberar);
			log_info(logger,"Se libera el frame %d de MP",frame_a_liberar->nro_frame);
			pthread_mutex_unlock(&mutex_lista_paginas_principal_general);
		}
		list_remove_and_destroy_element(tabla_pagina_encontrada->lista_paginas,i,(void*) eliminar_paginas);
		log_info(logger,"Se elimino la pagina %d, de la tabla de paginas de la patota %d",i,tid);
	}
	//list_remove_and_destroy_by_condition(tabla_pagina_encontrada->lista_id_tripulantes,(void*)buscar_id_tripulante,(void*)eliminar_id_tripulante);
	list_remove_by_condition(tabla_pagina_encontrada->lista_id_tripulantes,(void*)buscar_id_tripulante);
}

void liberar_frame(t_frame* frame){
	frame->libre = true;
	frame->usado = false;
	frame->en_uso = false;
}

void eliminar_tripulante_en_mapa(uint32_t tid){
	/*
	pthread_mutex_lock(&mutex_mapa);
	err = item_borrar(nivel, tid+'0');
	nivel_gui_dibujar(nivel);
	pthread_mutex_unlock(&mutex_mapa);
	*/
}

void mover_tripulante_en_mapa(uint32_t tid,uint32_t pos_x,uint32_t pos_y){
	/*
	pthread_mutex_lock(&mutex_mapa);
	err = item_mover(nivel, tid+'0', pos_x, pos_y);
	nivel_gui_dibujar(nivel);
	pthread_mutex_unlock(&mutex_mapa);
	*/
}

void modificarPosicionTripulante2(uint32_t tid, uint32_t posicion_x, uint32_t posicion_y){

	if(strcmp(ALGORITMO_MEMORIA,"SEGMENTACION")== 0){
	t_tcb* tcb_encontrado= buscar_tripulante_segmentacion(tid);
	t_segmento* segmento_tripulante = buscar_segmento_tid_de_tabla_general(tid);
	guardar_tripulante_en_memoria(tcb_encontrado, segmento_tripulante->inicio);
	mover_tripulante_en_mapa(tcb_encontrado->tid,tcb_encontrado->pos_x,tcb_encontrado->pos_y);
	log_info(logger, "El tripulante %d se movio a la posicion %d|%d",tid,posicion_x, posicion_y);
	free(tcb_encontrado);
	}else{
		t_tcb* tcb_encontrado = buscar_tcb_paginacion(tid);
		tcb_encontrado->pos_x= posicion_x;
		tcb_encontrado->pos_y = posicion_y;
		guardar_tripulante_paginacion(tcb_encontrado);
		mover_tripulante_en_mapa(tcb_encontrado->tid,tcb_encontrado->pos_x,tcb_encontrado->pos_y);
		log_info(logger, "El tripulante %d se movio a la posicion %d|%d",tid,posicion_x, posicion_y);
		free(tcb_encontrado);
	}
}

char* modificarPosicionTripulante(uint32_t tid, char* posicion){

	if(strcmp(ALGORITMO_MEMORIA,"SEGMENTACION")== 0){
	t_tcb* tcb_encontrado= buscar_tripulante_segmentacion(tid);
	char** posiciones = string_n_split( posicion,2,"|");
	tcb_encontrado->pos_x=(uint32_t) atoi(posiciones[0]);
	tcb_encontrado->pos_y = (uint32_t)atoi(posiciones[1]);
	t_segmento* segmento_tripulante = buscar_segmento_tid_de_tabla_general(tid);
	guardar_tripulante_en_memoria(tcb_encontrado, segmento_tripulante->inicio);
	mover_tripulante_en_mapa(tcb_encontrado->tid,tcb_encontrado->pos_x,tcb_encontrado->pos_y);
	log_info(logger, "El tripulante %d se movio a la posicion %s",tid,posicion);
	char* mensaje = string_new();
			string_append(&mensaje, "tripulante ");
			string_append(&mensaje, string_itoa(tid));
			string_append(&mensaje, " movido");
			return mensaje;
	}else{
		t_tcb* tcb_encontrado = buscar_tcb_paginacion(tid);
		char** posiciones = string_n_split( posicion,2,"|");
		tcb_encontrado->pos_x=(uint32_t) atoi(posiciones[0]);
		tcb_encontrado->pos_y = (uint32_t)atoi(posiciones[1]);
		guardar_tripulante_paginacion(tcb_encontrado);
		mover_tripulante_en_mapa(tcb_encontrado->tid,tcb_encontrado->pos_x,tcb_encontrado->pos_y);
		log_info(logger, "El tripulante %d se movio a la posicion %s",tid,posicion);

		char* mensaje = string_new();
					string_append(&mensaje, "tripulante ");
					string_append(&mensaje, string_itoa(tid));
					string_append(&mensaje, " movido");
					return mensaje;
	}
}

void iniciarTripulante2(uint32_t pid, uint32_t cantidad, char* posiciones){
	t_pcb* pcb =buscar_patota_de_memoria_con_pid( pid);
	char** listaPosiciones = string_split( posiciones," ");
	pthread_mutex_lock(&mutex_contador_id_tripulantes);
	bool sigue_iterando = true;
	for(int i=0;i<cantidad;i++){
		if(listaPosiciones[i] != NULL && sigue_iterando){
			t_tcb* tcb = crear_estructura_tcb(contador_id_tripulantes++,listaPosiciones[i],(uint32_t)pcb);
			registrarTripulante(tcb,pid);
			mapear_tripulante(tcb->pos_x,tcb->pos_y,tcb->tid);
			free(tcb);
		}
		if(listaPosiciones[i] == NULL || !sigue_iterando){
			sigue_iterando = false;
			t_tcb* tcb = crear_estructura_tcb(contador_id_tripulantes++,"0|0",(uint32_t)pcb);
			registrarTripulante(tcb,pid);
			mapear_tripulante(tcb->pos_x,tcb->pos_y,tcb->tid);
			free(tcb);
		}
	}
	pthread_mutex_unlock(&mutex_contador_id_tripulantes);
	free(pcb);
	free(listaPosiciones);
}

char* iniciarTripulante(uint32_t pid, uint32_t cantidad, char* posiciones){
	t_pcb* pcb =buscar_patota_de_memoria_con_pid( pid);
	char** listaPosiciones = string_split( posiciones," ");
	pthread_mutex_lock(&mutex_contador_id_tripulantes);
	char* posicion = string_new();
	bool sigue_iterando = true;
	for(int i=0;i<cantidad;i++){
		if(listaPosiciones[i] != NULL && sigue_iterando){
			posicion = listaPosiciones[i];

		}
		if(listaPosiciones[i] == NULL || !sigue_iterando){
			posicion = "0|0";
			sigue_iterando = false;
		}
		t_tcb* tcb = crear_estructura_tcb(contador_id_tripulantes++,posicion,(uint32_t)pcb);
		registrarTripulante(tcb,pid);
		mapear_tripulante(tcb->pos_x,tcb->pos_y,tcb->tid);
	}
	pthread_mutex_unlock(&mutex_contador_id_tripulantes);
	return "OK";
}

char* solicitarTarea(uint32_t pid,uint32_t tid){
	log_info(logger,"Se solicita tarea para el tripulante %d de la patota %d",tid,pid);
	char* tareas = buscar_tareas(pid);
	log_info(logger,"Tarea obtenida de patota %d, TAREAS: %s",pid,tareas);
	if(strcmp(ALGORITMO_MEMORIA,"SEGMENTACION")== 0){
		t_tcb* tcb = buscar_tripulante_segmentacion(tid);
		t_segmento* segmento_encontrado = buscar_segmento_tid_de_tabla_general(tid);
		//divido las tareas
		char** listaTareas = string_split(tareas,"|");
		if(listaTareas[tcb->prox_instruccion] == NULL){
			log_info(logger,"La lista de tareas para el tripulante de la  patota %d esta vacia, se retorna mensaje FIN 0;0;0;0",tid,pid);
			return "FIN 0;0;0;0";
		}else{
			char* tarea =listaTareas[tcb->prox_instruccion];
			log_info(logger,"Tarea a enviar al tripulante %d: %s",tid,tarea);
			tcb->prox_instruccion ++;
			guardar_tripulante_en_memoria(tcb, segmento_encontrado->inicio);
			return tarea;
			}
		free(tcb);
		free(listaTareas);
	}else{
		t_tcb* tcb = buscar_tcb_paginacion(tid);
		char** listaTareas = string_split(tareas,"|");
		if(listaTareas[tcb->prox_instruccion] == NULL){
			log_info(logger,"La lista de tareas para el tripulante de la  patota %d esta vacia, se retorna mensaje FIN 0;0;0;0",tid,pid);
			return "FIN 0;0;0;0";
		}else{
			char* tarea =listaTareas[tcb->prox_instruccion];
			log_info(logger,"Tarea a enviar: %s",tarea);
			tcb->prox_instruccion ++;
			guardar_tripulante_paginacion(tcb);
			return tarea;
			}
		free(tcb);
		free(listaTareas);
	}

	free(tareas);
}

void actualizarTripulante2(uint32_t tid, char estado){
	log_info(logger, "Se solicita actualizar el tripulante %d a estado %c",tid,estado);
	if(strcmp(ALGORITMO_MEMORIA,"SEGMENTACION")== 0){
		t_tcb* tcb = buscar_tripulante_segmentacion(tid);
		tcb->estado = estado;
		t_segmento* segmento_encontrado = buscar_segmento_tid_de_tabla_general(tid);
		if(estado =='F'){
			segmento_encontrado->terminado = true;
		}
		guardar_tripulante_en_memoria(tcb, segmento_encontrado->inicio);
		verificar_tripulantes_de_tabla_segmentos(segmento_encontrado->id);
		log_info(logger,"Se modifica el status del tripulante %d con estado %c",tid,estado);
		free(tcb);
	}else{
		t_tcb* tcb = buscar_tcb_paginacion(tid);
		t_tabla_paginas* tabla=obtener_tabla_paginas_de_tid(tid);
		tcb->estado = estado;
		if(estado =='F'){
			tabla->cantidad_tcbs_terminados ++;
			verificar_tripulantes_de_tabla_paginas(tcb->tid,tabla);
		}else{
			guardar_tripulante_paginacion(tcb);
		}
		log_info(logger,"Se modifica el status del tripulante %d con estado %c",tid,estado);
		free(tcb);
	}
}

char* actualizarTripulante(uint32_t tid, char estado){
	if(strcmp(ALGORITMO_MEMORIA,"SEGMENTACION")== 0){
		t_tcb* tcb = buscar_tripulante_segmentacion(tid);
		tcb->estado = estado;
		t_segmento* segmento_encontrado = buscar_segmento_tid_de_tabla_general(tid);
		if(estado =='F'){
			segmento_encontrado->terminado = true;
		}
		guardar_tripulante_en_memoria(tcb, segmento_encontrado->inicio);
		verificar_tripulantes_de_tabla_segmentos(segmento_encontrado->id);
		log_info(logger,"Se modifica el status del tripulante %d",tid);
	}else{
		t_tcb* tcb = buscar_tcb_paginacion(tid);
		t_tabla_paginas* tabla=obtener_tabla_paginas_de_tid(tid);
		tcb->estado = estado;
		if(estado =='F'){
			tabla->cantidad_tcbs_terminados ++;
			verificar_tripulantes_de_tabla_paginas(tcb->tid,tabla);
		}else{
			guardar_tripulante_paginacion(tcb);
		}

	}
	return "N";
}

void verificar_tripulantes_de_tabla_paginas(uint32_t tid, t_tabla_paginas* tabla){
	//los frames que libere los voy a poner en en_uso = false
	int posicion_tripulante = buscar_posicion_de_tcb_en_tabla(tabla,tid);
		int inicio = tabla->fin_tareas + (posicion_tripulante*3);
		int limite = inicio +3;
	if(tabla->cantidad_tcbs_terminados == tabla->lista_id_tripulantes->elements_count){
		liberar_en_uso_frames_de_tabla_paginas(tabla,inicio,limite);
		eliminar_tabla_de_paginas(tabla->pid);
	}
	//busco los frames en MP,
}

void eliminar_tabla_de_paginas(uint32_t pid){
	//elimino la tabla

	void eliminar_paginas(void* pagina){
		t_pagina* pagina_a_borrar = (t_pagina*) pagina;
		free(pagina_a_borrar);
	}

	/*void eliminar_id_tripulante(void* id_tripulante){
		uint32_t id_tripulante_a_borrar = (uint32_t) id_tripulante;
		free(id_tripulante_a_borrar);
	}*/

	void eliminar_tabla_paginas(void* tabla){
		t_tabla_paginas* tabla_a_borrar = (t_tabla_paginas*) tabla;
		//libero las listas
		list_destroy_and_destroy_elements(tabla_a_borrar->lista_paginas,(void*)eliminar_paginas);
		list_destroy(tabla_a_borrar->lista_id_tripulantes);

		free(tabla_a_borrar);
	}

	bool buscar_tabla(void* tabla){
		t_tabla_paginas* tabla_a_borrar = (t_tabla_paginas*) tabla;
		return tabla_a_borrar->pid == pid;
	}
	pthread_mutex_lock(&mutex_lista_tabla_paginas);
	list_remove_and_destroy_by_condition(lista_tabla_paginas, (void*)buscar_tabla, (void*)eliminar_tabla_paginas);
	pthread_mutex_unlock(&mutex_lista_tabla_paginas);
	log_info(logger,"Se elimino la tabla de paginas de la Patota %d",pid);
}

void liberar_en_uso_frames_de_tabla_paginas(t_tabla_paginas* tabla, int inicio,int limite){
	t_frame* frame_a_liberar = NULL;
	t_pagina* pagina =NULL;
	for(int i=inicio;i<limite;i++){
		pagina = (t_pagina*) list_get(tabla->lista_paginas,i);
		if(pagina->enMemoriaSecundaria){
			pthread_mutex_lock(&mutex_lista_paginas_secundaria_general);
			frame_a_liberar = (t_frame*) list_get(lista_paginas_secundaria_general,pagina->nro_frame_secundario);
			liberar_frame(frame_a_liberar);
			pthread_mutex_unlock(&mutex_lista_paginas_secundaria_general);
		}else{
			pthread_mutex_lock(&mutex_lista_paginas_principal_general);
			frame_a_liberar = (t_frame*) list_get(lista_paginas_principal_general,pagina->nro_frame_secundario);
			liberar_frame(frame_a_liberar);
			pthread_mutex_unlock(&mutex_lista_paginas_principal_general);
		}

	}
}

void verificar_tripulantes_de_tabla_segmentos(uint32_t pid){
	bool buscar_tabla_con_pid(void* tabla_segmento){
			t_tabla_segmento* tabla_segmento_a_buscar = (t_tabla_segmento*) tabla_segmento;
										return tabla_segmento_a_buscar->id_patota == pid;
									}

	bool tripulantes_terminados(void* segmento){
		t_segmento* segmento_a_evaluar = (t_segmento*) segmento;
		if( segmento_a_evaluar->idHilo != 0){
			return segmento_a_evaluar->terminado;
		}else{
		return true;
		}
	}

	void destroy(void* stream){
		//solo elimino la tabla, no sus segmentos que son los de la tabla gral
		t_tabla_segmento* tabla_a_borrar = (t_tabla_segmento*) stream;
		list_destroy(tabla_a_borrar->lista_segmentos);
		free(tabla_a_borrar);
	}

		t_tabla_segmento* tabla_encontrada =(t_tabla_segmento*) list_find(lista_tabla_segmentos,(void*)buscar_tabla_con_pid);
		if(list_all_satisfy(tabla_encontrada->lista_segmentos,(void*)tripulantes_terminados)){
			//limpio los segmentos de la tabla de segmentos general
			for(int i=0;i<tabla_encontrada->lista_segmentos->elements_count;i++){
				t_segmento* segmento_a_limpiar =(t_segmento*) list_get(tabla_segmentos_general,i);
				if(segmento_a_limpiar->id == pid){
					segmento_a_limpiar->esPatota = false;
					segmento_a_limpiar->esTareas = false;
					segmento_a_limpiar->idHilo =0;
					segmento_a_limpiar->libre = true;
					segmento_a_limpiar->terminado = false;
					segmento_a_limpiar->id = 0;
				}
			}
			list_remove_and_destroy_by_condition(lista_tabla_segmentos,(void*)buscar_tabla_con_pid,(void*) destroy);
		}
}

void dump_memoria(){
	if(strcmp(ALGORITMO_MEMORIA,"SEGMENTACION")== 0){
		dump_memoria_segmentacion();
	}else{
		dump_memoria_paginacion();
	}
}

void dump_memoria_paginacion(){
	log_info(logger, "INICIANDO DUMP DE LA MEMORIA..");
	char* nombre_archivo = string_new();
	nombre_archivo = temporal_get_string_time("Dump_%y%m%d%H%M%S.dmp");
	pthread_mutex_lock(&mutex_dump);
	FILE* archivo = fopen(nombre_archivo,"w+");
	if(archivo == NULL){
		log_info(logger,"No se pudo crear el archivo");
	}
	char* fecha= string_new();
	fecha = temporal_get_string_time("%d/%m/%y %H:%M:%S");
	procesar_string_paginacion(fecha,archivo);
	fclose(archivo);
	pthread_mutex_unlock(&mutex_dump);
	log_info(logger, "FINALIZANDO DUMP DE LA MEMORIA..");
	free(fecha);
	free(nombre_archivo);
}

void dump_memoria_segmentacion(){
	log_info(logger, "INICIANDO DUMP DE LA MEMORIA..");
	char* nombre_archivo = string_new();
	nombre_archivo = temporal_get_string_time("Dump_%y%m%d%H%M%S.dmp");
	pthread_mutex_lock(&mutex_dump);
	FILE* archivo = fopen(nombre_archivo,"w+");
	if(archivo == NULL){
		log_info(logger,"No se pudo crear el archivo");
	}
	char* fecha= string_new();
	fecha = temporal_get_string_time("%d/%m/%y %H:%M:%S");
	procesar_string_segmentacion(fecha,archivo);
	fclose(archivo);
	pthread_mutex_unlock(&mutex_dump);
	log_info(logger, "FINALIZANDO DUMP DE LA MEMORIA..");
	free(fecha);
	free(nombre_archivo);
}
void procesar_string_segmentacion(char* hora,FILE* archivo){

	fprintf(archivo,"%s","---------------------------------\n");
	fprintf(archivo,"%s %s %s","Dump:",hora,"\n");
	//paso a recorrer las tablas de segmentacion por patota
	pthread_mutex_lock(&mutex_lista_tabla_segmentos);
	for(int i=0;i<lista_tabla_segmentos->elements_count;i++){
		t_tabla_segmento* tabla_a_buscar =(t_tabla_segmento*) list_get(lista_tabla_segmentos,i);
		//tabla_a_buscar->id_patota; tengo el id patota
		for(int j=0;j<tabla_a_buscar->lista_segmentos->elements_count;j++){
			t_segmento* segmento_a_buscar = (t_segmento*) list_get(tabla_a_buscar->lista_segmentos,j);
			if(segmento_a_buscar->inicio == 0){
				fprintf(archivo,"%s %d %s %d %s %s   %s %d%s ","Proceso:",tabla_a_buscar->id_patota,"Segmento:",segmento_a_buscar->nro_segmento,"Inicio:","0x0000","Tamanio:",segmento_a_buscar->tamanio,"b\n");
			}else{
				fprintf(archivo,"%s %d %s %d %s %p %s %d%s ","Proceso:",tabla_a_buscar->id_patota,"Segmento:",segmento_a_buscar->nro_segmento,"Inicio:",(void*)segmento_a_buscar->inicio,"Tamanio:",segmento_a_buscar->tamanio,"b\n");

			}


		}
	}
	pthread_mutex_unlock(&mutex_lista_tabla_segmentos);
	fprintf(archivo,"%s","---------------------------------\n");
}

void procesar_string_paginacion(char* hora,FILE* archivo){
	fprintf(archivo,"%s","---------------------------------\n");
	fprintf(archivo,"%s %s %s","Dump:",hora,"\n");
	pthread_mutex_lock(&mutex_lista_tabla_segmentos);

	t_frame* frame = NULL;
	t_pagina* pagina =NULL;
	pthread_mutex_lock(&mutex_lista_paginas_principal_general);
	pthread_mutex_lock(&mutex_lista_tabla_paginas);
	for(int i=0;i<CANTIDAD_PAGINAS;i++){
		frame = (t_frame*) list_get(lista_paginas_principal_general,i);
		if(frame->libre){
			fprintf(archivo,"%s %d %s","Marco:",frame->nro_frame,"Estado: Libre Proceso: - Pagina: -\n");
		}else{
			pagina =buscar_pagina_que_use_frame( frame->nro_frame);
			fprintf(archivo,"%s %d %s %d %s %d%s","Marco:",frame->nro_frame,"Estado: Ocupado Proceso:",pagina->pid,"Pagina:",pagina->nro_pagina,"\n");
		}
	}

	pthread_mutex_unlock(&mutex_lista_paginas_principal_general);
	pthread_mutex_unlock(&mutex_lista_tabla_paginas);
	fprintf(archivo,"%s","---------------------------------\n");
}

t_pagina* buscar_pagina_que_use_frame(uint32_t nro_frame){

	bool buscar_en_paginas(void* pagina){
		t_pagina* pagina_a_buscar = (t_pagina*) pagina;
		return !pagina_a_buscar->enMemoriaSecundaria && pagina_a_buscar->nro_frame == nro_frame;
	}

	bool buscar_tabla_pagina(void* tabla){
		t_tabla_paginas* tabla_a_buscar = (t_tabla_paginas*) tabla;
		return list_any_satisfy(tabla_a_buscar->lista_paginas,(void*) buscar_en_paginas);
	}


	t_tabla_paginas* tabla_pagina = (t_tabla_paginas* ) list_find(lista_tabla_paginas,(void*) buscar_tabla_pagina);
	return (t_pagina*)list_find(tabla_pagina->lista_paginas,(void*)buscar_en_paginas );
}

int existe_espacio_disponible_en_memoria(uint32_t peso){
	//verifica que si hay espacio, retorna 0, sino devuelve la cantidad de paginas faltantes a cargar en memoria
	uint32_t cantidadPaginasRequeridas = peso/ TAMANIO_PAGINA;
	log_info(logger,"Verificar espacio disponible para almacenar %d bytes",peso);
	int cantidadPaginasLibres =0;
	//recorro el bitmap para saber cuantos frames estan libres
	t_frame* frame = NULL;
	pthread_mutex_lock(&mutex_lista_paginas_principal_general);
	for(int i=0;i<lista_paginas_principal_general->elements_count;i++){
		frame = (t_frame*)list_get(lista_paginas_principal_general,i);
		if(frame->libre){
			cantidadPaginasLibres++;
		}
	}
	pthread_mutex_unlock(&mutex_lista_paginas_principal_general);

	if( cantidadPaginasLibres >= cantidadPaginasRequeridas){
		log_info(logger,"Hay suficientes paginas para guardar el elemento");
		return 0;
	}else{
		int faltante = cantidadPaginasRequeridas -cantidadPaginasLibres;
		log_info(logger,"No Hay suficientes paginas para guardar el elemento, faltan %d paginas",faltante);
		return faltante;
	}
}

void iniciar_memoria_secundaria(){
	ALGORITMO_REEMPLAZO = config_get_string_value(config,"ALGORITMO_REEMPLAZO");
	TAMANIO_MEMORIA_SECUNDARIA = atoi(config_get_string_value(config,"TAMANIO_SWAP") ) /8;
	CANTIDAD_PAGINAS_SECUNDARIA = TAMANIO_MEMORIA_SECUNDARIA/TAMANIO_PAGINA;
	PATH_SWAP = config_get_string_value(config,"PATH_SWAP");
	archivo_memoria_secundaria = open(PATH_SWAP,O_CREAT|O_RDWR, 07777);
	if(archivo_memoria_secundaria == -1){
		log_info(logger,"No se pudo abrir el archivo de swap");
	}else{
		ftruncate(archivo_memoria_secundaria, TAMANIO_MEMORIA_SECUNDARIA);
		//inicio de la memoria secundaria con mmap
		BLOQUE_MEMORIA_SECUNDARIA = mmap(NULL, TAMANIO_MEMORIA_SECUNDARIA,PROT_WRITE|PROT_READ,MAP_SHARED|MAP_FILE,archivo_memoria_secundaria, 0);
		//msync(BLOQUE_MEMORIA_SECUNDARIA, TAMANIO_MEMORIA_SECUNDARIA, MS_SYNC); //sincronizar

		//close(archivo_memoria_secundaria);

	}
}



t_pcb* buscar_pcb_paginacion(uint32_t pid){
	pthread_mutex_lock(&mutex_lista_tabla_paginas);
	t_tabla_paginas* tabla_pagina_encontrada = buscar_tabla_paginas(pid);
	pthread_mutex_unlock(&mutex_lista_tabla_paginas);
	int inicio = 0;
	int limite = tabla_pagina_encontrada->inicio_tareas;
	t_pagina* pagina = NULL;
	t_frame frame = NULL;
	chequear_paginas_en_memoria_principal(tabla_pagina_encontrada,inicio,limite);
	t_pcb* pcb = deserializar_pcb(   buscar_elemento_en_memoria_principal(tabla_pagina_encontrada,PCB,0,true));
	for(int i=inicio;i<limite;i++){
			pagina = (t_pagina*)list_get(tabla_pagina_encontrada->lista_paginas,i);
			pthread_mutex_lock(&mutex_lista_paginas_principal_general);
			frame = (t_frame*)list_get(lista_paginas_principal_general,pagina->nro_frame);
			frame->en_uso = false;
			pthread_mutex_unlock(&mutex_lista_paginas_principal_general);
		}
	return  pcb;
}

char* buscar_tareas_paginacion(uint32_t pid){

	pthread_mutex_lock(&mutex_lista_tabla_paginas);
	t_tabla_paginas* tabla_pagina_encontrada =buscar_tabla_paginas(pid);
	pthread_mutex_unlock(&mutex_lista_tabla_paginas);
	int inicio = tabla_pagina_encontrada->inicio_tareas;
	int limite = tabla_pagina_encontrada->fin_tareas;
	chequear_paginas_en_memoria_principal(tabla_pagina_encontrada,inicio,limite);

	char* tareas = (char*) buscar_elemento_en_memoria_principal(tabla_pagina_encontrada,TAREAS,2, false);
	t_pagina* pagina =NULL;
	t_frame* frame = NULL;
	//procedo a liberar el flag de en_uso
	for(int i=inicio;i<limite;i++){
		pagina = (t_pagina*)list_get(tabla_pagina_encontrada->lista_paginas,i);
		pthread_mutex_lock(&mutex_lista_paginas_principal_general);
		frame = (t_frame*)list_get(lista_paginas_principal_general,pagina->nro_frame);
		frame->en_uso = false;
		pthread_mutex_unlock(&mutex_lista_paginas_principal_general);
	}

	return tareas;
}

t_tabla_paginas* obtener_tabla_paginas_de_tid(uint32_t tid){
	bool buscar_id(void* numero){
			uint32_t num_a_buscar = (uint32_t) numero;
			return num_a_buscar == tid;
		}

		t_tabla_paginas* tabla_pagina_encontrada =NULL;
		pthread_mutex_lock(&mutex_lista_tabla_paginas);
		//busco tabla de paginas
		for(int i=0;i<lista_tabla_paginas->elements_count;i++){
			tabla_pagina_encontrada =(t_tabla_paginas*) list_get(lista_tabla_paginas,i);
			if(list_any_satisfy(tabla_pagina_encontrada->lista_id_tripulantes,(void*)buscar_id)){
				break;
			}
		}
		pthread_mutex_unlock(&mutex_lista_tabla_paginas);
		return tabla_pagina_encontrada;
}

int buscar_posicion_de_tcb_en_tabla(t_tabla_paginas* tabla, uint32_t tid){
	int posicion_tripulante;
		int id_a_buscar;
		//busco donde se encuentra la primera pagina del tripulante
		for(int i=0;i<tabla->lista_id_tripulantes->elements_count;i++){
			id_a_buscar = (uint32_t) list_get(tabla->lista_id_tripulantes,i);
			if(id_a_buscar == tid){
				posicion_tripulante = i;
				break;
			}
		}
		return posicion_tripulante;
}

t_tcb* buscar_tcb_paginacion(uint32_t tid){


	t_tabla_paginas* tabla_pagina_encontrada =obtener_tabla_paginas_de_tid(tid);
	int posicion_tripulante = buscar_posicion_de_tcb_en_tabla(tabla_pagina_encontrada,tid);
	//el nuevo inicio sera el nro de la ultima pagina de las tareas + (3* indice de posicion del tripulante
	//es 3 por el tamanio actual del tcb es igual a 3 paginas, asi que hay que correr de a 3 para obtener la pagina donde comienza a guardar el tripulante
	int inicio =tabla_pagina_encontrada->fin_tareas + (posicion_tripulante * 3);
	int limite = inicio + 3;
	chequear_paginas_en_memoria_principal(tabla_pagina_encontrada,inicio,limite);
	void* tcbSerializado = buscar_elemento_en_memoria_principal(tabla_pagina_encontrada,TCB,tid,true);
	t_tcb* tcb = deserializar_tripulante(tcbSerializado);
	free(tcbSerializado);
	return tcb;
}

void chequear_paginas_en_memoria_principal(t_tabla_paginas* tabla, int inicio, int limite){
	//chequeo si estan todas en MP, si hay alguna que esta en MS, la paso a MP
	t_pagina* pagina = NULL;
	t_frame* frame = NULL;
	//debe hacer recorridos, uno para marcar los marcos que si estan en MP com en_uso = true;
	for(int i=inicio;i< limite;i++){
		pagina = (t_pagina*) list_get(tabla->lista_paginas,i);
		if(!pagina->enMemoriaSecundaria){
			pthread_mutex_lock(&mutex_lista_paginas_principal_general);
			frame = (t_frame*) list_get(lista_paginas_principal_general,pagina->nro_frame);
			frame->en_uso = true;
			pthread_mutex_unlock(&mutex_lista_paginas_principal_general);
			log_info(logger,"La pagina %d ya esta en MP (frame %d), se marca en_uso",pagina->nro_pagina,pagina->nro_frame);
		}
	}

	//busco la memoriaprincipal
	for(int i=inicio;i< limite;i++){
		pagina = (t_pagina*) list_get(tabla->lista_paginas,i);
		if(pagina->enMemoriaSecundaria){
			//lo traigo a MP
			log_info(logger,"Pagina %d de la tabla del PCB %d esta en MS (frame %d), lo traigo a MP",pagina->nro_pagina,pagina->pid,pagina->nro_frame_secundario);
			traer_pagina_a_memoria_principal(pagina);
		}
	}


}

void traer_pagina_a_memoria_principal(t_pagina* pagina){
	log_info(logger,"Se trae la pagina %d de la tabla del PCB %d de MS a MP",pagina->nro_pagina,pagina->pid);
	if(strcmp(ALGORITMO_REEMPLAZO,"LRU")== 0){
		int indice = mover_pagina_a_memoria_secundaria_lru();
		log_info(logger,"Se libera el frame %d ,llevando su contenido a MS",indice);
		//ocupo el frame que se acaba de liberar
		pthread_mutex_lock(&mutex_lista_paginas_principal_general);
		t_frame* frame_a_ocupar = (t_frame*) list_get(lista_paginas_principal_general,indice);
		log_info(logger,"Se procede a ocupar el frame %d con el contenido de la pagina %d de la tabla del PCB %d",indice,pagina->nro_pagina,pagina->pid);
		cargar_frame_a_memoria_principal(pagina,frame_a_ocupar);
		pthread_mutex_unlock(&mutex_lista_paginas_principal_general);
		log_info(logger,"Se trajo el contenido de la pagina %d de la tabla de paginas del PCB %d a MP",pagina->nro_pagina,pagina->pid);
	}else{
		int indice = mover_pagina_a_memoria_secundaria_clock();
		log_info(logger,"Se libera el frame %d ,llevando su contenido a MS",indice);
		//ocupo el frame que se acaba de liberar
		pthread_mutex_lock(&mutex_lista_paginas_principal_general);
		t_frame* frame_a_ocupar = (t_frame*) list_get(lista_paginas_principal_general,indice);
		log_info(logger,"Se procede a ocupar el frame %d con el contenido de la pagina %d de la tabla del PCB %d",indice,pagina->nro_pagina,pagina->pid);
		cargar_frame_a_memoria_principal(pagina,frame_a_ocupar);
		pthread_mutex_unlock(&mutex_lista_paginas_principal_general);
		log_info(logger,"Se trajo el contenido de la pagina %d de la tabla de paginas del PCB %d a MP",pagina->nro_pagina,pagina->pid);
	}
}

void cargar_frame_a_memoria_principal(t_pagina* pagina,t_frame* frame){

	void* stream = obtener_stream_de_MS(pagina);
	guardar_stream_en_frame_de_MP(frame->nro_frame,  stream,pagina->tamanio);

	//actualizo el registro de cambio en el frame y pagina
	actualizar_frame(frame);
	frame->libre = false;
	frame->en_uso = true;
	pagina->enMemoriaSecundaria = false;
	pagina->nro_frame = frame->nro_frame;
}

void* obtener_stream_de_MS(t_pagina* pagina){
	void* stream = malloc(pagina->tamanio);
	int offset = TAMANIO_PAGINA * pagina->nro_frame_secundario;
	pthread_mutex_lock(&mutex_bloque_memoria_secundario);
	memcpy(stream,BLOQUE_MEMORIA_SECUNDARIA+offset,pagina->tamanio);
	pthread_mutex_unlock(&mutex_bloque_memoria_secundario);
	log_info(logger,"Se obtuvo el stream del bloque de memoria secundaria, offset: %d , tamanio: %d",offset,pagina->tamanio);
	return stream;
}

void guardar_stream_en_frame_de_MP(uint32_t nro_frame, void* stream,uint32_t tamanio){
	int offset = nro_frame * TAMANIO_PAGINA;
	pthread_mutex_lock(&mutex_bloque_memoria);
	memcpy(BLOQUE_MEMORIA + offset,stream,tamanio);
	pthread_mutex_unlock(&mutex_bloque_memoria);

	log_info(logger,"Se guarda el stream en frame: %d del bloque de memoria principal, offset: %d, tamanio: %d",nro_frame,offset,tamanio);
}


void* buscar_elemento_en_memoria_principal(t_tabla_paginas* tabla,t_tipo_estructura tipo_estructura,uint32_t tid,bool debeMantenerseEnMemoriaPrincipal){
	//una vez que todas las paginas que necesito estan en MP
	//precondicion, todas las paginas deben estar cargadas en MP
	//funciona solo para obtener las tareas y TCBs
	// el tid es opcional si se busca el tcb
	void* stream = NULL;
	void* stream_parcial;
	t_pagina* pagina = NULL;
	int indice_offset = 0;
	int offset;
	int inicio;
	int limite;
	if(tipo_estructura == TAREAS){
		stream = malloc(tabla->tamanio_tareas);
		inicio = tabla->inicio_tareas;
		limite = tabla->fin_tareas;

	}
	if(tipo_estructura == PCB){
		stream = malloc(sizeof(t_pcb));
		inicio = 0;
		limite = tabla->inicio_tareas;
	}
	if(tipo_estructura == TCB){
		stream = malloc(sizeof(t_tcb));
		int posicion_tripulante;
		uint32_t id_a_buscar;

		//busco donde se encuentra la primera pagina del tripulante
		for(int i=0;i<tabla->lista_id_tripulantes->elements_count;i++){
			id_a_buscar = (uint32_t) list_get(tabla->lista_id_tripulantes,i);
			if(id_a_buscar == tid){
				posicion_tripulante = i;
			}
		}
		//el nuevo inicio sera el nro de la ultima pagina de las tareas + (3* indice de posicion del tripulante
		//es 3 por el tamanio actual del tcb es igual a 3 paginas, asi que hay que correr de a 3 para obtener la pagina donde comienza a guardar el tripulante
		inicio =tabla->fin_tareas + (posicion_tripulante * 3);
		limite = inicio + 3;//de esta forma, se leera ene l for hasta aquella pagina ciclando las 3 paginas del tripulante
		}

	//tengo definido el inicio y el limite
	for(int i=inicio;i<limite;i++){
		pagina = (t_pagina*) list_get(tabla->lista_paginas,i);
		stream_parcial = obtener_stream_de_pagina_en_memoria_principal(pagina,debeMantenerseEnMemoriaPrincipal);
		//calculo el offset local para obtener  el stream completo

		offset= (indice_offset * TAMANIO_PAGINA) ;

		memcpy(stream+ offset,stream_parcial,pagina->tamanio);
		free(stream_parcial);
		indice_offset++;
		}//ya obtengo el stream completo, no necesito hacer nada mas

	return stream;
}

void* obtener_stream_de_pagina_en_memoria_principal(t_pagina* pagina, bool debeMantenerseEnMemoriaPrincipal){
	int offset = pagina->nro_frame * TAMANIO_PAGINA;
	void* stream = malloc(pagina->tamanio);
	log_info(logger,"Obteniedo stream de frame %d , offset %d , tamanio %d",pagina->nro_frame,offset,pagina->tamanio);
	pthread_mutex_lock(&mutex_bloque_memoria);
	memcpy(stream, BLOQUE_MEMORIA + offset, pagina->tamanio);
	pthread_mutex_unlock(&mutex_bloque_memoria);

	//tengo que actualizar el frame al que consulta para LRU y CLOCK
	bool buscar_frame_con_id(void* frame){
		t_frame* frame_a_buscar = (t_frame*) frame;
		return frame_a_buscar->nro_frame == pagina->nro_frame;
	}
	pthread_mutex_lock(&mutex_lista_paginas_principal_general);
	t_frame* frame_utilizado =(t_frame*) list_find(lista_paginas_principal_general,(void*)buscar_frame_con_id);
	actualizar_frame(frame_utilizado);
	frame_utilizado->usado = true;
	if(!debeMantenerseEnMemoriaPrincipal){
		frame_utilizado->en_uso = false;
	}
	pthread_mutex_unlock(&mutex_lista_paginas_principal_general);
	return stream;
}

void guardar_tripulante_paginacion(t_tcb* tcb){
	t_tabla_paginas* tabla_pagina =  obtener_tabla_paginas_de_tid(tcb->tid);
	int posicion_tripulante = buscar_posicion_de_tcb_en_tabla(tabla_pagina,tcb->tid);
	int inicio = tabla_pagina->fin_tareas + (posicion_tripulante*3);
	int limite = inicio +3;
	void* body= serializar_tripulante(tcb);
	guardarTodasLasPaginasEnMemoriaYaExistente(tabla_pagina,body,inicio,limite,sizeof(t_tcb));
	free(body);


}

int buscar_frame__a_reemplazar_clock() {
	pthread_mutex_lock(&mutex_lista_paginas_principal_general);
	int MAX_FRAMES = lista_paginas_principal_general->elements_count;
	t_frame* frame;

	while(1) {

		frame = list_get(lista_paginas_principal_general, POSICION_ACTUAL_FRAME);
		if(!frame->usado || !frame->en_uso || frame->libre) {
			POSICION_ACTUAL_FRAME ++;
			pthread_mutex_unlock(&mutex_lista_paginas_principal_general);
			return frame->nro_frame;
		}
		else{
			frame->usado = true;
			if((POSICION_ACTUAL_FRAME + 1) == MAX_FRAMES) {
				POSICION_ACTUAL_FRAME = 0;
			}
			else
				POSICION_ACTUAL_FRAME ++;
		}
	}
}
