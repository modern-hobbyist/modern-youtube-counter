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
#include <NTPClient.h>
#include <Espalexa.h>
#include "credentials.h"
#include <ArduinoJson.h> // This Sketch doesn't technically need this, but the library does so it must be installed.

#define NUM_DIGITS 4
#define LEDS_PER_DIGIT 7
#define DATA_PIN 26
#define ELECTRONICS_LED_PIN 27
#define CLOCK_PIN 13
#define DIGITS_NAME "Subscription Counter"
#define SUBSCRIPTION_MODE
//#define CLOCK_MODE
//#define TWELVE_HOUR_TIME
#define DEBUG

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);

Espalexa espalexa;

WiFiClientSecure client;
YoutubeApi api(CHANNEL_ID, API_KEY, client);

// Define the array of leds
CRGB leds[NUM_DIGITS * LEDS_PER_DIGIT];
CRGB led[1];

unsigned long api_mtbs = 60000; //mean time between api requests -- One Minute
unsigned long api_lasttime;   //last time api request has been done
bool power = true;
bool alexa_update = false;
int brightness = 255;
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
  {true, true, true, true, true, true, false}
};

void subscriptionCounterChanged(uint8_t brightness);

#ifdef CLOCK_MODE
  int timeStampHours = 0;
  int timeStampMinutes = 0;
  int timeStamp = 0;
#endif

void setup() {
  
  Serial.begin(115200);
  LEDS.addLeds<WS2811,DATA_PIN,RGB>(leds,NUM_DIGITS*LEDS_PER_DIGIT);
  LEDS.setBrightness(255);

  LEDS.addLeds<WS2811, ELECTRONICS_LED_PIN,RGB>(led, 1);
  LEDS.setBrightness(255);
  led[0] = CRGB(255, 255,255);
  LEDS.show();

  #ifdef DEBUG
    // Attempt to connect to Wifi network:
    Serial.print("Connecting Wifi: ");
    Serial.println(ssid);
  #endif

  /* Explicitly set the ESP32 to be a WiFi-client, otherwise, it by default,
     would try to act as both a client and an access-point and could cause
     network-issues with your other WiFi-devices on your WiFi-network. */
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    #ifdef DEBUG
      Serial.print(".");
    #endif
    delay(500);
  }

  #ifdef CLOCK_MODE
    // Initialize a NTPClient to get time
    timeClient.begin();
  
    //Check your UTC time offset at this link
    //https://en.wikipedia.org/wiki/List_of_UTC_time_offsets
    // Set offset time in seconds to adjust for your timezone
    //3600 seconds per hour offset
    //1 hour offset example
    timeClient.setTimeOffset(-3600*4);
  #endif
  espalexa.addDevice("Subscription Counter", subscriptionCounterChanged);
  espalexa.begin();
  
  IPAddress ip = WiFi.localIP();

  #ifdef DEBUG
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(ip);
  #endif

  #ifdef SUBSCRIPTION_MODE
    api.getChannelStatistics();
    updateDigits(api.channelStats.subscriberCount);
    Serial.print("Subscriber Count: ");
    Serial.println(api.channelStats.subscriberCount);
  #endif

  #ifdef CLOCK_MODE
    updateTime();
  #endif
}

void loop() {
  espalexa.loop();
  #ifdef CLOCK_MODE
    if(power){
      updateTime();
    }else{
      turnOffDigits();
    }
  #endif
  
  #ifdef SUBSCRIPTION_MODE
    if(power){
      led[0] = CRGB(255, 255,255);
      if (millis() - api_lasttime > api_mtbs || alexa_update)  {
        #ifdef DEBUG
          Serial.println("Checking...");
        #endif
        if(api.getChannelStatistics())
        {
          #ifdef DEBUG
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
          #endif
          updateDigits(api.channelStats.subscriberCount);
          led[0] = CRGB(255, 255,255);
        }else if(alexa_update){
          updateDigits(0);
          led[0] = CRGB(255, 255,255);
        }
        api_lasttime = millis();
        alexa_update = false;
      }
    }else{
      turnOffDigits();
    }
  #endif
}

void subscriptionCounterChanged(uint8_t brightness) { 
    if(brightness == 0){
      power = false;
    }else{
      power = true;
    }
    brightness = brightness;
    alexa_update = true;
}

#ifdef CLOCK_MODE
  /*
   * Updates the NTPClient every loop, but by default the NTPClient only pings the server once a minute
   */
  void updateTime(){
    #ifdef DEBUG
      Serial.println("Updating Time...");
    #endif
    if(timeClient.update()){
      timeStampHours = timeClient.getHours();
      timeStampMinutes = timeClient.getMinutes();
      timeStamp = timeStampHours * 100 + timeStampMinutes;
    
      #ifdef TWELVE_HOUR_TIME
        if(timeStamp >= 1200){
          timeStamp -= 1200;
        }
      #endif
    }else{
      timeStamp = 0;
    }
    
    #ifdef DEBUG
      Serial.print("Time: ");
      Serial.println(timeStamp);
    #endif
    
    updateDigits(timeStamp);
  }
#endif

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
          leds[j + offset] = CRGB(brightness,brightness,brightness);
       }else{
          leds[j + offset] = CRGB(0,0,0);
       }
    }
  }
  LEDS.show();
}

/*
 * Turns off all LEDs if Alexa tells it to turn off.
 */
void turnOffDigits(){  
  for(int i = 0; i< NUM_DIGITS*LEDS_PER_DIGIT; i++){
     leds[i] = CRGB(0,0,0);
  }
  led[0] = CRGB(0, 0, 0);
  LEDS.show();
}

int getDigit(int number, int digit){
  return (number / (int)pow(10, digit)) % 10;
}
