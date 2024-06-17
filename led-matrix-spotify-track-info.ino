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
#define BUF_SIZE  128
char trackInfo[BUF_SIZE] = "Spotify track info";

/* MQTT library definitions: */
WiFiClient net;
MQTTClient mqttClient;
IPAddress mqttBroker(192,168,10,111);

/* Spotify variables: */
String titleTopic = "spotify/metadata/title";
String albumTopic = "spotify/metadata/album";
String artistTopic = "spotify/metadata/artist";

String strTitle = "";
String strAlbum = "";
String strArtist = "";

void setup()
{  
  Serial.begin(115200);
  Serial.println();

  mqttClient.begin(mqttBroker, net);
  mqttClient.onMessage(message_received);
  led_matrix.begin();

  connect_wifi_network();

  connect_mqtt_broker();
  
  led_matrix.displayText(trackInfo, scrollAlign, scrollSpeed, scrollPause, scrollEffect, scrollEffect);
}

void loop()
{
  mqttClient.loop();
  
  if (led_matrix.displayAnimate())
  {
    led_matrix.displayReset();
    update_track_info();
  }
}

void connect_wifi_network()
{
  int numAttempts = 0;
  
  if (WiFi.SSID() != "")
  {
    Serial.print("Attempting connection to WiFi network: ");
    Serial.println(WiFi.SSID().c_str());
    WiFi.begin(WiFi.SSID().c_str(), WiFi.psk().c_str());

    while(numAttempts < WL_MAX_ATTEMPT_CONNECTION)
    {
      if (WiFi.status() != WL_CONNECTED)
      {
        delay(500);
        Serial.print(".");
        numAttempts++;
      }
      else
      {
        break;
      }
    }

    if (numAttempts >= WL_MAX_ATTEMPT_CONNECTION)
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
    if (mqttClient.connect("esp8266-spotifysub") == false)
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
    Serial.println("Connected to broker: " + mqttBroker.toString());
    mqttClient.subscribe(titleTopic);
    mqttClient.subscribe(albumTopic);
    mqttClient.subscribe(artistTopic);
  }
}

void message_received(String &topic, String &payload)
{
  if (topic.equals(titleTopic) == true)
  {
    strTitle = payload;
  }
  else if (topic.equals(albumTopic) == true)
  {
    strAlbum = payload;
  }
  else if (topic.equals(artistTopic) == true)
  {
    strArtist = payload;
  }
}

void update_track_info(void)
{
  sprintf(trackInfo, "%s - %s - %s", strTitle.c_str(), strArtist.c_str(), strAlbum.c_str());
  led_matrix.displayText(trackInfo, scrollAlign, scrollSpeed, scrollPause, scrollEffect, scrollEffect);
}
