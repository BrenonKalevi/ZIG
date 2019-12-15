/*
   Alimentador automatico



   Página do Facebook: @Aquaponia HAP
   Site: AquaponiaHAP.com.br

   @author Brenon Kalevi  Email: guilherme.brenon@gmail.com


*/

#include <SPI.h>

#include <Servo.h>
#include <DS3231.h>

Servo myservo;

Servo myservo2;
char leitor;
int pos = 0;
int tq = 8;
long FISHFEEDER = 3600000;
long endtime;
long now;
DS3231  rtc(SDA, SCL);

void setup()
{
  myservo.attach(7);
  myservo2.attach(8);
 


  myservo2.write(0);
  delay(15);

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
    myservo2.write(0);
    delay(1000);
    now = millis();
    if (Serial.available()) {
      leitor = Serial.read();

      switch (leitor) {
        case 'a':
          for (pos = 0; pos < 180; pos += 1) {
            myservo.write(pos);
            delay(tq);
          }
          for (pos = 180; pos >= 0; pos -= 1) {
            myservo.write(pos);
            delay(tq);
          }
          break;
        case 's':
          for (pos = 0; pos < 180; pos += 1) {
            myservo2.write(pos);
            delay(tq);
          }
          for (pos = 180; pos >= 1; pos -= 1) {
            myservo2.write(pos);
            delay(tq);
          }
          break;
      }

    }
  }


  for (pos = 0; pos < 180; pos += 1) {
    myservo.write(pos);
    delay(tq);
  }
  for (pos = 180; pos >= 1; pos -= 1) {
    myservo.write(pos);
    delay(tq);
  }

  for (pos = 0; pos < 180; pos += 1) {
    myservo2.write(pos);
    delay(tq);
  }
  for (pos = 180; pos >= 1; pos -= 1) {
    myservo2.write(pos);
    delay(tq);
  }

  
  Serial.print("Hora:  ");
  Serial.println(rtc.getTimeStr());

  Serial.print("Data: ");
  Serial.println(rtc.getDateStr());


}
