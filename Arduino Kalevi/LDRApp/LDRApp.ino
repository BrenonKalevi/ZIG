/*
   Leitura do LDR no APP
   @author Brenon Kalevi
*/

void setup() {
  Serial.begin(9600);
  pinMode (2, OUTPUT);
}

void loop() {
  int leitura;
  leitura = analogRead(A0); // 0 a 1023
  Serial.println(leitura);
  Serial.print("");
  delay(500);
  if (leitura < 850) {
    digitalWrite (2, HIGH);
  } else {
    digitalWrite(2, LOW) ;
  }
}
