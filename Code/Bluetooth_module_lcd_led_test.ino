#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27,20,3);  // set the LCD address to 0x3F for a 16 chars and 2 line display

void setup() {
  pinMode(7, OUTPUT);
  lcd.begin();
  lcd.backlight();      // Make sure backlight is on
  lcd.clear();         

  
  // Print a message on both lines of the LCD.
  lcd.setCursor(4,0);   //Set cursor to character 2 on line 0
  lcd.print("LCD Testing");
  
  lcd.setCursor(4,1);   //Move cursor to character 2 on line 1
  lcd.print("Ready");
  delay(15000);
}

void loop() {
  digitalWrite(7, HIGH);
  lcd.clear();
  lcd.setCursor(4,0);   //Set cursor to character 2 on line 0
  lcd.print("LED is");
  lcd.setCursor(4,1);   //Move cursor to character 2 on line 1
  lcd.print("On");
  delay(750);
  digitalWrite(7, LOW);
  lcd.clear();
  lcd.setCursor(4,0);   //Set cursor to character 2 on line 0
  lcd.print("LED is");
  lcd.setCursor(4,1);   //Set cursor to character 2 on line 0
  lcd.print("Off");
  delay(750);
}
