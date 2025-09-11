// This code implements a smart fingerprint attendance system using an ESP32 microcontroller.
// It connects to a WiFi network, interacts with a fingerprint sensor, and logs attendance data
// to a remote server. If the WiFi connection is unavailable, it stores the data locally
// in SPIFFS and uploads it when the connection is restored. The system also supports OTA
// updates for easy maintenance. 

// Author: Shah Ahmed Raad
// Github: https://github.com/Thunder10046?tab=repositories 


#include <SPIFFS.h>
#include <FS.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <Adafruit_Fingerprint.h>
#include "time.h"
#include <ArduinoOTA.h>  

const char* ntpServer = "pool.ntp.org"; //Network Time Protocol server for clock synchronization
const long  gmtOffset_sec = 6 * 3600; // Dhaka is UTC + 6   
const int   daylightOffset_sec = 0;   // Set DST offset if needed

#define mySerial Serial1
#define RXD2 16 // Interfacing with ESP32 
#define TXD2 17

const char* logFileName = "/sensor_log.txt"; // Local log file in SPIFFS 
const char* serverName = "http://192.168.0.103/attendance_app/index.php"; // Replace with your server URL. "192.168.0.103" replace with your IPv4 address


// WiFi credentials to connect. Use provider name and password. Also, ESP32 uses 2.40 Ghz wifi channel, so do try to connect with the router's low band channel only i.e. 2.40 Ghz
const char* ssid = "No access to Rabby"; 
const char* password = "Cseipe20"; 

int recordIndex = 0; //used to limit local storage to 50 records (used as a thershold/flag)
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial); 

uint8_t id; // ID for enrollment


void setupOTA() {  // OTA implementation
  ArduinoOTA.setHostname("ESP32-Attendance"); 

  ArduinoOTA.setPassword("Thunder_2003042"); 

  ArduinoOTA
    .onStart([]() {
      String type;
      if (ArduinoOTA.getCommand() == U_FLASH)
        type = "sketch";
      else // U_FS
        type = "filesystem";
      Serial.println("Start updating " + type);
    })
    .onEnd([]() {
      Serial.println("\nUpdate End");
    })
    .onProgress([](unsigned int progress, unsigned int total) {
      Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
    })
    .onError([](ota_error_t error) {
      Serial.printf("Error[%u]: ", error);
      if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
      else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
      else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
      else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
      else if (error == OTA_END_ERROR) Serial.println("End Failed");
    });

  ArduinoOTA.begin();
  Serial.println("OTA ready. IP address: " + WiFi.localIP().toString());
}

// To store data locally

void store_locally(String data) { 
  if (recordIndex < 50) { //first check the thershold is exceeding or not
    File logFile = SPIFFS.open(logFileName, "a");
    if (logFile) {
      logFile.print(data);
      logFile.print("\n");
      logFile.close();
      Serial.println("Stored locally: " + data);
      recordIndex++;
    } else {
      Serial.println("Failed to open log file for writing.");
    }
  } else {
    Serial.println("Local log full! Please connect to WiFi to sync.");
  }
}

// Retrieve & push all local data to mysql DB 

void read_logs() { //Upload locally stored data to the server when WiFi is available
  File logFile = SPIFFS.open(logFileName, "r");
  if (!logFile) {
    Serial.println("Failed to open log file for reading.");
    return;
  }

  Serial.println("Uploading locally stored data from SPIFFS to the DB...");
  HTTPClient http; 
  http.begin(serverName);
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");

  while (logFile.available()) {
    String postData = logFile.readStringUntil('\n');
    int httpResponseCode = http.POST(postData);

    if (httpResponseCode == 200) {
      Serial.println("Uploaded local record Successfully");
    } else {
      Serial.println("Failed to Update local record");
      http.end();
      logFile.close();
      return; 
    }
  }
  http.end();
  logFile.close();
  SPIFFS.remove(logFileName); // Clear log after upload
  recordIndex = 0;
}

uint8_t getFingerprintEnroll(uint8_t id) { //Enroll fingerprint with given ID
  int p = -1;
  Serial.print("Waiting for valid finger to enroll as #"); Serial.println(id);
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
  }

  // Convert image to template
  p = finger.image2Tz(1);
  if (p != FINGERPRINT_OK) { Serial.println("Image conversion failed"); return; }

  Serial.println("Remove finger");
  delay(2000);

  // Wait for the same finger again
  p = -1;
  Serial.println("Place same finger again");
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
  }
  p = finger.image2Tz(2);
  if (p != FINGERPRINT_OK) { Serial.println("Second image conversion failed"); return; }

  // Create model
  p = finger.createModel();
  if (p != FINGERPRINT_OK) { Serial.println("Model creation failed"); return; }

  // Store model at given ID
  p = finger.storeModel(id);
  if (p == FINGERPRINT_OK) {
    Serial.print("Stored fingerprint at ID #"); Serial.println(id);
  } else {
    Serial.println("Storage failed");
  }
}

bool getFingerprintID() {
  if (finger.getImage() != FINGERPRINT_OK) return false;
  if (finger.image2Tz() != FINGERPRINT_OK) return false;
  if (finger.fingerSearch() != FINGERPRINT_OK) return false;
  Serial.print("Found ID #"); Serial.println(finger.fingerID);
  return true;
}

String getCurrentTime() { //Fetch current time from NTP server
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    Serial.println("Failed to obtain time");
    return "";
  }
  char timeString[25];
  strftime(timeString, sizeof(timeString), "%Y-%m-%dT%H:%M:%S", &timeinfo);
  return String(timeString);
}

void setup() {
  Serial.begin(115200); 

  if (!SPIFFS.begin(true)) {
    Serial.println("SPIFFS mount failed. Check your filesystem.");
    return;
  }
  Serial.println("SPIFFS mounted successfully.");

  WiFi.begin(ssid, password); 
  Serial.println("Connecting to WIFI....");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected, IP address: " + WiFi.localIP().toString());

  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  Serial.println("NTP time configured.");

  setupOTA(); // Initializing OTA

  mySerial.begin(57600, SERIAL_8N1, RXD2, TXD2); // Fingerprint sensor setup
  finger.begin(57600);
  if (finger.verifyPassword()) {
    Serial.println("Found fingerprint sensor!");
  } else {
    Serial.println("Did not find fingerprint sensor :(");
    while (1) { delay(1); }
  }

  Serial.println("Enter ID # (1-127) for enrollment:");
  while (!Serial.available());
  id = Serial.parseInt();
  if (id == 0 || id > 127) id = 1; // fallback to 1 if invalid

  Serial.println("Ready to enroll a fingerprint. Place finger on sensor.");
  while (getFingerprintEnroll(id) != FINGERPRINT_OK) {
    Serial.println("Retrying enrollment...");
    delay(1000);
  }
}

void loop() {

  ArduinoOTA.handle();  // Check for OTA updates 

  if (WiFi.status() == WL_CONNECTED && recordIndex > 0) {
    read_logs(); // First upload any locally stored data 
  }

  // Then send live data

  if (getFingerprintID()) {               
    String postData = "emp_id=" + String(finger.fingerID) + "&in_time=" + getCurrentTime();
    if (WiFi.status() == WL_CONNECTED) {
      HTTPClient http;
      http.begin(serverName);
      http.addHeader("Content-Type", "application/x-www-form-urlencoded");
      int httpResponseCode = http.POST(postData);

      if (httpResponseCode == 200) {
        Serial.println("Attendance logged to DB!");
      } else {
        Serial.println("Live send failed, storing locally");
        store_locally(postData);
      }
      http.end();
    } else {
      Serial.println("WiFi disconnected, storing locally");
      store_locally(postData);
    }

    Serial.println("Please remove finger...");
    while (finger.getImage() != FINGERPRINT_NOFINGER) {
      delay(100);
    }
    Serial.println("Finger removed. Ready for next scan.");
  }

  delay(200);
}