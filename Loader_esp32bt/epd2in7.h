/**
  ******************************************************************************
  * @file    edp2in7.h
  * @author  Waveshare Team
  * @version V1.0.0
  * @date    23-January-2018
  * @brief   This file describes initialisation of 2.7 and 2.7b e-Papers
  *
  ******************************************************************************
  */

int EPD_2IN7_V2_Init(void)
{
    EPD_Reset();
    EPD_WaitUntilIdle_high();

    EPD_2IN7_V2_SendCommand(0x12); //SWRESET
    EPD_WaitUntilIdle_high();

    EPD_2IN7_V2_SendCommand(0x45); //set Ram-Y address start/end position          
    EPD_2IN7_V2_SendData(0x00);
    EPD_2IN7_V2_SendData(0x00);
    EPD_2IN7_V2_SendData(0x07); //0x0107-->(263+1)=264
    EPD_2IN7_V2_SendData(0x01);

    EPD_2IN7_V2_SendCommand(0x4F);   // set RAM y address count to 0;    
    EPD_2IN7_V2_SendData(0x00);
    EPD_2IN7_V2_SendData(0x00);

    EPD_2IN7_V2_SendCommand(0x11);   // data entry mode
    EPD_2IN7_V2_SendData(0x03);

    EPD_2IN7_V2_SendCommand(0x24);
    delay(2);
	return 0;
}

void EPD_2IN7_V2_Show(void)
{
    EPD_2IN7_V2_SendCommand(0x22);  //Display Update Control
    EPD_2IN7_V2_SendData(0XF7);
    EPD_2IN7_V2_SendCommand(0x20);  //Activate Display Update Sequence
    EPD_WaitUntilIdle_high();
    delay(2);
    Serial.print("EPD_2IN7_V2_Show END\r\n");
    EPD_2IN7_V2_SendCommand(0X07);  	//deep sleep
    EPD_2IN7_V2_SendData(0xA5);
}
