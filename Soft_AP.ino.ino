#include <WiFi.h>

#if CONFIG_FREERTOS_UNICORE
static const BaseType_t app_cpu=0;
#else
static const BaseType_t app_cpu=1;
#endif

const char* ssid     = "G5";
static TaskHandle_t task_1=NULL;
WiFiServer server(80);

String header;

void SoftAP(void *parameter){
  Serial.print("Setting Access Point…");
  WiFi.softAP(ssid);

  IPAddress IP = WiFi.softAPIP();
  vTaskDelay(1000 / portTICK_PERIOD_MS);
  Serial.print("IP address: ");
  Serial.println(IP);
  server.begin();
  vTaskDelete(NULL);
}
void setup() {
  Serial.begin(115200);
  xTaskCreatePinnedToCore(SoftAP,"Wifi_Station",10000,NULL,1,&task_1,0);
}

void loop(){
  WiFiClient client = server.available();   // Listen for incoming clients

  if (client) {                             // If a new client connects,
    Serial.println("New Client.");          // print a message out in the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected()) {            // loop while the client's connected
      if (client.available()) {             // if there's bytes to read from the client,
         //Web Interface
    }
    header = "";
    // Close the connection
    client.stop();
    Serial.println(header);
    Serial.println("Client disconnected.");
    Serial.println("");
  }
  }
}
