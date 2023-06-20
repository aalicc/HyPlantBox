// Welcome to the ... project

//-------------------LIBRARIES---------------------

#include <Controllino.h>
#include <SoftwareSerial.h> 
#include <DFRobot_PH.h>

//---------------------UART------------------------

//dosing pumps, orp sensors
SoftwareSerial mySerial(13, 43);                 //1st dosing pump
SoftwareSerial mySerial1(11, 9);                 //2nd dosing pump
//SoftwareSerial mySerial(x, x);                 //3rd dosing pump
//SoftwareSerial mySerial1(x, x);                 //4th dosing pump

//water level monitors
const int echoPin_1 = 44;                        //1st water level sensor
const int trigPin_1 = 42;
//const int echoPin_2 = 44;
//const int trigPin_2 = 42; 
//const int echoPin_3 = 44;
//const int trigPin_3 = 42;
//const int echoPin_4 = 44;
//const int trigPin_4 = 42;
//const int echoPin_5 = 44;
//const int trigPin_5 = 42;

//pH sensor


//--------------------MODBUS-----------------------

//vaisala stuff

//--------------------CONFIG-----------------------

//disable extra responses
boolean extra_disabled = false;

//dosing pumps, orp          
String devicestring = "";                         
String devicestring1 = "";                
static unsigned long interval = 30000;           //interval between the 1st pump is on and off   
static unsigned long interval1 = 10000;          //interval between the 2nd pump is on and off   
unsigned long previous_millis = 0;               //timer for the 1st pump
unsigned long previous_millis1 = 0;              //timer for the 2nd pump

//water level meters
long duration;
int distance;                                    
int av_dist;
int water_lvl;

//---------------------SETUP-----------------------

void setup() {

  //general
  Serial.begin(19200);        
                           

  //dosing pumps, ORP devices 
  mySerial.begin(19200);
  mySerial1.begin(19200);                          
  devicestring.reserve(30);  
  devicestring1.reserve(30);
  disable_extra();                              //disables all unwanted responses

  //water level sensors
  pinMode(trigPin_1, OUTPUT); 
  pinMode(echoPin_1, INPUT);

}

//----------------------LOOP-----------------------

void loop(){
  stateMachine();
  }

void stateMachine(){
    static unsigned long start_machine = millis();
    // first 2000 ms are reserved for the last state processing
    static unsigned long start_idle = 2000;
    static unsigned long start_dose_pump = 3000;
    static unsigned long start_water_level = 6000;
    static unsigned long start_alarm = 7000;

    enum class controllinoState : uint8_t {
          IDLE,
          DOSE_PUMPS,
          //MAIN_PUMP,
          WATER_LEVEL,
          ALARM,
          //TEMP,
          //pH,
          //EC,
          //ORP,
      };

static controllinoState currentState = controllinoState::IDLE;

switch (currentState) {

    case controllinoState::IDLE:  
      if (millis() - start_machine >= start_idle) {
        displayState("»»———-IDLE state———-««");
        currentState = controllinoState::DOSE_PUMPS;
        }
      break;

    case controllinoState::DOSE_PUMPS:  
      if (millis() - start_machine >= start_dose_pump) {
        displayState("»»———-dosing pump state———-««");
        dose_pump();
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
        currentState = controllinoState::IDLE;
        
        start_machine = millis();                    //RESET TIME LINE -> ALWAYS IN THE LAST STATE
      }
      break;
                  
            
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

//-----------------SENSORS & DEVICES-------------------

//fan

//


//---------------Ultrasonic Range SENSORS-------------

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

//-----------------------BUZZER-----------------------

void alarm (){
  if ((water_lvl > 120)||(water_lvl < 20)){
    Serial.println("Check the water tanks ASAP!");
  }
}

//--------------Atlas Scientific PRODUCTS-------------

void disable_extra(){                               //disables all extra responses

//mySerial.println("Sleep"); 
//mySerial1.println("Sleep");

mySerial.println("*OK,0"); 
mySerial1.println("*OK,0");

mySerial.println("C,0");
mySerial1.println("C,0");

mySerial.println("O,R,V,TV,ATV,0");
mySerial1.println("O,R,V,TV,ATV,0");

Serial.println("configured");

}

void dose_pump(){

//SEND COMMAND IN ASCII (STRING) 

 unsigned long current_millis = millis();

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

//----------------DFRobot pH V2.0------------------


//-----------------Grove TDS(EC)-------------------


//---------------------FAN------------------------



/*-------------------REFERENCES--------------------


SGL-PMP-BX (pump): https://files.atlas-scientific.com/Arduino-Uno-PMP-sample-code.pdf


*/