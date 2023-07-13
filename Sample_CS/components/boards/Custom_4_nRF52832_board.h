/**
 * Copyright (c) 2014 - 2021, Nordic Semiconductor ASA
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form, except as embedded into a Nordic
 *    Semiconductor ASA integrated circuit in a product or a software update for
 *    such product, must reproduce the above copyright notice, this list of
 *    conditions and the following disclaimer in the documentation and/or other
 *    materials provided with the distribution.
 *
 * 3. Neither the name of Nordic Semiconductor ASA nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * 4. This software, with or without modification, must only be used with a
 *    Nordic Semiconductor ASA integrated circuit.
 *
 * 5. Any software provided in binary form under this license must not be reverse
 *    engineered, decompiled, modified and/or disassembled.
 *
 * THIS SOFTWARE IS PROVIDED BY NORDIC SEMICONDUCTOR ASA "AS IS" AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY, NONINFRINGEMENT, AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL NORDIC SEMICONDUCTOR ASA OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */
#ifndef CUSTOM_H
#define CUSTOM_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "nrf_gpio.h"

// LEDs definitions for CUSTOM
#define LEDS_NUMBER 5

#define LED_START 4
#define LED_1 4
#define LED_2 8
#define LED_3 7
#define LED_4 6
#define LED_5 5
#define LED_STOP 8

#define LEDS_ACTIVE_STATE 1

#define LEDS_INV_MASK LEDS_MASK

#define LEDS_LIST                         \
    {                                     \
        LED_1, LED_2, LED_3, LED_4, LED_5 \
    }

#define BSP_LED_0 LED_1
#define BSP_LED_1 LED_2
#define BSP_LED_2 LED_3
#define BSP_LED_3 LED_4
#define BSP_LED_4 LED_5

#define BUTTONS_NUMBER 8

#define BUTTON_START 9
#define BUTTON_1 21 // SW1
#define BUTTON_2 13 // SW_IN1
#define BUTTON_3 14 // SW_IN2
#define BUTTON_4 10 // HALL SENSOR 0
#define BUTTON_5 30 // HALL SENSOR 1
#define BUTTON_6 29 // HALL SENSOR 2
#define BUTTON_7 28 // HALL SENSOR 3
#define BUTTON_8 9  // HALL SENSOR 4
#define BUTTON_STOP 30
#define BUTTON_PULL NRF_GPIO_PIN_PULLUP

#define BUTTONS_ACTIVE_STATE 0

#define BUTTONS_LIST                                                                   \
    {                                                                                  \
        BUTTON_1, BUTTON_2, BUTTON_3, BUTTON_4, BUTTON_5, BUTTON_6, BUTTON_7, BUTTON_8 \
    }

#define BSP_BUTTON_0 BUTTON_1
#define BSP_BUTTON_1 BUTTON_2
#define BSP_BUTTON_2 BUTTON_3
#define BSP_BUTTON_3 BUTTON_4
#define BSP_BUTTON_4 BUTTON_5
#define BSP_BUTTON_5 BUTTON_6
#define BSP_BUTTON_6 BUTTON_7
#define BSP_BUTTON_7 BUTTON_8

#define RX_PIN_NUMBER 15
#define TX_PIN_NUMBER 16

// Custom Board Pins
#define TWI_SCL_PIN 2                            // SCL Signal Pin
#define TWI_SDA_PIN 3                            // SDA Signal Pin
#define BATTERY_VOLTAGE_PIN NRF_SAADC_INPUT_AIN7 // Battery ADC Voltage pin
#define BUZZER_PIN 11                            // Buzzer Pin
#define CHARGING_PIN 17                          // Charging Status Pin
#define SENSOR_BOARD_EN 19                       // Sensor Board ON/OFF Control Pin

// Display Pins
#define DISPLAY_DC 22   // Data/Command Select Pin
#define DISPLAY_RST 23  // Reset Pin
#define DISPLAY_BUSY 24 // Busy Pin
#define DISPLAY_SCK 25  // Clock Pin
#define DISPLAY_SDI 26  // Master In Slave Out Pin
#define DISPLAY_CS 27   // Chip Select Pin

#ifdef __cplusplus
}
#endif

#endif // CUSTOM_H
