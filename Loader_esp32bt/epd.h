/* SPI pin definition --------------------------------------------------------*/
#define EPD_SCK_PIN  13
#define EPD_MOSI_PIN  14
#define EPD_CS_PIN   15
#define EPD_BUSY_PIN 25//19
#define EPD_RST_PIN  26//21
#define EPD_DC_PIN   27//22

/* Pin level definition ------------------------------------------------------*/
#define GPIO_PIN_SET   1
#define GPIO_PIN_RESET 0

#define UBYTE   uint8_t
#define UWORD   uint16_t
#define UDOUBLE uint32_t

void EPD_initSPI()
{
    //Serial.println(SPI._spi_num);
    //Serial.println(SPI.get);

    pinMode(EPD_BUSY_PIN,  INPUT);
    pinMode(EPD_RST_PIN , OUTPUT);
    pinMode(EPD_DC_PIN  , OUTPUT);
    
    pinMode(EPD_SCK_PIN, OUTPUT);
    pinMode(EPD_MOSI_PIN, OUTPUT);
    pinMode(EPD_CS_PIN , OUTPUT);

    digitalWrite(EPD_CS_PIN , 1);
    digitalWrite(EPD_SCK_PIN, 0);
    //SPI.begin(); 
}

/* The procedure of sending a byte to e-Paper by SPI -------------------------*/
void EpdSpiTransferCallback(byte data) 
{
    //SPI.beginTransaction(spi_settings);
    digitalWrite(EPD_CS_PIN, GPIO_PIN_RESET);

    for (int i = 0; i < 8; i++)
    {
        if ((data & 0x80) == 0) digitalWrite(EPD_MOSI_PIN, GPIO_PIN_RESET); 
        else                    digitalWrite(EPD_MOSI_PIN, GPIO_PIN_SET);

        data <<= 1;
        digitalWrite(EPD_SCK_PIN, GPIO_PIN_SET);     
        digitalWrite(EPD_SCK_PIN, GPIO_PIN_RESET);
    }

    //SPI.transfer(data);
    digitalWrite(EPD_CS_PIN, GPIO_PIN_SET);
    //SPI.endTransaction();
}

byte lut_vcom0[] = { 15, 0x0E, 0x14, 0x01, 0x0A, 0x06, 0x04, 0x0A, 0x0A, 0x0F, 0x03, 0x03, 0x0C, 0x06, 0x0A, 0x00 };
byte lut_w    [] = { 15, 0x0E, 0x14, 0x01, 0x0A, 0x46, 0x04, 0x8A, 0x4A, 0x0F, 0x83, 0x43, 0x0C, 0x86, 0x0A, 0x04 };
byte lut_b    [] = { 15, 0x0E, 0x14, 0x01, 0x8A, 0x06, 0x04, 0x8A, 0x4A, 0x0F, 0x83, 0x43, 0x0C, 0x06, 0x4A, 0x04 };
byte lut_g1   [] = { 15, 0x8E, 0x94, 0x01, 0x8A, 0x06, 0x04, 0x8A, 0x4A, 0x0F, 0x83, 0x43, 0x0C, 0x06, 0x0A, 0x04 };
byte lut_g2   [] = { 15, 0x8E, 0x94, 0x01, 0x8A, 0x06, 0x04, 0x8A, 0x4A, 0x0F, 0x83, 0x43, 0x0C, 0x06, 0x0A, 0x04 };
byte lut_vcom1[] = { 15, 0x03, 0x1D, 0x01, 0x01, 0x08, 0x23, 0x37, 0x37, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
byte lut_red0 [] = { 15, 0x83, 0x5D, 0x01, 0x81, 0x48, 0x23, 0x77, 0x77, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
byte lut_red1 [] = { 15, 0x03, 0x1D, 0x01, 0x01, 0x08, 0x23, 0x37, 0x37, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

/* Sending a byte as a command -----------------------------------------------*/
void EPD_2IN7_V2_SendCommand(byte command) 
{
    digitalWrite(EPD_DC_PIN, 0);
    EpdSpiTransferCallback(command);
}

/* Sending a byte as a data --------------------------------------------------*/
void EPD_2IN7_V2_SendData(byte data) 
{
    digitalWrite(EPD_DC_PIN, 1);
    EpdSpiTransferCallback(data);
}


/* Waiting the e-Paper is ready for further instructions ---------------------*/
void EPD_WaitUntilIdle() 
{
    //0: busy, 1: idle
    while(digitalRead(EPD_BUSY_PIN) == 0) delay(100);  

}

/* Waiting the e-Paper is ready for further instructions ---------------------*/
static void EPD_2IN7_V2_ReadBusy(void)
{
    do {
        if(digitalRead(EPD_BUSY_PIN) == 0)
			break;
    } while(1);
    delay(20);
}

/* This function is used to 'wake up" the e-Paper from the deep sleep mode ---*/
static void EPD_2IN7_V2_Reset(void)
{
    digitalWrite(EPD_RST_PIN, 1);
    delay(20);
    digitalWrite(EPD_RST_PIN, 0);
    delay(2);
    digitalWrite(EPD_RST_PIN, 1);
    delay(20);
}

/* e-Paper initialization functions ------------------------------------------*/ 
bool EPD_invert;           // If true, then image data bits must be inverted
int  EPD_dispIndex;        // The index of the e-Paper's type
int  EPD_dispX, EPD_dispY; // Current pixel's coordinates (for 2.13 only)
void(*EPD_dispLoad)();     // Pointer on a image data writting function

/* Image data loading function for a-type e-Paper ----------------------------*/ 
void EPD_loadA()
{
    // Get the index of the image data begin
    int pos = 6;

    // Enumerate all of image data bytes
    while (pos < Buff__bufInd)
    {
        // Get current byte
        int value = Buff__getByte(pos);

        // Invert byte's bits in case of '2.7' e-Paper
        if (EPD_invert) value = ~value;

        // Write the byte into e-Paper's memory
        EPD_2IN7_V2_SendData((byte)0b11111111);
        EPD_2IN7_V2_SendData((byte)0b11111111);

        // Increment the current byte index on 2 characters
        pos++;
    }
}



UBYTE LUT_DATA_4Gray[159] =
{
0x40,	0x48,	0x80,	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,
0x8,	0x48,	0x10,	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,
0x2,	0x48,	0x4,	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,
0x20,	0x48,	0x1,	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,
0x0,	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,
0xA,	0x19,	0x0,	0x3,	0x8,	0x0,	0x0,					
0x14,	0x1,	0x0,	0x14,	0x1,	0x0,	0x3,					
0xA,	0x3,	0x0,	0x8,	0x19,	0x0,	0x0,					
0x1,	0x0,	0x0,	0x0,	0x0,	0x0,	0x1,					
0x0,	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,					
0x0,	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,					
0x0,	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,					
0x0,	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,					
0x0,	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,					
0x0,	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,					
0x0,	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,					
0x0,	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,					
0x22,	0x22,	0x22,	0x22,	0x22,	0x22,	0x0,	0x0,	0x0,			
0x22,	0x17,	0x41,	0x0,	0x32,	0x1C
};

static void EPD_2IN7_V2_Lut(void)
{
    unsigned int count;
    EPD_2IN7_V2_SendCommand(0x32); //vcom
    for(count = 0; count < 153; count++) {
        EPD_2IN7_V2_SendData(LUT_DATA_4Gray[count]);
    }
}

int EPD_2IN7_V2_Init(void)
{
  /*
  EPD_2IN7_V2_Reset();
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
    */
    EPD_2IN7_V2_Reset();

	EPD_2IN7_V2_ReadBusy();
	EPD_2IN7_V2_SendCommand(0x12); // soft reset
	EPD_2IN7_V2_ReadBusy();

	EPD_2IN7_V2_SendCommand(0x74); //set analog block control       
	EPD_2IN7_V2_SendData(0x54);
	EPD_2IN7_V2_SendCommand(0x7E); //set digital block control          
	EPD_2IN7_V2_SendData(0x3B);

	EPD_2IN7_V2_SendCommand(0x01); //Driver output control      
	EPD_2IN7_V2_SendData(0x07);
	EPD_2IN7_V2_SendData(0x01);
	EPD_2IN7_V2_SendData(0x00);

	EPD_2IN7_V2_SendCommand(0x11); //data entry mode       
	EPD_2IN7_V2_SendData(0x03);

	EPD_2IN7_V2_SendCommand(0x44); //set Ram-X address start/end position   
	EPD_2IN7_V2_SendData(0x00);
	EPD_2IN7_V2_SendData(0x15);    //0x15-->(21+1)*8=176

	EPD_2IN7_V2_SendCommand(0x45); //set Ram-Y address start/end position          
	EPD_2IN7_V2_SendData(0x00);
	EPD_2IN7_V2_SendData(0x00);
	EPD_2IN7_V2_SendData(0x07);//0x0107-->(263+1)=264
	EPD_2IN7_V2_SendData(0x01);


	EPD_2IN7_V2_SendCommand(0x3C); //BorderWavefrom
	EPD_2IN7_V2_SendData(0x00);	


	EPD_2IN7_V2_SendCommand(0x2C);     //VCOM Voltage
	EPD_2IN7_V2_SendData(LUT_DATA_4Gray[158]);    //0x1C


	EPD_2IN7_V2_SendCommand(0x3F); //EOPQ    
	EPD_2IN7_V2_SendData(LUT_DATA_4Gray[153]);
	
	EPD_2IN7_V2_SendCommand(0x03); //VGH      
	EPD_2IN7_V2_SendData(LUT_DATA_4Gray[154]);

	EPD_2IN7_V2_SendCommand(0x04); //      
	EPD_2IN7_V2_SendData(LUT_DATA_4Gray[155]); //VSH1   
	EPD_2IN7_V2_SendData(LUT_DATA_4Gray[156]); //VSH2   
	EPD_2IN7_V2_SendData(LUT_DATA_4Gray[157]); //VSL   
   
	EPD_2IN7_V2_Lut(); //LUT

	
	EPD_2IN7_V2_SendCommand(0x4E);   // set RAM x address count to 0;
	EPD_2IN7_V2_SendData(0x00);
	EPD_2IN7_V2_SendCommand(0x4F);   // set RAM y address count to 0X199;    
	EPD_2IN7_V2_SendData(0x00);
	EPD_2IN7_V2_SendData(0x00);
    EPD_2IN7_V2_ReadBusy();
	return 0;
}

void EPD_2IN7_V2_Show(void)
{
    EPD_2IN7_V2_SendCommand(0x22);  //Display Update Control
    EPD_2IN7_V2_SendData(0xC7);
    EPD_2IN7_V2_SendCommand(0x20);  //Activate Display Update Sequence
    EPD_2IN7_V2_ReadBusy();
    delay(2);
    Serial.print("EPD_2IN7_V2_Show END\r\n");
    EPD_2IN7_V2_SendCommand(0X07);  	//deep sleep
    EPD_2IN7_V2_SendData(0xA5);
}

/* The set of pointers on 'init', 'load' and 'show' functions, title and code */
struct EPD_dispInfo
{
    int(*init)(); // Initialization
    void(*chBk)();// Black channel loading
    int next;     // Change channel code
    void(*chRd)();// Red channel loading
    void(*show)();// Show and sleep
    char*title;   // Title of an e-Paper
};

/* Array of sets describing the usage of e-Papers ----------------------------*/
EPD_dispInfo EPD_dispMass[] =
{
    { EPD_2IN7_V2_Init,		EPD_loadA, 		-1  ,	0,				EPD_2IN7_V2_Show,	"2.7 inch V2"	},// 42
};

/* Initialization of an e-Paper ----------------------------------------------*/
void EPD_dispInit()
{
    // Call initialization function
    EPD_dispMass[0].init();

    // Set loading function for black channel
    EPD_dispLoad = EPD_dispMass[0].chBk;

    // Set initial coordinates
    EPD_dispX = 0;
    EPD_dispY = 0;

    // The inversion of image data bits isn't needed by default
    EPD_invert = false;
    
}

