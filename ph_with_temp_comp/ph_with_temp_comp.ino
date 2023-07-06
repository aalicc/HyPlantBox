// DFRobot (SEN0161) and Hanna Instruments (HI1286) analog
// pH sensors with a DFRobot pH meter version 1.1 (SEN0161).
#include <OneWire.h>
#include <DallasTemperature.h>

const byte water_temp_pin = 21;   
const byte ph_sensor_pin = A1;           // Analog 0    X1

const float Offset = 0.4;  //deviation compensate
float av_ph, E;

float temp_C, temp_K;

// Setup a oneWire instance to communicate with any OneWire devices
OneWire oneWire(water_temp_pin);
// Pass our oneWire reference to Dallas Temperature
DallasTemperature sensors(&oneWire);
// Insert address (to get, see DS18B20_get_address):
uint8_t sensor1[8] = { 0x28, 0xFF, 0xC8, 0xC2, 0xC1, 0x16, 0x04, 0xB5 };

void setup(void)
{
  Serial.begin(19200);
  sensors.begin(); // Start up the library
  Serial.println("start");  //Test the serial monitor
}
void loop(void)
{
  phLevel();
  temp_comp();
  delay(3000);
  water_temp();
}

void water_temp(void){
  sensors.requestTemperatures();
  temp_C = sensors.getTempC(sensor1);
  Serial.println("  Temp1 (°C): " + (String)temp_C);
  temp_K = temp_C + 273.15;

}

void phLevel (void){
  float ph_value, voltage, ph_raw, ph_sum = 0;
  
  for (int k=0; k<5; k++){
    ph_raw = analogRead(ph_sensor_pin);
    voltage = ph_raw*5/1024.0;
    ph_value = 3.5*voltage+Offset;
    ph_sum = ph_sum + ph_value;
  }
  av_ph = ph_sum / 5;
  Serial.print("  pH value: ");
  Serial.println(av_ph);
}

void temp_comp(void){
  E = 7 + 2.303*((8.314*temp_K)/(1*96485))*(-av_ph);            //E = E0 + 2.303(RT/nF)log(ah+)
  Serial.print("  t compensated pH: ");
  Serial.println(E);
}