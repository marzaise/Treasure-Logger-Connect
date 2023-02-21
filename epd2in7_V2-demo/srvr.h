/**
  ******************************************************************************
  * @file    srvr.h
  * @author  Waveshare Team
  * @version V2.0.0
  * @date    10-August-2018
  * @brief   ESP8266 WiFi server.
  *          This file provides firmware functions:
  *           + Sending web page of the tool to a client's browser
  *           + Uploading images from client part by part
  *
  ******************************************************************************
  */ 

/* Library includes ----------------------------------------------------------*/
#include <BluetoothSerial.h>
#include "GUI_Paint.h"

bool Srvr__btIsOn;// It's true when bluetooth is on
bool Srvr__btConn;// It's true when bluetooth has connected client 
int  Srvr__msgPos;// Position in buffer from where data is expected
int  Srvr__length;// Length of loaded data

UBYTE *BlackImage;
UWORD Imagesize;
unsigned char* arr = (unsigned char *) malloc(sizeof(unsigned char) * 11616);
int bytePosition;

/* Client ---------------------------------------------------------------------*/
BluetoothSerial Srvr__btClient; // Bluetooth client 

/* Avaialble bytes in a stream ------------------------------------------------*/
int Srvr__available()
{
    return Srvr__btIsOn ? Srvr__btClient.available() : false;
}

void Srvr__write(const char*value)
{
    // Write data to bluetooth
    if (Srvr__btIsOn) Srvr__btClient.write((const uint8_t*)value, strlen(value));
}

int Srvr__read()
{
    return Srvr__btIsOn ? Srvr__btClient.read() : -1;
}

void Srvr__flush()
{
    // Clear Bluetooth's stream
    if (Srvr__btIsOn) Srvr__btClient.flush();  
}

/* Project includes ----------------------------------------------------------*/
#include "buff.h"       // POST request data accumulator
#include "EPD_2in7_V2.h"        // e-Paper driver

void printTreasure(uint16_t X_Center, uint16_t Y_Center)
{
    Paint_DrawCircle(X_Center, Y_Center, 5, WHITE, DOT_PIXEL_5X5, DRAW_FILL_FULL);
    Paint_DrawCircle(X_Center, Y_Center, 4, GRAY2, DOT_PIXEL_5X5, DRAW_FILL_FULL);
    Paint_DrawCircle(X_Center, Y_Center, 2, GRAY4, DOT_PIXEL_5X5, DRAW_FILL_FULL);
}

bool Srvr__btSetup()                                              
{
    // Name shown in bluetooth device list of App part (PC or smartphone)
    String devName("esp32");

    // Turning on
    Srvr__btIsOn = Srvr__btClient.begin(devName);

    // Show the connection result
    if (Srvr__btIsOn) Serial.println("Bluetooth is on");
    else Serial.println("Bluetooth is off");

    // There is no connection yet
    Srvr__btConn = false;

    // Return the connection result
    return Srvr__btIsOn;
}

/* The server state observation loop -------------------------------------------*/
bool Srvr__loop() 
{
    // Bluetooh connection checking
    if (!Srvr__btIsOn) return false;

    // Show and update the state if it was changed
    if (Srvr__btConn != Srvr__btClient.hasClient())
    {
        Serial.print("Bluetooth status:");
        Srvr__btConn = !Srvr__btConn;
        if(Srvr__btConn)
            Serial.println("connected"); 
        else
            Serial.println("disconnected"); 
    }

    // Exit if there is no bluetooth connection
    if (!Srvr__btConn) return false; 

    // Waiting the client is ready to send data
    while(!Srvr__btClient.available()) 
    {
        delay(1);
    }

    // Set buffer's index to zero
    // It means the buffer is empty initially
    Buff__bufInd = 0;

    // While the stream of 'client' has some data do...
    while (Srvr__available())
    {
        // Read a character from 'client'
        int q = Srvr__read();

        // Save it in the buffer and increment its index
        
        Buff__bufArr[Buff__bufInd++] = (byte)q;
    }
    Serial.println();

    // Initialization
    if (Buff__bufArr[0] == 'I')
    {
        DEV_Module_Init();
        Srvr__length = 0;
        bytePosition = 0;

        // Initialization
        free(BlackImage);
        Imagesize = ((EPD_2IN7_V2_WIDTH % 4 == 0)? (EPD_2IN7_V2_WIDTH / 4 ): (EPD_2IN7_V2_WIDTH / 4 + 1)) * EPD_2IN7_V2_HEIGHT;
        if((BlackImage = (UBYTE *)malloc(Imagesize)) == NULL) {
            while (1);
        }
        EPD_2IN7_V2_Init_4GRAY();
        Paint_NewImage(BlackImage, EPD_2IN7_V2_WIDTH, EPD_2IN7_V2_HEIGHT, 90, WHITE);
        Paint_SetScale(4);
        Paint_Clear(0xff);

        Buff__bufInd = 0;
        Srvr__flush();
    }

    // Loading of pixels' data
    else if (Buff__bufArr[0] == 'L')
    {
        // Print log message: image loading
        Serial.print("<<<LOAD");
        int dataSize = Buff__getWord(1);

        int pos = 6;

        while (pos < Buff__bufInd){
          int value = Buff__getByte(pos);
          arr[bytePosition] = value;
          pos++;
          bytePosition++;
        }

        Serial.printf("going to add data to position %d", bytePosition);

        Srvr__length += dataSize;
                
        if ((Buff__bufInd < dataSize) || Srvr__length != Buff__getN3(3))
        {
            Buff__bufInd = 0;
            Srvr__flush();

            Serial.print(" - failed!>>>");
            Srvr__write("Error!");
            return true;
        }

        Buff__bufInd = 0;
        Srvr__flush();
    }

    // Show loaded picture
    else if (Buff__bufArr[0] == 'S')
    {    
        Paint_DrawBitMap(arr);

        Paint_DrawLine(105, 95, 50, 45, GRAY3, DOT_PIXEL_2X2, LINE_STYLE_SOLID);
        Paint_DrawLine(50, 45, 80, 25, GRAY3, DOT_PIXEL_2X2, LINE_STYLE_SOLID);
        Paint_DrawLine(80, 25, 35, 15, GRAY3, DOT_PIXEL_2X2, LINE_STYLE_SOLID);
        Paint_DrawLine(35, 15, 25, 50, GRAY3, DOT_PIXEL_2X2, LINE_STYLE_SOLID);
        Paint_DrawLine(25, 50, 65, 100, GRAY3, DOT_PIXEL_2X2, LINE_STYLE_SOLID);
        Paint_DrawLine(65, 100, 100, 50, GRAY3, DOT_PIXEL_2X2, LINE_STYLE_SOLID);
        Paint_DrawLine(100, 50, 180, 120, GRAY3, DOT_PIXEL_2X2, LINE_STYLE_SOLID);

        Paint_DrawCircle(105, 95, 5, BLACK, DOT_PIXEL_5X5, DRAW_FILL_FULL);
        Paint_DrawCircle(105, 95, 8, BLACK, DOT_PIXEL_1X1, DRAW_FILL_EMPTY);

        printTreasure(150, 150);
        printTreasure(120, 140);
        printTreasure(100, 130);
        printTreasure(200, 160);


        EPD_2IN7_V2_4GrayDisplay(BlackImage);
                
        Buff__bufInd = 0;
        Srvr__flush();

        //Print log message: show
        Serial.print("<<<SHOW");
    }

    // Send message "Ok!" to continue
    Srvr__write("Ok!");
    delay(1);

    // Print log message: the end of request processing
    Serial.print(">>>");
    return true;
}
