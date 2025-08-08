/*
 * task_ao_queue.c
 *
 *  Created on: Aug 7, 2025
 *      Author: maria
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "task_ao_queue.h"
#include "task_led.h"

typedef struct {

	led_message_t mensaje;
	struct cola_circular_t * front;
	struct cola_circular_t * rear;
	uint8_t priority;

}cola_circular_t;

static cola_circular_t cola[10] = {0};

void nueva(void) {




}
