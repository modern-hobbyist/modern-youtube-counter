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
#include <FastLED.h>
#include "credentials.h"
#include "YoutubeCounter.h"
#include <ArduinoJson.h> // This Sketch doesn't technically need this, but the library does so it must be installed.

#define NUM_DIGITS 6
#define LEDS_PER_DIGIT 7
#define DATA_PIN 26
#define CLOCK_PIN 13

WiFiClientSecure client;
YoutubeApi api(API_KEY, client);

// Define the array of leds
CRGB leds[NUM_DIGITS * LEDS_PER_DIGIT];

unsigned long api_mtbs = 10000; //mean time between api requests
unsigned long api_lasttime;   //last time api request has been done

int testcounter = 0;

bool digits[][LEDS_PER_DIGIT] = {
  {true, true, true, false, true, true, true},
  {true, false, false, false, true, false, false},
  {true, true, false, true, false, true, true},
  {true, true, false, true, true, true, false},
  {true, false, true, true, true, false, false},
  {false, true, true, true, true, true, false},
  {false, true, true, true, true, true, true},
  {true, true, false, false, true, false, false},
  {true, true, true, true, true, true, true},
  {true, true, true, true, true, false, false}
};

void setup() {

  Serial.begin(115200);
  LEDS.addLeds<WS2812,DATA_PIN,RGB>(leds,NUM_DIGITS*LEDS_PER_DIGIT);
  LEDS.setBrightness(255);
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
  api.getChannelStatistics(CHANNEL_ID);
  updateDigits(api.channelStats.subscriberCount);
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
    updateDigits(api.channelStats.subscriberCount);
  }
}

/*
 * The display will be made up of 7 sections of WS2811 leds which come in sections of 3. 
 */
void updateDigits(int subscribers){  
  int digit = 0;
  int offset = 0;
  for(int i = 0; i< NUM_DIGITS; i++){
    offset = LEDS_PER_DIGIT * i;
    digit = getDigit(subscribers, i);
    for(int j = 0; j<LEDS_PER_DIGIT; j++){
       if(digits[digit][j]){
          leds[j + offset] = CRGB(255, 255,255);
       }else{
          leds[j + offset] = CRGB(0,0,0);
       }
    }
  }
  LEDS.show();
}

int getDigit(int number, int digit){
  return (number / (int)pow(10, digit)) % 10;
}
