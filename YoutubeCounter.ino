/*******************************************************************
 *  Read YouTube Channel statistics from the YouTube API using an
 *  ESP32
 *
 *  By Brian Lough
 *  Modified By Charlie Steenhagen
 *  Updated to work with ArduinoJSON 6.14.0
 *******************************************************************/

#include "YoutubeApi.h"
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include "credentials.h"
#include <ArduinoJson.h> // This Sketch doesn't technically need this, but the library does so it must be installed.


WiFiClientSecure client;
YoutubeApi api(API_KEY, client);

unsigned long api_mtbs = 10000; //mean time between api requests
unsigned long api_lasttime;   //last time api request has been done

long subs = 0;

void setup() {

  Serial.begin(115200);

  // Attempt to connect to Wifi network:
  Serial.print("Connecting Wifi: ");
  Serial.println(ssid);

  /* Explicitly set the ESP32 to be a WiFi-client, otherwise, it by default,
     would try to act as both a client and an access-point and could cause
     network-issues with your other WiFi-devices on your WiFi-network. */
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  IPAddress ip = WiFi.localIP();
  Serial.println(ip);

}

void loop() {

  if (millis() - api_lasttime > api_mtbs)  {
    Serial.println("Checking...");
    if(api.getChannelStatistics(CHANNEL_ID))
    {
      Serial.println("---------Stats---------");
      Serial.print("Subscriber Count: ");
      Serial.println(api.channelStats.subscriberCount);
      Serial.print("View Count: ");
      Serial.println(api.channelStats.viewCount);
      Serial.print("Comment Count: ");
      Serial.println(api.channelStats.commentCount);
      Serial.print("Video Count: ");
      Serial.println(api.channelStats.videoCount);
      // Probably not needed :)
      //Serial.print("hiddenSubscriberCount: ");
      //Serial.println(api.channelStats.hiddenSubscriberCount);
      Serial.println("------------------------");

    }
    api_lasttime = millis();
  }
  //TODO update the display
}
