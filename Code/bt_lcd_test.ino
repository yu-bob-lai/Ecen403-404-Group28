#include <SoftwareSerial.h>
#include <SPI.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

//Define the data transmit/receive pimns in Arduino
#define TxD 3
#define RxD 2
LiquidCrystal_I2C lcd(0x27,20,3);  // set the LCD address to 0x3F for a 16 chars and 2 line display
SoftwareSerial mySerial(RxD, TxD); //RX, TX for Bluetooth
int LED = 8;

void setup() 
{
  // put your setup code here, to run once:
  mySerial.begin(9600); //For Bluetooth
  Serial.begin(9600); //For  the IDE monitor Tools -> Serial Monitor
  pinMode(LED, OUTPUT);
  lcd.begin();
  lcd.backlight();      // Make sure backlight is on
  lcd.clear();  

  lcd.setCursor(4,0);   //Set cursor to character 2 on line 0
  lcd.print("Ready");
}

void loop() 
{
  boolean isValidInput; do
  {
    byte c; //get the next character from the bluetooth serial port
    while(!mySerial.available());//Loop
    c = mySerial.read();//Execute the option based on the character recieved
    Serial.print(c);// Print the character received to the IDE serial monitor
    switch(c)
    {
      case'I'://turns on led when enter a
        mySerial.println("You've enterend an 'I', led on");
        lcd.setCursor(4,0);   
        lcd.print("You entered I");
        lcd.setCursor(4,1);   
        lcd.print("LED turned on");
        digitalWrite(LED, HIGH);
        isValidInput = true;
      break;
      case'B'://turns on led when enter a
        mySerial.println("You've enterend an 'B', led blink");
        lcd.print("You entered B");
        lcd.setCursor(4,1);   
        lcd.print("LED will blink 5 times");
        for(int i = 0; i < 5; i++)  
        {
          digitalWrite(LED, HIGH);   
          delay(1000);                       
          digitalWrite(LED, LOW);    
          delay(1000);   
        }
        isValidInput = true;
      break;
      case'O'://turns off led when enter b
        mySerial.println("You've enterend an 'O', led off");
        lcd.print("You entered O");
        lcd.setCursor(4,1);   
        lcd.print("LED turned off");
        digitalWrite(LED, LOW);
        isValidInput = true;
      break;
      default:
        mySerial.println("Please enter 'I', 'B', or 'O'");
        lcd.print("Invalid Input");
        lcd.setCursor(4,1);   
        lcd.print("Please enter I, B, or O");
        isValidInput = false;
      break;
    }
  }
  while(isValidInput == true);//repeat the loop
}
