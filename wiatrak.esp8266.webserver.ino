#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#define ssid_wifi     "void"                                   // your home WiFi name
#define pw_wifi       "password"                               // your home WiFi password
const char *ssid_ap = "frrrrrr";                               // name of access point
const char *pw_ap = "";                                        // set password for access point
const int wemosPins[] = {5, 4, 0, 2, 14, 12, 13};              // map for the pinmode and digitalwrite loop, D1, D2 ... D7
int inc = 0;

MDNSResponder mdns;
ESP8266WebServer server(80);

String webPage;
String getPage(){
  String webPage = "<!DOCTYPE html><html><head><style>body {background-color: white;}";
  webPage += ".button {color: white; padding: 30px 90px; text-align: center; text-decoration: none; outline: none;";
  webPage += "display: inline-block; font-size: 72px; margin: 64px 18px;font-weight: 550; border: none; -webkit-transition-duration: 0.4s; transition-duration: 0.4s;}";
  webPage += ".button1 {background-color: #073763; width: 70%; border-radius: 48px;} .button1:hover {background-color: white; color: #073763; border: 6px solid #073763;}";
  webPage += ".button2 {background-color: #D41717; width: 70%; border-radius: 48px;} .button2:hover {background-color: white; color: #D41717; border: 6px solid #D41717;}";
  webPage += ".button3 {background-color: #E87508; width: 70%; border-radius: 48px;} .button3:hover {background-color: white; color: #E87508; border: 6px solid #E87508;}";
  webPage += ".button4 {background-color: #FED903; width: 70%; border-radius: 48px;} .button4:hover {background-color: white; color: #FED903; border: 6px solid #FED903;}";
  webPage += ".button5 {background-color: #79b700; width: 70%; border-radius: 48px;} .button5:hover {background-color: white; color: #79b700; border: 6px solid #79b700;}";
  webPage += ".button6 {background-color: #008CBA; width: 70%; border-radius: 48px;} .button6:hover {background-color: white; color: #008CBA; border: 6px solid #008CBA;}"; 
  webPage += "</style></head><body><center><br><br><br><br><br>";
  webPage += "<a href='power'><button class='button button1'>ON / OFF</button></a><br>";
  webPage += "<a href='moist'><button class='button button2'>MOIST</button></a><br>";
  webPage += "<a href='osc'><button class='button button3'>OSC</button></a><br>";
  webPage += "<a href='speed'><button class='button button4'>SPEED</button></a><br>";
  webPage += "<a href='timer'><button class='button button5'>TIMER</button></a><br>";
  webPage += "<a href='mode'><button class='button button6'>MODE</button></a><br></center></body></html>";
  return webPage;
}

void setup() {

  for (int pin = 0; pin < 7; pin++) {             
    pinMode(wemosPins[pin], OUTPUT);                           // set pins as ouputs 
    digitalWrite(wemosPins[pin], HIGH);                        // for D1 - set 4503 outputs to high impedance state, and rest of the wemos pins to High - no active
  }

  Serial.begin(115200);                                        // serial port for debugging
  WiFi.begin(ssid_wifi, pw_wifi);                              // connect to the local network
  WiFi.softAP(ssid_ap, pw_ap);                                 // start access point
 
  while (WiFi.status() != WL_CONNECTED && inc<60) {
    delay(500);
    inc++;
    Serial.print(".");
  }

  if (mdns.begin("fan", WiFi.localIP()))                       // start mDNS responder, http://fan.local  (not working on android)
    Serial.println("MDNS responder started"); 

  Serial.print("Local IP address: ");
  Serial.println(WiFi.localIP());
  IPAddress myIP = WiFi.softAPIP();
  Serial.print("Access Point IP address: ");
  Serial.println(myIP);

  server.on("/", [](){                                    
    server.send(200, "text/html", getPage());
    });
  server.on("/power", [](){                                    // ON / OFF button
    impulse(D2);
    });
  server.on("/moist", [](){                                    // MOIST button
    impulse(D3);
    });
  server.on("/osc", [](){                                      // OSC button
    impulse(D4);
    });
  server.on("/speed", [](){                                    // SPEED button
    impulse(D5);
    });
  server.on("/timer", [](){                                    // TIMER button
    impulse(D6);
    });
  server.on("/mode", [](){                                     // MODE button
    impulse(D7);
    });
   
  server.begin();
  Serial.println("HTTP server started");
}

void impulse(char port){
    digitalWrite(D1,LOW);                                      // disable 4503 HIGH Z outputs state  
    delay(10);
    digitalWrite(port,LOW);                                    // simulate push of micro switch, active LOW state
    delay(100);
    digitalWrite(port,HIGH);                                   // needed for fan's mcu, to properly change the state
    delay(10);
    digitalWrite(D1,HIGH);                                     // enable 4503 HIGH Z ouputs state  
    Serial.print("Pin: "); 
    Serial.println( (uint8_t) port);                           // arduino pin number to console
    server.sendHeader("Location", "/", true);                  // replace both lines with: 
    server.send(302, "text/plane","");                         // server.send(200, "text/plane", getPage()); if no need redirect to webpage root
}

void loop() {
  server.handleClient();
  }