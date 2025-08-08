/*
 * task_oa_queue.h
 *
 *  Created on: Aug 7, 2025
 *      Author: mariano
 */

#ifndef INC_TASK_AO_QUEUE_H_
#define INC_TASK_AO_QUEUE_H_

#include <stdbool.h>

typedef enum {

  AO_LED_MESSAGE_ON,
  AO_LED_MESSAGE_OFF,
  AO_LED_MESSAGE__N,
} ao_led_action_t;

typedef enum {

  AO_LED_COLOR_RED,
  AO_LED_COLOR_GREEN,
  AO_LED_COLOR_BLUE,
} ao_led_color_t;

typedef struct {

	ao_led_action_t action;
	ao_led_color_t color;
} data_queue_t;

bool cola_ao_init(uint16_t cola_max_size);
bool encolar(data_queue_t data, uint8_t priority);
data_queue_t desencolar(void);

#endif /* INC_TASK_AO_QUEUE_H_ */
