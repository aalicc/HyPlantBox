#include <Controllino.h>
#include <SoftwareSerial.h>

//fan
int fan_speed;
//fan control HW-095
int fan_control_IN2_pin = 5;               //clockwise rotation pin
int fan_control_ENA_pin = 6;

int humidity_highest = 0;

#define mySerial Serial1

String fromPi;
void setup() {
  Serial.begin(19200);
  mySerial.begin(19200);
  pinMode(fan_control_IN2_pin, OUTPUT);                     
  pinMode(fan_control_ENA_pin, OUTPUT);  
  digitalWrite(fan_control_IN2_pin, LOW);

  Serial1.setTimeout(50);
}

//fan default speed  0% - 100%
int fan_speed_pct; 

void loop() {
pi_communication();
run_pump();

}

String get_ruuvi[10];
String get_values[10];
String get_humidity;
float humidity_1 = 0, humidity_2 = 0;

void pi_communication(){
    Serial1.println("bebik");

if (Serial1.available()){

/*    if ((Serial1.readString() == "r")){
      get_humidity = Serial1.readString();
      humidity = get_humidity.toInt();  // convert to float
      Serial.println(humidity);
    }*/

    if ((Serial1.readString() == "s")){
      for (int i = 1; i <= 8; i++){
        get_values[i]= Serial1.readStringUntil("o");
        Serial.println(get_values[i]); 
      }
    }

      else {
        for (int k = 1; k <= 4; k++){
        get_ruuvi[k] = Serial1.readString();
        Serial.println(get_ruuvi[k]);
      }
    }
}
String humidity_highest_pi = get_values[1];
humidity_highest = humidity_highest_pi.toFloat(); 
String fan_speed_pct_pi = get_values[2];
fan_speed_pct = fan_speed_pct_pi.toInt(); 

String humidity_1_pi = get_ruuvi[2];
humidity_1 = humidity_1_pi.toFloat(); 
String humidity_2_pi = get_ruuvi[4];
humidity_2 = humidity_2_pi.toFloat(); 
}

void run_pump(){

fan_speed = map(fan_speed_pct, 0, 100, 0, 255); 

if ((humidity_1 >= humidity_highest || humidity_2 >= humidity_highest) && (fan_speed_pct > 0)) {
  digitalWrite(fan_control_IN2_pin, HIGH);
  analogWrite(fan_control_ENA_pin, fan_speed );
  //Serial.println("  Fan speed set on " + (String)fan_speed_pct);
} 
  else{
    digitalWrite(fan_control_IN2_pin, LOW); 
    //Serial.println("  Fan OFF");
  }
}
