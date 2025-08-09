/*
 * task_ao_queue.c
 *
 *  Created on: Aug 7, 2025
 *      Author: mariano
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "task_ao_queue.h"

/********************** macros and definitions *******************************/
typedef struct cola_circular_t {

	data_queue_t data;
	struct cola_circular_t * proximo;
	struct cola_circular_t * anterior;
	uint8_t priority;
}cola_circular_t;

/********************** internal data definition *****************************/
static bool initialized = false;
static uint16_t elementos_guardados;
static uint16_t max_queue_size;
static cola_circular_t * elemento_salida;
static cola_circular_t * ultimo_elemento;

/********************** internal functions declaration ***********************/
static cola_circular_t * recorrer_cola(uint8_t priority);
static void insertar(cola_circular_t * nuevo);

/********************** external functions definition ************************/
bool cola_ao_init(uint16_t cola_max_size) {

	if(initialized)
		return false;

	if(1 >= cola_max_size)								// el algoritmo funciona con colas de tamaño mayor a 1
		return false;
	elemento_salida = NULL;
	ultimo_elemento = NULL;
	elementos_guardados = 0;
	max_queue_size = cola_max_size;
	initialized = true;
	return initialized;
}

bool encolar(data_queue_t data, uint8_t priority) {

	if(!initialized)
		return false;

	if(max_queue_size <= elementos_guardados) {			// elimino el último elemento de la cola para dejar luagar porque ya no queda espacio

		ultimo_elemento = ultimo_elemento->anterior;	// el puntero al último elemento ahora apunta al anteultimo
		free(ultimo_elemento->proximo);					// libero la memoria del último (borrado)
		ultimo_elemento->proximo = NULL;				// como ahora es el último no apunta a un siguiente
		elementos_guardados--;							// bajo el contador indicando el lugar libre
	}
	cola_circular_t* cola = (cola_circular_t*)malloc(sizeof(cola_circular_t));

    if(NULL == cola)
    	return false;
    memcpy(&cola->data, &data, sizeof(data_queue_t));
	cola->priority = priority;
	cola->anterior = NULL;
	cola->proximo = NULL;
   	insertar(cola);
	elementos_guardados++;
	return true;
}

data_queue_t desencolar(void) {

	data_queue_t info = {0};

	if(!initialized || NULL == elemento_salida)	// no hay nada en la cola o no está inicializado
		return info;
	info = elemento_salida->data;				// obtengo la información de debo devolver

	if(elemento_salida == ultimo_elemento) {	// es el último de la cola

		free(elemento_salida);					// elimino el elemento
		elementos_guardados = 0;				// reseteo todas las varibles
		elemento_salida = NULL;
		ultimo_elemento = NULL;
		return info;							// devuelvo la información
	}
	elemento_salida = elemento_salida->proximo;	// apunto el elemento de salida al proximo
	free(elemento_salida->anterior);			// elimino el elemento
	elemento_salida->anterior = NULL;			// como es el primero no hay anterior
	elementos_guardados--;
	return info;								// devuelvo la información
}

/********************** internal functions definition ************************/
static cola_circular_t * recorrer_cola(uint8_t priority) {

	cola_circular_t* actual = elemento_salida;

	while(NULL != actual) {

		if(actual->priority < priority)
			return actual;					// lugar donde voy a insertar
		actual = actual->proximo;
	}
	return NULL;							// no hay nadie con menor prioridad
}

static void insertar(cola_circular_t * nuevo) {

    if(0 == elementos_guardados) {			// caso inicial, no hay nada guardado en la cola

		elemento_salida = nuevo;			// dejo apuntado el primer elemento de la cola
		ultimo_elemento = nuevo;			// dejo apuntado el ultimo elemento de la cola
		return;								// dato agregado a la cola, salgo
    }
	cola_circular_t* proximo = recorrer_cola(nuevo->priority); // busco un lugar para guardar según la prioridad

	if(NULL == proximo) {					// este es el caso que no hay elementos de menor prioridad, así que lo almaceno
											// al final de la cola
		nuevo->anterior = ultimo_elemento;	// apunto desde el elemento agregado al que antes era el último elemento
		ultimo_elemento->proximo = nuevo;	// el que era el último elemento de la cola ahora apunta al nuevo último
		ultimo_elemento = nuevo;			// cambio al nuevo último elemento de la cola
		return;
	}

	if(NULL == proximo->anterior) {			// este es el caso de que lo debo almacenar al comienzo de la cola

		nuevo->proximo = proximo;			// apunto al que antes era el primero
		proximo->anterior = nuevo;			// el que antes era primero ahora es segundo asi que apunto al nuevo primero como predecesor
		elemento_salida = nuevo;			// es la nueva cabecera de la cola
		return;
	}
	// por último cuando lo almaceno entre otros elementos
	cola_circular_t* anterior = proximo->anterior;	// obtengo la direccion del elemento anterior en la cola
	nuevo->proximo = proximo;
	nuevo->anterior = anterior;
	proximo->anterior = nuevo;
	anterior->proximo = nuevo;
	return;
}
