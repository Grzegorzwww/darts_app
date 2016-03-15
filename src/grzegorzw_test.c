/*
===============================================================================
 Name        : grzegorzw_test.c
 Author      : $(author)
 Version     :
 Copyright   : $(copyright)
 Description : main definition
===============================================================================
*/

#ifdef __USE_CMSIS
#include "LPC17xx.h"
#endif

#include <cr_section_macros.h>

#include "lpc17xx_gpio.h"
#include "lpc17xx_pinsel.h"
#include "lpc17xx_uart.h"
#include "lpc17xx_spi.h"
#include "lpc17xx_adc.h"


#define BUFFER_SIZE 10

#define DEVICE (0x53)
#define POWER_CTL 0x2D   //Power Control Register
#define DATA_FORMAT 0x31
#define BW_RATE 0x2C
#define INT_ENABLE 0x2E
#define RANGE_2G   0
#define RANGE_4G   1
#define RANGE_8G   2
#define RANGE_16G   3
#define MEASURE    (1<<3)
#define DEVID 0x00
#define DATAX0 0x32   //X-Axis Data 0
#define DATAX1 0x33   //X-Axis Data 1
#define DATAY0 0x34   //Y-Axis Data 0
#define DATAY1 0x35   //Y-Axis Data 1
#define DATAZ0 0x36   //Z-Axis Data 0
#define DATAZ1 0x37   //Z-Axis Data 1
#define  FIFO_CTL   0x38

#define SSP_RNE (1<<2)
#define SSP_BSY (1<<4)


#define REG_WHO_AM_I    0x0F
#define REG_CTRL_REG1   0x20
#define REG_CTRL_REG2   0x21
#define REG_CTRL_REG3   0x22
#define REG_STATUS_REG  0x27
#define REG_OUT_X       0x29
#define REG_OUT_Y       0x2B
#define REG_OUT_Z       0x2D



#define ADC_BUFFOR_SIZE 32
#define ADC_BUFFER_HALF 16
#define MULTIPLI  976


#define ZERO_G 1652

typedef enum  {true = 1, false = 0} bool_t;
typedef unsigned char uint8_t;
volatile uint32_t msTicks;
uint8_t tempdata = 0;
uint32_t var;
uint8_t Tx_Buf[BUFFER_SIZE];
uint8_t Rx_Buf[BUFFER_SIZE];


uint16_t adc_AD0Buff[ADC_BUFFOR_SIZE];
uint16_t adc_AD1Buff[ADC_BUFFOR_SIZE];
uint16_t adc_AD2Buff[ADC_BUFFOR_SIZE];

uint8_t buff_counterAD0;
uint8_t buff_counterAD1;
uint8_t buff_counterAD2;

uint16_t adc_AD0;
uint16_t adc_AD1;
uint16_t adc_AD2;






void SysTick_Handler(void) {
	msTicks++;
}
__INLINE static void ms_delay (uint32_t delayTicks) {
	uint32_t currentTicks;
	currentTicks = msTicks;	// read current tick counter
	while ((msTicks - currentTicks) < delayTicks);
}


void INOUT_Inicializacion(void);
void SPI(void);
void SPI_TXRX(uint8_t* tx, uint8_t* rx, uint16_t len);
void ADXL_INIT(void);
void LIS_INIT(void);
uint8_t LISSetread_write(uint8_t reg, char operation);
uint8_t LISSetincrement(uint8_t reg, char incr);


uint16_t SORTSamples(uint16_t *d, uint16_t x);
uint8_t GETFromBufforAdc(uint16_t *buffor, uint8_t *no);
void ADDToBufforAdc(uint16_t *buffor, uint16_t dana, uint8_t size);
uint16_t SIGNALCorrection(uint16_t *buff);

uint16_t Converion(uint16_t value, char axis);



int main(void) {

    volatile static int i = 0;

    bool_t is_pushButton1 = false;
    bool_t is_pushButton2 = false;

    uint16_t calibration_diff_X = 0;
    uint16_t calibration_diff_Y = 0;

    uint16_t adc_AD0 = 0;
    uint16_t adc_AD1 = 0;
    uint16_t adc_AD2 = 0;

    uint16_t X_move = 0;
    uint16_t Y_move = 0;
    uint16_t Z_move = 0;

    uint8_t vol_msbX = 0;
    uint8_t vol_lsbX = 0;
    uint8_t vol_msbY = 0;
    uint8_t vol_lsbY = 0;
    uint8_t vol_msbZ = 0;
    uint8_t vol_lsbZ = 0;

    if (SysTick_Config(SystemCoreClock / 1000)) {
       	while (1);  // Capture error
       }




    UART1_Inicializacion(115200);
    ADC_Initialization();
    INOUT_Inicializacion();


    while(1) {

    	ms_delay(2);


    	adc_AD0 = ADC_ChannelGetData(LPC_ADC,ADC_CHANNEL_0);
    	adc_AD1 = ADC_ChannelGetData(LPC_ADC,ADC_CHANNEL_1);
    	adc_AD2 = ADC_ChannelGetData(LPC_ADC,ADC_CHANNEL_2);


    	ADDToBufforAdc(adc_AD0Buff, adc_AD0, ADC_BUFFOR_SIZE);
    	ADDToBufforAdc(adc_AD1Buff, adc_AD1, ADC_BUFFOR_SIZE);
    	ADDToBufforAdc(adc_AD2Buff, adc_AD2, ADC_BUFFOR_SIZE);


    	X_move = SIGNALCorrection(adc_AD0Buff);
    	Y_move = SIGNALCorrection(adc_AD1Buff);
    	Z_move = SIGNALCorrection(adc_AD2Buff);

    	X_move = X_move - calibration_diff_X;
    	Y_move = Y_move - calibration_diff_Y;

    	X_move = Converion(X_move, 'x');
    	Y_move = Converion(Y_move, 'y');
    	Z_move = Converion(Z_move, 'z');

    	vol_msbX = (X_move >> 8);
    	vol_lsbX = (unsigned char)X_move;

    	vol_msbY = (Y_move >> 8);
    	vol_lsbY = (unsigned char)Y_move;

    	vol_msbZ = (Z_move >> 8);
    	vol_lsbZ = (unsigned char)Z_move;

    	UART_SendByte((LPC_UART_TypeDef *)LPC_UART1, 0xC1);
    	UART_SendByte((LPC_UART_TypeDef *)LPC_UART1, vol_msbX);
    	UART_SendByte((LPC_UART_TypeDef *)LPC_UART1, vol_lsbX);
    	UART_SendByte((LPC_UART_TypeDef *)LPC_UART1, vol_msbY);
    	UART_SendByte((LPC_UART_TypeDef *)LPC_UART1, vol_lsbY);
    	UART_SendByte((LPC_UART_TypeDef *)LPC_UART1, vol_msbZ);
    	UART_SendByte((LPC_UART_TypeDef *)LPC_UART1, vol_lsbZ);

    	//LPC_GPIO0->FIOCLR = (1<<22);

    	//

    	var = LPC_GPIO0->FIOPIN;

    	if (!(var & (1 << (27)))){
    		if(is_pushButton1){
    			is_pushButton1 = false;
    			LPC_GPIO0->FIOSET = (1<<22);
    			LPC_GPIO0->FIOSET = (1<<2);
    		}
    		else {
    			is_pushButton1 = true;
    			LPC_GPIO0->FIOCLR = (1<<22);
    			LPC_GPIO0->FIOCLR = (1<<2);
    		}
    		//UART_SendByte((LPC_UART_TypeDef *)LPC_UART1, 0xFF);
    	}
    	if (!(var & (1 << (28)))){

        		if(is_pushButton2){
        			is_pushButton2 = false;
        			calibration_diff_X = X_move;
        			calibration_diff_Y = Y_move;
        		}
        		else {
        			is_pushButton2 = true;
        			calibration_diff_X = 0;
        			calibration_diff_Y = 0;

        		}
    		//UART_SendByte((LPC_UART_TypeDef *)LPC_UART1, 0x55);
    	}
    	ms_delay(5);

    }
    return 0 ;
}

uint16_t Converion(uint16_t value, char axis)
{
	uint16_t to_return = ((value * MULTIPLI) / 1000);


	if(ZERO_G > to_return)
	{
		//to_return = kalibracja(to_return, axis);
		to_return = ZERO_G - to_return;
		//to_return = ((to_return *245) / 100);

		to_return |= 0x8000;
	}
	else if(ZERO_G < to_return){
		//to_return = kalibracja(to_return, axis);
		to_return = to_return - ZERO_G;

		//to_return = ((to_return *245) / 100);
		to_return &= ~(1 << 15);
	}



	return to_return;
}



uint16_t SIGNALCorrection(uint16_t *buff)
{
	return SORTSamples(buff, ADC_BUFFOR_SIZE);
}

void ADDToBufforAdc(uint16_t *buffor, uint16_t dana, uint8_t size)
{
	int i;
	for(i = size - 1; i > 0; i--){
		buffor[i] = buffor[i - 1];
	}
	buffor[0] = dana;

}

uint8_t GETFromBufforAdc(uint16_t *buffor, uint8_t *no)
{
	int i;
	uint8_t dana = buffor[0];
	for(i = 0; i < no; i++)
	{
		buffor[i] = buffor[i + 1];
	}
	*no--;
	return dana;
}
uint16_t SORTSamples(uint16_t *d, uint16_t x)
{
	uint16_t tab[ADC_BUFFOR_SIZE];
	int j, i;
	uint16_t temp;

	for(i = 0; i < x;i++){
		tab[i] = d[i]  &= 0xFFFC;
	}
	for(j = x - 1; j > 0; j--)
		for(i = 0; i < j; i++)
			if(tab[i] > tab[i + 1]){
				temp = tab[i];
				tab[i] = tab[i + 1];
				tab[i + 1] = temp;
			}
	return tab[ADC_BUFFER_HALF];
}


void UART1_IRQHandler(void) {


	uint8_t IIRValue, LSRValue;
	uint8_t Dummy = Dummy;
	IIRValue = LPC_UART1->IIR;
	LSRValue = LPC_UART1->LSR;

	IIRValue >>= 1;            /* skip pending bit in IIR */
	IIRValue &= 0x0E;        /* check bit 1~3, interrupt identification */
	IIRValue <<= 1;

	if (LSRValue & UART_LSR_RDR){    /* Receive Data Ready */
		/* If no error on RLS, normal ready, save into the data buffer. */
		/* Note: read RBR will clear the interrupt */
		tempdata = LPC_UART1->RBR;
	} else if (IIRValue == UART_IIR_INTID_RDA) {   /* Receive Data Available */
		/* Receive Data Available */
		tempdata = LPC_UART1->RBR;
	}
}
void SPI(void)
{

		 LPC_SC->PCONP |= (1 << 10); // Power ON SSP1
		 LPC_SC->PCLKSEL0 &= ~(3<<20);	//Peripheral clock selection for SSP1, PCLK_peripheral = CCLK/4
		 //LPC_SC->PCLKSEL0 |= (1<<20);	//PCLK_peripheral = CCLK/2

		  //   P0.6 as SSEL1 (2 at 13:12)
		 LPC_PINCON->PINSEL0 &= ~(3 << 12);
		 LPC_PINCON->PINSEL0 |= (2 << 12);

		  //   P0.7 as SCK1 (2 at 15:14)
		 LPC_PINCON->PINSEL0 &= ~(3 << 14);
		 LPC_PINCON->PINSEL0 |= (2 << 14);

		 //   P0.8 as MISO1 (2 at 17:16)
		 LPC_PINCON->PINSEL0 &= ~(3 << 16);
		 LPC_PINCON->PINSEL0 |= (2 << 16);

		 //   P0.9 as MOSI1 (2 at 19:18)
		 LPC_PINCON->PINSEL0 &= ~(3 << 18);
		 LPC_PINCON->PINSEL0 |= (2 << 18);

		 LPC_GPIO0->FIODIR = (1 << 6);

		 // SSP1 Control Register 0
		 //   8-bit transfers (7 at 3:0)
		 //   SPI (0 at 5:4)
		 //	  CPOL = 1 CPHA = 1 (1 at 7:6)
		 //   Polarity and Phase default to Mode 0
		 LPC_SSP1->CR0 = 0xC7;
		 //LPC_SSP1->CR0 = 0x07; // CPOL = 0 CPHA = 0 (0 at 7:6)
		 //SSP1 Prescaler at 200khz
		 LPC_SSP1->CPSR = SystemCoreClock / 200000;
		 // SPI Control Register 1
		 //   Defaults to Master
		 //   Start serial communications (bit 1)
		 LPC_SSP1->CR1 |= (1 << 1);
}


void SPI_TXRX(uint8_t* tx, uint8_t* rx, uint16_t len)
{
	/* Embed: transmit and receive len bytes
	 * Remember:
	 *   SPI transmits and receives at the same time
	 *   If tx == NULL and you are only receiving then transmit all 0xFF
	 *   If rx == NULL and you are only transmitting then dump all recieved bytes
	 */
  volatile uint_fast8_t dummy;

  ms_delay(1);

  while (len--) {
    if (tx == NULL) {
      LPC_SSP1->DR = 0xff;
    } else {
      LPC_SSP1->DR = *tx++;
    }

    while ( (LPC_SSP1->SR & (SSP_BSY | SSP_RNE)) != SSP_RNE );

    if (rx == NULL) {
      dummy = LPC_SSP1->DR;
    } else {
      *rx++ = LPC_SSP1->DR;
    }
  }


}



void INOUT_Inicializacion(void)
{


	PINSEL_CFG_Type PinCfg;

//	PinCfg.Funcnum = 1;
//	PinCfg.OpenDrain = 0;
//	PinCfg.Pinmode = 0;
//	PinCfg.Portnum = 0;
//	PinCfg.Pinnum = 22;
//	PINSEL_ConfigPin(&PinCfg);


	PinCfg.Funcnum = 0;
	PinCfg.OpenDrain = 0;
	PinCfg.Pinmode = PINSEL_PINMODE_PULLUP;
	PinCfg.Portnum = 0;
	PinCfg.Pinnum = 28;
	PINSEL_ConfigPin(&PinCfg);

	PinCfg.Pinnum = 27;
	PINSEL_ConfigPin(&PinCfg);

//	PinCfg.Pinnum = 26;
//	PINSEL_ConfigPin(&PinCfg);
//	PinCfg.Pinnum = 25;
//	PINSEL_ConfigPin(&PinCfg);


//	LPC_GPIO0->FIODIR = (1<<2);
//	LPC_GPIO0->FIODIR = (1<<3);


	LPC_GPIO0->FIODIR &= ~(1<<28);
	LPC_GPIO0->FIODIR &= ~(1<<27);

	LPC_GPIO0->FIODIR |= (1<<22);
	LPC_GPIO0->FIODIR |= (1<<2);
	LPC_GPIO0->FIODIR |= (1<<3);





}



void ADXL_INIT(void)
{

	Tx_Buf[1] = DATA_FORMAT;
	Tx_Buf[2] = 0x0B;
	Tx_Buf[3] = POWER_CTL;
	Tx_Buf[4] = 0x08;
	Tx_Buf[5] = INT_ENABLE;
	Tx_Buf[6] = 0x80;

	SPI_TXRX(Tx_Buf, Rx_Buf, 6);

}
