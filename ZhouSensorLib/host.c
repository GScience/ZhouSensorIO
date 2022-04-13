
// Zhou Sensor Host
#include "host.h"
#include "log.h"
#include <SetupAPI.h>
#include <hidsdi.h>
#include <process.h>

#pragma comment(lib,"setupapi.lib")
#pragma comment(lib,"hid.lib")

#define HOST_BUFFER ((host_buffer_t*)host_buffer)

uplink_data_t   uplink_data;
downlink_data_t downlink_data;

LPVOID host_buffer  = NULL;
HANDLE host_map     = NULL;

static HANDLE host_connect_to_pad()
{
    GUID HidGuid;
    HidD_GetHidGuid(&HidGuid);
    HDEVINFO hDevInfo = SetupDiGetClassDevs(&HidGuid, NULL, NULL, DIGCF_DEVICEINTERFACE | DIGCF_PRESENT);

    if (INVALID_HANDLE_VALUE == hDevInfo)
    {
        LOG(L"Failed to get system device list\n");
        return NULL;
    }

    SP_DEVICE_INTERFACE_DATA device_interface_data;
    device_interface_data.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);

    DWORD required_size = 0;
    DWORD enum_index = 0;

    HANDLE device_handle = NULL;
    SP_DEVICE_INTERFACE_DETAIL_DATA* device_interface_detail = NULL;
    HIDD_ATTRIBUTES Attributes;

    while (SetupDiEnumDeviceInterfaces(hDevInfo, NULL, &HidGuid, enum_index++, &device_interface_data))
    {
        SetupDiGetDeviceInterfaceDetail(hDevInfo, &device_interface_data, NULL, 0, &required_size, NULL);

        device_interface_detail = (SP_DEVICE_INTERFACE_DETAIL_DATA*)malloc(required_size);

        if (device_interface_detail == NULL)
        {
            LOG(L"Fail to allocate memory\n");
            break;
        }
        device_interface_detail->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);

        if (!SetupDiGetDeviceInterfaceDetail(hDevInfo, &device_interface_data, device_interface_detail, required_size, NULL, NULL))
        {
            LOG(L"Fail to do SetupDiGetInterfaceDeviceDetail\n");
            free(device_interface_detail);
            break;
        }

        device_handle = CreateFile(device_interface_detail->DevicePath, 0xC0000000, 3u, 0, 3u, 0, 0);
        free(device_interface_detail);

        if (device_handle == NULL)
            continue;

        if (!HidD_GetAttributes(device_handle, &Attributes))
        {
            CloseHandle(device_handle);
            device_handle = NULL;
            continue;
        }

        if (Attributes.VendorID == 0x1973 &&
            Attributes.ProductID == 0x2001)
        {
            LOG(L"Find ZhouSensor\n");
            break;
        }

        CloseHandle(device_handle);
        device_handle = NULL;
    }

    SetupDiDestroyDeviceInfoList(hDevInfo);

    return device_handle;
}

static void host_thread_proc(void* arg)
{
    HANDLE device_handle;

    // Wait for handle
    do
    {
        LOG(L"Wait for connection...\n");
        device_handle = host_connect_to_pad();
        Sleep(1000);
    } while (device_handle == NULL);

    LOG(L"Connected\n");

    memset(downlink_data.empty, 0, sizeof(downlink_data.empty));

    while (1)
    {
        // read data
        if (!ReadFile(device_handle, &uplink_data, sizeof(uplink_data), NULL, NULL))
            break;

        HOST_BUFFER->ir_value = uplink_data.ir_value;
        HOST_BUFFER->buttons = uplink_data.buttons;
        memcpy(HOST_BUFFER->touch_value, uplink_data.touch_value, sizeof(HOST_BUFFER->touch_value));
        HOST_BUFFER->card_statue = uplink_data.card_statue;
        memcpy(HOST_BUFFER->card_id, uplink_data.card_id, sizeof(HOST_BUFFER->card_id));

        // send data
        downlink_data.address = 0;
        memcpy(downlink_data.reader_led, HOST_BUFFER->reader_led, sizeof(downlink_data.reader_led));
        memcpy(downlink_data.touch_led, HOST_BUFFER->touch_led, sizeof(downlink_data.touch_led));

        if (!WriteFile(device_handle, &downlink_data, sizeof(downlink_data), NULL, NULL))
            break;

        Sleep(1);
    }

    CloseHandle(device_handle);
    _beginthread(host_thread_proc, 0, NULL);
}

static int host_is_started()
{
    HANDLE file_map = OpenFileMapping(
        FILE_MAP_READ,          // dwDesiredAccess
        0,						// bInheritHandle
        HOST_ACCESS_NAME		// lpName
    );

    if (file_map == NULL)
        return 0;

    CloseHandle(file_map);
    return 1;
}

// Start sensor host
void host_start()
{
    if (host_is_started())
    {
        LOG(L"Host already started");
        return;
    }

    LOG(L"Start IO host");

    host_map = CreateFileMapping(
        INVALID_HANDLE_VALUE,   // hFile
        NULL,                   // lpAttributes
        PAGE_READWRITE,         // flProtect
        0,                      // dwMaximumSizeHigh
        HOST_BUFFER_SIZE,       // dwMaximumSizeLow
        HOST_ACCESS_NAME        // lpName
    );

    if (host_map == NULL)
    {
        LOG(L"Failed to do FileMapping with name %s\n", HOST_ACCESS_NAME);
        return;
    }
    host_buffer = MapViewOfFile(host_map, FILE_MAP_ALL_ACCESS, 0, 0, 0);

    if (host_buffer == NULL)
    {
        LOG(L"Failed to do MapViewOfFile with name %s\n", HOST_ACCESS_NAME);
        return;
    }

    _beginthread(host_thread_proc, 0, NULL);
}

// Stop sensor host
void host_stop()
{
    UnmapViewOfFile(host_buffer);
    CloseHandle(host_map);
}