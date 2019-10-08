/*
   Pianinho
   @author Brenon Kalevi
*/

void setup() {
  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);

  pinMode(7, OUTPUT);

  pinMode(8, INPUT);
  pinMode(9, INPUT);
  pinMode(10, INPUT);
  pinMode(11, INPUT);





}

void loop() {
  if (digitalRead(8) == HIGH) {
    digitalWrite(2, HIGH);
    tone(7,262);
    delay(300); 
  } else {
    digitalWrite(2, LOW);
    noTone(7);
  }
  if (digitalRead(9) == HIGH) {
    digitalWrite(3, HIGH);
    tone(7,294);
    delay(300);
  } else {
    digitalWrite(3, LOW);
    noTone(7);
  }
  if (digitalRead(10) == HIGH) {
    digitalWrite(4, HIGH);
    tone(7,330);
    delay(300);
  } else {
    digitalWrite(4, LOW);
    noTone(7);
  }
  if (digitalRead(11) == HIGH) {
    digitalWrite(5, HIGH);
    tone(7,349);
    delay(300);
  } else {
    digitalWrite(5, LOW);
    noTone(7);
  }

}
