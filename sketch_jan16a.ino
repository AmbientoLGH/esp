#include <SocketIoClient.h>

#include <Adafruit_NeoPixel.h>
#include <ESP8266WiFi.h>
#ifdef _AVR_
#include <avr/power.h>
#endif
#include <ESP8266WebServer.h>
#define PIN 4 
#define NUMPIXEL 28  // Anzahl der LEDs im Streifen

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXEL, PIN, NEO_GRB + NEO_KHZ800);

SocketIoClient socket;

ESP8266WebServer server(80);

void setup() { // Verbinden mit Kernmodul und Einrichten des LED-Streifens
  Serial.begin(115200);
  Serial.println("Initialization");

  pixels.begin();
  setColor(255,255,255,255);

  connect();
  socket.begin("192.168.43.107", 3000);

  socket.on("connect",connectionSuccessful);
  socket.on("error",connectionError);
  socket.on("ambiento",connectionAmbiento);
}

void connect() { //Einbeziehung der API durch eine WLAN-Verbindung //Steuerung über API
  if(WiFi.status() !=WL_CONNECTED) {
    WiFi.begin ("L OnePlus 5T", "22111999");

    while(WiFi.status() != WL_CONNECTED) {
      Serial.println ("Connecting... Status: "+WiFi.status());
      delay(200);
    }

    Serial.println("WiFi connected.");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
  }
}

void connectionSuccessful(const char * payload, size_t length) {
  Serial.println("Socket connected");
  socket.emit("device","{\"type\":\"esp\"}");
}

void connectionError(const char * payload, size_t length) {
  Serial.println("Socket Error");
}

void connectionAmbiento(const char * payload, size_t length) { // Verarbeiten der eingegebenen Befehlsdaten
  String command = String(payload);
  char string[50];

  command.toCharArray(string,50);
  
  char split[] = ";";
  char* items = strtok(string, split);

  String type = "";
  int brightness = 0;
  int red = 0;
  int green = 0;
  int blue = 0;
  int lights = 100;

  int index = 0;

  while(items != NULL) {
   if(index==0) type = items;
   if(index==1) brightness = atoi(items);
   if(index==2) red = atoi(items);
   if(index==3) green = atoi(items);
   if(index==4) blue = atoi(items);
   if(index==5) lights = atoi(items);

   items = strtok(NULL,split);
   index++;
  }

  if(type=="color") {
    updateColor(brightness,red,green,blue,lights);
  } else if(type=="on") {
    setColor(255,255,255,255);
  } else if(type=="off") {
    setColor(0,0,0,0);
  } else if(type=="chain-reaction") {
    chain_reaction();
    setColor(255,255,255,255);
  } else if(type=="blink") {
    blinken();
    setColor(255,255,255,255);
  }
}

void loop() { //Steuern der Kettenreaktion über API
  connect();
  socket.loop();
  //chain_reaction();
}

void blinken()    //Funktion blinken
{
  //rgb(26,188,156)
  setColor(255, 26, 188,156);
  delay(300);
  //rgb(44,62,80)
  setColor(255, 44, 62, 80);
  delay(300);
  //rgb(192,57,43)
  setColor(255, 192, 57, 43);
  delay(300);
  //rgb(211,84,0)
  setColor(255, 211, 84, 0);
  delay(300);
  //rgb(241,196,15)
  setColor(255, 241, 196, 15);
  delay(300);
  //rgb(52,152,219)
  setColor(255, 52, 152,219);
  delay(300);
  //rgb(46,204,113)
  setColor(255, 46, 204, 113);
  delay(300);
  //rgb(142,68,173)
  setColor(255, 142, 68, 173);
  delay(300);
  //rgb(189,195,199)
  setColor(255, 189, 195, 199);
  delay(300);
  //rgb(46,204,113)
  setColor(255, 46, 204, 113);
  delay(300);
}

void updateColor(int brightness, int r, int g, int b, int lights){ //Steuern der verschiedenen Farben mit API
  if(lights > NUMPIXEL) {
    lights = NUMPIXEL; 
  }

  for(int i = 0; i < lights; i++){
    pixels.setPixelColor(i,r,g,b);
  }
  for(int i = lights; i < NUMPIXEL; i++){
    pixels.setPixelColor(i,0,0,0);
  }
  pixels.setBrightness(brightness);
  pixels.show();
}

void setColor(int brightness, int r, int g, int b) { 
  for(int i = 0; i < NUMPIXEL; i++){
    pixels.setPixelColor(i,r,g,b);
  }

  pixels.setBrightness(brightness);
  pixels.show();  
}

void setLight(int position, int r, int g, int b){
  pixels.setPixelColor(position,r,g,b);
  pixels.show();
}

void chain_reaction() 
{
  for(int i = 0; i < NUMPIXEL; i++)//schaltet LEDs der Reihe nach an
  {
    setLight(i, random(0, 255), random(0, 255), random(0, 255));
    delay(50);
  }

  for(int i = NUMPIXEL; i > 0; i--) //schaltet LEDs der Reihe nach aus
  {
    setLight(i, 0, 0, 0);
    delay(50);
  }

}
  
