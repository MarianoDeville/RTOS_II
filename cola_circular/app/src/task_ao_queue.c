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
static uint16_t elementos_guardados;
static cola_circular_t * elemento_salida;
static cola_circular_t * ultimo_elemento;
static uint16_t max_queue_size;

/********************** internal functions declaration ***********************/
static cola_circular_t * recorrer_cola(uint8_t priority);
static void insertar(cola_circular_t * nuevo);

/********************** external functions definition ************************/
bool cola_ao_init(uint16_t cola_max_size) {

	elemento_salida = NULL;
	ultimo_elemento = NULL;
	elementos_guardados = 0;
	max_queue_size = cola_max_size;
	return true;
}

bool encolar(data_queue_t data, uint8_t priority) {

	if(0 == max_queue_size)
		return false;

	if(max_queue_size <= elementos_guardados) {		// elimino el último elemento de la cola para dejar luagar porque ya no queda espacio

		cola_circular_t* ultimo = ultimo_elemento;	// cargo el último elemento
		ultimo_elemento = ultimo->anterior;			// el puntero al último elemento ahora apunta al anteultimo
		free(ultimo);								// libero la memoria del último (borrado)
		ultimo = ultimo_elemento;					// ahora cargo el nuevo último elemento
		ultimo->proximo = NULL;						// como ahora es el último no apunta a un siguiente
		elementos_guardados--;						// bajo el contador indicando el lugar libre
	}
	cola_circular_t* cola = (cola_circular_t*)malloc(sizeof(cola_circular_t));

    if(NULL == cola)
    	return false;
    memcpy(&cola->data, &data, sizeof(data));
	cola->priority = priority;
   	insertar(cola);
	elementos_guardados++;
	return true;
}

data_queue_t desencolar(void) {

	cola_circular_t* cola = elemento_salida;	// obtengo el primer elemento de la cola
	data_queue_t info = cola->data;				// obtengo la información de debo devolver
	elemento_salida = cola->proximo;			// apunto el elemento de salida al proximo
	free(cola);									// elimino el elemento
	cola = elemento_salida;						// cargo el que ahora es el primer elemento de la cola
	cola->anterior = NULL;						// como es el primero no hay anterior
	return info;								// devuelvo la información
}

/********************** internal functions definition ************************/
static cola_circular_t * recorrer_cola(uint8_t priority) {

	cola_circular_t* actual = elemento_salida;
	uint16_t i = 0;

	while(actual != NULL || i > elementos_guardados) {

		if(actual->priority <= priority)
			return actual;
		actual = actual->proximo;
		i++;
	}
	return NULL;
}

static void insertar(cola_circular_t * nuevo) {

    if(0 == elementos_guardados) {			// caso inicial, no hay nada guardado en la cola

    	nuevo->proximo = NULL;				// no hay elemento adelante
    	nuevo->anterior = NULL;				// no hay elemento atras
		elemento_salida = nuevo;			// dejo apuntado el primer elemento de la cola
		ultimo_elemento = nuevo;			// dejo apuntado el ultimo elemento de la cola
		return;								// dato agregado a la cola, salgo
    }
	cola_circular_t* proximo = recorrer_cola(nuevo->priority); // busco un lugar para guardar según la prioridad

	if(NULL == proximo) {					// si no hay elementos de menor prioridad lo almaceno al final de la cola

		cola_circular_t* anterior = ultimo_elemento;
		nuevo->proximo = NULL;				// como es el útimo no hay elemento siguiente
		nuevo->anterior = ultimo_elemento;	// apunto desde el elemento agregado al que antes era el último elemento
		anterior->proximo = nuevo;
		ultimo_elemento = nuevo;			// dejo apuntado el ultimo elemento de la cola
		return;
	}

	if(NULL == proximo->anterior) {			// lo almaceno al comienzo de la cola

		nuevo->proximo = proximo;			// apunto al que antes era el primero
		nuevo->anterior = NULL;				// como es el primero no hay elemento antes
		proximo->anterior = nuevo;			// el que antes era primero ahora es segundo asi que apunto al nuevo primero como predecesor
		elemento_salida = nuevo;			// es la nueva cabecera de la cola
		return;
	}
	cola_circular_t* anterior = proximo->anterior;	// obtengo la direccion del elemento anterior en la cola
	// meto en el medio la nueva estructura
	nuevo->proximo = proximo;
	nuevo->anterior = anterior;
	proximo->anterior = nuevo;
	anterior->proximo = nuevo;
	return;
}
