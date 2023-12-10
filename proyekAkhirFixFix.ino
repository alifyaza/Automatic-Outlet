#define BLYNK_TEMPLATE_ID "TMPL67H5zBm-Y"
#define BLYNK_TEMPLATE_NAME "Smart Socket"
#define BLYNK_AUTH_TOKEN "Wp_qZrDssJklxR_DLmgOimgbzdXNMya-"

#include <BlynkSimpleEsp32.h>
#include <PZEM004Tv30.h>
#include <WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <WiFiManager.h>

#if defined(ESP32)
PZEM004Tv30 pzem(Serial2, 16, 17);
#else
PZEM004Tv30 pzem(Serial2);
#endif

#define onOffPin V0
#define putHours V1
#define displayAV V2
#define displayWJ V3
#define putMin V4

SemaphoreHandle_t xSemaphore;
//TimerHandle_t timeoutTimer;
TimerHandle_t ntpUpdateTimer;
WiFiManager wifiManager;
const int relayPin = 26;
const int relayPin2 = 27;

static float current=0;
static float voltage = 0;
static float power = 0;
static float energy = 0;
static float frequency = 0;
static float pf = 0;
static bool isOn = false;
static char hours[3]={'0','0','0'};
static char minutes[3]={'0','0','0'};
static int numHours=0;
static int numMinutes=0;
static int offHour=0;
static int offMin=0;
static bool isOnRequested = false;
static unsigned long lastBlynkWriteTime = 0;
const unsigned long debounceDelay = 1000;
//static int quartMin_counter;

//char* ssid = "vivo Y53s";
//char* passwd = "12345678";
const char *ntpServer = "pool.ntp.org";
const long gmtOffset_sec = 3600*7; // GMT offset in seconds (for example, GMT+1 = 3600 seconds)
const int daylightOffset_sec = 0; // Daylight offset in seconds (for example, DST=1 hour = 3600 seconds)
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, ntpServer, gmtOffset_sec, daylightOffset_sec);

BLYNK_WRITE(onOffPin){
  //Serial.println(param.asInt());
  if(param.asInt()==1){
    
    isOn = true;
    //Serial.println("BLYNK WRITE V0 : device on");
    digitalWrite(relayPin, LOW);  //matiin relay
    digitalWrite(relayPin2, LOW);
    // else{
    //   //Serial.println("BLYNK WRITE V0 : cannot on");
    // }
  }
  else{
    isOn = false;
    //Serial.println("BLYNK WRITE V0 : device off");
    digitalWrite(relayPin, HIGH);
    digitalWrite(relayPin2, HIGH);
    // else{
    //   Serial.println("BLYNK WRITE V0 : cannot off");
    // }
  }
}

BLYNK_WRITE(putHours){
  offHour=param.asInt();
  //Serial.println("putHours " +offHour);
  //if(offHour == numHours && offMin == numMinutes){
    //   isOn = false;
    // }
   
}

BLYNK_WRITE(putMin){
  offMin=param.asInt();
  //Serial.println("putMin " +offMin);
    // if(offHour == numHours && offMin == numMinutes){
    //   isOn = false;
    // } 
}

// void callbackTimer(TimerHandle_t timeoutTimer) {
//   if (isOn) {
//     isOn = false;
//     Serial.println("Relay off from timer");
//   }
// }

void updateNTPTime(TimerHandle_t xTimer) {
  
  timeClient.update();
  snprintf(hours, sizeof(hours), "%02d", timeClient.getHours());
  snprintf(minutes, sizeof(minutes), "%02d", timeClient.getMinutes());
  numHours = String(hours).toInt();
  numMinutes = String(minutes).toInt();


  if(offHour == numHours && offMin == numMinutes){
    Serial.println("check");
    if(isOn){
      Serial.println("Dead");
      digitalWrite(relayPin, HIGH);  //matiin relay
      digitalWrite(relayPin2, HIGH);
      isOn = false;
    }
    else{
      Serial.println("Alive");
      digitalWrite(relayPin, LOW);  //matiin relay
      digitalWrite(relayPin2, LOW);
      isOn = true;
    }
    offHour = 0;
    offMin = 0;
  }
  Serial.print(numHours);
  Serial.print(":");
  Serial.print(numMinutes);
  Serial.print("   ");
  Serial.print(offHour);
  Serial.print(":");
  Serial.println(offMin);
}

void readSensor(void *pvParameters) {
  while (1) {
    // if (isOn) {
      if (xSemaphoreTake(xSemaphore, portMAX_DELAY) == pdTRUE) {
        voltage = pzem.voltage();
        current = pzem.current();
        power = pzem.power();
        energy = pzem.energy();
        //frequency = pzem.frequency();
        xSemaphoreGive(xSemaphore);
      }
      // Serial.print("readSensor voltage: ");
      // Serial.println(voltage);
      // if (current == 0.00 && prevCurrent!= 0.00) {
      //   xTimerStart(timeoutTimer, 0);
      //   Serial.println("start timer");
      // } else if(current != 0.00) {
      //   xTimerReset(timeoutTimer, 0);
      //   Serial.println("reset timer");
      // }
      //prevCurrent=current;
    // } else {
    //   Serial.println("readSensor:device off");
    // }
    vTaskDelay(pdMS_TO_TICKS(2000));
  }
}

void sendData(void *pvParameters) {
  String toSendAV = "--";
  String toSendJW = "--";
  while (1) {
    if (isOn) {
      if (xSemaphoreTake(xSemaphore, portMAX_DELAY) == pdTRUE) {
        toSendAV = String(voltage) + "V — " + String(current) + "A , device on";
        //Serial.println(toSendAV);
        Blynk.virtualWrite(displayAV, toSendAV);
        toSendJW = String(power) + "W — " + String(energy) + "kWh, device on";
        //Serial.println(toSendJW);
        Blynk.virtualWrite(displayWJ, toSendJW);
        xSemaphoreGive(xSemaphore);
      }
    } else {
      //Serial.println("send data :device off");
      toSendAV = String(voltage) + "V — --A , device off";
      toSendJW = "--W — " + String(energy) + "kWh, device off";
      Blynk.virtualWrite(displayAV, toSendAV);
      Blynk.virtualWrite(displayWJ, toSendJW);
    }
    vTaskDelay(pdMS_TO_TICKS(5000));
  }
}

// void serialTask(void *parameter) {
//   while (1) {
//     if (Serial.available() > 0) {
//       char inputChar = Serial.read();
//       if (inputChar == '1') {
//         isOn = true;
//       } else if (inputChar == '0') {
//         isOn = false;
//       } 
//       delay(100);  
//     }
//     vTaskDelay(pdMS_TO_TICKS(1000));
//   }
// }

void checkWiFi(void *pvParameters) {
 while (1) {
   if (WiFi.status() != WL_CONNECTED) {
     //Serial.println("WiFi disconnected. Attempting to reconnect...");
     while (WiFi.status() != WL_CONNECTED) {
       wifiManager.autoConnect("ESP32-AP");
     }
     
     //Serial.println("WiFi reconnected.");
     vTaskDelay(3000);
     Blynk.begin(BLYNK_AUTH_TOKEN, wifiManager.getWiFiSSID(false).c_str(), wifiManager.getWiFiPass(false).c_str());
     Blynk.syncVirtual(V0, V1, V4);
     //Serial.println("Blyn synced");
   }
   vTaskDelay(pdMS_TO_TICKS(5000)); // Check every 5 seconds
 }
}

void setupWiFi() {
  wifiManager.autoConnect("ESP32-AP");

  if (WiFi.status() == WL_CONNECTED) {
    //Serial.println("\nConnected to WiFi");
    timeClient.begin();
    timeClient.update();
    snprintf(hours, sizeof(hours), "%02d", timeClient.getHours());
    snprintf(minutes, sizeof(minutes), "%02d", timeClient.getMinutes());
    numHours = String(hours).toInt();
    numMinutes = String(minutes).toInt();
  } else {
    //Serial.println("\nFailed to connect to WiFi");
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(relayPin, OUTPUT);
  pinMode(relayPin2, OUTPUT);

  digitalWrite(relayPin, HIGH);  //matiin relay
  digitalWrite(relayPin2, HIGH);
  //isOn = false;

  wifiManager.setConfigPortalTimeout(30);
  
  xSemaphore = xSemaphoreCreateMutex();
  ntpUpdateTimer = xTimerCreate("timeout", pdMS_TO_TICKS(15000), pdTRUE, (void *)0, updateNTPTime);
  if (ntpUpdateTimer != NULL) {
    xTimerStop(ntpUpdateTimer, 0);
  } else {
    //Serial.println("fail to create timer");
  }

  xTimerStart(ntpUpdateTimer, 0);
  setupWiFi();
  Blynk.begin(BLYNK_AUTH_TOKEN, wifiManager.getWiFiSSID(false).c_str(), wifiManager.getWiFiPass(false).c_str());

  Blynk.syncVirtual(V0, V1, V4);
  // Serial.print("IsOn=");
  // Serial.println(isOn);


  //xTaskCreatePinnedToCore(relayCtr, "for relay", 1024, NULL, 1, NULL, 0);
  xTaskCreatePinnedToCore(sendData, "for print", 4096, NULL, 1, NULL, 1);
  xTaskCreatePinnedToCore(readSensor, "for sensor", 1024, NULL, 1, NULL, 1);
  //xTaskCreatePinnedToCore(serialTask, "for input", 1024, NULL, 1, NULL, 0);
  xTaskCreatePinnedToCore(checkWiFi, "for wifi", 4096, NULL, 0, NULL, 0);
  //xTaskCreatePinnedToCore(blynkTask, "for input", 1024, NULL, 0, NULL, 1);

  
}

void loop() {
  Blynk.run();
  vTaskDelay(1000);
}
