 /*
    Automação com Arduino
    @author Brenon Kalevi



*/
char leitor;

void setup() {
  pinMode(12, OUTPUT);
  pinMode(13, OUTPUT);

  Serial.begin(9600);


}

void loop() {
  if (Serial.available()) {
    leitor = Serial.read();
  
    switch (leitor) {
      case 'a':
        digitalWrite(13, HIGH);
        break;
      case 's':
        digitalWrite(13, LOW);
        break;
      case 'q':
        digitalWrite(12, HIGH);
        break;
      case 'w':
        digitalWrite(12, LOW);
        break;
    }

  }


}
