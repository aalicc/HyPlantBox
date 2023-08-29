  #include <Controllino.h>
  #include <SoftwareSerial.h>

  #define mySerial Serial1

  String fromPi;
  void setup() {
  Serial.begin(19200);
  mySerial.begin(19200);
  Serial1.setTimeout(50);
  }

  void loop() {
  pi_communication();
  }

  void pi_communication(){

  String send_data = "80&81&79&90&45&22.5&7.55&106.0&100&ON";
  Serial1.println(send_data);
  delay(1000);
  }

