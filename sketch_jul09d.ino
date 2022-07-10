#include <Arduino.h>
#if defined(ESP32)
  #include <WiFi.h>
#elif defined(ESP8266)
  #include <ESP8266WiFi.h>
#endif
#include <Firebase_ESP_Client.h>


#include "addons/TokenHelper.h"
#include "addons/RTDBHelper.h"

#define WIFI_SSID "Imantha"
#define WIFI_PASSWORD "12345678"

#define API_KEY "t5IOFAyUhyJ35mWadMVdGHsA7XiMPeAgRtOvQtFx"
#define DATABASE_URL "firebase-adminsdk-wclum@rtos-99b7e.iam.gserviceaccount.com" 


FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

unsigned long sendDataPrevMillis = 0;
int intValue;
float floatValue;
bool signupOK = false;

const int buzzerPin = 14;  // 6 corresponds to GPIO14
const int ledPin  = 12;  // 5 corresponds to GPIO12
const int pir = 2;
unsigned int PIR;

int pirValue;
const int freq = 5000;
const int ledChannel = 5;
const int buzzerChannel = 6;
const int resolution = 8;

String Motion;
String LED;
String Buzzer;

int dutycycle=125; 

TaskHandle_t TaskHandle=NULL;

void PWM(void *parameters) {
  while(1){
      ledcWrite(ledChannel,    dutycycle);
      ledcWrite(buzzerChannel, dutycycle);
      vTaskDelay(1500 / portTICK_PERIOD_MS);
  }
  vTaskDelete(NULL);
}

void Handle(void * parameters){
  while(1){
     pirValue=digitalRead(2);
     if(pirValue==HIGH){
        Serial.println("Detected");
        dutycycle=256;
        xTaskCreatePinnedToCore(PWM,"Function Detected",1024,NULL,1,&TaskHandle,0);    
    } 
    else{
        Serial.println("not Detected");
        dutycycle=0;
    } 
  }
}

void Task1(void *parameters){
  
   PIR = digitalRead(pir);
   if(pir == 1){
   if (Firebase.ready() && signupOK && (millis() - sendDataPrevMillis > 15000 || sendDataPrevMillis == 0)){
    sendDataPrevMillis = millis();
    if (Firebase.RTDB.getString(&fbdo, "/Buzzer/")) {
      if (fbdo.dataType() == "ON") {
        Buzzer = fbdo.intData();
        Serial.println(Buzzer);
        for(int dutyCycle = 0; dutyCycle <= 255; dutyCycle++){   
        // changing the LED brightness with PWM
        ledcWrite(buzzerChannel, dutyCycle);
        delay(15);
        }
      }else if (fbdo.dataType() == "OFF") {
          Buzzer = fbdo.intData();
          Serial.println(Buzzer);
          for(int dutyCycle; dutyCycle = 0;){   
          // changing the LED brightness with PWM
          ledcWrite(buzzerChannel, dutyCycle);
          delay(15);
        }
      }
    }
    if (Firebase.RTDB.getString(&fbdo, "/LED/")) {
      if (fbdo.dataType() == "ON") {
        LED = fbdo.intData();
        Serial.println(LED);
        for(int dutyCycle = 0; dutyCycle <= 255; dutyCycle++){   
        // changing the LED brightness with PWM
        ledcWrite(ledChannel, dutyCycle);
        delay(15);
        }
      }else if (fbdo.dataType() == "OFF"){
        LED = fbdo.intData();
        Serial.println(LED);
        for(int dutyCycle = 0; dutyCycle = 0;){   
        // changing the LED brightness with PWM
        ledcWrite(ledChannel, dutyCycle);
        delay(15);
       }          
      }
    }
    if (Firebase.RTDB.getString(&fbdo, "/Motion/")) {
      if (fbdo.dataType() == "Detected") {
        Motion = fbdo.intData();
        Serial.println(Motion);
      }
    }   
    }
   }
}
void setup() {
  Serial.begin(115200);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  pinMode(pir, INPUT); 

  /* Assign the api key (required) */
  config.api_key = API_KEY;

  /* Assign the RTDB URL (required) */
  config.database_url = DATABASE_URL;

  /* Sign up */
  if (Firebase.signUp(&config, &auth, "", "")) {
    Serial.println("ok");
    signupOK = true;
  }
  else {
    Serial.printf("%s\n", config.signer.signupError.message.c_str());
  }
  config.token_status_callback = tokenStatusCallback; //see addons/TokenHelper.h

  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);

  ledcSetup(ledChannel, freq, resolution);
  ledcAttachPin(ledPin, ledChannel);
  ledcSetup(buzzerChannel, freq, resolution);
  ledcAttachPin(buzzerPin, buzzerChannel);
  xTaskCreatePinnedToCore(Handle,"Control_Mech",10000,NULL,1,NULL,1);
  xTaskCreatePinnedToCore(Task1,"Task1",10000,NULL,1,NULL,0); 
//  xTaskCreatePinnedToCore(Task2,"Task2",10000,NULL,2,NULL,1); 
}



void loop() {

}
