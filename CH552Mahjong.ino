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

#include "src/userUsbHidKeyboard/USBHIDKeyboard.h"

#define LED_BUILTIN 33

#define ROWS 5
#define COLS 6

const char keys[ROWS][COLS] = {
  {'A','E','I','M',KEY_LEFT_CTRL, '1'},
  {'B','F','J','N',KEY_LEFT_SHIFT,'3'},
  {'C','G','K',' ','Z','5'},
  {'D','H','L',KEY_LEFT_ALT,'2','4'},
  {KEY_RIGHT_ALT,KEY_RIGHT_CTRL,KEY_RIGHT_SHIFT,'Y',KEY_RETURN,KEY_BACKSPACE}
};
const byte rowPins[ROWS] = {17,16,15,14,32};
const byte colPins[COLS] = {13,37,36,35,34,33};

void setup()
{
  USBInit();
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
  for(char r = 0; r < ROWS; r++){
         pinMode(rowPins[r], INPUT);    //set the row pins as input
         digitalWrite(rowPins[r], HIGH);    //turn on the pullups
   }
  digitalWrite(LED_BUILTIN, HIGH);
  for(char c = 0; c < COLS; c++){
         pinMode(colPins[c], OUTPUT);   //set the column pins as output
  }
  digitalWrite(LED_BUILTIN, LOW);
}

char getKey(){
      char k = 0;
      
      for(char c = 0; c < COLS; c++){
        digitalWrite(colPins[c], LOW);
         for(char r = 0; r < ROWS; r++){
            if(digitalRead(rowPins[r]) == LOW){
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
    digitalWrite(LED_BUILTIN, HIGH);
    Keyboard_write(key);
    digitalWrite(LED_BUILTIN, LOW);
  }
}
