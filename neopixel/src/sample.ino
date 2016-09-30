//Contoh Espectro - RGB LED Controller via Web

#include <Adafruit_NeoPixel.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
//ga dipake
#ifdef __AVR__
#include <avr/power.h>
#endif

//Pilih PIN RGB
// On a Trinket or Gemma we suggest changing this to 1
#define PIN 3

//Jumlah neopixel led
#define NUMPIXELS 1

//Wifi name & pass
const char *ssid = "wifi berbayar";
const char *password = "123qweasd";

//for web server
MDNSResponder mdns;
ESP8266WebServer server(80);

// When we setup the NeoPixel library, we tell it how many pixels, and which pin to use to send signals.
// Note that for older NeoPixel strips you might need to change the third parameter--see the strandtest
// example for more information on possible values.
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

void setup()
{
// This is for Trinket 5V 16MHz, you can remove these three lines if you are not using a Trinket
#if defined(__AVR_ATtiny85__)
    if (F_CPU == 16000000)
        clock_prescale_set(clock_div_1);
#endif
// End of trinket special code

    //connect wifi
    Serial.begin(115200);
    WiFi.begin(ssid, password);
    Serial.println("");
    // Wait for connection
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }
    Serial.println("");
    Serial.print("Connected to ");
    Serial.println(ssid);
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
    if (mdns.begin("esp8266", WiFi.localIP()))
    {
        Serial.println("MDNS responder started");
    }
    server.on("/", []() { handleRoot(); });
    server.on("/white", []() {setColor(255,255,255); handleRoot(); });
    server.on("/red", []() {setColor(150,0,0); handleRoot(); });
    server.on("/green", []() {setColor(0,150,0); handleRoot(); });
    server.on("/blue", []() {setColor(0,0,150); handleRoot(); });
    server.on("/black", []() {setColor(0,0,0); handleRoot(); });
    server.onNotFound(handleNotFound);
    server.begin();
    Serial.println("HTTP server started");
    pixels.begin(); // This initializes the NeoPixel library.
}

void setColor(int R, int G, int B)
{
    //cetak ke serial jika ada pergantian warna dari web
    Serial.println("Set Color R:"+String(R)+" G:"+String(G)+" B:"+String(B));
    // pixels.Color takes RGB values, from 0,0,0 up to 255,255,255
    //set warna led ke 0
    pixels.setPixelColor(0, pixels.Color(R, G, B));

    pixels.show(); // This sends the updated pixel color to the hardware.

    //for testing only
    //delay(100);
}

void handleRoot()
{
    String out = "<html><head><title>Control RGB LED</title></head>";
    out += "<body style='background-color:gray'>";
    out += "<span style='display:block; width:100%; font-size:2em; font-family:Verdana; text-align:center'>Pilih Warna Bray</span><br/>";
    out += "<a href='white'><span style='display:block; background-color:white; width:100%; height:6em;'></span></a><br/>";
    out += "<a href='red'><span style='display:block; background-color:red; width:100%; height:6em;'></span></a><br/>";
    out += "<a href='green'><span style='display:block; background-color:green; width:100%; height:6em;'></span></a><br/>";
    out += "<a href='blue'><span style='display:block; background-color:blue; width:100%; height:6em;'></span></a><br/>";
    out += "<a href='black'><span style='display:block; background-color:black; width:100%; height:6em;'></span></a>";
    out += "</body>";
    out += "</html>";
    server.send(200, "text/html", out);
}

void handleNotFound()
{
    String message = "File Not Found\n\n";
    message += "URI: ";
    message += server.uri();
    message += "\nMethod: ";
    message += (server.method() == HTTP_GET) ? "GET" : "POST";
    message += "\nArguments: ";
    message += server.args();
    message += "\n";
    for (uint8_t i = 0; i < server.args(); i++)
    {
        message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
    }
    server.send(404, "text/plain", message);
}
void loop()
{

    // Untuk Testing Saja
    /*
    for (int R = 0; R < 255; R++)
    {
        setColor(R, 0, 0);
    }
    for (int G = 0; G < 255; G++)
    {
        setColor(0, G, 0);
    }
    for (int B = 0; B < 255; B++)
    {
        setColor(0, 0, B);
    }*/
    mdns.update();
    server.handleClient();
}
