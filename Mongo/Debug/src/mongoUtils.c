#include "mongoUtils.h"

int getPuerto(char* key) {
	return config_get_int_value(configMongo, key);
}

char* getIp(char* key) {
	return config_get_string_value(configMongo, key);
}
