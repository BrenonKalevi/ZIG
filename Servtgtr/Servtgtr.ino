/*
   Aquaponia HAP
   Segundo site para a coleta de dados pela internet do nosso sistema de aquaponia
   que fica na escola estadual Hermelina de Albuquerque Passarella, Mairiporã - SP/
   Página do Facebook: @Aquaponia HAP
   Site: AquaponiaHAP.com.br

   @author Brenon Kalevi  Email: guilherme.brenon@gmail.com
   @author Leandro Derwin Email: ldsleandro@gmail.com

*/
//BIBLIOTECAS
#include <SPI.h>
#include <Ethernet.h>
#include <OneWire.h> //INCLUSÃO DE BIBLIOTECA
#include <DallasTemperature.h> //INCLUSÃO DE BIBLIOTECA
#include <DHT.h> //Carrega a biblioteca DHT
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_TSL2561_U.h>
#include <DS3231.h>

Adafruit_TSL2561_Unified tsl = Adafruit_TSL2561_Unified(TSL2561_ADDR_FLOAT, 12345);

//DEFINIÇÃO DOS PINOS
#define pinSensorD2 28
#define pinSensorD 8
#define DS18B20 7 //DEFINE O PINO DIGITAL UTILIZADO PELO SENSOR
#define DHTPIN 6
#define DHTTYPE DHT22
#define Rele1 9
//#define Rele2 5
int ph_pin = A0; //This is the pin number connected to Po

//VARIÁVEIS
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip(192, 168, 0, 16);
EthernetServer server(8080);
OneWire ourWire(DS18B20); //CONFIGURA UMA INSTÂNCIA ONEWIRE PARA SE COMUNICAR COM O SENSOR
DallasTemperature sensors(&ourWire); //BIBLIOTECA DallasTemperature UTILIZA A OneWire
DHT dht(DHTPIN, DHTTYPE);
DS3231  rtc(SDA, SCL);

unsigned int pingSpeed = 50; // How frequently are we going to send out a ping (in milliseconds). 50ms would be 20 times a second.
unsigned long pingTimer;     // Holds the next ping time.

int boiaT = 22;
int boiaT2 = 23;
int boiaC = 24;
int boiaC2 = 25;
int boiaF = 26;
int boiaF2 = 27;

byte statusLed    = 13;
byte sensorInterrupt = 0;  // 0 = digital pin 2
byte sensorPin       = 2;



void setup()
{
  Serial.begin(9600);
  Ethernet.begin(mac, ip);
  server.begin();
  sensors.begin(); //INICIA O SENSOR
  delay(1000);
  pinMode(pinSensorD, INPUT);
  pinMode(pinSensorD2, INPUT);
  dht.begin();
  rtc.begin();
  if (!tsl.begin())
  {

    Serial.print("Ooops, no TSL2561 detected ... Check your wiring or I2C ADDR!");
    while (1);
  }

  pingTimer = millis(); // Start now.

  pinMode(boiaT, INPUT);
  pinMode(boiaT2, INPUT);
  pinMode(boiaC, INPUT);
  pinMode(boiaC2, INPUT);
  pinMode(boiaF, INPUT);
  pinMode(boiaF2, INPUT);
  
  pinMode(statusLed, OUTPUT);
  digitalWrite(statusLed, HIGH);  // We have an active-low LED attached

  pinMode(sensorPin, INPUT);
  digitalWrite(sensorPin, HIGH);

  pinMode(30, OUTPUT);
  pinMode(31, OUTPUT);

  pinMode(Rele1, OUTPUT);
  // pinMode(Rele2, OUTPUT);
  digitalWrite(Rele1, HIGH);
  //digitalWrite(Rele2, HIGH);
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
          client.println("<h1><font color=#4279c7>Aquaponia HAP</font></h1>");
          client.println("<hr/>");
          client.println("<h1>Rel&oacute;gio</h1>");
          Relogio(client);
          client.println("<h1>Temperatura da &Aacute;gua</h1>");
          SensorDeTemperatura(client);
          client.println("<h1>Chuva</h1>");
          SensorDeChuva(client);
          client.println("<h1>&Aacute;gua na cama de cultivo</h1>");
          SensorDeUmidadeDoSolo(client);
          client.println("<br/>");
          client.println("<h1>Temperatura ambiente</h1>");
          SensorDeTemperaturaAr(client);
          client.println("<br/>");
          client.println("<h1>Umidade do ar</h1>");
          SensorUmidade(client);
          client.println("<h1>Luminosidade</h1>");
          Luminosidade(client);
          client.println("<br/>");
          client.println("<h1>pH do tanque de carpas</h1>");
          pH(client);
          client.println("<br/>");
          client.println("<h1>N&iacute;vel do tanque de tilapias</h1>");
          NivelT(client);
          client.println("<br/>");
          client.println("<h1>N&iacute;vel do tanque de carpas</h1>");
          NivelC(client);
          client.println("<br/>");
          client.println("<h1>N&iacute;vel do filtro biol&oacute;gico</h1>");
          NivelFB(client);
          client.println("<br/>");
          //client.println("<h1>Sensor de vaz&atilde;o aquaponia</h1>");
          //vazao(client);
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

  digitalWrite(30, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(100);                       // wait for a second
  digitalWrite(31, HIGH);    // turn the LED off by making the voltage LOW
  delay(100);

  digitalWrite(30, LOW);   // turn the LED on (HIGH is the voltage level)
  delay(100);                       // wait for a second
  digitalWrite(31, LOW);    // turn the LED off by making the voltage LOW
  delay(100);

  float Temperatura = sensors.getTempCByIndex(0);
  sensors.requestTemperatures();//SOLICITA QUE A FUNÇÃO INFORME A TEMPERATURA DO SENSOR
  Serial.print("Temperatura do tanque de peixes: "); //IMPRIME O TEXTO NA SERIAL
  Serial.print(sensors.getTempCByIndex(0)); //IMPRIME NA SERIAL O VALOR DE TEMPERATURA MEDIDO
  Serial.println("*C"); //IMPRIME O TEXTO NA SERIAL
  delay(250);//INTERVALO DE 250 MILISSEGUNDOS



  if (Temperatura < 25) {
    digitalWrite(Rele1, LOW);

  } else {
    digitalWrite(Rele1, HIGH);
  }

}

void SensorDeChuva(EthernetClient client_aux) {

  if (digitalRead(pinSensorD)) {
    client_aux.print("Sem chuva ");
  } else {
    client_aux.print("Est&aacute; chovendo ");
  }

}

void Luminosidade(EthernetClient client_aux) {
  sensors_event_t event;
  tsl.getEvent(&event);


  if (event.light)
  {
    client_aux.print(event.light); client_aux.println(" lux");
  }
  else
  {
    client_aux.println("Escuro");
  }
  delay(250);


}

void Relogio(EthernetClient client_aux) {

  client_aux.print("Hora:  ");
  client_aux.println(rtc.getTimeStr());

  client_aux.print("Data: ");
  client_aux.println(rtc.getDateStr());

  delay(1000);
}

void SensorDeTemperatura(EthernetClient client_aux) {

  sensors.requestTemperatures();//SOLICITA QUE A FUNÇÃO INFORME A TEMPERATURA DO SENSOR
  client_aux.print("Temperatura do tanque de peixes: "); //IMPRIME O TEXTO NA SERIAL
  client_aux.print(sensors.getTempCByIndex(0)); //IMPRIME NA SERIAL O VALOR DE TEMPERATURA MEDIDO
  client_aux.println("*C"); //IMPRIME O TEXTO NA SERIAL
  delay(250);//INTERVALO DE 250 MILISSEGUNDOS

}

void SensorDeTemperaturaAr(EthernetClient client_aux) {

  float t = dht.readTemperature(); //Le o valor da temperatura
  client_aux.print("Temp : ");
  client_aux.print(" ");
  client_aux.print(t, 1);
  client_aux.print("*C");
  delay(2000);
}

void SensorUmidade(EthernetClient client_aux) {
  float h = dht.readHumidity(); //Le o valor da umidade
  client_aux.print("Umid : ");
  client_aux.print(" ");
  client_aux.print(h, 1);
  client_aux.print("%");
  delay(2000);


}

void SensorDeUmidadeDoSolo(EthernetClient client_aux) {

  if (digitalRead(pinSensorD2)) {
    client_aux.print("Sem &Aacute;gua ");
  } else {
    client_aux.print("Com &Aacute;gua ");

  }
}

void criarformulario(EthernetClient client_aux, String parametro_aux)
{


  /* for(int i = 0; i<2; i++)
    {
       client_aux.println("Porta D" + String(i)+ ":");
       client_aux.println("<input type=\"checkbox\"  name=\"D" + String(i) + "\"  value=\"1\" onclick=\"submit()\" disabled>PINO NÃO UTILIZADO (DESABILITADO)</br>");
    }  */

  for (int i = 48; i < 53; i++)
  {

    client_aux.println("Porta D" + String(i) + " (Aquecedor " + String (i - 1) + ") :");
    if (parametro_aux.indexOf("D" + String(i) + "=1") > -1)
    {
      client_aux.println("<input type=\"checkbox\"  name=\"D" + String(i) + "\"  value=\"1\" onclick=\"submit()\" checked> LIGADA</br>");
      digitalWrite(i, LOW);
    }
    else
    {
      client_aux.println("<input type=\"checkbox\"  name=\"D" + String(i) + "\"  value=\"1\" onclick=\"submit()\"> DESLIGADA</br>");
      digitalWrite(i, HIGH);
    }
  }

  /* for(int i = 6; i<10; i++)
    {
       client_aux.println("Porta D" + String(i)+ ":");
       client_aux.println("<input type=\"checkbox\"  name=\"D" + String(i) + "\"  value=\"1\" onclick=\"submit()\" disabled>PINO NÃO UTILIZADO (DESABILITADO)</br>");
    }


    for(int i = 10; i<14; i++)
    {
       client_aux.println("Porta D" + String(i)+ ":");
       client_aux.println("<input type=\"checkbox\"  name=\"D" + String(i) + "\"  value=\"1\" onclick=\"submit()\" disabled>EM USO PELO ETHERNET SHIELD W5100 (DESABILITADO)</br>");
    }
    }*/
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

void NivelC(EthernetClient client_aux) {

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
