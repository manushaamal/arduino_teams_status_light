/**
 * BasicHTTPClient.ino
 *
 *  Created on: 24.05.2015
 *
 */

#include <Arduino.h>

#include <WiFi.h>
#include <WiFiMulti.h>
#include <ArduinoJson.h>

#include <HTTPClient.h>

#define USE_SERIAL Serial

WiFiMulti wifiMulti;
char json[500];

const int LED_1 = 13;
const int LED_2 = 12;
const int LED_3 = 14;

void setup() {

    USE_SERIAL.begin(115200);
    pinMode(LED_1, OUTPUT);
    pinMode(LED_2, OUTPUT);
    pinMode(LED_3, OUTPUT);
    USE_SERIAL.println();
    USE_SERIAL.println();
    USE_SERIAL.println();
    digitalWrite(LED_1, LOW);
    for(uint8_t t = 4; t > 0; t--) {
        USE_SERIAL.printf("[SETUP] WAIT %d...\n", t);
        USE_SERIAL.flush();
        delay(1000);
    }

    wifiMulti.addAP("WIFI-SSID", "Password");

}

void loop() {
    // wait for WiFi connection
    if((wifiMulti.run() == WL_CONNECTED)) {

        HTTPClient http;

        USE_SERIAL.print("[HTTP] begin...\n");
        http.begin("https://graph.microsoft.com/v1.0/users/{user-id}/presence"); //HTTP
        http.addHeader("Authorization","Bearer 'Your Token'");

        USE_SERIAL.print("[HTTP] GET...\n");
        // start connection and send HTTP header
        int httpCode = http.GET();

        // httpCode will be negative on error
        if(httpCode > 0) {
            // HTTP header has been send and Server response header has been handled
            USE_SERIAL.printf("[HTTP] GET... code: %d\n", httpCode);

            // file found at server
            if(httpCode == HTTP_CODE_OK) {
                String payload = http.getString();
                USE_SERIAL.println(payload);
                payload.toCharArray(json, 500);
                StaticJsonDocument<1024> doc;
                deserializeJson(doc, json);
                String availability = doc ["availability"];
                USE_SERIAL.println(availability);
                if (availability == "Available"){
                digitalWrite(LED_1, HIGH);
                digitalWrite(LED_2, LOW);
                digitalWrite(LED_3, LOW);}
                else if (availability == "Busy"){
                digitalWrite(LED_2, HIGH);
                digitalWrite(LED_1, LOW);
                digitalWrite(LED_3, LOW);}
                else if (availability == "Away"){
                digitalWrite(LED_3, HIGH);
                digitalWrite(LED_2, LOW);
                digitalWrite(LED_1, LOW);}
            }
        } else {
            USE_SERIAL.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
        }

        http.end();
    }

    delay(5000);
}
