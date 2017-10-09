// Libraries
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiClient.h>
#include "FS.h"

// Hostname
#define HOSTNAME "esp8266"

// WiFi network created by ESP8266
const char* ssid = "esp8266-setup";

// Create server
WiFiServer server(80);

// Functions declaration

// connect to a network given an SSID and password
// returns 0 on success and -1 on failure
int connect_network(String wifiName, String wifiPassword);

// gets the esp8266 connected under any circumstance
void getConnected();

// makes esp8266 an AP and scans for networks
void networkScan();

void setup() {

  // Start Serial port
  Serial.begin(115200);
  delay(10);

  SPIFFS.begin();
}

void loop() {
  
  if(WiFi.status() != WL_CONNECTED)
    getConnected();
  
}

void getConnected() {

  // keep track of whether or not we were able to get a successful connection
  int success = -1;

  // try to get the ssid and password stored on EEPROM (filesystem)
  File ssidFile = SPIFFS.open("/ssid.txt", "r");
  File passFile = SPIFFS.open("/pass.txt", "r");
  if (ssidFile && passFile) {
      Serial.print("Successfully opened files");
      success = connect_network(ssidFile.readString(), passFile.readString());
  }

  ssidFile.close();
  passFile.close();

  // wait for a client to tell us SSID and password
  while(success == -1) {

    networkScan();
    
    // Check if a client has connected
    WiFiClient client = server.available();
    while(!client)
      client = server.available();
    Serial.println("");
    Serial.println("New client");
  
    // Wait for data from client to become available
    while(client.connected() && !client.available()){
      delay(1);
    }
    
    // Read the first line of the request
    String req = client.readStringUntil('\r');
    //Serial.println(req);
    client.flush();
    
    // Prepare the response
    String s = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n";
    s += "<html><head>";
    // s += "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">";
    // s += "<script src=\"https://code.jquery.com/jquery-2.1.3.min.js\"></script>";
    // s += "<link rel=\"stylesheet\" href=\"https://maxcdn.bootstrapcdn.com/bootstrap/3.3.4/css/bootstrap.min.css\">";
    s += "</head>";
    s += "<body><div class=\"container\">";
    
    // Match the request
    if (req.indexOf("/login") != -1){
  
      Serial.println("Connecting to local network");
  
      // Parse response
      String wifiNameParameter = "wifiName=";
      String wifiPasswordParameter = "wifiPassword=";
      int indexName = req.indexOf(wifiNameParameter);
      int indexPassword = req.indexOf(wifiPasswordParameter);
      int indexBoard = req.indexOf("board=");
  
      String wifiName = req.substring(indexName + wifiNameParameter.length(), indexPassword - 1);
      String wifiPassword = req.substring(indexPassword + wifiPasswordParameter.length(), indexBoard - 1);

      // save the SSID and password to EEPROM (filesystem)
      File ssidFile = SPIFFS.open("/ssid.txt", "w");
      File passFile = SPIFFS.open("/pass.txt", "w");
  
      ssidFile.print(wifiName);
      passFile.print(wifiPassword);
  
      ssidFile.close();
      passFile.close();
  
      Serial.print("WiFi name: ");
      Serial.println(wifiName);
      Serial.print("WiFi password: ");
      Serial.println(wifiPassword);
  
      // Build answer
      s += "<div>Logged in!</div>";
      s += "<div>Now please connect to the WiFi network called " + wifiName + "</div>";
      s += "</div></body></html>\r\n";
     
      // Send the response to the client
      client.print(s);
      delay(100);
      client.stop();
      Serial.println("Client disonnected");
      
      // Connect to other network
      success = connect_network(wifiName, wifiPassword);
  
      // Do other things 
      // Your code goes here
      
    }
    else {
      
      Serial.println("Serving interface");
    
      s += "<div>WiFi name: <input type='text' id='wname'></div>";
      s += "<div>WiFi password: <input type='text' id='pass'></div>";
      s += "<div><input class=\"btn\" type=\"button\" value=\"Validate\" onclick=\"login()\"></div>";
      s += "</div>";
   
      s += "<script>function login() {";
      s += "var wifiName = document.getElementById('wname').value;";
      s += "var wifiPassword = document.getElementById('pass').value;";
      s += "window.location.href = '/login?wifiName=' + wifiName + '&wifiPassword=' + wifiPassword + '&board=';}";
      s += "</script>";
      s += "</body></html>\r\n";
       
      // Send the response to the client
      client.print(s);
      delay(100);
      client.stop();
      Serial.println("Client disconnected");
    }
    
  }
  
}

void networkScan() {
  
  // Create WiFi network
    WiFi.mode(WIFI_AP);
    WiFi.softAP(ssid);
    
    // Start the server
    server.begin();
    Serial.println("Server started");
    Serial.println(WiFi.softAPIP());
  
    // Look for networks
    int n = WiFi.scanNetworks();
    Serial.println("scan done");
    if (n == 0)
      Serial.println("no networks found");
    else
    {
      Serial.print(n);
      Serial.println(" networks found");
      for (int i = 0; i < n; ++i)
      {
        // Print SSID and RSSI for each network found
        Serial.print(i + 1);
        Serial.print(": ");
        Serial.print(WiFi.SSID(i));
        Serial.print(" (");
        Serial.print(WiFi.RSSI(i));
        Serial.print(")");
        Serial.println((WiFi.encryptionType(i) == ENC_TYPE_NONE)?" ":"*");
        delay(10);
      }
    }
    Serial.println("");
}

int connect_network(String wifiName, String wifiPassword) {
  
  // Connect to other network
  WiFi.mode(WIFI_STA);
        
  const char* other_ssid = wifiName.c_str();
  const char* other_password = wifiPassword.c_str();

  WiFi.begin(other_ssid, other_password);
  Serial.println("");
  
  // Wait for connection
  for(int tries = 0; tries < 30; tries++) {

    if(WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    } else {
      Serial.println("");
      Serial.print("Connected to ");
      Serial.println(other_ssid);
      Serial.print("IP address: ");
      Serial.println(WiFi.localIP());
      return 0;
    }
    
  }

  return -1;
  
}

