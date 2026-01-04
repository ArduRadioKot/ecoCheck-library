#ifndef ECOCHECK_H
#define ECOCHECK_H

#if defined(ESP8266)
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <EEPROM.h>
#define PLATFORM_WEBSERVER ESP8266WebServer
#elif defined(ESP32)
#include <WiFi.h>
#include <WebServer.h>
#include <EEPROM.h>
#define PLATFORM_WEBSERVER WebServer
#else
#error "Platform not supported. Use ESP8266 or ESP32"
#endif

struct WiFiConfig {
  char ssid[32];
  char password[64];
  bool configured;
};

class EcoCheck {
public:
  EcoCheck(const char* serverIP, int serverPort, const char* deviceID = "esp01");
  
  void begin();
  
  void loop();
  
  void setTemperature(float value);
  void setHumidity(float value);
  void setAQI(int value);
  void setTVOC(float value);
  void setECO2(float value);
  void setCO(float value);
  void setAlcohol(float value);
  void setCO2(float value);
  void setToluene(float value);
  void setAmmonia(float value);
  void setAcetone(float value);
  void setPM25(float value);
  void setPM10(float value);
  void setDustDensity(float value);
  void setUVIndex(float value);

  bool sendData();
  
  // Автоматическая отправка данных (включить/выключить)
  void setAutoSend(bool enabled, unsigned long interval = 10000);

  bool isWiFiConnected();

  IPAddress getLocalIP();

  void resetWiFiConfig();
  void setAPName(const char* apSSID, const char* apPassword = "12345678");

private:
  const char* _serverIP;
  int _serverPort;
  const char* _deviceID;

  const char* _apSSID;
  const char* _apPassword;
  

  PLATFORM_WEBSERVER* _server;
  WiFiConfig _wifiConfig;
  
  // Флаги состояний
  bool _isConfigured;
  bool _shouldReboot;
  unsigned long _lastDataSend;
  unsigned long _dataInterval;
  bool _autoSendEnabled;
  
  // Хранилище данных датчиков
  struct SensorData {
    float temperature;
    float humidity;
    int aqi;
    float tvoc;
    float eco2;
    float co;
    float alcohol;
    float co2;
    float toluene;
    float nh4;
    float acetone;
    float pm25;
    float pm10;
    float dust_density;
    float uv_index;
    bool dataValid;
    unsigned long lastUpdate;
  };
  
  // Флаги установленных параметров
  struct SensorFlags {
    bool temperature;
    bool humidity;
    bool aqi;
    bool tvoc;
    bool eco2;
    bool co;
    bool alcohol;
    bool co2;
    bool toluene;
    bool nh4;
    bool acetone;
    bool pm25;
    bool pm10;
    bool dust_density;
    bool uv_index;
  };
  
  SensorData _currentData;
  SensorFlags _sensorFlags;
  
  // Приватные методы
  void setupAP();
  void handleSaveConfig();
  void loadConfig();
  void saveConfig();
  void connectToWiFi();
  bool sendDataToServer();
};

#endif

