/*
   Aquaponia HAP
   Segundo site para a coleta de dados pela internet do nosso sistema de captação de água
   que fica na escola estadual Hermelina de Albuquerque Passarella, Mairiporã - SP

   Página do Facebook:  https://www.facebook.com/aquaponiahap/?view_public_for=334955727348557
   Site:  http://aquaponiahap.com.br/

   @author Brenon Kalevi  Email: guilherme.brenon@gmail.com   Whats: (11)956527593
   @author Leandro Derwin Email: ldsleandro@gmail.com         Whats: (11)942541760

*/

#include <SPI.h>
#include <Ethernet.h>

int boiaCM = 7;
int boiaMB = 6;

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

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0x1D };

IPAddress ip(192, 168, 0, 20);
EthernetServer server(8081);

void setup() {
  Ethernet.begin(mac, ip);
  server.begin();

  pinMode(boiaCM, INPUT);
  pinMode(boiaMB, INPUT);

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

  pinMode(10, OUTPUT);
  pinMode(9, OUTPUT);

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
          client.println("<h1><font color=#4279c7>Capta&ccedil;&atilde;o de &Aacute;gua da Chuva</font></h1>");
          client.println("<hr/>");
          client.println("<h1>N&iacute;vel da &Aacute;gua</h1>");
          NivelCM(client);
          client.println("<br/>");
          client.println("<h1>Sensor de vaz&atilde;o torneira</h1>");
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

  digitalWrite(10, HIGH);   // turn the LED on (HIGH is the voltage level)
  //delay(200);                       // wait for a second
  digitalWrite(9, LOW);    // turn the LED off by making the voltage LOW
  delay(300);

  digitalWrite(9, HIGH);   // turn the LED on (HIGH is the voltage level)
                         // wait for a second
  digitalWrite(10, LOW);    // turn the LED off by making the voltage LOW
  delay(300);

}

void NivelCM(EthernetClient client_aux) {

  int estado = digitalRead(boiaCM);
  int estado2 = digitalRead(boiaMB);
  client_aux.print("Capta&ccedil;&atilde;o de &aacute;gua: ");

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
