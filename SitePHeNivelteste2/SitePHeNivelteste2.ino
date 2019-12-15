/*
   Aquaponia HAP
   Terceiro site para a coleta de dados pela internet do nosso sistema de aquaponia
   que fica na escola estadual Hermelina de Albuquerque Passarella, Mairiporã - SP
   Página do Facebook: @AquaponiaHAP
   Site: AquaponiaHAP.com.br

   @author Brenon Kalevi  Email: guilherme.brenon@gmail.com   
   @author Leandro Derwin Email: ldsleandro@gmail.com         

*/

#include <SPI.h>
#include <Ethernet.h>
#include <DS3231.h>

int ph_pin = A0; //This is the pin number connected to Po

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0x3D };

IPAddress ip(192, 168, 0, 18);
EthernetServer server(80);
DS3231  rtc(SDA, SCL);
int boiaT = 22;
int boiaT2 = 23;


byte statusLed    = 13;

byte sensorInterrupt = 0;  // 0 = digital pin 2
byte sensorPin       = 2;

// The hall-effect flow sensor outputs approximately 4.5 pulses per second per
// litre/minute of flow.
float calibrationFactor = 4.5;

volatile byte pulseCount;

float flowRate;
unsigned int flowMilliLitres;
unsigned long totalMilliLitres;

unsigned long oldTime;

void setup() {
  Ethernet.begin(mac, ip);
  server.begin();
  rtc.begin();

  pinMode(boiaT, INPUT);
  pinMode(boiaT2, INPUT);
  pinMode(8, OUTPUT);
  pinMode(7, OUTPUT);
  

  pinMode(statusLed, OUTPUT);
  digitalWrite(statusLed, HIGH);  // We have an active-low LED attached

  pinMode(sensorPin, INPUT);
  digitalWrite(sensorPin, HIGH);

  pulseCount        = 0;
  flowRate          = 0.0;
  flowMilliLitres   = 0;
  totalMilliLitres  = 0;
  oldTime           = 0;

  // The Hall-effect sensor is connected to pin 2 which uses interrupt 0.
  // Configured to trigger on a FALLING state change (transition from HIGH
  // state to LOW state)
  attachInterrupt(sensorInterrupt, pulseCounter, FALLING);

}

void loop() {
  EthernetClient client = server.available();

  if (client)
  {
    boolean currentLineIsBlank = true;
    while (client.connected())
    {
      if (client.available())
      {
        char c = client.read();

        if (c == '\n' && currentLineIsBlank) {

          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println("Connection: close");

          client.println();

          client.println("<!DOCTYPE html>");
          client.println("<html>");
          client.println("<head>");
          client.println("<title>Aquaponia HAP</title>");
          client.println("</head>");
          client.println("<body>");
          client.println("<h1><font color=#4279c7>pH e N&iacute;vel da &Aacute;gua da aquaponia</font></h1>");
          client.println("<hr/>");
          client.println("<h1>Rel&oacute;gio</h1>");
          Relogio(client);
          client.println("<br/>");
          client.println("<h1>pH do tanque de carpas</h1>");
          pH(client);
          client.println("<br/>");
          client.println("<h1>N&iacute;vel do tanque de tilapias</h1>");
          NivelT(client);
          client.println("<br/>");
          //client.println("<h1>N&iacute;vel do tanque de carpas</h1>");
          //NivelC(client);
          //client.println("<br/>");
          //client.println("<h1>N&iacute;vel do filtro biol&oacute;gico</h1>");
          //NivelFB(client);
          //client.println("<br/>");
          client.println("<h1>Sensor de vaz&atilde;o aquaponia</h1>");
          vazao(client);
          client.println("<br/>");
          client.println("</body>");
          client.println("</html>");
          break;
        }

        if (c == '\n')
        {
          currentLineIsBlank = true;
        }
        else if (c != '\r')
        {
          currentLineIsBlank = false;
        }
      }
    }

    delay(1);
    client.stop();

  }

  digitalWrite(7, HIGH);
  delay(100);
  digitalWrite(7, LOW);
  delay(100);
  digitalWrite(8, HIGH);
  delay(100);
  digitalWrite(8, LOW);
  delay(100);
  


}
void pH(EthernetClient client_aux) {

  int measure = analogRead(ph_pin);
  //Serial.print("Measure: ");
  //Serial.print(measure);

  double voltage = 5 / 1024.0 * measure; //classic digital to voltage conversion
  // Serial.print("\tVoltage: ");
  //Serial.print(voltage, 3);

  // PH_step = (voltage@PH7 - voltage@PH4) / (PH7 - PH4)
  // PH_probe = PH7 - ((voltage@PH7 - voltage@probe) / PH_step)
  float Po = 7 + ((2.5 - voltage) / 0.18);
  client_aux.print("\tPH: ");
  client_aux.print(Po, 3);

  client_aux.println("");
  delay(2000);

}

void NivelT(EthernetClient client_aux) {

  int estado = digitalRead(boiaT);
  int estado2 = digitalRead(boiaT2);
  //client_aux.print(": ");

  switch (estado + estado2)
  {
    case 0:
      // Fica vermelho o led pois a caixa estaria cheia e a bomba seria ligada para passar a água para outro lugar.
      client_aux.println("N&iacute;vel M&aacute;ximo");
      break;

    case 1:
      // Fica vermelho o led pois a caixa estaria cheia e a bomba seria ligada para passar a água para outro lugar.
      client_aux.println("N&iacute;vel M&eacute;dio");
      break;

    case 2:
      // Fica verde o led pois a caixa estaria enchendo
      client_aux.println("N&iacute;vel Baixo");

  }
}

/*void NivelC(EthernetClient client_aux) {

  int estado3 = digitalRead(boiaC);
  int estado4 = digitalRead(boiaC2);
  //client_aux.print("Capta&ccedil;&atilde;o de &aacute;gua: ");

  switch (estado3 + estado4)
  {
    case 0:
      // Fica vermelho o led pois a caixa estaria cheia e a bomba seria ligada para passar a água para outro lugar.
      client_aux.println("N&iacute;vel M&aacute;ximo");
      break;

    case 1:
      // Fica vermelho o led pois a caixa estaria cheia e a bomba seria ligada para passar a água para outro lugar.
      client_aux.println("N&iacute;vel M&eacute;dio");
      break;

    case 2:
      // Fica verde o led pois a caixa estaria enchendo
      client_aux.println("N&iacute;vel Baixo");

  }
}

void NivelFB(EthernetClient client_aux) {

  int estado5 = digitalRead(boiaF);
  int estado6 = digitalRead(boiaF2);
  // client_aux.print("Capta&ccedil;&atilde;o de &aacute;gua: ");

  switch (estado5 + estado6)
  {
    case 0:
      // Fica vermelho o led pois a caixa estaria cheia e a bomba seria ligada para passar a água para outro lugar.
      client_aux.println("N&iacute;vel M&aacute;ximo");
      break;

    case 1:
      // Fica vermelho o led pois a caixa estaria cheia e a bomba seria ligada para passar a água para outro lugar.
      client_aux.println("N&iacute;vel M&eacute;dio");
      break;

    case 2:
      // Fica verde o led pois a caixa estaria enchendo
      client_aux.println("N&iacute;vel Baixo");

  }
}
*/
void vazao(EthernetClient client_aux) {

  if ((millis() - oldTime) > 1000)   // Only process counters once per second
  {
    // Disable the interrupt while calculating flow rate and sending the value to
    // the host
    detachInterrupt(sensorInterrupt);

    // Because this loop may not complete in exactly 1 second intervals we calculate
    // the number of milliseconds that have passed since the last execution and use
    // that to scale the output. We also apply the calibrationFactor to scale the output
    // based on the number of pulses per second per units of measure (litres/minute in
    // this case) coming from the sensor.
    flowRate = ((1000.0 / (millis() - oldTime)) * pulseCount) / calibrationFactor;

    // Note the time this processing pass was executed. Note that because we've
    // disabled interrupts the millis() function won't actually be incrementing right
    // at this point, but it will still return the value it was set to just before
    // interrupts went away.
    oldTime = millis();

    // Divide the flow rate in litres/minute by 60 to determine how many litres have
    // passed through the sensor in this 1 second interval, then multiply by 1000 to
    // convert to millilitres.
    flowMilliLitres = (flowRate / 60) * 1000;

    // Add the millilitres passed in this second to the cumulative total
    totalMilliLitres += flowMilliLitres;

    unsigned int frac;

    // Print the flow rate for this second in litres / minute
    client_aux.print("Flow rate: ");
    client_aux.print(int(flowRate));  // Print the integer part of the variable
    client_aux.println(".");             // Print the decimal point
    // Determine the fractional part. The 10 multiplier gives us 1 decimal place.
    frac = (flowRate - int(flowRate)) * 10;
    client_aux.print(frac, DEC) ;      // Print the fractional part of the variable
    client_aux.println("L/min             |");
    // Print the number of litres flowed in this second
    client_aux.print("  Fluxo atual: ");             // Output separator
    client_aux.print(flowMilliLitres);
    client_aux.print("mL/Sec               |");

    // Print the cumulative total of litres flowed since starting
    client_aux.print(" Quantidade de &aacute;gua: ");             // Output separator
    client_aux.print(totalMilliLitres);
    client_aux.println("mL");

    // Reset the pulse counter so we can start incrementing again
    pulseCount = 0;

    // Enable the interrupt again now that we've finished sending output
    attachInterrupt(sensorInterrupt, pulseCounter, FALLING);
  }
}

/*
  Insterrupt Service Routine
*/
void pulseCounter()
{
  // Increment the pulse counter
  pulseCount++;
}

void Relogio(EthernetClient client_aux) {

  client_aux.print("Hora:  ");
  client_aux.println(rtc.getTimeStr());

  client_aux.print("Data: ");
  client_aux.println(rtc.getDateStr());

  delay(1000);
}
