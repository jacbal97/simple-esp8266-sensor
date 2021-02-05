#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "DHT.h"

#define DHTPin 2
#define DHTTYPE DHT22

//wifi data
const char* WiFi = "WiFi_name";
const char* WiFi_2 = "Second WiFi";
const char* password = "password";

const char *mqtt_broker = "serwer";
const char topic[] = "toppic";
const char mqtt_username[] = "username";
const char client_id[] = "id";
const char pass[] = "password";
const int mqtt_port = port;

const long int sleepTime = 1e6*60*15;
//int sleepTime = 1e6*20*60;
int wait = 0;

WiFiClient espClient;
PubSubClient client(espClient);
DHT dht(DHTPin, DHTTYPE);
ADC_MODE(ADC_VCC);

bool connectWiFi(const char* login, const char* pass)
{
  WiFi.begin(login, pass);
  for(int i = 0;i < 8; i++)
  {
    if(WiFi.status() != WL_CONNECTED)
    {
      delay(1000);
    }
    else
    {
      return false;
    }
  }
  return true;
}

//connect to wifi and publish data
bool connect()
{
  if(connectWiFi(jaskinia, password) || connectWiFi(fbi, password))
    return false;

  client.setServer(mqtt_broker, mqtt_port);
  
  for(int i =0; i < 8; i++) 
  {
     if (!client.connect(client_id, mqtt_username, pass )) 
     {
         delay(1000);
     }
  }

  return true;
}

void setup() 
{
  dht.begin();
  delay(2000);
}

void loop() 
{
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  
  float p = ESP.getVcc();
  
  String string = "{\"temperature\": "+String(t)+ ", \"humidity\": "+String(h)+", \"voltage\": "+String(p)+"}";
  if(!connect())
    ESP.deepSleep(sleepTime, WAKE_RF_DEFAULT);
  for (int i = 0; i < 4; i++)
  {
    if(client.publish(topic, string.c_str()))
    {
      client.disconnect();
      ESP.deepSleep(sleepTime, WAKE_RF_DEFAULT);
    }
    else
    delay(200);
  }
  ESP.deepSleep(sleepTime, WAKE_RF_DEFAULT);
}
