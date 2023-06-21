// Welcome to the ... project

//-------------------------LIBRARIES---------------------------

#include <Controllino.h>
#include <SoftwareSerial.h> 
#include <DFRobot_PH.h>
#include <EEPROM.h>
#include <OneWire.h>
#include <DallasTemperature.h>

//--------------------------PINOUT------------------------------

//ORP -> X1
//SoftwareSerial mySerial(53, 20);              

//CO2 -> X1
const byte modbus_serial = 50;                   //RS485

//water level monitors -> X1
/*const int echoPin[5] = {19, 4, 18, 5, 21};
const int trigPin[5] = {2, 6, 3, 7, 51};
*/
const int echoPin_1 = 19;                        //1st water level sensor     
const int trigPin_1 = 2;
const int echoPin_2 = 4;                         //2nd                        
const int trigPin_2 = 6; 
const int echoPin_3 = 18;                        //3rd                        
const int trigPin_3 = 3;
const int echoPin_4 = 5;                         //4th
const int trigPin_4 = 7;
const int echoPin_5 = 21;                        //5th
const int trigPin_5 = 51;                   

//Water temperature -> X1
const byte water_temp_pin = 53;                   

//pH sensor -> X1
const byte pH_pin = A1;

//TDS sensor -> X1
const byte tds_pin = A0;                       

//Alarm -> X2
const byte buzzer_pin = 45;  

//dosing pumps -> X2
SoftwareSerial mySerial1(13, 43);                   //1st dosing pump          
SoftwareSerial mySerial2(11, 9);                    //2nd 
//SoftwareSerial mySerial3(12, 42);                 //3rd 
//SoftwareSerial mySerial4(10, 8);                  //4th 

//Raspberry -> X1
const byte raspi_serial = 1;           

//--------------------------RELAYS-----------------------------

//Fan -> Relay 0
const byte fan_pin = 22;               

//Main pump -> Relay 1
const byte main_pump_pin = 23;          

//--------------------------CONFIG-----------------------------

//Water level indicators
long duration;
int distance;                                  
int av_dist;
int water_lvl;

/*long duration[5] = {1,2,3,4,5};
int distance[5] = {1,2,3,4,5};                                    
int av_dist[5] = {1,2,3,4,5};
int water_lvl[5] = {1,2,3,4,5};*/

//Temperature
float temp_1, temp_2, av_temp;
//OneWire settings
OneWire oneWire(water_temp_pin);
DallasTemperature sensors(&oneWire);
uint8_t sensor1[8] = { 0x28, 0x7F, 0x17, 0xAC, 0x13, 0x19, 0x01, 0x9A };
uint8_t sensor2[8] = { 0x28, 0xFF, 0xC8, 0xC2, 0xC1, 0x16, 0x04, 0xB5 };

//pH sensor
float voltage,phValue,temperature = 25;
DFRobot_PH ph;

//EC sensor
const float a = 0.020;
float av_EC;

//Dosing pumps & ORP
String devicestring1 = "";                         
String devicestring2 = "";                
static unsigned long interval_1 = 20000;              //interval between the 1st pump is on and off   
static unsigned long interval_2 = 15000;              //interval between the 2nd pump is on and off   
unsigned long previous_millis_1 = 0;                  //timer for the 1st pump
unsigned long previous_millis_2 = 0;                  //timer for the 2nd pump

//Raspberry
String from_Pi;
byte alarm_flag;

//--------------------------MODBUS-----------------------------

//vaisala stuff

//--------------------------SETUP------------------------------

void setup() {

  //general
  Serial.begin(19200);   
  sensors.begin();

  //ORP
  //mySerial.begin(19200);    
       
  //water level & pH sensors
  pinMode(trigPin_1, OUTPUT); 
  pinMode(echoPin_1, INPUT);
  ph.begin();
  
  //dosing pumps & ORP
  mySerial1.begin(19200);
  mySerial2.begin(19200);        
  //mySerial3.begin(19200);
  //mySerial4.begin(19200);                    
  devicestring1.reserve(60);  
  devicestring2.reserve(30);
  //devicestring3.reserve(30);  
  //devicestring4.reserve(30);

  //other
  //disable_extra();
}

//---------------------------LOOP------------------------------

void loop(){
  stateMachine();
  }

void stateMachine(){
    static unsigned long start_machine = millis();

    static unsigned long start_idle = 2000;
    static unsigned long start_water_level = 3000;
    static unsigned long start_alarm = 4000;
    //static unsigned long start_water_temp = 5000;
    static unsigned long start_pH = 5000;
    //static unsigned long start_EC = 4000;
    //static unsigned long start_ORP = 4000;
    //static unsigned long start_CO2 = 4000;
    static unsigned long start_dose_pump = 6000;
    //static unsigned long start_main_pump = 4000;
    //static unsigned long start_fan = 4000;

    enum class controllinoState : uint8_t {
          IDLE,
          WATER_LEVEL,
          ALARM,
          //WATER,
          pH,
          //EC,
          //ORP,
          //CO2,
          DOSE_PUMPS,
          //MAIN_PUMP,          
          //FAN,
      };

static controllinoState currentState = controllinoState::IDLE;

switch (currentState) {

    case controllinoState::IDLE:  
      if (millis() - start_machine >= start_idle) {
        displayState("»»———-IDLE state———-««");
        currentState = controllinoState::WATER_LEVEL;
        }
      break;

      case controllinoState::WATER_LEVEL:  
      if (millis() - start_machine >= start_water_level) {
        displayState("»»———-water level state———-««");
        water_level();
        currentState = controllinoState::ALARM;
      }
      break;

      case controllinoState::ALARM:  
      if (millis() - start_machine >= start_alarm) {
        displayState("»»———-emergency state———-««");
        alarm();
        currentState = controllinoState::pH; 
        
      }
      break;

/*      case controllinoState::WATER_TEMP:  
      if (millis() - start_machine >= start_water_temp) {
        displayState("»»———-water t° state———-««");
        water_temp();
        currentState = controllinoState::pH;
        
      }
      break;*/

      case controllinoState::pH:  
      if (millis() - start_machine >= start_pH) {
        displayState("»»———-pH level state———-««");
        pH_sensor();
        currentState = controllinoState::DOSE_PUMPS;
      }
      break;
                  
    //EC state
    //ORP state

    case controllinoState::DOSE_PUMPS:  
      if (millis() - start_machine >= start_dose_pump) {
        displayState("»»———-dosing pump state———-««");
        dose_pump();
        currentState = controllinoState::IDLE;

        start_machine = millis();                        //Reset time line -> always in the last state       
      }
      break;          

    //Main pump state
    //Fan state
    //Raspberry

    default:
      // Nothing to do here
      Serial.println("'Default' Switch Case reached - Error");
      }
}

void displayState(String currentState){
    static String prevState = "";

    if (currentState != prevState) {
        Serial.println(currentState);
        prevState = currentState;
    }
}

//------------------SENSORS & OUTPUT DEVICES-------------------

//----------------------DFRobot SEN0208------------------------

void water_level(){
    float water_level_sum = 0; 
    for (int i=0 ; i<10 ; i++){                         
      digitalWrite(trigPin_1, LOW);                     // Clears the trigPin condition first
      delayMicroseconds(2);
      digitalWrite(trigPin_1, HIGH);                    // Sets the water_level_trig_pin HIGH (ACTIVE) for 10 microseconds (time for 8 cycle sonic bursts)
      delayMicroseconds(10); 
      digitalWrite(trigPin_1, LOW);
      duration = pulseIn(echoPin_1, HIGH);              // Reads the water_level_echo_pin, returns the sound wave travel time in microseconds
      distance = duration * 0.034 / 2;                  // Speed of sound wave divided by 2 (go and back)
      water_level_sum = water_level_sum + distance;
      delay(20);
    }
    av_dist = round(water_level_sum / 10.0);            
    water_lvl = map(av_dist, 2, 24, 0, 100);            //(average distance, start cm, stop cm, start %, stop %)
    Serial.print("  Distance in cm: ");             
    Serial.print(av_dist);
    Serial.print("\n  Distance in %: ");  
    Serial.println(water_lvl);
}

//-----------------------Buzzer---------------------------

void alarm (){
  if ((water_lvl > 120)||(water_lvl < 20)){
    Serial.println("Check the water tanks ASAP!");
  }
  else if((water_lvl <= 5)){
    Serial.println("emergency");
    }
  else{
    Serial.println("  OK");
  }
}

//--------------------DS18B20 One wire--------------------

void water_temp(void){
  sensors.requestTemperatures();
  temp_1 = sensors.getTempC(sensor1);
  Serial.println("  Temp1 (°C): " + (String)temp_1);
  
  temp_2 = sensors.getTempC(sensor2);
  Serial.println("  Temp2 (°C): " + (String)temp_2);

  av_temp = (temp_1 + temp_2)/2;
}

//---------------------DFRobot pH V2.0---------------------

void pH_sensor(){

    static unsigned long timepoint = millis();
    if(millis()-timepoint>1000U){                       //time interval: 1s
        timepoint = millis();
        //temperature = readTemperature();              // read your temperature sensor to execute temperature compensation
        voltage = analogRead(pH_pin)/1024.0*5000;       // read the voltage
        phValue = ph.readPH(voltage,temperature);       // convert voltage to pH with temperature compensation
        //Serial.print("  temperature:");
        //Serial.print(temperature,1);
        Serial.print("  pH: ");
        Serial.println(phValue,2);
    }
    ph.calibration(voltage,temperature);                // calibration process by Serail CMD
}

//------------------------Grove TDS(EC)------------------------

void EC_sensor (void){

      int TDS_raw;
      float voltage_EC;
      float TDS_25;
      float EC_25;
      float EC_sum = 0;
      float EC;
      for (int i=0 ; i<5; i++){
        TDS_raw = analogRead(tds_pin);
        voltage_EC = TDS_raw*5/1024.0;                  
        //voltage to TDS ->
        TDS_25=(133.42*voltage_EC*voltage_EC*voltage_EC - 255.86*voltage_EC*voltage_EC + 857.39*voltage_EC)*0.5;
        EC_25 = TDS_25*2;
        EC = (1 + a*(av_temp - 25))*EC_25;               //temp compensation 
        EC_sum = EC_sum + EC;                            //sum formula for the following average calculation
        delay(10);
      }
      av_EC = EC_sum / 5; // average of 5 samples
      Serial.print("  EC (uS): "); 
      Serial.println(av_EC);
}

//---------------------Atlas_Scientific_ORP--------------------



//-----------------------Vaisala GMP252------------------------



//------------------Atlas Scientific SGL-PMP-BX----------------

void disable_extra(){                                     //disables all extra responses

//mode with low power consumption 
//mySerial1.println("Sleep"); 
//mySerial2.println("Sleep");

mySerial1.println("*OK,0"); 
mySerial1.println("C,0");
mySerial1.println("O,R,V,TV,ATV,0");

mySerial2.println("*OK,0");
mySerial2.println("C,0");
mySerial2.println("O,R,V,TV,ATV,0");

Serial.println("configured!");

}

void dose_pump(){

 unsigned long current_millis = millis();

//SEND COMMAND IN ASCII (STRING) 
if (current_millis - previous_millis_1 >= interval_1){      //pumps up water every n (interval) seconds
  mySerial1.println("status");
  previous_millis_1 = current_millis;
}
if (current_millis - previous_millis_2 >= interval_2){      //pumps up water every n (interval) seconds
  mySerial2.println("status");
  previous_millis_2 = current_millis;
}
//STRING READING
if (mySerial1.available() > 0){       
    String inChar = mySerial1.readStringUntil('\r');  
    devicestring1 += inChar;   
    Serial.println("  Pump 1: " + (String)devicestring1);   //adds text before "devicestring1"
    devicestring1 = "";          
    }
    if (mySerial2.available() > 0) {                          // Data available on software serial port
    String inChar = mySerial2.readStringUntil('\r'); 
    devicestring2 += inChar; 
    Serial.println("  Pump 2: " + (String)devicestring2);   //adds text before "devicestring2"
    devicestring2 = "";    
    }
}
//STRING READING
/*  if (mySerial1.available() > 0) {       
    char inChar = mySerial1.read();      
    devicestring1 += inChar;              
    if (inChar == '\r') {                
      sensor_string_complete_1 = true;      
    }
  }
  if (sensor_string_complete_1) {
    Serial.println("  Pump 1: ");
    Serial.println(devicestring1);        
    devicestring1 = "";                  
    sensor_string_complete_1 = false;      
  }
  if (mySerial2.available() > 0) {       
    char inChar = mySerial2.read();      
    devicestring2 += inChar;              
    if (inChar == '\r') {                
      sensor_string_complete_2 = true;      
    }
  }
  if (sensor_string_complete_2) {
    Serial.println("  Pump 2: ");
    Serial.println(devicestring2);        
    devicestring2 = "";                  
    sensor_string_complete_2 = false;      
  }
}*/




//-------------------------MAIN PUMP---------------------------

//----------------------------FAN------------------------------







/*-------------------REFERENCES--------------------

SGL-PMP-BX (pump): https://files.atlas-scientific.com/Arduino-Uno-PMP-sample-code.pdf

*/