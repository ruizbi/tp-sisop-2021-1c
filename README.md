# tp-2021-1c-CarpinchOS



void actualizarTripulante(char* bitacoras, char* accion) {
	t_config* bitacoraAux = config_create(bitacoras);
	int sizeBita = config_get_int_value(bitacoraAux, "SIZE");
	char** blocks = config_get_array_value(bitacoraAux, "BLOCKS");
	t_list* listaBlocks = list_create();
	int i = 0;
	int cantidadBlocks = calculoBloques(sizeBita);
	i = cantidadBlocks - 1;
	int aux;
	aux = strtol(blocks[i], NULL, 10);
	char*info = buscarBloque(aux);

	if (sizeBita % configSB.tamanio_blocks + strlen(accion)
			<= configSB.tamanio_blocks && !bloquelleno(info)) {
		char* datos = grabarDatosBloque(info, accion);
		insertarBloqueMapeo(datos, aux);
		free(datos);
		free(info);

	} else {
		int longitud = 0;
		//bloque lleno filtrar
		if (bloquelleno(info)) {
			longitud = strlen(accion);
			free(info);
		} else {
			longitud = strlen(accion) - configSB.tamanio_blocks
					+ sizeBita % configSB.tamanio_blocks;
			char* datos = grabarDatosBloque(info, accion);
			insertarBloqueMapeo(datos, aux);
			free(datos);
			free(info);
		}
		while (longitud > 0) {
			char* restoLlenado = string_substring_from(accion,
					strlen(accion) - longitud);
			int nroBloque = buscarPosicionLibre();
			char* numBlock = string_itoa(nroBloque);
			char*restoInfo = buscarBloque(nroBloque);
			char* restoDatos = grabarDatosBloque(restoInfo, restoLlenado);
			insertarBloqueMapeo(restoDatos, nroBloque);
			free(restoInfo);
			free(restoDatos);
			free(restoLlenado);

			list_add(listaBlocks, (void*) nroBloque);

			free(numBlock);
			longitud -= configSB.tamanio_blocks;
		}
	}
	char* bloques = agregarListaBlocks(blocks, cantidadBlocks, listaBlocks);
	config_set_value(bitacoraAux, "BLOCKS", bloques);
	list_destroy(listaBlocks);
	sizeBita += strlen(accion);
	char* nuevoSize = string_itoa(sizeBita);
	config_set_value(bitacoraAux, "SIZE", nuevoSize);
	config_save(bitacoraAux);
	config_destroy(bitacoraAux);
	free(nuevoSize);
	liberarBloques(blocks, cantidadBlocks);
	free(blocks);
	free(bloques);
}
