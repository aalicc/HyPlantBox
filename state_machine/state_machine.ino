// Welcome to the ... project

//----------------------LIBRARIES------------------------

#include <Controllino.h>
#include <SoftwareSerial.h> 
#include <DFRobot_PH.h>
#include <EEPROM.h>

//------------------------UART---------------------------

//ORP -> X1
SoftwareSerial mySerial4(53, 20);              

//CO2 -> X1
const byte modbus_serial = 50;                   //RS485

//water level monitors -> X1
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

//water temperature -> X1
const byte water_temp_pin = 21;                   

//pH sensor -> X1
const byte pH_pin = A1;

//TDS sensor -> X1
const byte tds_pin = A0;                       

//Alarm -> X2
const byte buzzer_pin = 45;  

//dosing pumps -> X2
SoftwareSerial mySerial(13, 43);                 //1st dosing pump          
SoftwareSerial mySerial1(11, 9);                 //2nd 
SoftwareSerial mySerial2(12, 42);                //3rd 
SoftwareSerial mySerial3(10, 8);                 //4th 

//Raspberry -> X1
const byte raspi_serial = 1;           

//-----------------------RELAYS--------------------------

//Fan -> Relay 0
const byte fan_pin = 22;               

//Main pump -> Relay 1
const byte water_pump_pin = 23;          

//-----------------------CONFIG--------------------------

//water level meters
long duration;
int distance;                                    
int av_dist;
int water_lvl;

//Temperature
float temp_1;
float temp_2;

//pH sensor
float voltage,phValue,temperature = 25;
DFRobot_PH ph;

//EC sensor
const float a = 0.020;
float av_EC;

//dosing pumps & ORP       
String devicestring = "";                         
String devicestring1 = "";                
static unsigned long interval = 10000;           //interval between the 1st pump is on and off   
static unsigned long interval1 = 5000;           //interval between the 2nd pump is on and off   
unsigned long previous_millis = 0;               //timer for the 1st pump
unsigned long previous_millis1 = 0;              //timer for the 2nd pump

//Raspberry
String fromPi;
byte alarm_flag;

//other
boolean extra_disabled = false;

//-----------------------MODBUS--------------------------

//vaisala stuff

//-----------------------SETUP---------------------------

void setup() {

  //general
  Serial.begin(19200);       
       
  //water level & pH sensors
  pinMode(trigPin_1, OUTPUT); 
  pinMode(echoPin_1, INPUT);
  ph.begin();
  
  //dosing pumps & ORP
  mySerial.begin(19200);
  mySerial1.begin(19200);        
  mySerial2.begin(19200);
  mySerial3.begin(19200);                    
  devicestring.reserve(30);  
  devicestring1.reserve(30);
  devicestring2.reserve(30);  
  devicestring3.reserve(30);

  mySerial4.begin(19200);

  //other
  disable_extra();
}

//------------------------LOOP---------------------------

void loop(){
  stateMachine();
  }

void stateMachine(){
    static unsigned long start_machine = millis();

    static unsigned long start_idle = 2000;
    static unsigned long start_dose_pump = 3000;
    static unsigned long start_water_level = 6000;
    static unsigned long start_alarm = 7000;
    static unsigned long start_pH = 8000;

    enum class controllinoState : uint8_t {
          IDLE,
          WATER_LEVEL,
          ALARM,
          //TEMP,
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

    //temperature state

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

        start_machine = millis();                    //RESET TIME LINE -> ALWAYS IN THE LAST STATE        
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

//-------------SENSORS & MECHANICAL DEVICES--------------

//-------------------DFRobot SEN0208---------------------

void water_level(){
    float water_level_sum = 0; 
    for (int i=0 ; i<10 ; i++){                     // 10 samples are taken for accuracy
      digitalWrite(trigPin_1, LOW);                 // Clears the water_level_trig_pin condition first
      delayMicroseconds(2);
      digitalWrite(trigPin_1, HIGH);                // Sets the water_level_trig_pin HIGH (ACTIVE) for 10 microseconds (time for 8 cycle sonic bursts)
      delayMicroseconds(10); 
      digitalWrite(trigPin_1, LOW);
      duration = pulseIn(echoPin_1, HIGH);          // Reads the water_level_echo_pin, returns the sound wave travel time in microseconds
      distance = duration * 0.034 / 2;              // Speed of sound wave divided by 2 (go and back)
      water_level_sum = water_level_sum + distance;
      delay(20);
    }
    av_dist = round(water_level_sum / 10.0);         // one average value of distance in cm
    water_lvl = map(av_dist, 2, 24, 0, 100);        //(average distance, start cm, stop cm, start %, stop %)
    Serial.print("  Distance in cm: ");             
    Serial.print(av_dist);
    Serial.print("\n  Distance in %: ");  
    Serial.println(water_lvl);
}

//--------------------Buzzer-----------------------

void alarm (){
  if ((water_lvl > 120)||(water_lvl < 20)){
    Serial.println("Check the water tanks ASAP!");
  }
}

//----------------DS18B20 One wire-----------------



//----------------DFRobot pH V2.0------------------

void pH_sensor(){

    static unsigned long timepoint = millis();
    if(millis()-timepoint>1000U){                  //time interval: 1s
        timepoint = millis();
        //temperature = readTemperature();         // read your temperature sensor to execute temperature compensation
        voltage = analogRead(pH_pin)/1024.0*5000;  // read the voltage
        phValue = ph.readPH(voltage,temperature);  // convert voltage to pH with temperature compensation
        //Serial.print("  temperature:");
        //Serial.print(temperature,1);
        Serial.print("  pH: ");
        Serial.println(phValue,2);
    }
    ph.calibration(voltage,temperature);           // calibration process by Serail CMD
}

//---------------------Grove TDS(EC)---------------------

//-----------------Atlas_Scientific_ORP------------------

//-------------------Vaisala GMP252----------------------

//-------------Atlas Scientific SGL-PMP-BX---------------

void disable_extra(){                               //disables all extra responses

//mode with low power consumption 
//mySerial.println("Sleep"); 
//mySerial1.println("Sleep");

mySerial.println("*OK,0"); 
mySerial.println("C,0");
mySerial.println("O,R,V,TV,ATV,0");

mySerial1.println("*OK,0");
mySerial1.println("C,0");
mySerial1.println("O,R,V,TV,ATV,0");

Serial.println("configured!");

}

void dose_pump(){

 unsigned long current_millis = millis();

//SEND COMMAND IN ASCII (STRING) 

if (current_millis - previous_millis >= interval){     //pumps up water every n (interval) seconds
  mySerial.println("status");
  previous_millis = current_millis;
}
if (current_millis - previous_millis1 >= interval1){   //pumps up water every n (interval) seconds
  mySerial1.println("status");
  previous_millis1 = current_millis;
}

//STRING READING

if (mySerial.available() > 0){       
    String inChar = mySerial.readStringUntil('\r');  
    devicestring += inChar;   
    Serial.println("  Status: " + (String)devicestring);          //adds text before "devicestring"
    devicestring = "";          
    }
    if (mySerial1.available() > 0) {                              // Data available on software serial port for Device 2
    String inChar = mySerial1.readStringUntil('\r'); 
    devicestring1 += inChar; 
    Serial.println("  Status1: " + (String)devicestring1);         //adds text before "devicestring1"
    devicestring1 = "";    
    }
}

//----------------------MAIN PUMP------------------------

//-------------------------FAN---------------------------







/*-------------------REFERENCES--------------------

SGL-PMP-BX (pump): https://files.atlas-scientific.com/Arduino-Uno-PMP-sample-code.pdf

*/