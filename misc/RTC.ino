#include <SPI.h>
#include <Controllino.h>

int day, weekday, month, year, hour, minute, second; 

const byte main_pump_pin = 30;

char weekdays[7][12] = {
  "Sunday",
  "Monday",
  "Tuesday",
  "Wednesday",
  "Thursday",
  "Friday",
  "Saturday"
};

void setup() {
 Serial.begin(19200);
 Controllino_RTC_init(0);
 //Controllino_SetTimeDate(17,1,07,23,10,01,00); // set initial values to the RTC chip
   pinMode(main_pump_pin, OUTPUT);
  digitalWrite(main_pump_pin, LOW);

}

void loop() {
RTC();
main_pump_on();
}  

void RTC(){

  day = Controllino_GetDay();
  weekday = Controllino_GetWeekDay();
  month = Controllino_GetMonth();
  year = Controllino_GetYear();
  hour = Controllino_GetHour();
  minute = Controllino_GetMinute();
  second = Controllino_GetSecond(); //add later if needed

  Serial.println();
  Serial.print(day);
  Serial.print("/");
  Serial.print(month);
  Serial.print("/");
  Serial.print(year);
  Serial.print(" (");
  Serial.print(weekdays[Controllino_GetWeekDay()]);
  Serial.print(")  ");
  Serial.print(hour);
  Serial.print(":");
  Serial.print(minute);

  delay(5000); 

}


void main_pump_on(){
  if ((second >= 0) && (second <= 10)){
    digitalWrite(main_pump_pin, HIGH);
  }
  else {
    digitalWrite(main_pump_pin, LOW);
  }
}

