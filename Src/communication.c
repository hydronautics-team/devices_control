
#include <communication.h>
#include <string.h>
#include "checksum.h"
#include "messages.h"
#include "device.h"
#include "main.h"

uint8_t  transmit_buffer[TRANSMIT_BUFFER_SIZE];

bool parse_normal_package(device_settings *device_struct,  uint8_t  *message)  {
	if  (IsChecksumm8bCorrect(message, NORMAL_REQUEST_LENGTH))  {
		struct Request req;
		memcpy((void*)&req,  (void*)message,  NORMAL_REQUEST_LENGTH);
        if  (req.address  ==  device_struct->device_adress)  {
        	device_struct->PWM_Duty  =  req.velocity;
        	UpdateDeviceSettings(&device_struct);
            return true;
        }
	}
    return false;
}

bool parse_device_package(device_settings *device_struct,  uint8_t  *message)  {
	if  (IsChecksumm8bCorrect(message, DEVICES_REQUEST_LENGTH))  {
		struct DevicesRequest req;
		memcpy((void*)&req,  (void*)message,  DEVICES_REQUEST_LENGTH);
        if  (req.address  ==  device_struct->device_adress)  {
        	device_struct->PWM_Duty  =  req.velocity1;
        	UpdateDeviceSettings(&device_struct);
            return true;
        }
	}
	return false;
}

void normal_response(device_settings  *device_struct)  {
	struct Response resp;
	resp.AA            = 0xAA;
	resp.type          = NORMAL_REQUEST_TYPE;
	resp.address       = device_struct->device_adress;
	resp.state         = 0x00;
	resp.current       = 0x00;
	resp.speed_period  = 0x55;

	memcpy((void*)transmit_buffer,  (void*)&resp,  NORMAL_RESPONSE_LENGTH - 1);
	AddChecksumm8b(transmit_buffer,  NORMAL_RESPONSE_LENGTH);

    HAL_GPIO_WritePin(RS485_DIR_PORT,  RS485_DIR_PIN,  GPIO_PIN_SET);

    HAL_UART_Transmit_IT(&huart1,  transmit_buffer,  NORMAL_RESPONSE_LENGTH);
}

void device_response(device_settings  *device_struct)  {
	struct DevicesResponse  resp;
	resp.AA         = 0xAA;
	resp.address    = device_struct->device_adress;
	resp.errors     = 0x00;
	resp.velocity1  = device_struct->PWM_Duty;
	resp.velocity2  = 0x00;
	resp.current1   = 0x00;
	resp.current2   = 0x00;

	memcpy((void*)transmit_buffer,  (void*)&resp,  DEVICES_RESPONSE_LENGTH - 1);
	AddChecksumm8b(transmit_buffer,  DEVICES_RESPONSE_LENGTH);

    HAL_GPIO_WritePin(RS485_DIR_PORT,  RS485_DIR_PIN,  GPIO_PIN_SET);

    HAL_UART_Transmit_IT(&huart1,  transmit_buffer,  DEVICES_RESPONSE_LENGTH);
}