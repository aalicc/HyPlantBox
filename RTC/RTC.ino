#include <SPI.h>
#include <Controllino.h>

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
 Serial.begin(9600);
 Controllino_RTC_init(0);
 //Controllino_SetTimeDate(17,1,07,23,10,01,0); // set initial values to the RTC chip
}

void loop() {
  int day, weekday, month, year, hour, minute, second; 

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
