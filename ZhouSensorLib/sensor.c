
// Zhou Sensor IO

#include "host.h"
#include "sensor.h"

LPVOID sensor_buffer	= NULL;
HANDLE sensor_map		= NULL;

#define SENSOR_BUFFER		((host_buffer_t*)sensor_buffer)
#define SENSOR_COLOR_SCALE	0xF

// Start sensor
SENSOR_API int sensor_start()
{
	sensor_stop();
	host_start();

	sensor_map = OpenFileMapping(
		FILE_MAP_READ | FILE_MAP_WRITE,	// dwDesiredAccess
		0,								// bInheritHandle
		HOST_ACCESS_NAME				// lpName
	);

	if (sensor_map == NULL)
		return 0;

	sensor_buffer = MapViewOfFile(sensor_map, FILE_MAP_READ | FILE_MAP_WRITE, 0, 0, 0);

	if (sensor_buffer == NULL)
	{
		CloseHandle(sensor_map);
		return 0;
	}

	return 1;
}

// Stop sensor
SENSOR_API void sensor_stop()
{
	if (sensor_buffer != NULL)
		UnmapViewOfFile(sensor_buffer);
	if (sensor_map != NULL)
		CloseHandle(sensor_map);
}

SENSOR_API uint8_t sensor_get_ir(int fix)
{
	if (sensor_buffer == NULL)
		return 0;

	if (fix)
	{
		uint8_t ir = 0;
		uint8_t mask1;
		uint8_t mask2;
		uint8_t i;

		for (i = 0; i < 3; i++)
		{
			mask1 = (1 << ((i * 2) + 0));
			mask2 = (1 << ((i * 2) + 1));
			ir |= (mask1 & SENSOR_BUFFER->ir_value) << 1;
			ir |= (mask2 & SENSOR_BUFFER->ir_value) >> 1;
		}

		return ir;
	}
	else
		return SENSOR_BUFFER->ir_value;
}

SENSOR_API uint8_t sensor_get_buttons()
{
	if (sensor_buffer == NULL)
		return 0;

	return SENSOR_BUFFER->buttons;
}

SENSOR_API void sensor_get_touch(uint8_t* buffer)
{
	if (sensor_buffer == NULL)
		return;

	memcpy(buffer, SENSOR_BUFFER->touch_value, sizeof(SENSOR_BUFFER->touch_value));
}

SENSOR_API uint8_t sensor_get_card_statue()
{
	if (sensor_buffer == NULL)
		return 0;

	return SENSOR_BUFFER->card_statue;
}

SENSOR_API void sensor_set_card_led(uint8_t r, uint8_t g, uint8_t b)
{
	SENSOR_BUFFER->reader_led[0] = r;
	SENSOR_BUFFER->reader_led[1] = g;
	SENSOR_BUFFER->reader_led[2] = b;
}

SENSOR_API HRESULT sensor_get_aime_id(uint8_t* id)
{
	if (sensor_buffer == NULL)
		return S_FALSE;

	if (SENSOR_BUFFER->card_statue != 1)
		return S_FALSE;

	uint8_t* data = SENSOR_BUFFER->card_id;
	memcpy(id, SENSOR_BUFFER->card_id, sizeof(uint8_t) * 10);

	return S_OK;
}

SENSOR_API HRESULT sensor_get_felica_id(uint64_t* id)
{
	if (sensor_buffer == NULL)
		return S_FALSE;

	if (SENSOR_BUFFER->card_statue != 1)
		return S_FALSE;

	memcpy(id, SENSOR_BUFFER->card_id, sizeof(uint64_t));

	return S_OK;
}

SENSOR_API void sensor_set_color(uint8_t* color)
{
	if (sensor_buffer == NULL)
		return;

	// Compress color
	for (int i = 0; i < 16; ++i)
	{
		uint32_t compress_color =
			(((int)(color[i * 6 + 0] / 255.0f * SENSOR_COLOR_SCALE)) << 0) |
			(((int)(color[i * 6 + 1] / 255.0f * SENSOR_COLOR_SCALE)) << 4) |
			(((int)(color[i * 6 + 2] / 255.0f * SENSOR_COLOR_SCALE)) << 8) |
			(((int)(color[i * 6 + 3] / 255.0f * SENSOR_COLOR_SCALE)) << 12) |
			(((int)(color[i * 6 + 4] / 255.0f * SENSOR_COLOR_SCALE)) << 16) |
			(((int)(color[i * 6 + 5] / 255.0f * SENSOR_COLOR_SCALE)) << 20);

		memcpy(&SENSOR_BUFFER->touch_led[i * 3], &compress_color, sizeof(uint8_t) * 3);
	}
}