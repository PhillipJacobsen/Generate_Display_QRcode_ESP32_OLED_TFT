/********************************************************************************
    QRCode Generate and Display on OLED display
    Generate_Display_QRcode.ino
    2019 @phillipjacobsen

    Program Features:
    Encodes text string into a matrix representing the QR Code
      -program does not check length of text to make sure the QRcode version is able to generate it
    Displays the resulting code on the OLED display
    The QRcode will be doubled in size so that each QR code pixel shows up as four pixels on the OLED display.
    Default is lit background with Black QRcode. Inverse can also be displayed however codes seem to scan much better with lit background.
********************************************************************************/


/********************************************************************************
               Electronic Hardware Requirements and Pin Connections
   ESP32 Adafruit Huzzah
      Source:  


    0.96 I2C 128x64 OLED display
      Source:    https://www.aliexpress.com/store/product/Free-shipping-Matrix-360-NAND-Programmer-MTX-USB-SPI-Flasher-V1-0-For-XBOX-360-Game/334970_1735255916.html?spm=2114.12010612.0.0.73fe44c9mSwirS
      Pins cannot be remapped when using DMA mode
      SDA ->NodeMCU D2 pin (GPIO4)
      SCL ->NodeMCU D1 pin (GPIO5)
      VCC -> 3.3V
      GND
********************************************************************************/



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

    If you have a ? in your QR text then I think the QR code operates in "Byte" mode. 
********************************************************************************/
#include "qrcode.h"
const int QRcode_Version = 3;   //  set the version (range 1->40)
//version 3 with double size and starting at y0 =  2 is good
//veresion 6 with regular size and starting at y0=19 is good for dual color OLED.
//version 3 with ECC_LOW gives 53 "bytes". 

const int QRcode_ECC = 0;       //  set the Error Correction level (range 0-3) or symbolic (ECC_LOW, ECC_MEDIUM, ECC_QUARTILE and ECC_HIGH)

QRCode qrcode;                  // Create the QR code


/********************************************************************************
  U8g2lib Monochrome Graphics Display Library
    Available through Arduino Library Manager
    https://github.com/olikraus/u8g2

  ESP32 module ->use hardware I2C connections
    SDA ->SDA
    SCL ->SCL

  UNO connections -> use hardware I2C connections
    SDA ->A4
    SCL ->A5
  Frame Buffer Examples: clearBuffer/sendBuffer. Fast, but may not work with all Arduino boards because of RAM consumption
  Page Buffer Examples: firstPage/nextPage. Less RAM usage, should work with all Arduino boards.
  U8x8 Text Only Example: No RAM usage, direct communication with display controller. No graphics, 8x8 Text only.
********************************************************************************/
#include <U8g2lib.h>

// U8g2 Contructor List for Frame Buffer Mode.
// This uses the Hardware I2C peripheral on ESP32 with DMA interface
// The complete list is available here: https://github.com/olikraus/u8g2/wiki/u8g2setupcpp
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);



#define Lcd_X  128
#define Lcd_Y  64



void setup() {
  Serial.begin(115200);

  u8g2.begin();
  u8g2.clearBuffer();

  u8g2.setContrast(220);    // set OELD brightness(0->255)

  //u8g2.setFont(u8g2_font_ncenB12_tr );  // 12 pixel height
  //u8g2.setFont(u8g2_font_ncenB08_tr);  // 8 pixel height
  //u8g2.setFont(u8g2_font_9x15_tf );  // 10 pixel height
  u8g2.setFont(u8g2_font_6x13_te );  // 9 pixel height

  u8g2.drawStr(0, 12, "Generating QR Code"); // write something to the internal memory
  u8g2.sendBuffer();          // transfer internal memory to the display


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
  qrcode_initText(&qrcode, qrcodeData, QRcode_Version, QRcode_ECC, "dark:DFcWwEGwBaYCNb1wxGErGN1TJu8QdQYgCt?amount=0.3");    //dARK address 51 bytes.
  

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



  //--------------------------------------------
  //display generation time to OLED display
  u8g2.drawStr(0, 30, "Generation Time(ms)"); // write something to the internal memory
  u8g2.setCursor(0, 50);
  u8g2.print(dt);             // display time it took to generate code
  u8g2.sendBuffer();
  delay(3000);

  //--------------------------------------------
  //Turn on all pixels
  for (uint8_t y = 0; y < 63; y++) {
    for (uint8_t x = 0; x < 127; x++) {
      u8g2.setDrawColor(1);       //change 0 to make QR code with black background
      u8g2.drawPixel(x, y);
    }
  }


  //uint8_t x0 = (Lcd_X - qrcode.size) / 2;
  //uint8_t y0 = (Lcd_Y - qrcode.size) / 2;

  //--------------------------------------------
  //setup the top left corner of the QRcode
  uint8_t x0 = 20;
  //uint8_t y0 =  19;   //16 is the start of the blue portion OLED in the yellow/blue split 64x128 OLED

 uint8_t y0 =  2;   //
  

  //--------------------------------------------
  //display QRcode
  for (uint8_t y = 0; y < qrcode.size; y++) {
    for (uint8_t x = 0; x < qrcode.size; x++) {

      if (qrcode_getModule(&qrcode, x, y) == 0) {     //change to == 1 to make QR code with black background
        u8g2.setDrawColor(1);

        //uncomment to display code in normal size.  Comment to double the QRcode
       // u8g2.drawPixel(x0 + x, y0 + y);

        //uncomment to double the QRcode. Comment to display normal code size
        u8g2.drawPixel(x0 + 2 * x,     y0 + 2 * y);
         u8g2.drawPixel(x0 + 2 * x + 1, y0 + 2 * y);
        u8g2.drawPixel(x0 + 2 * x,     y0 + 2 * y + 1);
        u8g2.drawPixel(x0 + 2 * x + 1, y0 + 2 * y + 1);


      } else {
        u8g2.setDrawColor(0);

         //uncomment to display code in normal size.  Comment to double the QRcode
        //u8g2.drawPixel(x0 + x, y0 + y);

        //uncomment to double the QRcode. Comment to display normal code size
        u8g2.drawPixel(x0 + 2 * x,     y0 + 2 * y);
        u8g2.drawPixel(x0 + 2 * x + 1, y0 + 2 * y);
        u8g2.drawPixel(x0 + 2 * x,     y0 + 2 * y + 1);
        u8g2.drawPixel(x0 + 2 * x + 1, y0 + 2 * y + 1);
      }

    }
  }
  u8g2.sendBuffer();


}

void loop() {

 // delay(6000);
//  u8g2.setContrast(1);    //dim display


}
