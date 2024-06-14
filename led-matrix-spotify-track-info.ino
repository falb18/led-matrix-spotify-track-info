#include <ESP8266WiFi.h>
#include <MD_Parola.h>
#include <MD_MAX72xx.h>
#include <MQTT.h>
#include <SPI.h>

// Define the number of devices we have in the chain and the hardware interface
#define HARDWARE_TYPE MD_MAX72XX::FC16_HW
#define MAX_DEVICES 4

#define CLK_PIN   D5  // or SCK
#define DATA_PIN  D7  // or MOSI
#define CS_PIN    D8  // or CS

MD_Parola led_matrix = MD_Parola(HARDWARE_TYPE, D7, D5, D8, MAX_DEVICES);

/* MD_MAX72xx library definitions: */

// Text parameters
#define CHAR_SPACING  1 // pixels between characters

/* MD_Parola library definitions: */
uint8_t scrollSpeed = 25;    // default frame delay value
textEffect_t scrollEffect = PA_SCROLL_LEFT;
textPosition_t scrollAlign = PA_LEFT;
uint16_t scrollPause = 2000; // in milliseconds

// Global message buffers shared by Serial and Scrolling functions
#define BUF_SIZE  75
char message[BUF_SIZE] = "Spotify track info";

/* MQTT library definitions: */
WiFiClient net;
MQTTClient mqtt;
IPAddress mqttBroker(192,168,10,111);

void setup()
{
  int num_attempt_connections = 0;
  
  Serial.begin(115200);
  Serial.println();

  mqtt.begin(mqttBroker, net);
  mqtt.onMessage(message_received);
  led_matrix.begin();

  if (WiFi.SSID() != "")
  {
    Serial.print("Attempting connection to WiFi network: ");
    Serial.println(WiFi.SSID().c_str());
    WiFi.begin(WiFi.SSID().c_str(), WiFi.psk().c_str());

    while(num_attempt_connections < WL_MAX_ATTEMPT_CONNECTION)
    {
      if (WiFi.status() != WL_CONNECTED)
      {
        delay(500);
        Serial.print(".");
        num_attempt_connections++;
      }
      else
      {
        break;
      }
    }

    if (num_attempt_connections >= WL_MAX_ATTEMPT_CONNECTION)
    {
      Serial.println("Connection failed.");
      led_matrix.print("WiFi!");
      smartconfig_connect_ap();
    }
  }
  else
  {
    smartconfig_connect_ap();
  }

  Serial.println();
  Serial.println("Connected.");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  connect_mqtt_broker();
  
  led_matrix.displayText(message, scrollAlign, scrollSpeed, scrollPause, scrollEffect, scrollEffect);
}

void loop()
{
  if (led_matrix.displayAnimate())
  {
    led_matrix.displayReset();
  }
}

void smartconfig_connect_ap()
{
  // Init WiFi as Station, start SmartConfig
  WiFi.mode(WIFI_STA);
  delay(500);
  WiFi.beginSmartConfig();

  //Wait for SmartConfig packet from mobile
  Serial.println("Waiting for SmartConfig.");
  while (!WiFi.smartConfigDone()) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("SmartConfig received.");

  //Wait for WiFi to connect to AP
  Serial.println("Waiting for WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
}

void connect_mqtt_broker()
{
  int numAttempts = 0;
  
  Serial.println("Connecting to MQTT broker");
  
  while (numAttempts < 10)
  {
    if (mqtt.connect("esp8266-spotifysub") == false)
    {
      Serial.print(".");
      numAttempts++;
    }
    else
    {
      break;
    }
  }

  if (numAttempts >= 10)
  {
    Serial.println("");
    Serial.println("Connection to broker failed");
    led_matrix.print("Track!");
    delay(5000);
  }
  else
  {
    Serial.println("");
    Serial.print("Connected to broker: ");
    Serial.println(mqttBroker.toString());
  }
}

void message_received(String &topic, String &payload)
{
  return;
}
