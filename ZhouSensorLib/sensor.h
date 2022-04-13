#pragma once

#include <stdint.h>
#include <Windows.h>

#define SENSOR_API

SENSOR_API int sensor_start();
SENSOR_API void sensor_stop();

SENSOR_API uint8_t sensor_get_ir(int fix);
SENSOR_API uint8_t sensor_get_buttons();
SENSOR_API void sensor_get_touch(uint8_t* buffer);
SENSOR_API void sensor_set_color(uint8_t* color);

SENSOR_API uint8_t sensor_get_card_statue();
SENSOR_API void sensor_set_card_led(uint8_t r, uint8_t g, uint8_t b);
SENSOR_API HRESULT sensor_get_aime_id(uint8_t* id);
SENSOR_API HRESULT sensor_get_felica_id(uint64_t* id);