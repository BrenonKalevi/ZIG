/*
   Exemplo de uso dom array
   @Author Brenon Kalevi
*/
void setup() {
  Serial.begin(9600);
  randomSeed(analogRead(A0));
  String faces[13] = {"A","2","3","4","5","6","7","8","9","10","J","Q","K"};
  String nipes[4] = {"♦","♠","♥","♣",};
  int f = random(13);
  int n = random(4);
  Serial.print(faces[f]);
  Serial.println(nipes[n]);  
}

void loop() {
  // put your main code here, to run repeatedly:

}
