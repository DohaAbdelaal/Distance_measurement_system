/*
 * distanceSensor.c
 *
 *  Created on: May 24, 2023
 *      Author: Doha
 */
#include "distanceSensor.h"

#include"GPIO.h"
#include<util/delay.h>



 uint16 g_time_high=0;//pulse time
 uint8 g_edge_Count=0;//it is updated each time the ICU detect an edge

void Ultrasonic_init(void)
{
	/*ICU initialization*/
	//configure ICU to work on the rising edge with prescaler 8
	Icu_ConfigType Icu_Config = {F_CPU_8,RISING};

	/* Set the Call back function pointer in the ICU driver */
	Icu_setCallBack(Ultrasonic_edgeProcessing);

	Icu_init(&Icu_Config);

	/*configure PB5 as an output pin for trigger pin*/
	GPIO_setupPinDirection(PORTB_ID,TRIGGERPIN,PIN_OUTPUT);


}
void Ultrasonic_Trigger(void)
{
	/*send trigger pulse to ultrasonic sensor */
	GPIO_writePin(PORTB_ID,TRIGGERPIN,LOGIC_HIGH);
	_delay_us(10);

	GPIO_writePin(PORTB_ID,TRIGGERPIN,LOGIC_LOW);

}

uint16 Ultrasonic_readDistance(void)
{
	Ultrasonic_Trigger();/*start trigger wave to wait for the echo signal*/

	/* Distance=3400/time
	 * time is counted by us
	 * Distance=3400/time*10^-6=time*0.017 */
	 return (g_time_high*0.017);
}

void Ultrasonic_edgeProcessing(void)
{
	g_edge_Count++;
	if(g_edge_Count==1)/*first rising edge is detected*/
	{
		//start the timer
		Icu_clearTimerValue();
		/*Now we need to detect the following falling edge*/
		Icu_setEdgeDetectionType(FALLING);
	}
	else if(g_edge_Count==2)/*first falling edge is detected we could calculate the pulse time*/
	{
		/*save the time taken to finish the first pulse*/
		g_time_high=Icu_getInputCaptureValue();
		/*reset the edge counter and wait for the first rising edge*/
		Icu_setEdgeDetectionType(RISING);
		g_edge_Count=0;
	}

}




