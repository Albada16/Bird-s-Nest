
#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <ESPmDNS.h>
 
const char* ssid = "Sapido_RB-1602G3_d526bf";
const char* password = "";
int fsr_value=0;
int FV=1;
int SC=0;
int ST=0;
const int led_pin = 23;
const int fsr_pin = A0;
unsigned long timer=0;
String website;
WebServer server(80);

void buildweb(){
        website += "<html>";
        website += "<head>\n";
        website += "<meta charset='UTF-8'/>\n";
        website += "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\"/>";
        website += "<meta http-equiv=\"X-UA-Compatible\" content=\"ie=edge\">";
        website += "<script src=\"https://code.jquery.com/jquery-2.2.4.min.js\"></script>\n";
        website += "<script>";
        website += "setInterval(requestData, 1000);";
        website += "function requestData(){";
        website += "$.get(\"/flag\")\n";
        website += ".done(function(data){ ";
        website += " if(data){";
        website += " $(\"#score\").text(data.ss);\n";
        website += " if((data.ff)==2){";
        website += " $(\"#pic\").attr(\"src\",\"https://i.imgur.com/zrhLC1p.gif\");}\n";
        website += " else if((data.ff)==3){";
        website += " $(\"#pic\").attr(\"src\",\"https://i.imgur.com/2aEPXTx.gif\");}\n";
        website += " else if((data.ff)==0){";
        website += " $(\"#pic\").attr(\"src\",\"https://i.imgur.com/alDbImp.gif\");}\n";
        website += " }";
        website += "else {";
        website += " $(\"#pic\").attr(\"src\",\"?\");";
        website += " }\n";
        website += "})";
        website += ".fail(function(){";
        website += "console.log(\"The was a problem retrieving the data.\");";
        website += " });\n";
        website += "}\n";
        website += "</script>\n";
        website += "<title>Document</title>";
        website += "<style>";
        website += " html{margin:auto;}";
        website += ".pic{display:flex;justify-content:center;}";
        website += ".SC{display:flex;justify-content:center;font-family: Arial;font-weight:bold;font-size:30px; border:2px; border-radius:50px;margin-top:60px;margin-left:30%;margin-bottom:60px;margin-right:30%;padding:-20px;background-color:rgb(255,187,38);color:white;}";
        website += "</style>";
        website += "</head>";
        website += "<body>";
        website += "<div class=\"pic\"><img src=\"https://i.imgur.com/alDbImp.gif\" id=\"pic\" width='100%' height='60%'></div>";
        website += "<div class=\"SC\"><p>score: <span id=\"score\"></span></p></div>";
        website += "</body>";
        website += "</html>"; 
        Serial.print("WEB");
      }


void handleRoot() {
  server.send(200, "text/html", website);
    Serial.print("HR");
}

void handleNotFound() {
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
  Serial.print("NOT");
  Serial.print(" ");
}


void FLAG() {
  Serial.print("F");
  fsr_value = analogRead(fsr_pin); // 讀取FSR
  
  int flag = FV;
  int i = SC;
  
  String json = "{\"ff\":";
  json += flag;
  json += ",\"ss\":";
  json += i;
  json +="}";
  
  server.send(200, "application/json", json);
  
  Serial.println(fsr_value);
  if(fsr_value > 0)
   {
    digitalWrite(led_pin,HIGH);
    timer++;
    ST++;
    if(timer==20){FV=2; Serial.print("A");}
    if(timer==40){FV=3; Serial.print("B");}
    if(ST==40){SC++;ST=0;}
   }
  else {digitalWrite(led_pin,LOW); timer=0;FV=0;ST=0;}
  Serial.println("time=");
  Serial.println(timer);
  Serial.println(FV);
  Serial.println(SC);
  Serial.println("-------------");
  }
  
void setup(void)
{
  Serial.begin(115200);
  pinMode(led_pin, OUTPUT);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");
  buildweb();
    Serial.print("SUCCESS");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
    if (MDNS.begin("esp32")) {
    Serial.println("MDNS responder started");
  }
   server.on("/", handleRoot);
  server.on("/flag", FLAG);
  
  server.on("/inline", []() {
    server.send(200, "text/plain", "this works as well");
  });
  
  server.onNotFound(handleNotFound);
  
  server.begin();
  
  Serial.println("HTTP server started");
 
}


void loop(void)
{
  server.handleClient();
}
