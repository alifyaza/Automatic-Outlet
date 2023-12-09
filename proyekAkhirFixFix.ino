#define BLYNK_TEMPLATE_ID "TMPL6Z3pkF4mf"
#define BLYNK_TEMPLATE_NAME "Test"
#define BLYNK_AUTH_TOKEN "gPoFT9pxs7VIZdtQET7TVl_ORvA2b1Ko"

#include <BlynkSimpleEsp32.h>
#include <PZEM004Tv30.h>
#include <WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

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
const int relayPin = 26;
const int relayPin2 = 27;

float current=0;
float voltage = 0;
float power = 0;
float energy = 0;
float frequency = 0;
float pf = 0;
float prevCurrent = 0;
bool isOn = false;
char hours[3]={'0','0','0'};
char minutes[3]={'0','0','0'};
int numHours=0;
int numMinutes=0;
int offHour=0;
int offMin=0;
bool isOnRequested = false;
unsigned long lastBlynkWriteTime = 0;
const unsigned long debounceDelay = 1000;

char* ssid = "vivo Y53s";
char* passwd = "12345678";
const char *ntpServer = "pool.ntp.org";
const long gmtOffset_sec = 3600*7; // GMT offset in seconds (for example, GMT+1 = 3600 seconds)
const int daylightOffset_sec = 0; // Daylight offset in seconds (for example, DST=1 hour = 3600 seconds)
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, ntpServer, gmtOffset_sec, daylightOffset_sec);

BLYNK_WRITE(onOffPin){
  if(param.asInt()==1){
    if(!isOn){
      isOn = true;
      Serial.println("BLYNK WRITE V0 : device on");
    }
    else{
      Serial.println("BLYNK WRITE V0 : cannot on");
    }
  }
  else{
    if(isOn){
      isOn = false;
      Serial.println("BLYNK WRITE V0 : device off");
    }
    else{
      Serial.println("BLYNK WRITE V0 : cannot off");
    }
  }
}

BLYNK_WRITE(putHours){
  if(isOn){
    offHour=param.asInt();
    Serial.println("putHours " +offHour);
    // if(offHour == numHours && offMin == numMinutes){
    //   isOn = false;
    // }
  } 
}

BLYNK_WRITE(putMin){
  if(isOn){
    offMin=param.asInt();
    //Serial.println("putMin " +offMin);
    // if(offHour == numHours && offMin == numMinutes){
    //   isOn = false;
    // }
  } 
}

// void callbackTimer(TimerHandle_t timeoutTimer) {
//   if (isOn) {
//     isOn = false;
//     Serial.println("Relay off from timer");
//   }
// }

void updateNTPTime(TimerHandle_t xTimer) {
  numMinutes++;
  if(numMinutes == 60){
    numHours++;
    numMinutes=0;
  }
  if(numHours==24){
    numHours=0;
  }
  if(offHour == numHours && offMin == numMinutes){
    isOn = false;
  }
  Serial.print(numHours);
  Serial.print(":");
  Serial.print(numMinutes);
  Serial.print("   ");
  Serial.print(offHour);
  Serial.print(":");
  Serial.println(offMin);
}

void relayCtr(void *pvParameters) {
  while (1) {
    if (isOn) {
      //isOn=true;
      digitalWrite(relayPin, LOW);  //nyalain relay
      digitalWrite(relayPin2, LOW);
      Serial.println("relayCtr: device on");
    } else {
      //isOn=false;
      digitalWrite(relayPin, HIGH);
      digitalWrite(relayPin2, HIGH);
      Serial.println("relayCtr: device off");
    }
    vTaskDelay(pdMS_TO_TICKS(2000));
  }
}

void readSensor(void *pvParameters) {
  while (1) {
    if (isOn) {
      if (xSemaphoreTake(xSemaphore, portMAX_DELAY) == pdTRUE) {
        voltage = pzem.voltage();
        current = pzem.current();
        power = pzem.power();
        energy = pzem.energy();
        frequency = pzem.frequency();
        xSemaphoreGive(xSemaphore);
      }
      Serial.print("readSensor voltage: ");
      Serial.println(voltage);
      // if (current == 0.00 && prevCurrent!= 0.00) {
      //   xTimerStart(timeoutTimer, 0);
      //   Serial.println("start timer");
      // } else if(current != 0.00) {
      //   xTimerReset(timeoutTimer, 0);
      //   Serial.println("reset timer");
      // }
      //prevCurrent=current;
    } else {
      Serial.println("readSensor:device off");
    }
    vTaskDelay(pdMS_TO_TICKS(2000));
  }
}

void sendData(void *pvParameters) {
  String toSendAV = "--";
  String toSendJW = "--";
  while (1) {
    if (isOn) {
      if (xSemaphoreTake(xSemaphore, portMAX_DELAY) == pdTRUE) {
        toSendAV = String(voltage) + "V-" + String(current) + "A , device on";
        Serial.println(toSendAV);
        Blynk.virtualWrite(displayAV, toSendAV);
        toSendJW = String(power) + "W-" + String(energy) + "kWh, device on";
        Serial.println(toSendJW);
        Blynk.virtualWrite(displayWJ, toSendJW);
        xSemaphoreGive(xSemaphore);
      }
    } else {
      Serial.println("send data :device off");
      toSendJW="device off";
      Blynk.virtualWrite(displayAV, toSendJW);
      Blynk.virtualWrite(displayWJ, toSendJW);
    }
    vTaskDelay(pdMS_TO_TICKS(2000));
  }
}

void serialTask(void *parameter) {
  while (1) {
    if (Serial.available() > 0) {
      char inputChar = Serial.read();
      if (inputChar == '1') {
        isOn = true;
      } else if (inputChar == '0') {
        isOn = false;
      } 
      delay(100);  
    }
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}

void checkWiFi(void *pvParameters) {
 while (1) {
   if (WiFi.status() != WL_CONNECTED) {
     Serial.println("WiFi disconnected. Attempting to reconnect...");
     WiFi.begin(ssid, passwd);
     while (WiFi.status() != WL_CONNECTED) {
       delay(1000);
       Serial.print(".");
     }
     Serial.println("WiFi reconnected.");
   }
   vTaskDelay(pdMS_TO_TICKS(5000)); // Check every 5 seconds
 }
}

void setupWiFi() {
  WiFi.begin(ssid, passwd);
  Serial.println("Connecting to WiFi...");

  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    delay(1000);
    Serial.print(".");
    attempts++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nConnected to WiFi");
    timeClient.begin();
    timeClient.update();
    snprintf(hours, sizeof(hours), "%02d", timeClient.getHours());
    snprintf(minutes, sizeof(minutes), "%02d", timeClient.getMinutes());
    numHours = String(hours).toInt();
    numMinutes = String(minutes).toInt();
  } else {
    Serial.println("\nFailed to connect to WiFi");
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(relayPin, OUTPUT);
  pinMode(relayPin2, OUTPUT);
  
  xSemaphore = xSemaphoreCreateMutex();
  ntpUpdateTimer = xTimerCreate("timeout", pdMS_TO_TICKS(60000), pdTRUE, (void *)0, updateNTPTime);
  if (ntpUpdateTimer != NULL) {
    xTimerStop(ntpUpdateTimer, 0);
  } else {
    Serial.println("fail to create timer");
  }

  xTimerStart(ntpUpdateTimer, 0);
  setupWiFi();
  Blynk.begin(BLYNK_AUTH_TOKEN,ssid,passwd);

  xTaskCreatePinnedToCore(relayCtr, "for relay", 1024, NULL, 1, NULL, 0);
  xTaskCreatePinnedToCore(sendData, "for print", 4096, NULL, 1, NULL, 1);
  xTaskCreatePinnedToCore(readSensor, "for sensor", 1024, NULL, 1, NULL, 1);
  xTaskCreatePinnedToCore(serialTask, "for input", 1024, NULL, 1, NULL, 0);
  xTaskCreatePinnedToCore(checkWiFi, "for wifi", 4096, NULL, 1, NULL, 0);
  //xTaskCreatePinnedToCore(blynkTask, "for input", 1024, NULL, 0, NULL, 1);
}

void loop() {
  Blynk.run();
  delay(1000);
}