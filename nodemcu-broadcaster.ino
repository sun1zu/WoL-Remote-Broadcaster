#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

// Settings
// Fill this out manually!
const char* ssid             = "Your-WIFI-name";
const char* password         = "Your-WIFI-Password";
const unsigned int localPort = 51234;                                          // A port for your NodeMCU to accept packets on
const byte mac[]             = {0xaa, 0xbb, 0xcc, 0xee, 0xff, 0xaa};           // MAC address of the device to boot up using WOL
const bool debug             = false;                                          // Disable for less resource usage

// Buffer for inbound packets
char packetBuffer[256]; 

WiFiUDP Udp;

void setup() {
  if(debug) {
    Serial.begin(115200);
    Serial.println();
  }

  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);


  WiFi.begin(ssid, password);

  if(debug) Serial.print("Connecting to WiFi");
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    if(debug) Serial.print(".");
  }
  
  if(debug) {
    Serial.println("\nWiFi connected");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
  }

  Udp.begin(localPort);
  
  if(debug) {
    Serial.print("Listening on port: ");
    Serial.println(localPort);
  }
  
  digitalWrite(LED_BUILTIN, HIGH);
}

void loop() {
  int packetSize = Udp.parsePacket();
  
  if (packetSize) {
    digitalWrite(LED_BUILTIN,  LOW);
    int len = Udp.read(packetBuffer, 255);
    if (len > 0) {
      packetBuffer[len] = 0;
    }

    if(debug) 
    {
      Serial.printf("Received packet of size %d from %s:%d\n", packetSize, Udp.remoteIP().toString().c_str(), Udp.remotePort());
      printCharArrayAsHex(packetBuffer, len);
    }
    
    int res = check(packetBuffer, len);
    if(res == 1 && debug) {
      Serial.println("Received packet is not a WOL packet, dropping");
    }
    else if (res == 2 && debug) {
      Serial.println("Received WOL packet targets wrong MAC, dropping");
    }
    else {
      if(debug) Serial.println("Received WOL packet is valid. Broadcasting...");
      IPAddress broadcastIP(192, 168, 0, 255);
    
      Udp.beginPacket(broadcastIP, localPort);
      Udp.write(packetBuffer, len);
      Udp.endPacket();

      if(debug) Serial.println("Packet forwarded to 192.168.0.255");
    }

    
    digitalWrite(LED_BUILTIN, HIGH);
  }
}

void printCharArrayAsHex(const char* array, int length) {
  if (!debug) return;
  
  for (int i = 0; i < length; i++) {
    
    if ((byte)array[i] < 0x10) {
      Serial.print("0");
    }
    
    Serial.print((byte)array[i], HEX);
    
    if (i < length - 1) {
      Serial.print(" ");
    }
  }
  Serial.println();
}


// Checks if it is WOL packet for my PC
// Exit codes:
// 0 - OK
// 1 - not a WOL packet
// 2 - wrong target MAC (not whitelisted)
int check(const char* array, int length) {
  // Check if packet is WOL
  if(length != 102) return 1;
  for (int i=0; i<6; i++) {
    if((byte)array[i] != 0xFF) return 1; 
  }

  // Check if MAC is correct
  int ptrs[16] = {6};
  for(int i=1; i<16; i++) ptrs[i]=ptrs[i-1]+6;

  for(int i=0; i<6; i++) {
    byte whitelistedMacByte = mac[i];

    for (int p=0; p<16; p++) {
      if(array[ptrs[p]] != whitelistedMacByte) {
        if(debug) Serial.printf("[DEBUG] mac[i]=%02X, array[ptrs[p]]=%02X, ptrs[p]=%d, p=%d\n", mac[i], array[ptrs[p]], ptrs[p], p);
        return 2;
      }
      ptrs[p]++;
    }
  }

  return 0;
}
