# Generate_Display_QRcode_on_OLED_ESP32_TFT

Program Features:
    This program has been tested with ESP32 Adafruit Huzzah however it should also work with ESP8266 modules with minor changes to hardware connections and wifi libraries.

Encodes text string into a matrix representing the QR Code
 -program does not check length of text to make sure the QRcode version is able to generate it
Displays the resulting code on a 128x64 OLED display or a 240x320 TFT display
The QRcode can be doubled in size so that each QR code pixel shows up as four pixels on the OLED display.
Default is lit background with Black QRcode. Inverse can also be displayed however codes seem to scan much better with lit background.
    

Uses QRCode library by Richard Moore version 0.0.1
        https://github.com/ricmoo/QRCode

![Alt text](../Generate_Display_QRcode_ESP32_OLED_TFT/240x320 TFT with QRcode.jpg?raw=true "240x320 TFT")

        
      
