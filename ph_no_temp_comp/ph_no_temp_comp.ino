// DFRobot (SEN0161) and Hanna Instruments (HI1286) analog
// pH sensors with a DFRobot pH meter version 1.1 (SEN0161).


const byte ph_sensor_pin = A1;           // Analog 0    X1

const float Offset = 0.4;  //deviation compensate
const byte LED = 10;
float av_ph;

void setup(void)
{
  Serial.begin(9600);
  Serial.println("pH meter experiment!");  //Test the serial monitor
}
void loop(void)
{
  phLevel();
  delay(3000);
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