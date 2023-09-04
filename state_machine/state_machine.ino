// Welcome to HyPlantBox! project!

//-------------------------LIBRARIES----------------------------

#include <Controllino.h>
#include <SPI.h>
#include <SoftwareSerial.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include "DFRobot_PH.h"
#include <EEPROM.h>
#include <string.h>
#include <stdlib.h>

//--------------------------PINOUT------------------------------

//Water level -> X1
const int echo_pin[] = {3, 7, 51, 2, 6};
const int trig_pin[] = {5, 21, 52, 4, 20};

//Alarm -> X2
const byte buzzer_pin = 45;

//Water t° -> X1
const byte temperature_pin = 53;

//pH sensor -> X1
const byte pH_pin = A1;
float pH_voltage, pH;
DFRobot_PH ph;

//TDS sensor -> X1                                  //mentined later as EC sensor
const byte TDS_pin = A0;

//Dosing pumps -> X2 
  SoftwareSerial mySerial1(13, 43);                 //(rx, tx) = (SDA, SCL)
  SoftwareSerial mySerial2(11, 9);                  
  SoftwareSerial mySerial3(12, 42);               
  SoftwareSerial mySerial4(10, 8);                

//fan
int fan_speed;
//fan controller MDD3A
int fan_control_M1A_pin = 45;
int fan_control_M2A_pin = 44;

//raspberry Pi -> X1
#define mySerial Serial1

//--------------------------RELAYS-----------------------------

//Dosing pumps -> Relay Block B
const byte dose_pump_12V_pin_1 = 23;
const byte dose_pump_12V_pin_2 = 24;
const byte dose_pump_12V_pin_3 = 25;
const byte dose_pump_12V_pin_4 = 26;

//Main pump -> Relay Block B
const byte main_pump_pin = 22;

//--------------------------CONFIG-----------------------------

//Water level 
float duration[5];
float distance_1, distance_2, distance_3, distance_4, distance_5;
int water_level_1, water_level_2, water_level_3, water_level_4, water_level_5;

//Water t°
float temperature_C;
//OneWire settings
OneWire oneWire(temperature_pin);
DallasTemperature sensors(&oneWire);
uint8_t sensor1[8] = {0x28, 0x12, 0xF2, 0x20, 0x0F, 0x00, 0x00, 0x21};

//TDS sensor
const float a = 0.020;
float TDS;                

//dosing pumps
//amount of nutrients to add (in milliliters)
float pH_up_dosage = 5;                                     
float pH_down_dosage = 5;
float TDS_up_dosage = 5;
float TDS_down_dosage =5;

//main pump cycle
unsigned long previous_millis = 0;                   //timer settings
bool pump_status = false;                            //current condition of the pump (false == LOW, true == HIGH)

//raspberry Pi
float get_control[10];                               //data from the control panel
float get_ruuvi[5];                                  //data from Ruuvi tags
float humidity_1 = 0, humidity_2 = 0;
float temperature_air_1 = 0, temperature_air_2 = 0;

//--------------------------SETUP------------------------------

void setup() {
  //general
  Serial.begin(19200);
  sensors.begin();

  //water level
    for (int i = 0; i < 5 ; i++) {                   //declare all pins using for() function
    pinMode(trig_pin[i], OUTPUT);
    pinMode(echo_pin[i], INPUT);}

  //pH level
  ph.begin();

  //dosing pumps
  pinMode(dose_pump_12V_pin_1, OUTPUT);
  digitalWrite(dose_pump_12V_pin_1, HIGH);
  pinMode(dose_pump_12V_pin_2, OUTPUT);
  digitalWrite(dose_pump_12V_pin_2, HIGH);
  pinMode(dose_pump_12V_pin_3, OUTPUT);
  digitalWrite(dose_pump_12V_pin_3, HIGH);
  pinMode(dose_pump_12V_pin_4, OUTPUT);
  digitalWrite(dose_pump_12V_pin_4, HIGH);

  mySerial1.begin(19200);
  mySerial2.begin(19200);
  mySerial3.begin(19200);
  mySerial4.begin(19200);

  //fan
  pinMode(fan_control_M1A_pin, OUTPUT);  
  digitalWrite(fan_control_M1A_pin, LOW);
  pinMode(fan_control_M2A_pin, OUTPUT);  
  digitalWrite(fan_control_M2A_pin, LOW);

  //main pump
  pinMode(main_pump_pin, OUTPUT);
  digitalWrite(main_pump_pin, LOW);

  //raspberry Pi
  mySerial.begin(19200);
}

//---------------------------LOOP------------------------------

void loop() {
  stateMachine();
}

void stateMachine() {
  static unsigned long start_state_machine = millis();

  static unsigned long start_idle = 1000;
  static unsigned long start_water_level = 1500;
  static unsigned long start_temp = 2000;
  static unsigned long start_pH = 2500;
  static unsigned long start_TDS = 3000;
  static unsigned long start_dose_pump_1 = 3500;
  static unsigned long start_dose_pump_2 = 4000;
  static unsigned long start_dose_pump_3 = 4500;
  static unsigned long start_dose_pump_4 = 5000;
  static unsigned long start_main_pump = 5500;
  static unsigned long start_fans = 5750;
  static unsigned long start_raspberry = 6000;

  enum class controllinoState : uint8_t {
    IDLE,
    WATER_LEVEL,
    WATER_TEMP,
    pH,
    TDS,
    DOSE_PUMP_1,
    DOSE_PUMP_2,
    DOSE_PUMP_3,
    DOSE_PUMP_4,
    MAIN_PUMP,
    FANS,
    RASPBERRY_PI,
  };

  static controllinoState currentState = controllinoState::IDLE;

  switch (currentState) {

    case controllinoState::IDLE:
      if (millis() - start_state_machine >= start_idle) {
        displayState("»»———-IDLE state———-««");
        currentState = controllinoState::WATER_LEVEL;
      }
      break;

    case controllinoState::WATER_LEVEL:
      if (millis() - start_state_machine >= start_water_level) {
        displayState("»»———-water level state———-««");
        water_level();
        currentState = controllinoState::WATER_TEMP;
      }
      break;

    case controllinoState::WATER_TEMP:
      if (millis() - start_state_machine >= start_temp) {
        displayState("»»———-water t° state———-««");
        water_temp();
        currentState = controllinoState::pH;
      }
      break;

    case controllinoState::pH:
      if (millis() - start_state_machine >= start_pH) {
        displayState("»»———-pH level state———-««");
        pH_level();
        currentState = controllinoState::TDS;
      }
      break;

    case controllinoState::TDS:
      if (millis() - start_state_machine >= start_TDS) {
        displayState("»»———-TDS state———-««");
        TDS_level();
        currentState = controllinoState::DOSE_PUMP_1;
      }
      break;

    case controllinoState::DOSE_PUMP_1:
      if (millis() - start_state_machine >= start_dose_pump_1) {
        displayState("»»———-1st dosing pump state———-««");
        dose_pump_pH_up();
        currentState = controllinoState::DOSE_PUMP_2;
      }
      break;

      case controllinoState::DOSE_PUMP_2:
      if (millis() - start_state_machine >= start_dose_pump_2) {
        displayState("»»———-2nd dosing pump state———-««");
        dose_pump_TDS_up();
        currentState = controllinoState::DOSE_PUMP_3;
      }
      break;

    case controllinoState::DOSE_PUMP_3:
      if (millis() - start_state_machine >= start_dose_pump_3) {
        displayState("»»———-3rd dosing pump state———-««");
        dose_pump_pH_down();
        currentState = controllinoState::DOSE_PUMP_4;
      }
      break;

      case controllinoState::DOSE_PUMP_4:
      if (millis() - start_state_machine >= start_dose_pump_4) {
        displayState("»»———-4th dosing pump state———-««");
        dose_pump_TDS_down();
        currentState = controllinoState::MAIN_PUMP;
      }
      break;

      case controllinoState::MAIN_PUMP:
      if (millis() - start_state_machine >= start_main_pump) {
        displayState("»»———-main pump state———-««");
        main_pump();
        currentState = controllinoState::FANS;
      }
      break;

      case controllinoState::FANS:
      if (millis() - start_state_machine >= start_fans) {
        displayState("»»———-fans state———-««");
        fans();
        currentState = controllinoState::RASPBERRY_PI;
      }
      break;

      case controllinoState::RASPBERRY_PI:
      if (millis() - start_state_machine >= start_raspberry) {
        displayState("»»———-Raspberry state———-««");
        Pi_send();
        Pi_receive();
        currentState = controllinoState::IDLE;

        start_state_machine = millis();  
      }
      break;

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

//-------------------------RASPBERRY PI----------------------------

//USER'S SELECTION ON THE CONTROL PANEL:
//min & max amount of nutrients in the main tank (checkup)
float pH_lowest = 0;
float pH_highest = 0;
float TDS_lowest = 0;
float TDS_highest = 0;
//fan default speed  0% - 100%
int fan_speed_pct = 0; 
int humidity_highest = 0;
float temperature_highest = 0;
//main pump working cycle in minutes
float main_pump_on_min = 0;
float main_pump_off_min = 0;  

//incoming data from Raspberry Pi is in the String format
String fan_speed_pct_pi, pH_lowest_pi, pH_highest_pi, TDS_lowest_pi, TDS_highest_pi, humidity_highest_pi, main_pump_on_min_pi, main_pump_off_min_pi, humidity_1_ruuvi, humidity_2_ruuvi, temperature_1_ruuvi, temperature_2_ruuvi;

void Pi_send(){
//upcoming data from CONTROLLINO
String sensor_data = (String)water_level_1 + "&" + (String)water_level_2 + "&" +(String)water_level_3 + "&" + (String)water_level_4 + "&" + (String)water_level_5 + "&" + (String)temperature_C + "&" + (String)pH + "&" + (String)TDS + "&" + (String)fan_speed_pct + "&" + (String)pump_status;
Serial1.println(sensor_data);
}

void Pi_receive(){
char ruuvi_chars[30];                                   //ruuvi values in char format
char control_chars[60];                                 //values from the control panel in char format
char *delim = ",";
float token_float;

  if (Serial1.available()){
      String pi_string = Serial1.readString();          //read data from Pi as a single string
      if (pi_string.indexOf("s") != -1){                //continue if first character in string is "s"
          pi_string.toCharArray(control_chars, 30);     //previously received string to char array
          char *token = strtok(control_chars,delim);    //splits char array to separate values for later use
            for (int k = 1; k < 10; k ++){              //creating an array of 10 elements considering delimeters
              if (token != NULL){
                token = strtok (NULL,delim);
                token_float = atof(token);              //char to float
            }
            get_control[k] = token_float;
            Serial.println(get_control[k]);        
        }
      }
        else {                     
        //same functionality as before, but the start letter is different
        pi_string.toCharArray(ruuvi_chars, 30);        
        char *token = strtok(ruuvi_chars, delim);       

        for (int i = 1; i < 5; i++){                    
          if (token != NULL){
            token = strtok (NULL,delim);
            token_float = atof(token);
          }
          get_ruuvi[i] = token_float;
          Serial.println(get_ruuvi[i]);
        }
    }
  }

//convert received data to the right format
//settings from the control panel
fan_speed_pct = get_control[1];
pH_lowest = get_control[2];
pH_highest = get_control[3];
TDS_lowest = get_control[4];
TDS_highest = get_control[5];
main_pump_on_min = get_control[6];
main_pump_off_min = get_control[7];
humidity_highest = get_control[8];
temperature_highest =  get_control[9];

//data from Ruuvi
temperature_air_1 = get_ruuvi[1];
humidity_1 = get_ruuvi[2]; 
temperature_air_2 = get_ruuvi[3];
humidity_2 = get_ruuvi[4];
}

//------------------SENSORS & OUTPUT DEVICES-------------------

//--------------------------HC-SR04----------------------------

void water_level(void) {
  float water_level_sum_1 = 0, water_level_sum_2 = 0;
  for (int i = 0; i < 5 ; i++) {                          //"i" = number of ultrasonic sensor used
    digitalWrite(trig_pin[i], LOW);
    delayMicroseconds(2);
    digitalWrite(trig_pin[i], HIGH);
    delayMicroseconds(10);
    digitalWrite(trig_pin[i], LOW);   
    duration[i] = pulseIn(echo_pin[i], HIGH);
    delay(20);
}
distance_1 = round(duration[0] * 0.034 / 2);              //common formula for distance calculation
distance_2 = round(duration[1] * 0.034 / 2); 
distance_3 = round(duration[2] * 0.034 / 2);            
distance_4 = round(duration[3] * 0.034 / 2); 
distance_5 = round(duration[4] * 0.034 / 2); 

//map(real distance, min distance, max distance, min distance in %, max distance in %)
water_level_1 = map(distance_1, 0, 34, 100, 0);               
water_level_2 = map(distance_2, 0, 34, 100, 0);      
water_level_3 = map(distance_3, 0, 28, 100, 0);               
water_level_4 = map(distance_4, 0, 34, 100, 0);       
water_level_5 = map(distance_5, 0, 34, 100, 0);    
//TEST
//distance in cm 
/*Serial.println("  Distance 1 in cm: " + (String)distance_1);
Serial.println("  Distance 1 in cm: " + (String)distance_2); 
//distance in %
Serial.println("  Distance in %: " + (String)water_level_1);
Serial.println("  Distance in %: " + (String)water_level_2);
Serial.println("  Distance in %: " + (String)water_level_3);
Serial.println("  Distance in %: " + (String)water_level_4);
Serial.println("  Distance in %: " + (String)water_level_5);*/
}

//--------------------DS18B20 One wire--------------------

void water_temp(void) {
  sensors.requestTemperatures();
  temperature_C = sensors.getTempC(sensor1);
  Serial.println("  Temp1 (°C): " + (String)temperature_C);
}

//--------------------DFRobot pH V2.0----------------------

void pH_level(void) {
  pH_voltage = analogRead(pH_pin)/1024.0*5000;              // read the voltage
  pH = ph.readPH(pH_voltage, temperature_C);                // convert voltage to pH with temperature compensation
  //Serial.print("temperature:");
  //Serial.print(temperature_C,1);
  Serial.print("  pH: ");
  Serial.println(pH,2);             
}

//-----------------------Grove TDS------------------------

void TDS_level(void) {
  float TDS_raw, TDS_volt = 0, TDS_raw_sum = 0, TDS_raw_average = 0, temp_comp;
  
  temp_comp = (1 + a * (temperature_C - 25.0));                                                               //t° compensation formula
  for (int i = 0; i < 5; i++) {
    TDS_raw = analogRead(TDS_pin);
    TDS_raw_sum = TDS_raw_sum + TDS_raw;                                                                      //get sum of received raw value from sensor for better accuracy
  }  
    TDS_raw_average = TDS_raw_sum / 5;
    TDS_volt = (TDS_raw_average * 5 / 1024.0) / temp_comp;                                                    //converting raw value to voltage and applying temperature compensation formula
    TDS = (133.42 / TDS_volt * TDS_volt * TDS_volt - 255.86 * TDS_volt * TDS_volt + 857.39 * TDS_volt) * 0.5; //voltage to TDS
    Serial.println("  TDS (ppm): " + (String)TDS);
  }

//--------------Atlas Scientific SGL-PMP-BX----------------

void dose_pump_pH_up(void) {                               //pump 1
if (pH < pH_lowest){
  //SEND COMMAND IN ASCII (STRING)
  mySerial1.println("d,"+ (String)pH_up_dosage);           //dispose x milliliters -> D,X 
  Serial.println("  Pump 1 is raising the pH...");
  }
  else{
    mySerial1.println("d,Sleep");                          //enter low power mode
    Serial.println("  Pump 1 is sleeping...");
  }
}
void dose_pump_TDS_up(void) {                               //pump 2
if (TDS < TDS_lowest){
  //SEND COMMAND IN ASCII (STRING)
    mySerial2.println("d," + (String)TDS_up_dosage);
    Serial.println("  Pump 2 is raising the TDS...");
  }
  else{
    mySerial2.println("d,Sleep");                          //enter low power mode
    Serial.println("  Pump 2 is sleeping...");
  }
}
void dose_pump_pH_down(void) {                             //pump 3
if (pH > pH_highest){
  //SEND COMMAND IN ASCII (STRING)
    mySerial3.println("d," + (String)pH_down_dosage);
    Serial.println("  Pump 3 is lowering the pH...");
  }
    else{
    mySerial3.println("d,Sleep");                          //enter low power mode
    Serial.println("  Pump 3 is sleeping...");
  }
}
void dose_pump_TDS_down(void) {                            //pump 4
if (TDS > TDS_highest){
  //SEND COMMAND IN ASCII (STRING)
    mySerial4.println("d," + (String)TDS_down_dosage);
    Serial.println("  Pump 4 is lowering the TDS...");
  }
    else{
    mySerial4.println("d,Sleep");                          //enter low power mode
    Serial.println("  Pump 4 is sleeping...");
  }
}

//-----------------------MAIN WATER PUMP------------------------

void main_pump(){
  float main_pump_on_ms = main_pump_on_min * 60000;                                     //convert user input from minutes to milliseconds
  float main_pump_off_ms = main_pump_off_min * 60000;

  unsigned long current_millis = millis();

  if (pump_status && (current_millis - previous_millis >= main_pump_on_ms)) {           // pump ON for x milliseconds
    Serial.println("  main pump ON");
    digitalWrite(main_pump_pin, LOW);
    pump_status = false;
    previous_millis = current_millis;
  }
    else if (!pump_status && (current_millis - previous_millis >= main_pump_off_ms)) {  // pump OFF
      Serial.println("  main pump OFF");
      digitalWrite(main_pump_pin, HIGH);
      pump_status = true;
      previous_millis = current_millis;
  }
}

//---------------------------FANS------------------------------

void fans() {
fan_speed = map(fan_speed_pct, 0, 100, 0, 255);                                    //convert % to the actual speed value

if ((humidity_1 >= humidity_highest || humidity_2 >= humidity_highest) && (fan_speed_pct > 0)) {
  analogWrite(fan_control_M1A_pin, fan_speed );
  analogWrite(fan_control_M2A_pin, fan_speed );
  //Serial.println("  Fan speed set on " + (String)fan_speed_pct);
} 
  else{
    digitalWrite(fan_control_M1A_pin, LOW); 
    digitalWrite(fan_control_M2A_pin, LOW); 
    //Serial.println("  Fan OFF");
  }
}