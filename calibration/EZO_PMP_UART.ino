//INSTRUCTIONS FOR CALIBRATION AND OTHER COMMANDS: https://files.atlas-scientific.com/EZO_PMP_Datasheet.pdf
#include <Controllino.h>
#include <SoftwareSerial.h>

//change the pin names according to the dosing pump you're using (rx, tx). Program one pump at a time. 
SoftwareSerial mySerial(13, 43);                 //1st
//SoftwareSerial mySerial(11, 9);                //2nd
//SoftwareSerial mySerial(12, 42);               //3rd
//SoftwareSerial mySerial(10, 8);                //4th

String inputstring = "";            
String devicestring = "";           
boolean device_string_complete = false;   
boolean sensor_string_complete = false;   

void setup() { 
  Serial.begin(19200);          
  mySerial.begin(19200);        
  
  inputstring.reserve(10);     
  devicestring.reserve(30);    

}

void loop() {
  if (Serial.available() > 0) {     
    char inChar = Serial.read();    
    inputstring += inChar;          
    if (inChar == '\r') {           
      device_string_complete = true;   
    }
  }
  
  if (mySerial.available() > 0) {       
    char inChar = mySerial.read();      
    devicestring += inChar;              
    if (inChar == '\r') {                
      sensor_string_complete = true;      
    }
  }

  if (device_string_complete) {
    mySerial.print(inputstring);        
    mySerial.print('\r');               
    inputstring = "";                   
    device_string_complete = false;     
  }
  
  if (sensor_string_complete) {
    Serial.println(devicestring);        
    devicestring = "";                  
    sensor_string_complete = false;      
  }
}