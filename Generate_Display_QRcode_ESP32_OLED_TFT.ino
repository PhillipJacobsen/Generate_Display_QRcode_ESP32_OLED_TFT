/********************************************************************************
    QRCode Generate and Display on OLED or TFT display
    Generate_Display_QRcode.ino
    2019 @phillipjacobsen

    Program Features:
    This program has been tested with ESP32 Adafruit Huzzah however it should also work with ESP8266 modules with minor changes to hardward connections and wifi libraries.

    Encodes text string into a matrix representing the QR Code
      -program does not check length of text to make sure the QRcode version is able to generate it
    Displays the resulting code on a 128x64 OLED display or a 240x320 TFT display
    The QRcode can be doubled in size so that each QR code pixel shows up as four pixels on the OLED display.
    Default is lit background with Black QRcode. Inverse can also be displayed however codes seem to scan much better with lit background.
********************************************************************************/


/********************************************************************************
               Electronic Hardware Requirements and Pin Connections
   ESP32 Adafruit Huzzah
      Source:  https://www.adafruit.com/product/3405

  Use these connections if using the 0.96 I2C display
    0.96 I2C 128x64 OLED display
      Source:    https://www.aliexpress.com/store/product/Free-shipping-Matrix-360-NAND-Programmer-MTX-USB-SPI-Flasher-V1-0-For-XBOX-360-Game/334970_1735255916.html?spm=2114.12010612.0.0.73fe44c9mSwirS
      SDA -> SDA, pin 23
      SCL -> SCL, pin 22
      VCC -> 3.3V
      GND -> GND

  Use these connections if using the TFT FeatherWing 2.4" 240x320 Touchscreen
    The TFT FeatherWing module from Adafruit is designed with headers to directly mount on the ESP32 Adafruit Huzzah.
    No additional wiring is required for operation.
      Source:   https://www.adafruit.com/product/3315
    TFT_CS  -> pin 15
    TFT_DC  -> pin 33
    RT      -> pin 32
    SD      -> pin 14
    SCK     -> SCK, pin 5
    MISO    -> MISO, pin 19
    MOSI    -> MOSI, pin 18

  Use these connections if using the TFT FeatherWing 3.5" 320x480 Touchscreen
    The TFT FeatherWing module from Adafruit is designed with headers to directly mount on the ESP32 Adafruit Huzzah.
    No additional wiring is required for operation.
    Requires: Adafruit HX8357 TFT Library,  Adafruit GFX Library
      Source:   https://www.adafruit.com/product/3651
    TFT_CS  -> pin 15
    TFT_DC  -> pin 33
    RT      -> pin 32
    SD      -> pin 14
    SCK     -> SCK, pin 5
    MISO    -> MISO, pin 19
    MOSI    -> MOSI, pin 18



  Other I/O
    LED on Huzzah board -> pin 13
********************************************************************************/


/********************************************************************************
  Conditional Assembly
********************************************************************************/
//select one of the following 2 screens
//#define _128x64_OLED      // Uncomment this if you are using the 0.96 OLED display
//#define _240x320_TFT      // Uncomment this if you are using the 2.4" TFT display with resistive touchscreen
#define _320x480_TFT      // Uncomment this if you are using the 3.5" TFT display with resistive touchscreen



/********************************************************************************
                              Library Requirements
********************************************************************************/

/********************************************************************************
    QRCode by Richard Moore version 0.0.1
      Available through Arduino Library Manager
        https://github.com/ricmoo/QRCode

    The QR code data encoding algorithm defines a number of 'versions' that increase in size and store increasing amounts of data.
    The version (size) of a generated QR code depends on the amount of data to be encoded.
    Increasing the error correction level will decrease the storage capacity due to redundancy pixels being added.

    If you have a '? character in your QR text then I think the QR code operates in "Byte" mode.
********************************************************************************/
#include "qrcode.h"
QRCode qrcode;                  // Create the QR code

#ifdef _128x64_OLED
//version 3 code with double sized code and starting at y0 = 2 is good
//version 3 with ECC_LOW gives 53 "bytes".
const int QRcode_Version = 3;   //  set the version (range 1->40)
const int QRcode_ECC = 0;       //  set the Error Correction level (range 0-3) or symbolic (ECC_LOW, ECC_MEDIUM, ECC_QUARTILE and ECC_HIGH)
#define _QR_doubleSize    //
#define Lcd_X  128
#define Lcd_Y  64

#endif

#ifdef _240x320_TFT
//version 8 code with double sized code and starting at y0 = 2 is good
//version 8 with ECC_LOW gives 192 "bytes".
const int QRcode_Version = 8;   //  set the version (range 1->40)
const int QRcode_ECC = 0;       //  set the Error Correction level (range 0-3) or symbolic (ECC_LOW, ECC_MEDIUM, ECC_QUARTILE and ECC_HIGH)
#define _QR_doubleSize    //
#define Lcd_X  240
#define Lcd_Y  320

#endif

#ifdef _320x480_TFT
//version 8 code with double sized code and starting at y0 = 2 is good
//version 8 with ECC_LOW gives 192 "bytes".
const int QRcode_Version = 8;   //  set the version (range 1->40)
const int QRcode_ECC = 0;       //  set the Error Correction level (range 0-3) or symbolic (ECC_LOW, ECC_MEDIUM, ECC_QUARTILE and ECC_HIGH)
#define _QR_doubleSize    //
#define Lcd_X  320
#define Lcd_Y  480

#endif




#ifdef _128x64_OLED
/********************************************************************************
  U8g2lib Monochrome Graphics Display Library
  This library is used for the 0.96" OLED
    Available through Arduino Library Manager
    https://github.com/olikraus/u8g2

  Frame Buffer Examples: clearBuffer/sendBuffer. Fast, but may not work with all Arduino boards because of RAM consumption
  Page Buffer Examples: firstPage/nextPage. Less RAM usage, should work with all Arduino boards.
  U8x8 Text Only Example: No RAM usage, direct communication with display controller. No graphics, 8x8 Text only.
********************************************************************************/
#include <U8g2lib.h>

// U8g2 Constructor List for Frame Buffer Mode.
// This uses the Hardware I2C peripheral on ESP32 with DMA interface
// The complete list is available here: https://github.com/olikraus/u8g2/wiki/u8g2setupcpp
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);

#endif



#ifdef  _240x320_TFT
/********************************************************************************
  GFX library available from Adafruit
  This library is used for the 240x320 TFT
    Available through Arduino Library Manager
********************************************************************************/
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>

#ifdef ESP8266
   #define STMPE_CS 16
   #define TFT_CS   0
   #define TFT_DC   15
   #define SD_CS    2
#endif
#ifdef ESP32
   #define STMPE_CS 32
   #define TFT_CS   15
   #define TFT_DC   33
   #define SD_CS    14
#endif

Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);

#endif



#ifdef  _320x480_TFT
/********************************************************************************
  GFX library available from Adafruit
  This library is used for the 320x480 TFT
    Available through Arduino Library Manager
********************************************************************************/
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_HX8357.h>

#ifdef ESP8266
   #define STMPE_CS 16
   #define TFT_CS   0
   #define TFT_DC   15
   #define SD_CS    2
#endif
#ifdef ESP32
   #define STMPE_CS 32
   #define TFT_CS   15
   #define TFT_DC   33
   #define SD_CS    14
#endif

#define TFT_RST -1
// Use hardware SPI and the above for CS/DC
Adafruit_HX8357 tft = Adafruit_HX8357(TFT_CS, TFT_DC, TFT_RST);

#endif



#define TFT_BLACK       0x0000  ///<   0,   0,   0
#define TFT_WHITE       0xFFFF  ///< 255, 255, 255


void setup() {
  Serial.begin(115200);

#ifdef _128x64_OLED
  u8g2.begin();
  u8g2.clearBuffer();
  u8g2.setContrast(220);                      // set OELD brightness(0->255)
  u8g2.setFont(u8g2_font_6x13_te );           // 9 pixel height
  u8g2.drawStr(0, 12, "Generating QR Code");  // write something to the internal memory
  u8g2.sendBuffer();                          // transfer internal memory to the display
#endif

#ifdef  _240x320_TFT | _320x480_TFT
  tft.begin();
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_WHITE);
  tft.setCursor(0, 32);
  tft.setTextSize(2);  //(30 pixels tall I think)
  tft.println("Generating QR Code");
#endif


  // Start time
  uint32_t dt = millis();

  //--------------------------------------------
  // Allocate memory to store the QR code.
  // memory size depends on version number
  uint8_t qrcodeData[qrcode_getBufferSize(QRcode_Version)];

  //--------------------------------------------
  //configure the text string to code
  //https://github.com/ArkEcosystem/AIPs/blob/master/AIPS/aip-13.md#simpler-syntax

  // qrcode_initText(&qrcode, qrcodeData, QRcode_Version, QRcode_ECC, "{\"a\":\"DE6os4N86ef9bba6kVGurqxmhpBHKctoxY\"}"); //dARK address
  //qrcode_initText(&qrcode, qrcodeData, QRcode_Version, QRcode_ECC, "DE6os4N86ef9bba6kVGurqxmhpBHKctoxY");   //dARK address
  //qrcode_initText(&qrcode, qrcodeData, QRcode_Version, QRcode_ECC, "ark:AePNZAAtWhLsGFLXtztGLAPnKm98VVC8tJ?amount=20.3");    //ARK address

  //qrcode_initText(&qrcode, qrcodeData, QRcode_Version, QRcode_ECC, "DFcWwEGwBaYCNb1wxGErGN1TJu8QdQYgCt?amount=0.3");    //dARK address 51 bytes.
  //qrcode_initText(&qrcode, qrcodeData, QRcode_Version, QRcode_ECC, "dark:DFcWwEGwBaYCNb1wxGErGN1TJu8QdQYgCt?amount=0.3");    //dARK address 51 bytes.

  //qrcode_initText(&qrcode, qrcodeData, QRcode_Version, QRcode_ECC, "AUjnVRstxXV4qP3wgKvBgv1yiApvbmcHhx?amount=0.3");    //jake address 51 bytes
  //qrcode_initText(&qrcode, qrcodeData, QRcode_Version, QRcode_ECC, "AUjnVRstxXV4qP3wgKvBgv1yiApvbmcHhx");    //jake address
  //qrcode_initText(&qrcode, qrcodeData, QRcode_Version, QRcode_ECC, "jake:AUjnVRstxXV4qP3wgKvBgv1yiApvbmcHhx?amount=0.3");    //jake address 51 bytes.
  qrcode_initText(&qrcode, qrcodeData, QRcode_Version, QRcode_ECC, "ark:AUjnVRstxXV4qP3wgKvBgv1yiApvbmcHhx?amount=0.3");    //jake address 51 bytes.


  //  qrcode_initText(&qrcode, qrcodeData, QRcode_Version, QRcode_ECC, "hello pj");
  //qrcode_initText(&qrcode, qrcodeData, QRcode_Version, QRcode_ECC, "1BGJvqAuZvr23EixA65PEe5PMLAjVTeyMn");     //bitcoin address

  //--------------------------------------------
  // Print Code Generation Time
  dt = millis() - dt;
  Serial.print("QR Code Generation Time: ");
  Serial.print(dt);
  Serial.print("\n");

  //--------------------------------------------
  //    This prints the QR code to the serial monitor as solid blocks. Each module
  //    is two characters wide, since the monospace font used in the serial monitor
  //    is approximately twice as tall as wide.
  // Top quiet zone
  Serial.print("\n\n\n\n");
  for (uint8_t y = 0; y < qrcode.size; y++) {
    // Left quiet zone
    Serial.print("        ");
    // Each horizontal module
    for (uint8_t x = 0; x < qrcode.size; x++) {
      // Print each module (UTF-8 \u2588 is a solid block)
      Serial.print(qrcode_getModule(&qrcode, x, y) ? "\u2588\u2588" : "  ");
    }
    Serial.print("\n");
  }
  // Bottom quiet zone
  Serial.print("\n\n\n\n");


#ifdef _128x64_OLED
  //--------------------------------------------
  //display generation time to OLED display
  u8g2.drawStr(0, 30, "Generation Time(ms)"); // write something to the internal memory
  u8g2.setCursor(0, 50);
  u8g2.print(dt);             // display time it took to generate code
  u8g2.sendBuffer();
  delay(3000);
#endif



#ifdef  _240x320_TFT | _320x480_TFT
  tft.setCursor(0, 60);
  tft.setTextSize(1);  //(20 pixels tall I think)
  tft.println();
  tft.print("Generation Time(ms)");
  tft.println(dt);
  delay(3000);
#endif

#ifdef _128x64_OLED
  //--------------------------------------------
  //Turn on all pixels
  for (uint8_t y = 0; y < 63; y++) {
    for (uint8_t x = 0; x < 127; x++) {
      u8g2.setDrawColor(1);       //change to 0 to make QR code with black background
      u8g2.drawPixel(x, y);
    }
  }
#endif

#ifdef  _240x320_TFT | _320x480_TFT
  //--------------------------------------------
  //Turn on all pixels so screen has a white background
  tft.fillScreen(TFT_WHITE);
#endif

  //--------------------------------------------
  //this will put the QRcode in the middle of the screen
  //uint8_t x0 = (Lcd_X - qrcode.size) / 2;
  //uint8_t y0 = (Lcd_Y - qrcode.size) / 2;



#ifdef _128x64_OLED
  uint8_t x0 = 20;
  uint8_t y0 =  2;   //

  //--------------------------------------------
  //display QRcode
  for (uint8_t y = 0; y < qrcode.size; y++) {
    for (uint8_t x = 0; x < qrcode.size; x++) {

      if (qrcode_getModule(&qrcode, x, y) == 0) {     //change to == 1 to make QR code with black background
        u8g2.setDrawColor(1);

#ifdef  _QR_doubleSize
        //uncomment to double the QRcode. Comment to display normal code size
        u8g2.drawPixel(x0 + 2 * x,     y0 + 2 * y);
        u8g2.drawPixel(x0 + 2 * x + 1, y0 + 2 * y);
        u8g2.drawPixel(x0 + 2 * x,     y0 + 2 * y + 1);
        u8g2.drawPixel(x0 + 2 * x + 1, y0 + 2 * y + 1);
#else
        //uncomment to display code in normal size.  Comment to double the QRcode
        u8g2.drawPixel(x0 + x, y0 + y);
#endif

      } else {
        u8g2.setDrawColor(0);


#ifdef  _QR_doubleSize
        //uncomment to double the QRcode. Comment to display normal code size
        u8g2.drawPixel(x0 + 2 * x,     y0 + 2 * y);
        u8g2.drawPixel(x0 + 2 * x + 1, y0 + 2 * y);
        u8g2.drawPixel(x0 + 2 * x,     y0 + 2 * y + 1);
        u8g2.drawPixel(x0 + 2 * x + 1, y0 + 2 * y + 1);
#else
        //uncomment to display code in normal size.  Comment to double the QRcode
        u8g2.drawPixel(x0 + x, y0 + y);
#endif

      }

    }
  }
  u8g2.sendBuffer();

#endif




#ifdef  _240x320_TFT |_320x480_TFT
  //this will put the QRcode on the top left corner
  uint8_t x0 = 20;
  uint8_t y0 =  20;   //
  //--------------------------------------------
  //display QRcode
  for (uint8_t y = 0; y < qrcode.size; y++) {
    for (uint8_t x = 0; x < qrcode.size; x++) {

      if (qrcode_getModule(&qrcode, x, y) == 0) {     //change to == 1 to make QR code with black background


#ifdef  _QR_doubleSize
        //uncomment to double the QRcode. Comment to display normal code size
        tft.drawPixel(x0 + 2 * x,     y0 + 2 * y, TFT_WHITE);
        tft.drawPixel(x0 + 2 * x + 1, y0 + 2 * y, TFT_WHITE);
        tft.drawPixel(x0 + 2 * x,     y0 + 2 * y + 1, TFT_WHITE);
        tft.drawPixel(x0 + 2 * x + 1, y0 + 2 * y + 1, TFT_WHITE);
#else
        //uncomment to display code in normal size.  Comment to double the QRcode
        tft.drawPixel(x0 + x, y0 + y, TFT_WHITE);
#endif

      } else {


#ifdef  _QR_doubleSize
        //uncomment to double the QRcode. Comment to display normal code size
        tft.drawPixel(x0 + 2 * x,     y0 + 2 * y, TFT_BLACK);
        tft.drawPixel(x0 + 2 * x + 1, y0 + 2 * y, TFT_BLACK);
        tft.drawPixel(x0 + 2 * x,     y0 + 2 * y + 1, TFT_BLACK);
        tft.drawPixel(x0 + 2 * x + 1, y0 + 2 * y + 1, TFT_BLACK);
#else
        //uncomment to display code in normal size.  Comment to double the QRcode
        tft.drawPixel(x0 + x, y0 + y, TFT_BLACK);
#endif
      }

    }
  }

#endif

}


void loop() {

  // delay(6000);
  //  u8g2.setContrast(1);    //dim display


}
