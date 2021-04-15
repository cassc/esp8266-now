#include <ESP8266WiFi.h>
#include <espnow.h>


// REPLACE WITH THE MAC Address of your receiver
// target F4:CF:A2:ED:C4:5E
uint8_t broadcastAddress[] = {0xF4, 0xCF, 0xA2, 0xED, 0xC4, 0x5E};
// target wnfc F4:CF:A2:ED:C0:C3
// uint8_t broadcastAddress[] = {0xF4, 0xCF, 0xA2, 0xED, 0xC0, 0xC3};


typedef struct struct_message {
  String cmd;
} struct_message;


// Callback when data is sent
void OnDataSent(uint8_t *mac_addr, uint8_t sendStatus) {
  Serial.print("Last Packet Send Status: ");
  if (sendStatus == 0){
    Serial.println("Delivery success");
  }
  else{
    Serial.println("Delivery fail");
  }
}

String mac2String(uint8_t* ar) {
  String s;
  for (byte i = 0; i < 6; ++i)
    {
      char buf[3];
      sprintf(buf, "%02X", *(ar+i));
      s += buf;
      if (i < 5) s += ':';
    }
  return s;
}

struct_message recvMsg;

// Callback when data is received
void OnDataRecv(uint8_t * mac, uint8_t *incomingData, uint8_t len) {
  memcpy(&recvMsg, incomingData, sizeof(recvMsg));  
  Serial.print(" from ");
  Serial.print(mac2String(mac));
  Serial.print(", ");

  Serial.print(millis());
  Serial.print(", received: ");
  Serial.println(recvMsg.cmd);
  Serial.println(len);
}


void setup() {
  // Init Serial Monitor
  Serial.begin(115200);

  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);
  Serial.println("My Address:");
  Serial.println(WiFi.macAddress());
  WiFi.disconnect();

  // Init ESP-NOW
  if (esp_now_init() != 0) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Set ESP-NOW Role
  esp_now_set_self_role(ESP_NOW_ROLE_COMBO);

  // Once ESPNow is successfully Init, we will register for Send CB to
  // get the status of Trasnmitted packet
  esp_now_register_send_cb(OnDataSent);

  // Register peer
  esp_now_add_peer(broadcastAddress, ESP_NOW_ROLE_COMBO, 1, NULL, 0);

  // Register for a callback function that will be called when data is received
  esp_now_register_recv_cb(OnDataRecv);
}


const long interval = 50; 
unsigned long previousMillis = 0;    // will store last time DHT was updated 
unsigned long id = 0;

struct_message sendMsg;
void loop() {
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    // save the last time you updated the DHT values
    previousMillis = currentMillis;

    // Send message via ESP-NOW
    String cmd = String("hi: ") + (id++);
    Serial.print("Sending ");
    Serial.println(cmd);

    sendMsg.cmd = cmd;
    esp_now_send(broadcastAddress, (uint8_t *) &sendMsg, sizeof(sendMsg));

  }
}
