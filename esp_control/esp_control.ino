#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <DHT.h>
#define DHTTYPE DHT11

const int WaterPin = D1;
uint8_t DHTPin = D4; 
DHT dht(DHTPin, DHTTYPE);  
const int MoistPin = D2;
const int LightPin = D3;
const int analogpin = A0;
const int PumpPin = D0;
float Temperature = 0.0;
float Humidity = 0.0;
int Water = 0;
int Light = 0;
int Moist = 0;
const char* ssid = "Test";
const char* password = "121212121";
String host = "http://personal-farming.herokuapp.com/api/record";
String Data = "";
int Time = 0;
  
void setup(){
  Serial.begin(115200);
  ConnectWifi();
  pinMode(DHTPin, INPUT);
  dht.begin();
  pinMode(LightPin,OUTPUT); 
  pinMode(MoistPin,OUTPUT);
  pinMode(WaterPin,OUTPUT);
  pinMode(PumpPin, OUTPUT);
  digitalWrite(WaterPin, LOW);
  digitalWrite(MoistPin, LOW);
  digitalWrite(LightPin, LOW);
  digitalWrite(PumpPin, HIGH);
  Time = millis();
}



void ConnectWifi(){
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  for (int i = 0; i<100; i++){
    if (WiFi.status() == WL_CONNECTED){
      Serial.println("connected");
      break;
    }
    delay(500);
    Serial.print(".");
  }
  if (WiFi.status() != WL_CONNECTED){
    Serial.println("connection failed, reconneting...");
    ConnectWifi();
  }
  Serial.println("WiFi connected");
  Serial.println("IP address is: ");
  Serial.println(WiFi.localIP());
  return;
}

void PostData(){
  HTTPClient http;
  http.begin(host);
  http.addHeader("Content-Type", "text/plain");
  String temp = "water=";
  Data = temp + Water + "&" + "light=" + Light + "&" + "moisture=" + Moist + "&" + "temperature=" + Temperature + "&" + "humidity=" + Humidity + "&" + "key=Helloplants!";
  //DynamicJsonDocument doc(1024);
  //deserializeJson(doc, temp);
  //JsonObject obj = doc.as<JsonObject>();
  int httpcode = http.POST(Data);
  Serial.print("Response Code:"); 
  Serial.println(httpcode);   
  if (httpcode>0) { 
    String payload = http.getString();   
    Serial.println(payload);  
    //const int capacity = JSON_OBJECT_SIZE(2);
    //StaticJsonDocument<capacity> doc;
    //DeserializationError error = deserializeJson(doc, payload);
    //if (error) {
      //Serial.print(F("deserializeJson() failed with code "));
      //Serial.println(error.c_str());
    //}
    //const char* pump = doc["pump"];
    //const char* msg= doc["msg"];
    String command(payload); 
    PumpControl(command);
  }  
  else{
    Serial.println("POST failed.");
  }
  http.end();
}


void PumpControl(String pump){
  if (Time+12000>=millis()){  
    return;
  }
  if (pump == "on"){
    Serial.println("Turning on Pump");
    Time = millis();
    digitalWrite(PumpPin, LOW);
  }
  else{
    Serial.println("Turning off pump");
    digitalWrite(PumpPin, HIGH);
  }
}

void WaterControl(){
  digitalWrite(WaterPin, HIGH);
  Water = analogRead(analogpin);
  Serial.println("Water reading:");
  Serial.println(Water);
  delay(100);
  digitalWrite(WaterPin, LOW);
  delay(100);
}

void LightControl(){
  digitalWrite(LightPin, HIGH);
  Light = analogRead(analogpin);
  Serial.println("Light reading:");
  Serial.println(Light);
  delay(100);
  digitalWrite(LightPin, LOW);
  delay(100);
}

void MoistureControl(){
  digitalWrite(MoistPin, HIGH);
  Moist = analogRead(analogpin);
  Moist = map(Moist, 10, 500, 0, 100);
  Serial.println("Moist reading:");
  Serial.println(Moist);
  delay(100);
  digitalWrite(MoistPin, LOW);
  delay(100);
}
void TempControl(){
  Temperature = dht.readTemperature();
  Humidity = dht.readHumidity();
  Serial.println("Temperature reading:");
  Serial.println(Temperature);
  Serial.println("Humidity reading:");
  Serial.println(Humidity);
  delay(200);
}

void loop() {
  WaterControl();
  LightControl();
  TempControl();
  MoistureControl();
  PostData();
  delay(200);
}
