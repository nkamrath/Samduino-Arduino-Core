/* 
* usb_serial.cpp
*
* Created: 3/11/2016 9:20:37 AM
* Author: Nate
*/


#include "usb_serial.h"

usb_serial Serial;

// default constructor
usb_serial::usb_serial()
{
	rx_buffer_end = &rx_buffer[USB_SERIAL_BUFFER_SIZE - 1];
	rx_buffer_head = rx_buffer;
	rx_buffer_tail = rx_buffer;
	rx_buffer_size = 0;
} //usb_serial

// default destructor
usb_serial::~usb_serial()
{
} //~usb_serial

usb_serial::operator bool()
{
	return true;
}

void usb_serial::begin(unsigned long baud)
{
	stdio_usb_init();
}

void usb_serial::end(void)
{
	stdio_usb_disable();
}

int usb_serial::available(void)
{
	return rx_buffer_size;
}

int usb_serial::read(void)
{
	//return udi_cdc_getc();
	uint8_t data = 0;
	if(rx_buffer_size > 0)
	{
		rx_buffer_dequeue(&data);
		return data;
	}
	else
	{
		return -1;
	}
}

int usb_serial::peek(void)
{
	return (*rx_buffer_head);
}

void usb_serial::flush(void)
{
	//TODO: actually clear the buffers
}

size_t usb_serial::write(uint8_t data)
{
	while(udi_cdc_is_tx_ready() == 0); //wait for tx to be ready to send
	return udi_cdc_putc(data & 0xff);
}

size_t usb_serial::write(const uint8_t* buffer, size_t bufferLength)
{
	int bytesSent = 0;
	for(size_t i = 0; i < bufferLength; i++)
	{
		//bytesSent += udi_cdc_putc(buffer[bytesSent]);
		while(udi_cdc_is_tx_ready() == 0); //wait for tx to be ready to send
		bytesSent += udi_cdc_putc(buffer[bytesSent]);
	}
	return bytesSent;
}

size_t usb_serial::write(unsigned long data)
{
	while(udi_cdc_is_tx_ready() == 0); //wait for tx to be ready to send
	return udi_cdc_putc(data & 0xff);
}

size_t usb_serial::write(long data)
{
	while(udi_cdc_is_tx_ready() == 0); //wait for tx to be ready to send
	return udi_cdc_putc(data & 0xff);
}

size_t usb_serial::write(unsigned int data)
{
	while(udi_cdc_is_tx_ready() == 0); //wait for tx to be ready to send
	return udi_cdc_putc(data & 0xff);
}

size_t usb_serial::write(int data)
{
	//return tx_buffer_enqueue(data & 0xff);
	while(udi_cdc_is_tx_ready() == 0); //wait for tx to be ready to send
	return udi_cdc_putc(data & 0xff);
}

void usb_serial::rx_callback(void)
{
	rx_buffer_enqueue(udi_cdc_getc());
}

uint8_t usb_serial::rx_buffer_enqueue(uint8_t data)
{
	if(rx_buffer_size < USB_SERIAL_BUFFER_SIZE)
	{
		*rx_buffer_tail = data;
		if(rx_buffer_tail == rx_buffer_end)
		{
			rx_buffer_tail = rx_buffer;
		}
		else
		{
			rx_buffer_tail++;
		}
		rx_buffer_size++;
		return 1;
	}
	else
	{
		return 0;  //indicate the buffer was full
	}
}

uint8_t usb_serial::rx_buffer_dequeue(uint8_t* data)
{
	if(rx_buffer_size > 0)
	{
		*data = *rx_buffer_head;
		if(rx_buffer_head == rx_buffer_end)
		{
			rx_buffer_head = rx_buffer;
		}
		else
		{
			rx_buffer_head++;
		}
		rx_buffer_size--;
		return 1;
	}
	else
	{
		return 0;//indicate the buffer was empty
	}
}


void usb_serial_rx_notify(uint8_t port)
{
	if(port == 0)
	{
		Serial.rx_callback();
	}
}