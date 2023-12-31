/*   
   Mahjong CP Multiplex

           -------------------------
COM 0- 3   | A | E | I | M | K | S |
COM 1- 2   | B | F | J | N | R | B |
COM 2- 6   | C | G | K | C | R |   |
COM 3-10   | D | H | L | P |   |   |
COM 4- 7   | L | T | W | F | B | S |
           -------------------------
             K   K   K   K   K   K
             E   E   E   E   E   E
             Y   Y   Y   Y   Y   Y
             0   1   2   3   4   5
             -   -   -   -   -   -
             4   5   8   9  11   1

*/

#ifndef USER_USB_RAM
#error "This example needs to be compiled with a USER USB setting"
#endif

#include "src/userUsbComposite/USBCDC.h"
#include "src/userUsbComposite/USBHIDKeyboard.h"

__xdata char recvStr[64];
char machine[12];
uint8_t recvStrPtr = 0;
bool stringComplete = false;

void USBInit() {
  USBDeviceCfg();         // Device mode configuration
  USBDeviceEndPointCfg(); // Endpoint configuration
  USBDeviceIntCfg();      // Interrupt configuration
  UEP0_T_LEN = 0;
  UEP1_T_LEN = 0; // Pre-use send length must be cleared
  UEP2_T_LEN = 0;
  UEP3_T_LEN = 0;
}

#define ROWS 5
#define COLS 6
#define LED_PIN 30

const char keys[ROWS][COLS] = {
  {'A','E','I','M',KEY_LEFT_CTRL, '1'},
  {'B','F','J','N',KEY_LEFT_SHIFT,'3'},
  {'C','G','K',' ','Z','5'},
  {'D','H','L',KEY_LEFT_ALT,'2','4'},
  {KEY_RIGHT_ALT,KEY_RIGHT_CTRL,KEY_RIGHT_SHIFT,'Y',KEY_RETURN,KEY_BACKSPACE}
};
const byte rowPins[ROWS] = {17,16,15,14,32};
const byte colPins[COLS] = {31,37,36,35,34,33};

void setup()
{
  USBInit();
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, HIGH);
  delay(100);
  digitalWrite(LED_PIN, LOW);
   
  for(char r = 0; r < ROWS; r++){
         pinMode(rowPins[r], INPUT_PULLUP);
   }
  for(char c = 0; c < COLS; c++){
         pinMode(colPins[c], OUTPUT);   //set the column pins as output
         digitalWrite(colPins[c], HIGH); 
  }
}

char getKey(){
      char k = 0;
      
      for(char c = 0; c < COLS; c++){
        digitalWrite(colPins[c], LOW);
         for(char r = 0; r < ROWS; r++){
            if(digitalRead(rowPins[r]) == LOW){
              digitalWrite(LED_PIN, HIGH);
              delay(20);    //20ms debounce time
            while(digitalRead(rowPins[r])== LOW);
              k = keys[r][c];
            }
         }
      digitalWrite(colPins[c], HIGH); 
      }
      return k;
}

void loop()
{
  char key = getKey();

  if (key != 0){
    Keyboard_write(key);
    digitalWrite(LED_PIN, LOW);
  }
  while (USBSerial_available()) {
    char serialChar = USBSerial_read();
    if ((serialChar == '\n') || (serialChar == '\r') ) {
      recvStr[recvStrPtr] = '\0';
      if (recvStrPtr > 0) {
        stringComplete = true;
        break;
      }
    } else {
      recvStr[recvStrPtr] = serialChar;
      recvStrPtr++;
      if (recvStrPtr == 63) {
        recvStr[recvStrPtr] = '\0';
        stringComplete = true;
        break;
      }
    }
  }

  if (stringComplete) {
    if(strstr(recvStr,"mame_start = ")) {
      strncpy(machine, recvStr + 13, 12);
      USBSerial_println("nice machine");
      USBSerial_flush();
    } else if (strstr(recvStr,"=")) {
      uint8_t ledn = 0;
      for (uint8_t i = 0; i < NUM_LEDS; i++) {
        if(strchr(recvStr,i+'0')) {
          ledn=i;
        }
      }
      if (strstr(recvStr,"red")) {
        //set_pixel_for_GRB_LED(ledData, ledn, 0, 1, 0);
      } else if (strstr(recvStr,"green")) {
        //set_pixel_for_GRB_LED(ledData, ledn, 1, 0, 0);
      } else if (strstr(recvStr,"blue")) {
        //set_pixel_for_GRB_LED(ledData, ledn, 0, 0, 1);
      }
      //neopixel_show_P1_5(ledData, NUM_BYTES);
    }
    
    stringComplete = false;
    recvStrPtr = 0;
  }
}
