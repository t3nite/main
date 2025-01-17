/*
 * Copyright (c) 2020 Libre Solar Technologies GmbH
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <zephyr/logging/log.h>
#include <dk_buttons_and_leds.h>
#include <inttypes.h>
#include <stddef.h>
#include <stdint.h>
#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <zephyr/sys/util.h>
#include "adc.h"
#include <zephyr/device.h>
#include <zephyr/devicetree.h>

#include "confusion.h"

#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#define USER_LED1         	 	DK_LED1
#define USER_LED2          		DK_LED2
#define USER_LED3               DK_LED3
#define USER_LED4               DK_LED4

#define USER_BUTTON_1           DK_BTN1_MSK
#define USER_BUTTON_2           DK_BTN2_MSK
#define USER_BUTTON_3           DK_BTN3_MSK
#define USER_BUTTON_4           DK_BTN4_MSK

#define DEBUG 0  // 0 = changes direction when button 3 is pressed
                 // 1 = fake 100 measurements done to each 6 directions when 3 pressed.

static int direction = -1;	// 0 = x direction high // CP[1] // suunta 4 
							// 1 = x directon low	// CP[0] // suunta 2 
							// 2 = y direction high // CP[3] // suunta 6 
							// 3 = y direction low  // CP[2] // suunta 5 
							// 4 = z direction high // CP[5] // suunta 1 
							// 5 = z direction low  // CP[4] // suunta 3 
                				 

LOG_MODULE_REGISTER(MAIN, LOG_LEVEL_INF);

static void button_changed(uint32_t button_state, uint32_t has_changed)
{
	//printk("button_state = %d\n",button_state);
	//printk("has_changed = %d\n",has_changed);
	if ((has_changed & USER_BUTTON_1) && (button_state & USER_BUTTON_1)) 
	{
		printk("Button 1 down, printing current Confusion Matrix\n");
		printConfusionMatrix();
	}

	if ((has_changed & USER_BUTTON_2) && (button_state & USER_BUTTON_2)) 
	{
		printk("Button 2 down, resetting confusion matrix\n");
		resetConfusionMatrix();
		printConfusionMatrix();
	}		
	
	if ((has_changed & USER_BUTTON_3) && (button_state & USER_BUTTON_3)) 
	{
		printk("Button 3 down, making fake 100 meas or one real meas depending on DEBUG state\n");
		direction = (direction +1)%6;
		printk("suunta= %d\n",direction);

		struct Measurement m = readADCValue();
		printk("x = %d,  y = %d,  z = %d\n",m.x,m.y,m.z);

		
	}		

	if ((has_changed & USER_BUTTON_4) && (button_state & USER_BUTTON_4)) 
	{
		//direction = (direction +1)%6;
		//printk("button 4 down, one meas and classification with current direction =%d\n",direction);
		makeOneClassificationAndUpdateConfusionMatrix2(direction);
		printConfusionMatrix();
	}		
}

void main(void)
{
	// Siemen satunnaislukugeneraattorille
    srand(time(NULL));

	// Testejä
	//int x =1511, y = 1384, z = 1807; // suunta 1
	//int x =1212, y = 1487, z = 1535; // suunta 2
	//int x =1505, y = 1595, z = 1248; // suunta 3
	int x =1800, y = 1496, z = 1494; // suunta 4
	//int x =1501, y = 1210, z = 1405; // suunta 5
	//int x =1520, y = 1771, z = 1631; // suunta 6

   // int winner = calculateDistanceToAllCentrePointsAndSelectWinner(x, y, z);
    
	int err;
	err = dk_leds_init();
	if (err) {
		LOG_ERR("LEDs init failed (err %d)\n", err);
		return;
	}

	err = dk_buttons_init(button_changed);
	if (err) {
		printk("Cannot init buttons (err: %d)\n", err);
		return;
	}
	
	if(initializeADC() != 0)
	{
	printk("ADC initialization failed!");
	return;
	}

	while (1) 
	{
		//struct Measurement m = readADCValue();
		//printk("x = %d,  y = %d,  z = %d\n",m.x,m.y,m.z);
		
		k_sleep(K_MSEC(1000));
		
		dk_set_led_on(USER_LED1);
		dk_set_led_on(USER_LED2);
		dk_set_led_on(USER_LED3);
		dk_set_led_on(USER_LED4);
		 
		k_sleep(K_MSEC(1000));
		
		dk_set_led_off(USER_LED1);
		dk_set_led_off(USER_LED2);
		dk_set_led_off(USER_LED3);
		dk_set_led_off(USER_LED4);
	}
}