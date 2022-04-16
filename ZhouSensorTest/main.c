
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <Windows.h>
#include "../ZhouSensorLib/sensor.h"

#define BYTE_TO_BINARY_PATTERN "%c\n%c\n%c\n%c\n%c\n%c"
#define BYTE_TO_BINARY(byte)  \
  (byte & 0x20 ? '1' : '0'), \
  (byte & 0x10 ? '1' : '0'), \
  (byte & 0x08 ? '1' : '0'), \
  (byte & 0x04 ? '1' : '0'), \
  (byte & 0x02 ? '1' : '0'), \
  (byte & 0x01 ? '1' : '0') 

#define GET_BINARY_MASK(i) \
	((i == 0) ? 0x01 : \
	(i == 1) ? 0x02 : \
	(i == 2) ? 0x04 : \
	(i == 3) ? 0x08 : \
	(i == 4) ? 0x10 : \
	(i == 5) ? 0x20 : 0)

#define GET_BINARY_BYTE(byte, i) \
	(byte & GET_BINARY_MASK(i) ? '1' : '0')

static void set_cursor(int x, int y)
{
	HANDLE hCon = GetStdHandle(STD_OUTPUT_HANDLE);
	COORD setps;
	setps.X = x; setps.Y = y;
	SetConsoleCursorPosition(hCon, setps);
}

static void hide_cursor()
{
	CONSOLE_CURSOR_INFO cursor;
	cursor.bVisible = FALSE;
	cursor.dwSize = sizeof(cursor);
	HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleCursorInfo(handle, &cursor);
}

int main()
{
	sensor_start();
	hide_cursor();

	uint8_t touch[32];
	uint8_t color[96];
	
	uint32_t color_loop = 0;

	while (1)
	{
		color_loop += 0x010203;

		if (color_loop > 0xFFFFFF)
			color_loop = 0;

		memset(color, 0, sizeof(color));

		set_cursor(0, 0);
		printf("==================Zhou Sensor Test!==================");

		// Ir
		uint8_t ir = sensor_get_ir(1);

		// Ir light
		for (int i = 5; i >= 0; --i)
		{
			if (GET_BINARY_BYTE(ir, i) == '0')
				continue;

			color[31 * 3 + 0] = i * 40 + 50;
			color[31 * 3 + 1] = i * 40;
			color[31 * 3 + 2] = i * 40 + 50;
		}

		int sensor_y = 3;
		int sensor_x = 0;

		// Left
		for (int i = 0; i < 6; ++i)
		{
			set_cursor(sensor_x, i + sensor_y);
			printf("%c ", GET_BINARY_BYTE(ir, 5 - i));
		}

		// Touch pad
		sensor_get_touch(touch);

		for (uint8_t i = 0; i < 16; ++i)
		{
			set_cursor(i * 3 + 3 + sensor_x, 5 + sensor_y);
			printf("%02x ", touch[i * 2]);
			set_cursor(i * 3 + 3 + sensor_x, 6 + sensor_y);
			printf("%02x ", touch[i * 2 + 1]);
			
			color[(15 - i) * 3 * 2 + 1] = (uint8_t)((touch[i * 2] + touch[i * 2 + 1]) / 510.0f * 128) + 128;
			color[(15 - i) * 3 * 2 + 2] = (uint8_t)((touch[i * 2] + touch[i * 2 + 1]) / 510.0f * 255);
			color[(15 - i) * 3 * 2 + 0] = (uint8_t)((touch[i * 2] + touch[i * 2 + 1]) / 510.0f * 128);

			if (i != 0)
			{
				color[(15 - i) * 3 * 2 + 4] = (color_loop & 0xFF) >> 0;
				color[(15 - i) * 3 * 2 + 5] = (color_loop & 0xFF00) >> 8;
				color[(15 - i) * 3 * 2 + 3] = (color_loop & 0xFF0000) >> 16;
			}
		}

		sensor_set_color(color);

		// Right
		for (int i = 0; i < 6; ++i)
		{
			set_cursor(52 + sensor_x, i + sensor_y);
			printf("%c ", GET_BINARY_BYTE(ir, 5 - i));
		}

		// Card
		uint8_t card_statue = sensor_get_card_statue();

		set_cursor(60 + sensor_x, sensor_y + 1);
		if (card_statue == 0)
		{
			printf("-------No  Card-------");
			set_cursor(60 + sensor_x, sensor_y + 2);
			printf("|                    |");
			sensor_set_card_led(255, 0, 0);
		}
		else if(card_statue == 1)
		{
			printf("------Aime  Card------");
			set_cursor(60 + sensor_x, sensor_y + 2);
			uint8_t buffer[10];
			sensor_get_aime_id(buffer);
			printf("|");
			for (int i = 0; i < 10; ++i)
				printf("%02x", buffer[i]);
			printf("|");
			sensor_set_card_led(0, 255, 0);
		}
		else if (card_statue == 2)
		{
			printf("-----Felica  Card-----");
			set_cursor(60 + sensor_x, sensor_y + 2);
			uint64_t buffer;
			sensor_get_felica_id(&buffer);
			printf("|%20llu|", buffer);
			sensor_set_card_led(0, 255, 0);
		}
		set_cursor(60 + sensor_x, sensor_y + 3);
		printf("----------------------");
		Sleep(1);
	}
}