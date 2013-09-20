#include <LiquidCrystal.h>

LiquidCrystal lcd(3,4,5,10,11,12,13);

void setup(){
  analogWrite(6,2.5/5*255);
  lcd.begin(16,2);
  lcd.print("hello World");
}

void loop(){
}
  


