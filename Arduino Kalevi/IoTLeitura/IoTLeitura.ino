/*
   IoT - Leitura de um sensor no APP
   @author Brenon Kalevi
*/

#include <Ultrasonic.h>
Ultrasonic ultrasonic(12, 13);
int distancia;

void setup() {
  Serial.begin(9600);
}

void loop() {

  distancia = ultrasonic.read();
  Serial.println(distancia);
  Serial.print("");
  delay(500);
}
