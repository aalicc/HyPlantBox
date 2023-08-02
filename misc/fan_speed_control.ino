//This code can be used to check HW-095 motor driver or similar. 
//Type the speed right in the Serial monitor (0 - 100%).

int IN2 = 5;
int ENA = 6;                                            //PWM signal
int speed;
int speed_pct;

void setup() {

Serial.begin(19200);

pinMode(IN2, OUTPUT);
pinMode(ENA, OUTPUT);                                   //ENA
digitalWrite(IN2, LOW);                                 // clockwise 

Serial.println("--hello, enter the fan speed in %--");
}

void loop() {
EnterSpeed();
}


void EnterSpeed(){
  
  if (Serial.available() > 0){
    speed_pct = Serial.parseInt();                       //reads whole integer in %
    speed = map(speed_pct, 0, 100, 0, 255);              //convert % to the actual speed value
  }
      if ((speed_pct > 0) && (speed_pct <= 100)){        //change to >= 0 if skip print
      digitalWrite(IN2, HIGH);
      analogWrite(ENA, speed );
      Serial.println("current speed = ");
      Serial.println(speed);                             //prints actual speed
      delay(3000);
      }
}