/*
 * buttons.c:
 *	Read the Gertboard buttons. Each one will act as an on/off
 *	tiggle switch for 3 different LEDs
 *
 * Copyright (c) 2012-2013 Gordon Henderson. <projects@drogon.net>
 ***********************************************************************
 * This file is part of wiringPi:
 *	https://projects.drogon.net/raspberry-pi/wiringpi/
 *
 *    wiringPi is free software: you can redistribute it and/or modify
 *    it under the terms of the GNU Lesser General Public License as published by
 *    the Free Software Foundation, either version 3 of the License, or
 *    (at your option) any later version.
 *
 *    wiringPi is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU Lesser General Public License for more details.
 *
 *    You should have received a copy of the GNU Lesser General Public License
 *    along with wiringPi.  If not, see <http://www.gnu.org/licenses/>.
 ***********************************************************************
 */

#include <stdio.h>
#include <wiringPi.h>

#define KEY1 28
#define KEY2 29

#define LED1 0
#define LED2 1
#define LED3 2
#define LED4 3

int main (void)
{
  int i ;

  printf ("Raspberry Pi Gertboard Button Test\n") ;

  wiringPiSetup () ;


    pinMode      (LED1, OUTPUT) ;
    pinMode      (LED2, OUTPUT) ;
    pinMode      (LED3, OUTPUT) ;
    pinMode      (LED4, OUTPUT) ;
    digitalWrite (LED1, LOW) ;
	digitalWrite (LED2, LOW) ;
	digitalWrite (LED3, LOW) ;
	digitalWrite (LED4, LOW) ;
  

  
    pinMode         (KEY1, INPUT) ;
    pinMode         (KEY2, INPUT) ;
    pullUpDnControl (KEY1, PUD_UP) ;
	pullUpDnControl (KEY2, PUD_UP) ;

  for (;;)
  {
	  
   if (digitalRead (KEY1) == LOW)	// Low is pushed    
	{
	printf ("KEY1 is press!\n") ;
    digitalWrite (LED1, HIGH) ;
	digitalWrite (LED2, HIGH) ;
	digitalWrite (LED3, HIGH) ;
	digitalWrite (LED4, HIGH) ;
	}
  while (digitalRead (KEY1) == LOW)	// Wait for release
    delay (10) ;
		
  if (digitalRead (KEY2) == LOW)	// Low is pushed    	
	{
	printf ("KEY2 is press!\n") ;
    digitalWrite (LED1, LOW) ;
	digitalWrite (LED2, LOW) ;
	digitalWrite (LED3, LOW) ;
	digitalWrite (LED4, LOW) ;
	}
  while (digitalRead (KEY2) == LOW)	// Wait for release
    delay (10) ;


//printf ("KEY is not press!\n") ;

  }
  
   return 0 ;
}
