#include <Wire.h>
#include <LiquidCrystal_I2C.h> //LCD display library
#include <MCP342x.h> //ADC library
#include <Adafruit_MCP4725.h> //DAC library

Adafruit_MCP4725 dac; //Initiate DAC

uint32_t potValueAnalog = 0; //actual potentiometer voltage value that will be converted to digital
uint32_t potValueDig = 0;    //digital potentiometer voltage value that will set the input current value
float currentValue = 0.0;    //value of the input current
float voltageValue = 0.0;    //value of the input voltage

uint8_t addressADC = 0x6A; //I2C address of the DAC
MCP342x adc = MCP342x(addressADC);

// set the LCD address to 0x27 for a 20 chars and 3 line display
LiquidCrystal_I2C lcd(0x27,20,3);  

void setup(void){
  Serial.begin(9600);
  pinMode(A0, INPUT); //Read potentiometer value to set current value
  //pinMode(A1, INPUT); //Read arduino voltage for reference value.
  Wire.begin();
  
  // Reset devices
  MCP342x::generalCallReset();
  delay(1); // MC342x needs 300us to settle, wait 1ms
  
  // Check device present
  Wire.requestFrom(addressADC, (uint8_t)1);
  if (!Wire.available()) {
    Serial.print("No device found at address ");
    Serial.println(addressADC, HEX);
    while (1);
  }
   //LCD screen initial setup
  lcd.init(); //Initiate lcd screen
  lcd.clear(); //clear the screen
  lcd.backlight(); // Make sure backlight is on
  
  dac.begin(0x64); //Initiate DAC at I2C address
}

void loop(void){
  potValueAnalog = analogRead(A0); //read analog voltage at potentiometer
 // Serial.println(potValueAnalog);
  potValueDig = (potValueAnalog*4095)/1023; //convert voltage to 10 bit digital value
  dac.setVoltage(potValueDig, false); //set DAC to desired voltage
  
  long digiCurrentvalue = 0; //digital 16 bit current value
  long digiVoltageValue = 0; //digital 16 bit voltage value

  MCP342x::Config status;
  // Initiate a conversion; convertAndRead() will wait until it can be read
  uint8_t err1 = adc.convertAndRead(MCP342x::channel1, MCP342x::oneShot,
           MCP342x::resolution16, MCP342x::gain1,
           1000000, digiCurrentvalue, status);
  uint8_t err2 = adc.convertAndRead(MCP342x::channel2, MCP342x::oneShot,
           MCP342x::resolution16, MCP342x::gain1,
           1000000, digiVoltageValue, status);
  if (err1) {   //data conversion error message 2
    Serial.print("Current Convert error: ");
    Serial.println(err1);
  }
  else if (err2) { //data conversion error message 2
    Serial.print("Voltage Convert error: ");
    Serial.println(err2);
  }
  else { 
    //calculate actual current value
    currentValue = (0.100*digiCurrentvalue)*4.1; 
    currentValue = 5*currentValue/32766;

    //calculate actual voltage value
    voltageValue = (42.5*digiVoltageValue)/(32766); //42.02 is the voltage divider ratio reading the input voltage

    //print to serial monitor 
    Serial.print("I = ");
    Serial.print(currentValue, 3);
    Serial.println(" A");
    delay(1);
    
    // Print a message on both lines of the LCD.
    lcd.setCursor(0,0);   //Set cursor to character 0 on line 0
    lcd.print("Current in = ");
    lcd.print(currentValue, 3);
    delay(1);
    lcd.setCursor(0,1);   //Set cursor to character 0 on line 0
    lcd.print("Voltage in = ");
    lcd.print(voltageValue, 2);
    
  }
  
  delay(5);
}
