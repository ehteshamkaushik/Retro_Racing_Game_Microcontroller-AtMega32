/*
 * project_Dot_Matrix.c
 *
 * Created: 1/11/2018 1:28:40 AM
 * Author : Kaushik
 */ 

#define F_CPU 1000000UL
#define D4 eS_PORTB4
#define D5 eS_PORTB5
#define D6 eS_PORTB6
#define D7 eS_PORTB7
#define RS eS_PORTB0
#define EN eS_PORTB1

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include<time.h>
#include <stdlib.h>
#include "lcd.h"

unsigned char car[4] = {0b00100100, 0b01111110, 0b00100100, 0b01011010};
unsigned char carPatternLeft[5] = {0b01010000, 0b00100000, 0b01110000, 0b00100000, 0};
unsigned char carPatternRight[5] = {0b00001010, 0b00000100, 0b00001110, 0b00000100, 0};
unsigned char buffer[16];


int indexUp, indexDown, index;
int speed, carCount, level;
int score, carPosition = 0;
int position1, carSide1, position2, carSide2, position3, carSide3;




void modifyBuffer(int p, int s)
{
	int i;
	for (i = 0; i < 5; i++)
	{
		if (p < 0)
		{
			break;
		}
		else if (p >= 16)
		{
			p--;
			continue;
		}
		else
		{
			if (s == 0)
			{
				if(i == 4 && p ==15)
				{
					buffer[p] &= 0b11011111;
				}
				else{
					buffer[p] = (buffer[p] & (0b00001111));
					buffer[p] |= carPatternLeft[i];
				}
			}
			else
			{
				if(i == 4 && p == 15)
				{
					buffer[p] &= 0b11111011;
				}
				else{
						buffer[p] = (buffer[p] & 0b11110000);
						buffer[p] |= carPatternRight[i];
				}
			}
			p--;
		}
	}
}


void modifyBufferFixedCar(int side)
{
	int i;
	char w;
	for (i = 0; i < 4; i++)
	{
		w = car[i];
		if (side == 0)
		{
			w &= 0b11110000;
			(buffer[i+12]) &= 0b11110000;
		}
		else
		{
			w &= 0b00001111;
			buffer[i+12] &= 0b00001111;
		}
		buffer[i+12] |= w;
	}
}

void drawCar()
{
	for(index = 0; index < 16; index++)
	{
		PORTD = (index << 4);
		if (index <= 7)
		{
			PORTC = buffer[index];
			PORTA = 0;
		}
		else
		{
			PORTA = buffer[index];
			PORTC = 0;
		}
		_delay_ms(1);
	}
}

//void drawCarLeft(int pos)
//{
	//for(indexUp = 0; indexUp < 8; indexUp++)
	//{
		//PORTD = indexUp;
		//if (indexUp <= pos)
		//{
			//PORTC = ((car[7 - pos + indexUp]) & 0b11110000);
		//}
		//else
		//{
			//PORTC = 0;
		//}
		//_delay_ms(1);
	//}
	//for(indexDown = 0; indexDown < 8; indexDown++)
	//{
		//PORTD = indexDown + 8;
		//if (indexDown <= pos-8)
		//{
			//PORTA = ((car[7 - (pos -8) + indexDown]) & 0b11110000);
		//}
		//else
		//{
			//PORTA = 0;
		//}
		//_delay_ms(1);
	//}
//}

ISR(INT0_vect)
{
	if (carPosition == 0)
	{
		modifyBufferFixedCar(1);
		carPosition = 1;
		GIFR = 0xff;
	}
	
}

ISR(INT1_vect)
{
	if (carPosition == 1)
	{
		modifyBufferFixedCar(0);
		carPosition = 0;
		GIFR = 0xff;
	}
}


void crashCarDraw()
{
	int i, j;
	for (j = 0; j < 6; j++)
	{
		modifyBufferFixedCar(carPosition);
		for(i = 0; i < 10; i++){
			drawCar();
		}
		for (i = 0; i < 4; i++)
		{
			if (carPosition == 0)
			{
				buffer[i+12] &= (~(car[i] & 0b11110000));
			}
			else
			{
				buffer[i+12] &= (~(car[i] & 0b00001111));
			}
		}
		for(i = 0; i < 10; i++){
			drawCar();
		}
	}
}


void game_init()
{
	int i;
	for(i = 0; i < 16; i++)
	{
		buffer[i] = 0;
	}
	GICR = (1<<INT0)|(1<<INT1);
	MCUCR = (1<<ISC01)|(1<<ISC11);
	sei();
	position1= 0;
	carSide1 = 0;
	position2 = -1;
	carSide2 = 1;
	position3 = -1;
	carSide3 = 0;
	carPosition = 0;
	modifyBufferFixedCar(0);
	speed = 20;
	carCount = 0;
	level = 1;
	score = 0;
	
	Lcd4_Init();
	Lcd4_Clear();
	Lcd4_Set_Cursor(1, 0);
	Lcd4_Write_String("Score : ");
	Lcd4_Set_Cursor(1, 8);
	Lcd4_Write_String(itoa(score, buffer, 10));
	
	Lcd4_Set_Cursor(2, 0);
	Lcd4_Write_String("Level : 0");
	Lcd4_Set_Cursor(2, 8);
	Lcd4_Write_String(itoa(level, buffer, 10));
}


int main(void)
{
	int i, j, l = 0;
	char buffer[10];

	srand(time(0));
	MCUCSR |= (1 << JTD);
	MCUCSR |= (1 << JTD);
    /* Replace with your application code */
	DDRA = 0xff;
	DDRB = 0xff;
	DDRC = 0xff;
	DDRD = 0xf0;
	game_init();
    while (1) 
    {
		if (position1 >=0 && position1 <= 19)
		{
			modifyBuffer(position1, carSide1);
			position1++;
		}
		if (position2 >=0 && position2 <= 19)
		{
			modifyBuffer(position2, carSide2);
			position2++;
		}
		if (position3 >=0 && position3 <= 19)
		{
			modifyBuffer(position3, carSide3);
			position3++;
		}
		if (position1 == 10)
		{
			position2 = 0;
			carSide2 = rand() % 2;
		}
		else if (position2 == 10)
		{
			position3 = 0;
			carSide3 = rand() % 2;
		}
		else if (position3 == 10)
		{
			position1 = 0;
			carSide1 = rand() % 2;
		}
		if (position1 == 20)
		{
			position1 = -1;
			carCount++;
			score++;
			Lcd4_Set_Cursor(1, 8);
			Lcd4_Write_String(itoa(score, buffer, 10));
		}
		else if (position2 == 20)
		{
			position2 = -1;
			carCount++;
			score++;
			Lcd4_Set_Cursor(1, 8);
			Lcd4_Write_String(itoa(score, buffer, 10));
		}
		else if (position3 == 20)
		{
			position3 = -1;
			carCount++;
			score++;
			Lcd4_Set_Cursor(1, 8);
			Lcd4_Write_String(itoa(score, buffer, 10));
		}
		if (carCount == 3)
		{
			carCount = 0;
			if (level <= 15)
			{
				level++;
			}
			
		
			Lcd4_Set_Cursor(2, 8);
			Lcd4_Write_String(itoa(level, buffer, 10));
			if (speed > 10)
			{
				speed -= 10;
			}
			else if (speed > 2 && speed <= 10)
			{
				speed--;
			}
		}
		
		if (position1 > 12 && carSide1 == carPosition)
		{
			cli();
			crashCarDraw();
			game_init();
		}
		else if (position2 > 12 && carSide2 == carPosition)
		{
			cli();
			crashCarDraw();
			game_init();
		}
		else if (position3 > 12 && carSide3 == carPosition)
		{
			cli();
			crashCarDraw();
			game_init();
		}
		
		for(i = 0; i < speed; i++)
		{
			drawCar();
		}
    }
}

