/* ========================================
 *
 * Copyright YOUR COMPANY, THE YEAR
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF your company.
 *
 * ========================================
*/

#include "main.h"

uint8 wrBuf[SPI_WRITE_BUFFER_SIZE]; /* SPI write buffer */
uint8 rdBuf[SPI_READ_BUFFER_SIZE];  /* SPI read buffer */
uint32 byteCnt;  /* variable to store the number of bytes written by SPI mater */

uint8 sendNotifications; 	/* Flag to check notification enabled/disabled */
CYBLE_API_RESULT_T	apiResult;  /*  variable to store BLE component API return */

/*******************************************************************************
* Function Name: SPIBrigeInit
********************************************************************************
* Summary:
*    This function initializes the BLE and SPI component
*
* Parameters:
*  void
*
* Return:
*  void
*
*******************************************************************************/

void SPIBrigeInit(void)
{
	/* Start BLE operation */
    apiResult = CyBle_Start(AppCallBack);
	
	if(apiResult != CYBLE_ERROR_OK)
	{
		
#ifdef	LED_INDICATION			
		DISCON_LED_ON();
#endif /* LED_INDICATION */		

		while(1);
		
		/* Failed to initialize stack */
	}

#ifndef	 ENABLE_SPI_ONLY_WHEN_CONNECTED	
	/* Start SPI Slave operation */
	SPI_Start();
	
#endif	
}


/*******************************************************************************
* Function Name: main
********************************************************************************
* Summary:
*        System entrance point. This calls the initializing function and
* continuously process BLE and I2C events.
*
* Parameters:
*  void
*
* Return:
*  int
*

*******************************************************************************/
int main()
{	
	/* Enable the Global Interrupt */
	CyGlobalIntEnable;

#ifdef	LED_INDICATION	
	/* Turn off all LEDS */
	ALL_LED_OFF();
#else
	/* Set the LED pins drive mode to Strong drive */
	DISCON_LED_SetDriveMode(DISCON_LED_DM_ALG_HIZ);
	CONNECT_LED_SetDriveMode(CONNECT_LED_DM_ALG_HIZ);
	ADV_LED_SetDriveMode(ADV_LED_DM_ALG_HIZ);
#endif	/* LED_INDICATION */

	SPIBrigeInit();
	
	for(;;) /* Loop forever */
	{
	
#ifdef LOW_POWER_MODE		
	  	/* put the BLESS and MCU core to lowest possible power state to save power*/
		handleLowPowerMode();
#endif /* LOW_POWER_MODE */

		/* Process queued BLE events */
		CyBle_ProcessEvents();
		
#ifdef ENABLE_I2C_ONLY_WHEN_CONNECTED
		if(cyBle_state == CYBLE_STATE_CONNECTED)
		{
			/* Handle SPI read and write */
			handleSPITraffic();
		}
#else
		handleSPITraffic();
#endif		
	}
}


