// Welcome to LeafyAquaHub!

//-------------------------LIBRARIES----------------------------

#include <Controllino.h>
#include <SoftwareSerial.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include "DFRobot_PH.h"
#include <EEPROM.h>


//--------------------------PINOUT------------------------------

//Water level -> X1
const int echo_pin_1 = 3;                   //1st water level sensor
const int trig_pin_1 = 5;
const int echo_pin_2 = 7;                   //2nd
const int trig_pin_2 = 21;
const int echo_pin_3 = 51;                  //3rd
const int trig_pin_3 = 52;
const int echo_pin_4 = 2;                   //4th
const int trig_pin_4 = 4;
const int echo_pin_5 = 6;                   //5th
const int trig_pin_5 = 20;

//Alarm -> X2
const byte buzzer_pin = 45;

//Water t° -> X1
const byte temperature_pin = 53;

//CO2 -> X1
const byte modbus_pin = 50;                 //RS485

//pH sensor -> X1
const byte pH_pin = A1;
float pH_voltage, pH_compensated;
DFRobot_PH ph;

//TDS sensor -> X1                          //mentined later as EC sensor
const byte TDS_pin = A0;

//Dosing pumps -> X2 
SoftwareSerial mySerial1(13, 43);           //1st dosing pump
SoftwareSerial mySerial2(11, 9);            //2nd
//SoftwareSerial mySerial3(12, 42);         //3rd
//SoftwareSerial mySerial4(10, 8);          //4th

//fan
int fan_speed;
//fan control HW-095
int fan_control_IN2_pin = 4;               //clockwise rotation pin
int fan_control_ENA_pin = 2;

//Raspberry Pi -> X1
const byte raspberry_pi_pin = 1;

//--------------------------RELAYS-----------------------------

//Dosing pumps -> Relay 1
const byte dose_pump_12V_pin_1 = 23;
const byte dose_pump_12V_pin_2 = 24;

//Fan -> Relay 0
const byte fan_pin_1 = 22;
//const byte fan_pin_2 = 

//Main pump -> Relay 8
const byte main_pump_pin = 30;

//--------------------------CONFIG-----------------------------

//Water level 
float duration;
float distance;
float distance_average;
int water_level_1;

//Water t°
float temperature_C;
//OneWire settings
OneWire oneWire(temperature_pin);
DallasTemperature sensors(&oneWire);
uint8_t sensor1[8] = { 0x28, 0x77, 0x07, 0x97, 0x94, 0x07, 0x03, 0x0F };
//uint8_t sensor2[8] = { 0x28, 0x83, 0x53, 0x97, 0x94, 0x13, 0x03, 0xFC };

//EC sensor
const float a = 0.020;
float EC_average;

//Raspberry Pi
String from_Pi;
byte alarm_flag;

//--------------------------MODBUS-----------------------------

//vaisala stuff

//--------------------------SETUP------------------------------

void setup() {

  //general
  Serial.begin(19200);
  sensors.begin();

  //water level
  pinMode(trig_pin_1, OUTPUT);
  pinMode(echo_pin_1, INPUT);

  //pH level
  ph.begin();

  //dosing pumps
  pinMode(dose_pump_12V_pin_1, OUTPUT);
  digitalWrite(dose_pump_12V_pin_1, HIGH);
  pinMode(dose_pump_12V_pin_2, OUTPUT);
  digitalWrite(dose_pump_12V_pin_2, HIGH);
  mySerial1.begin(19200);
  mySerial2.begin(19200);
  //mySerial3.begin(19200);
  //mySerial4.begin(19200);

  //fan
  pinMode(fan_control_IN2_pin, OUTPUT);                     
  pinMode(fan_control_ENA_pin, OUTPUT);  
  digitalWrite(fan_control_IN2_pin, LOW);

  //main pump
 // pinMode(main_pump_pin,OUTPUT);
}

//---------------------------LOOP------------------------------

void loop() {
  stateMachine();
}

void stateMachine() {
  static unsigned long start_machine = millis();

  static unsigned long start_idle = 2000;
  static unsigned long start_water_level = 3000;
  static unsigned long start_alarm = 4000;
  static unsigned long start_temp = 5000;
  static unsigned long start_pH = 6000;
  static unsigned long start_EC = 7000;
  //static unsigned long start_CO2 = 4000;
  static unsigned long start_dose_pump_1 = 8000;
  static unsigned long start_dose_pump_2 = 9000;
  static unsigned long start_dose_pump_3 = 10000;
  static unsigned long start_dose_pump_4 = 11000;
  //static unsigned long start_main_pump = 4000;
  static unsigned long start_fan_1 = 12000;
  //static unsigned long start_fan_2 = 13000;

  enum class controllinoState : uint8_t {
    IDLE,
    WATER_LEVEL,
    ALARM,
    WATER_TEMP,
    pH,
    EC,
    //CO2,
    DOSE_PUMP_1,
    DOSE_PUMP_2,
    DOSE_PUMP_3,
    DOSE_PUMP_4,
    //MAIN_PUMP,
    FAN_1,
    //FAN_2;
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
        currentState = controllinoState::WATER_TEMP;
      }
      break;

    case controllinoState::WATER_TEMP:
      if (millis() - start_machine >= start_temp) {
        displayState("»»———-water t° state———-««");
        water_temp();
        currentState = controllinoState::pH;
      }
      break;

    case controllinoState::pH:
      if (millis() - start_machine >= start_pH) {
        displayState("»»———-pH level state———-««");
        pH_level();
        currentState = controllinoState::EC;
      }
      break;

    case controllinoState::EC:
      if (millis() - start_machine >= start_EC) {
        displayState("»»———-EC state———-««");
        EC_level();
        currentState = controllinoState::DOSE_PUMP_1;
      }
      break;

    case controllinoState::DOSE_PUMP_1:
      if (millis() - start_machine >= start_dose_pump_1) {
        displayState("»»———-1st dosing pump state———-««");
        dose_pump_pH_up();
        currentState = controllinoState::DOSE_PUMP_2;
      }
      break;

      case controllinoState::DOSE_PUMP_2:
      if (millis() - start_machine >= start_dose_pump_2) {
        displayState("»»———-2nd dosing pump state———-««");
        dose_pump_EC_up();
        currentState = controllinoState::DOSE_PUMP_3;
      }
      break;

    case controllinoState::DOSE_PUMP_3:
      if (millis() - start_machine >= start_dose_pump_3) {
        displayState("»»———-3rd dosing pump state———-««");
        dose_pump_pH_down();
        currentState = controllinoState::DOSE_PUMP_4;
      }
      break;

      case controllinoState::DOSE_PUMP_4:
      if (millis() - start_machine >= start_dose_pump_4) {
        displayState("»»———-4th dosing pump state———-««");
        dose_pump_EC_down();
        currentState = controllinoState::FAN_1;
      }
      break;

      //Main pump state

      case controllinoState::FAN_1:
      if (millis() - start_machine >= start_fan_1) {
        displayState("»»———-fan state———-««");
        fan();
        currentState = controllinoState::IDLE;

        start_machine = millis();  
      }
      break;

      //Raspberry Pi

    default:
      // Nothing to do here
      Serial.println("ERROR");
  }
}

void displayState(String currentState) {
  static String prevState = "";

  if (currentState != prevState) {
    Serial.println(currentState);
    prevState = currentState;
  }
}

//-------------------------SETTINGS----------------------------

//USER SELECT:
//min & max amount of nutrients in the main tank (checkup)
float pH_lowest = 5;
float pH_highest = 9;
float EC_lowest = 5;
float EC_highest = 100;
//amount of nutrients to add (in milliliters)
float pH_up_dosage = 5;                                     
float pH_down_dosage = 5;
float EC_up_dosage = 5;
float EC_down_dosage =5;
//fan default speed  0% - 100%
int fan_speed_pct = 0; 
//fan speed if CO2 too high
int fan_speed_pct_CO2 = 100;

//------------------SENSORS & OUTPUT DEVICES-------------------

//----------------------DFRobot SEN0208------------------------

void water_level(void) {
  float water_level_sum = 0;
  for (int i = 0; i < 3; i++) {
    digitalWrite(trig_pin_1, LOW);                          // Clears the trigPin condition first
    delayMicroseconds(2);
    digitalWrite(trig_pin_1, HIGH);                         // Sets the water_level_trig_pin HIGH (ACTIVE) for 10 microseconds (time for 8 cycle sonic bursts)
    delayMicroseconds(10);
    digitalWrite(trig_pin_1, LOW);
    duration = pulseIn(echo_pin_1, HIGH);                   // Reads the water_level_echo_pin, returns the sound wave travel time in microseconds
    distance = duration * 0.034 / 2;                        // Speed of sound wave divided by 2 (go and back)
    water_level_sum = water_level_sum + distance;
    delay(20);
  }
  distance_average = round(water_level_sum / 3.0);
  water_level_1 = map(distance_average, 0, 61, 0, 100);      //(average distance, min cm, max cm, min %, max %)
  Serial.println("  Distance in cm: " + (String)distance_average);
  //Serial.println("  Distance in %: " + (String)water_level_1);
}

//-----------------------Buzzer---------------------------

void alarm(void) {
  if ((water_level_1 > 120) || (water_level_1 < 20)) {
    Serial.println("  Check the water tanks ASAP!");
    tone(buzzer_pin, 40);
    } else if ((water_level_1 <= 5)) {
      Serial.println("emergency");
      tone(buzzer_pin, 40);
      delay(200);
      } else {
        Serial.println("  OK");
        noTone(buzzer_pin);
      }
}

//--------------------DS18B20 One wire--------------------

void water_temp(void) {
  sensors.requestTemperatures();
  temperature_C = sensors.getTempC(sensor1);
  Serial.println("  Temp1 (°C): " + (String)temperature_C);
}

//--------------------DFRobot pH V2.0----------------------

void pH_level(void) {
  pH_voltage = analogRead(pH_pin)/1024.0*5000;            // read the voltage
  pH_compensated = ph.readPH(pH_voltage,temperature_C);   // convert voltage to pH with temperature compensation
  //Serial.print("temperature:");
  //Serial.print(temperature_C,1);
  Serial.print("  pH: ");
  Serial.println(pH_compensated,2);             
}

//-----------------------Grove TDS------------------------

void EC_level(void) {
  int TDS_raw;
  float EC_voltage = 0, TDS_25 = 0, EC_25, EC_sum = 0, EC;
  for (int i = 0; i < 5; i++) {
    TDS_raw = analogRead(TDS_pin);
    EC_voltage = TDS_raw * 5 / 1024.0;
    //voltage to TDS ->
    TDS_25 = (133.42 * EC_voltage * EC_voltage * EC_voltage - 255.86 * EC_voltage * EC_voltage + 857.39 * EC_voltage) * 0.5;
    //TDS to EC ->
    EC_25 = TDS_25 * 2;
    EC = (1 + a * (temperature_C - 25)) * EC_25;           //t° compensation
    EC_sum = EC_sum + EC;                                  //sum for the following average calculation
  }
  EC_average = EC_sum / 5;                                 
  Serial.println("  EC (mS/m): " + (String)EC_average);
}

//-----------------------Vaisala GMP252------------------------



//------------------Atlas Scientific SGL-PMP-BX----------------

void dose_pump_pH_up(void) {                               //pump 1
if (pH_compensated < pH_lowest){
  //SEND COMMAND IN ASCII (STRING)
  mySerial1.println("d,"+ (String)pH_up_dosage);           //dispose x milliliters -> D,X 
  Serial.println("  Pump 1 is raising the pH...");
  }
  else{
    mySerial1.println("d,Sleep");                          //enter low power mode
    Serial.println("  Pump 1 is sleeping...");
  }
}
void dose_pump_EC_up(void) {                               //pump 2
if (EC_average < EC_lowest){
  //SEND COMMAND IN ASCII (STRING)
    mySerial2.println("d," + (String)EC_up_dosage);
    Serial.println("  Pump 2 is raising the EC...");
  }
  else{
    mySerial2.println("d,Sleep");                          //enter low power mode
    Serial.println("  Pump 2 is sleeping...");
  }
}
void dose_pump_pH_down(void) {                           //pump 3
if (EC_average < pH_highest){
  //SEND COMMAND IN ASCII (STRING)
    //mySerial3.println("d," + (String)pH_down_dosage);
    Serial.println("  Pump 3 is lowering the pH...");
  }
    else{
    //mySerial3.println("d,Sleep");                          //enter low power mode
    Serial.println("  Pump 3 is sleeping...");
  }
}
void dose_pump_EC_down(void) {                             //pump 4
if (EC_average > EC_highest){
  //SEND COMMAND IN ASCII (STRING)
    //mySerial4.println("d," + (String)EC_down_dosage);
    Serial.println("  Pump 4 is lowering the EC...");
  }
    else{
    //mySerial4.println("d,Sleep");                          //enter low power mode
    Serial.println("  Pump 4 is sleeping...");
  }
}

//----------------------MAIN WATER PUMP------------------------

/*digitalWrite(main_pump_pin,HIGH);

*/

//---------------------------FANS------------------------------

void fan() {
  fan_speed = map(fan_speed_pct, 0, 100, 0, 255);               //convert % to the actual speed value

  if (fan_speed_pct > 0 && fan_speed_pct < 100){
    digitalWrite(fan_control_IN2_pin, HIGH); 
    analogWrite(fan_control_ENA_pin, fan_speed );
    Serial.println("  Fan speed set on " + (String)fan_speed_pct);
  }
  else if (fan_speed_pct <= 0){
    digitalWrite(fan_control_IN2_pin, LOW); 
    Serial.println("  Fan OFF");
  }
  else{                                                    
    fan_speed = 255;
    digitalWrite(fan_control_IN2_pin, HIGH); 
    analogWrite(fan_control_ENA_pin, fan_speed );
    Serial.println("Fan's limit is 100%");
  }

//CO2 CONTROL
/*  if (CO2_level < 800){                                        //if CO2 value too low
    fan_speed = map(fan_speed_pct_CO2, 0, 100, 0, 255);     
    digitalWrite(fan_control_IN2_pin, HIGH); 
    analogWrite(fan_control_ENA_pin, fan_speed );
  }*/

}






/*------------------------REFERENCES---------------------------

SGL-PMP-BX (pump): https://files.atlas-scientific.com/Arduino-Uno-PMP-sample-code.pdf

*/