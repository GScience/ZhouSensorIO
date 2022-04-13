#pragma once

#ifndef ZHOUSENSOR_LIB_BUILD
#error host.h is used inside ZhouSensorLib. You can not include host.h
#endif

#include <Windows.h>
#include <stdint.h>

#define HOST_ACCESS_NAME	TEXT("ZhouSensorHost")
#define HOST_BUFFER_SIZE	sizeof(host_buffer_t)

typedef struct host_buffer
{
	uint8_t ir_value;
	uint8_t buttons;
	uint8_t touch_value[32];
	uint8_t card_statue;
	uint8_t card_id[10];

	uint8_t touch_led[48];
	uint8_t reader_led[3];
} host_buffer_t;

typedef struct uplink_data
{
	uint8_t address;
	uint8_t ir_value;
	uint8_t buttons;
	uint8_t touch_value[32];
	uint8_t card_statue;
	uint8_t card_id[10];
} uplink_data_t;

typedef struct downlink_data
{
	uint8_t address;
	uint8_t touch_led[48];
	uint8_t reader_led[3];
	uint8_t empty[11];
} downlink_data_t;

void host_start();
void host_stop();