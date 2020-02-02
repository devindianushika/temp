/*
 * GSM_Slave.c
 *
 * Created: 8/3/2019 3:40:01 AM
 * Author : Hashan Gunathilaka
 */ 
#define F_CPU 8000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>	
#include <string.h>	
//#include "I2C_Slave_H_File.h"
#include "SPI_Slave_H_file.h"
#include "USART_Interrupt.h" 
#include "gsm.h"

#define Slave_Address			0x20


void GSM_setup();
void GSM_interact(char*);
void GSM_Send_Msg(char* ,char* );
void LCD_Write_xy(char a,char b,char* str);

char buff[160];
volatile int buffer_pointer;
int position = 0;

int main(void)
{
	uint8_t count = 0;
    //I2C_Slave_Init(Slave_Address); 
	buffer_pointer = 0;
	USART_Init(9600);	//initialize USART
	sei();//enable global interrupts
	GSM_setup();
	_delay_ms(1000);
	
	SPI_Init();
	
	DDRC |= 1 << PINC7;
    while (1) 
    {
		
		count = SPI_Receive();
		if (count != 0) {
				PORTC |= 1 << PINC7;
				_delay_ms(3000);
				PORTC &= ~(1 << PINC7);
				GSM_interact(TXT_MSG_MODE);
				GSM_Send_Msg("+94716830842","Alchol detected. Bus Numner: 1234. Route: 255(Kottawa- Mt. Lavenia). Driver Name: Sadun Kumara");
				
		}
		count = 0;
// 		switch(I2C_Slave_Listen()) {
// 			case 0:
// 				{	
// 					count = I2C_Slave_Receive();/* Receive data byte*/
// 					if (count == 10) {
// 						DDRC |= 1 << PINC7;
// 						PORTC |= 1 << PINC7;
// 						_delay_ms(3000);
// 						PORTC &= ~(1 << PINC7);
// 						//GSM_interact(TXT_MSG_MODE);
// 						//GSM_Send_Msg("+94774747949","Alchol detected");
// 						count = 0;
// 						break;
// 					}	else if (count == -1) {
// 						count = 0;
// 						break;
// 					}
// 				}
// 			default:
// 				break;
				
				
	}
}


//Interrupt Service Routine for USART communication
ISR(USART_RXC_vect)
{
	buff[buffer_pointer] = UDR;	/* copy UDR (received value) to buffer */
	buffer_pointer++;
}


void GSM_setup(){
	GSM_interact(GSM_BEGIN);
	// 	GSM_interact(SET_GPRS,"GPRS Error");
	// 	GSM_interact(SET_APN,"APN Error");
	// 	GSM_interact(ENABLE_GPRS,"GPRS is disable");
}

void GSM_interact(char* AT_command){
	while(1)
	{
		buffer_pointer = 0;
		USART_SendString(AT_command);
		//USART_TxChar(0x1A);
		_delay_ms(1000);
		if(strstr(buff,"OK"))
		{
			//LCDClear();
			//LCDWriteStringXY(0,0,success);
			_delay_ms(1000);
			memset(buff,0,160);
			break;
		}
		else
		{
			//LCDClear();
			//LCDWriteStringXY(0,0,show_error);
			_delay_ms(3000);
			//LCDClear();
			memset(buff,0,160);
		}
	}
}


void GSM_Send_Msg(char *num,char *sms)
{
	char sms_buffer[35];
	buffer_pointer=0;
	sprintf(sms_buffer,"AT+CMGS=\"%s\"\r",num);
	USART_SendString(sms_buffer); /*send command AT+CMGS="Mobile No."\r */
	_delay_ms(200);
	while(1)
	{
		if(buff[buffer_pointer]==0x3e) /* wait for '>' character*/
		{
			buffer_pointer = 0;
			memset(buff,0,strlen(buff));
			USART_SendString(sms); /* send message to given no. */
			USART_TxChar(0x1a); /* send Ctrl+Z */
			break;
		}
		buffer_pointer++;
	}
	_delay_ms(300);
	buffer_pointer = 0;
	memset(buff,0,strlen(buff));
	memset(sms_buffer,0,strlen(sms_buffer));
}


