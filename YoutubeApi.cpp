/*
   Copyright (c) 2017 Brian Lough. All right reserved.
   Updated to work with ArduinoJSON 6.14.0 by Charlie Steenhagen

   YoutubeApi - An Arduino wrapper for the YouTube API

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with this library; if not, write to the Free Software
   Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */


#include "YoutubeApi.h"

YoutubeApi::YoutubeApi(String channelId, String apiKey, Client &client)	{
	_apiKey = apiKey;
  _channelId = channelId;
	this->client = &client;
}

void YoutubeApi::parseResponse(String httpsResponse){
  StaticJsonDocument<2048> doc;
  DeserializationError error = deserializeJson(doc, httpsResponse);
  
  // Test if parsing succeeds.
  if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.f_str());
    return;
  }
  String count = doc["items"][0]["statistics"]["subscriberCount"];
  channelStats.viewCount = doc["items"][0]["statistics"]["viewCount"];
  channelStats.subscriberCount = doc["items"][0]["statistics"]["subscriberCount"];
  channelStats.commentCount = doc["items"][0]["statistics"]["commentCount"];
  channelStats.hiddenSubscriberCount = doc["items"][0]["statistics"]["hiddenSubscriberCount"];
  channelStats.videoCount = doc["items"][0]["statistics"]["videoCount"];
}

bool YoutubeApi::getChannelStatistics(){
  String command="/youtube/v3/channels?part=statistics&id="+_channelId+"&key="+_apiKey; //If you can't find it(for example if you have a custom url) look here: https://www.youtube.com/account_advanced
  WiFiClientSecure *client = new WiFiClientSecure;
  if(client) {
//    client -> setCACert(rootCACertificate);
    client->setInsecure();

    {
      // Add a scoping block for HTTPClient https to make sure it is destroyed before WiFiClientSecure *client is 
      HTTPClient https;
  
      Serial.print("[HTTPS] begin...\n");
      if (https.begin(*client, YTAPI_HOST + command)) {  // HTTPS
        Serial.print("[HTTPS] GET...\n");
        
        int httpCode = https.GET();
  
        // httpCode will be negative on error
        if (httpCode > 0) {
          // HTTP header has been send and Server response header has been handled
          Serial.printf("[HTTPS] GET... code: %d\n", httpCode);
          // file found at server
          if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_FOUND) {
            parseResponse(https.getString());
          }
        } else {
          Serial.printf("[HTTPS] GET... failed, error: %s\n", https.errorToString(httpCode).c_str());
        }
  
        https.end();
      } else {
        Serial.printf("[HTTPS] Unable to connect\n");
      }

      // End extra scoping block
    }
  
    delete client;
  } else {
    Serial.println("Unable to create client");
  }
}

void YoutubeApi::closeClient() {
	if(client->connected()) {
		if(_debug) { Serial.println(F("Closing client")); }
		client->stop();
	}
}
