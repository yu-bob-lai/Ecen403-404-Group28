#include <LiquidCrystal_I2C.h>

#include <LiquidCrystal.h>

#include <Wire.h>
//#include <LiquidCrystal_I2C.h> //LCD display library

#include <MCP342x.h> //ADC library
#include <Adafruit_MCP4725.h> //DAC library
#include <SoftwareSerial.h> //Bluetooth library

#define TxD 2
#define RxD 3
SoftwareSerial mySerial(RxD, TxD); //RX, TX for Bluetooth

Adafruit_MCP4725 dac; //Initiate DAC

uint32_t potValueAnalog = 0; //actual potentiometer voltage value that will be converted to digital
uint32_t potValueDig = 0;    //digital potentiometer voltage value that will set the input current value
float currentValue = 0.0;    //value of the input current
float voltageValue = 0.0;    //value of the input voltage
float powerValue = 0.0;      //value of the input power

uint8_t addressADC = 0x6A; //I2C address of the DAC
MCP342x adc = MCP342x(addressADC);

// set the LCD address to 0x27 for a 20 chars and 3 line display
LiquidCrystal_I2C lcd(0x27,20,3);  

void setup(void){
  Serial.begin(9600);
  mySerial.begin(9600); //For Bluetooth
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
  //possibly uncomment lcd initiate later after testing
   //LCD screen initial setup
  lcd.init(); //Initiate lcd screen
  lcd.clear(); //clear the screen
  lcd.backlight(); // Make sure backlight is on
  
  dac.begin(0x64); //Initiate DAC at I2C address
}

void loop(void){
  int mode; //**0 read from pot|| **1 read from application
  int state;
  
  
  /*if(mySerial.available()){
    state = mySerial.read();
    if(state%10 != 1){
      state = 0;
    }
  }
  if (state%10 == 0){
  potValueAnalog = analogRead(A0); //read analog voltage at potentiometer
 // Serial.println(potValueAnalog);
  potValueDig = (potValueAnalog*4095)/1023; //convert voltage to 10 bit digital value?
  }
  else if(state%10 == 1){ 
    do{
          if(mySerial.available()){
            float temp = mySerial.read();
            if(state/10 == 33)
            {
              potValueDig = temp;
            }else if(state/10 == 34)
            {
              
            }
          potValueDig = mySerial.read();
          }
      }while(mySerial.read() != 150);
  }*/
  if(mySerial.available()){
    mode = mySerial.read();
  }
   //210  pot on
    while (mode == 0){
        if(mySerial.available()){
          state = mySerial.read();
        }
      
        if (state > 200){
          if(state==210){ // pot off
            mode = 1;
          }
          else if (state == 211){
          state = 30;
          mode = 1;
          }
          else if (state == 212){
          state = 31;
          mode = 1;
          }
          else if (state == 213){
          state = 32;
          mode = 1;
          }
          }
    
        
         
          potValueAnalog = analogRead(A0); //read analog voltage at potentiometer
 // Serial.println(potValueAnalog);
  potValueDig = (potValueAnalog*4095)/1023; //convert voltage to 10 bit digital value?
  dac.setVoltage(potValueDig, false); //set DAC to desired voltage
   Serial.print("Pot Analog: ");
          Serial.println(potValueAnalog);
          
  long digiCurrentvalue = 0; //digital 16 bit current value
  long digiVoltageValue = 0; //digital 16 bit voltage value

  MCP342x::Config status;
  // Initiate a conversion; convertAndRead() will wait until it can be read
  uint8_t err1 = adc.convertAndRead(MCP342x::channel2, MCP342x::oneShot,
           MCP342x::resolution16, MCP342x::gain1,
           1000000, digiCurrentvalue, status);
  uint8_t err2 = adc.convertAndRead(MCP342x::channel1, MCP342x::oneShot,
           MCP342x::resolution16, MCP342x::gain1,
           1000000, digiVoltageValue, status);
  if (err1) {   //data conversion error message 2
    Serial.print("Current Convert error: ");
    Serial.println(err1);
    mySerial.print("Current Convert error: ");
    mySerial.println(err1);
  }
  else if (err2) { //data conversion error message 2
    Serial.print("Voltage Convert error: ");
    Serial.println(err2);
    mySerial.print("Voltage Convert error: ");
    mySerial.println(err2);
  }
  else { 
    //calculate actual current value
    currentValue = (0.100*digiCurrentvalue)*4.1; 
    currentValue = 5*currentValue/32766;

    //calculate actual voltage value
    voltageValue = (42.5*digiVoltageValue)/(32766); //42.02 is the voltage divider ratio reading the input voltage
    powerValue = currentValue * voltageValue;
    String reading = String(currentValue, 3);
    //print to serial monitor 
    //Serial.print("I = ");
    //Serial.print(currentValue, 3);
    //Serial.println(" A");
    if (mySerial.available()){
    state = mySerial.read();
    lcd.println(state);
    } 
    //state: 30=Read current|| 31=Read voltage|| 32=Read power|| 33=set current|| 34=set power
    if(state == 30){
    mySerial.println(String(currentValue, 3) + " A");
    delay(1000);
    }
    else if(state == 31){
    mySerial.println(String(voltageValue, 3) + " V");
    delay(1000);
    }
    else if(state == 32){
    mySerial.println(String(powerValue, 3) + " W");
    delay(1000);
    }
    Serial.print("Current: ");
    Serial.println(currentValue);
    
    // Print a message on both lines of the LCD.
    lcd.setCursor(0,0);   //Set cursor to character 0 on line 0
    lcd.print("Current in = ");
    lcd.print(currentValue, 3);
    lcd.print(" A");
    delay(1);
    lcd.setCursor(0,1);   //Set cursor to character 0 on line 0
    lcd.print("Voltage in = ");
    lcd.print(voltageValue, 3);
    lcd.print(" V");
    lcd.setCursor(0,2);   //Set cursor to character 0 on line 0
    lcd.print("Power   in = ");
    lcd.print(powerValue, 3);
    lcd.print(" W");
  }
  
  delay(5);
        
    }
  
while (mode == 1){
  String byteOne = "";
  String byteTwo = "";
  String byteFinal = "";
  int x = 0;
  int y;
  int sum = 0;
  if(state != 224){
        if(mySerial.available()){
          state = mySerial.read();
        }
  }
        
         
          if (state > 200){
          if(state==220){ // pot off
            mode = 0;
          }
          else if (state == 221){
          state = 30;
          mode = 0;
          }
          else if (state == 222){
          state = 31;
          mode = 0;
          }
          else if (state == 223){
          state = 32;
          mode = 0;
          }
          }
               
          Serial.print("State: ");
          Serial.println(state);
          while(state == 224 || state == 225){
            if(mySerial.available()){
              x = mySerial.read();
              if(x == 1){
                state = 30;
                break;
                
              }
              
              else{
             
          for(int i = 0;i<8; i++){
                byteOne = byteOne + String(bitRead(x,7-i));
                }
                Serial.print("X: ");
          Serial.println(x);
           Serial.print("Byte One: ");
          Serial.println(byteOne);
                }
                if(x != 0){
                  if(mySerial.available()){
                  x = mySerial.read();
                  
                  
                  for(int i = 0; i < 8; i++){
                byteTwo = byteTwo + String(bitRead(x,7-i));
                  }
                }
                  }
                  byteFinal = byteTwo + byteOne;
                  Serial.print("Final Byte: ");
                  char s[16];
                  byteFinal.toCharArray(s, 16);

              Serial.println(byteFinal);
              for (int i = 0; i < 16; i++){
               
                if(s[15-i] == '1'){
                 
                 
                      sum = sum + pow(2,i);
                }
                }
                Serial.print("Sum: ");
              Serial.println(sum);
              potValueAnalog = sum;
                potValueDig = (potValueAnalog*4095)/1023; //convert voltage to 10 bit digital value?
              dac.setVoltage(potValueDig, false); //set DAC to desired voltage
            
              Serial.print("Last Val: ");
              Serial.println(potValueAnalog);
              Serial.print("State: ");
              Serial.println(state);
              if(state == 224){
              y = 30;
              }
              else{
                y = 32;
                }
              break;
              }
             
            
          }
       
           
          
        
  long digiCurrentvalue = 0; //digital 16 bit current value
  long digiVoltageValue = 0; //digital 16 bit voltage value

  MCP342x::Config status;
  // Initiate a conversion; convertAndRead() will wait until it can be read
  uint8_t err1 = adc.convertAndRead(MCP342x::channel2, MCP342x::oneShot,
           MCP342x::resolution16, MCP342x::gain1,
           1000000, digiCurrentvalue, status);
  uint8_t err2 = adc.convertAndRead(MCP342x::channel1, MCP342x::oneShot,
           MCP342x::resolution16, MCP342x::gain1,
           1000000, digiVoltageValue, status);
  if (err1) {   //data conversion error message 2
    Serial.print("Current Convert error: ");
    Serial.println(err1);
    mySerial.print("Current Convert error: ");
    mySerial.println(err1);
  }
  else if (err2) { //data conversion error message 2
    Serial.print("Voltage Convert error: ");
    Serial.println(err2);
    mySerial.print("Voltage Convert error: ");
    mySerial.println(err2);
  }
  else { 
    //calculate actual current value
    currentValue = (0.100*digiCurrentvalue)*4.1; 
    currentValue = 5*currentValue/32766;

    //calculate actual voltage value
    voltageValue = (42.5*digiVoltageValue)/(32766); //42.02 is the voltage divider ratio reading the input voltage
    powerValue = currentValue * voltageValue;
    String reading = String(currentValue, 3);
    //print to serial monitor 
   // Serial.print("I = ");
   // Serial.print(currentValue, 3);
   // Serial.println(" A");
   
    //state: 30=Read current|| 31=Read voltage|| 32=Read power|| 33=set current|| 34=set power
    if(state == 30 || y == 30){
    mySerial.println(String(currentValue, 3) + " A (App)");
    delay(1000);
    }
    else if(state == 31){
    mySerial.println(String(voltageValue, 3) + " V (App)");
    delay(1000);
    }
    else if(state == 32 || y == 32){
    mySerial.println(String(powerValue, 3) + " W (App)");
    delay(1000);
    }
    y = 0;
     Serial.print("Current: ");
          Serial.println(currentValue);
          
    
    // Print a message on both lines of the LCD.
    lcd.setCursor(0,0);   //Set cursor to character 0 on line 0
    lcd.print("Current in = ");
    lcd.print(currentValue, 3);
    lcd.print(" A");
    delay(1);
    lcd.setCursor(0,1);   //Set cursor to character 0 on line 0
    lcd.print("Voltage in = ");
    lcd.print(voltageValue, 3);
    lcd.print(" V");
    lcd.setCursor(0,2);   //Set cursor to character 0 on line 0
    lcd.print("Power   in = ");
    lcd.print(powerValue, 3);
    lcd.print(" W");
  }

  
  
  delay(5);
          
    }
    
  /*potValueAnalog = analogRead(A0); //read analog voltage at potentiometer
 // Serial.println(potValueAnalog);
  potValueDig = (potValueAnalog*4095)/1023; //convert voltage to 10 bit digital value?
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
    mySerial.print("Current Convert error: ");
    mySerial.println(err1);
  }
  else if (err2) { //data conversion error message 2
    Serial.print("Voltage Convert error: ");
    Serial.println(err2);
    mySerial.print("Voltage Convert error: ");
    mySerial.println(err2);
  }
  else { 
    //calculate actual current value
    currentValue = (0.100*digiCurrentvalue)*4.1; 
    currentValue = 5*currentValue/32766;
    //calculate actual voltage value
    voltageValue = (42.5*digiVoltageValue)/(32766); //42.02 is the voltage divider ratio reading the input voltage
    powerValue = currentValue * voltageValue;
    String reading = String(currentValue, 3);
    //print to serial monitor 
    Serial.print("I = ");
    Serial.print(currentValue, 3);
    Serial.println(" A");
    if (mySerial.available()){
    state = mySerial.read();
    lcd.println(state);
    }
    //state: 30=Read current|| 31=Read voltage|| 32=Read power|| 33=set current|| 34=set power
    if(state/10 == 30){
    mySerial.println(String(currentValue, 3));
    delay(1000);
    }
    else if(state/10 == 31){
    mySerial.println(String(voltageValue, 3));
    delay(1000);
    }
    else if(state/10 == 32){
    mySerial.println(String(powerValue, 3));
    delay(1000);
    }
    
    
    // Print a message on both lines of the LCD.
    lcd.setCursor(0,0);   //Set cursor to character 0 on line 0
    lcd.print("Current in = ");
    lcd.print(currentValue, 3);
    lcd.print(" A");
    delay(1);
    lcd.setCursor(0,1);   //Set cursor to character 0 on line 0
    lcd.print("Voltage in = ");
    lcd.print(voltageValue, 3);
    lcd.print(" V");
    lcd.setCursor(0,2);   //Set cursor to character 0 on line 0
    lcd.print("Power   in = ");
    lcd.print(powerValue, 3);
    lcd.print(" W");
  }
  
  delay(5);*/
}
