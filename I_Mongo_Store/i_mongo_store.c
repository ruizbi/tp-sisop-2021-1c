#include "./i_mongo_store.h"

int main() {

	t_config* pathConfig = config_create( "I_MONGO_STORE.config" );
	PUERTO_MONGO_STORE = config_get_int_value( pathConfig, "PUERTO" );

	crearServer( PUERTO_MONGO_STORE, BACKLOG );

	return 0;
}
