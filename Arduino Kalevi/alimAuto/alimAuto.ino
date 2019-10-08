/*
   Alimentador automatico

   Escola estadual Hermelina de Albuquerque Passarella, Mairiporã - SP/

   Página do Facebook: @Aquaponia HAP
   Site: AquaponiaHAP.com.br

   @author Brenon Kalevi  Email: guilherme.brenon@gmail.com


*/


#include <Servo.h>


Servo myservo;
char leitor;
int pos = 0;

long FISHFEEDER = 21600000;
long endtime;
long now;

void setup()
{

  myservo.attach(9);

  myservo.write(0);
  delay(15);
  Serial.begin(9600);
}

void loop()
{
  now = millis();
  endtime = now + FISHFEEDER;

  while (now < endtime) {
    myservo.write(0);
    delay(1000);
    now = millis();
    if (Serial.available()) {
      leitor = Serial.read();

      switch (leitor) {
        case 'a':
          for (pos = 0; pos < 180; pos += 1) {
            myservo.write(pos);
            delay(15);
          }
          for (pos = 180; pos >= 1; pos -= 1) {
            myservo.write(pos);
            delay(15);
          }
          break;
      }

    }
  }

  for (pos = 0; pos < 180; pos += 1) {
    myservo.write(pos);
    delay(15);
  }
  for (pos = 180; pos >= 1; pos -= 1) {
    myservo.write(pos);
    delay(15);
  }

}
