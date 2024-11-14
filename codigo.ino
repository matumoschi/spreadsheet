#include <WiFi.h>

#include <DHT.h>
#include <DHT_U.h>

#define DHTPIN 2
#define DHTTYPE DHT11 

DHT dht(DHTPIN, DHTTYPE);


float temp = 0.0;
float hum = 0.0;
unsigned long timeRead = 0;

#include <HTTPClient.h>

const char* ssid = "PLATINI";
const char* password = "capitan02";
const char* serverName = "https://script.google.com/macros/s/AKfycbyENw4diHqXyz2aPlEJpHJGTUBWz4nYkC-EvYpdw4PId2YTBOlzldzVnD007VRqHU25/exec";

void setup() {

  Serial.begin(115200);  

  dht.begin();

  initWifi();
}

void loop() {

  if (millis() - timeRead > 4000) {
    
    temp = dht.readTemperature();
    hum = dht.readHumidity();

    Serial.print("Temperature: ");
    Serial.print(temp);
    Serial.write("\xC2\xB0");  
    Serial.println("C");
    Serial.print("Humidity: ");
    Serial.print(hum);
    Serial.println("%");

    sendToGoogleSheet();

    timeRead = millis();
  }
}

void initWifi() {
  Serial.print("Connecting to: ");
  Serial.print(ssid);

  WiFi.begin(ssid, password);

  int timeout = 10 * 4;  //10 segundos
  while (WiFi.status() != WL_CONNECTED && (timeout-- > 0)) {
    delay(250);
    Serial.print(".");
  }
  Serial.println("");

  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Failed to connect");
  }

  Serial.print("WiFi connected with IP address: ");
  Serial.println(WiFi.localIP());
}

void sendToGoogleSheet() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(serverName);
    http.addHeader("Content-Type", "application/json");

    String jsonData = "{\"temp\":\"" + String(temp) + "\", \"hum\":\"" + String(hum) + "\"}";

    int httpResponseCode = http.POST(jsonData);

    if (httpResponseCode > 0) {
      String response = http.getString();
      Serial.println(httpResponseCode);
      Serial.println(response);
    } else {
      Serial.print("Wrong request POST: ");
      Serial.println(httpResponseCode);
    }

    http.end();
  }
}
