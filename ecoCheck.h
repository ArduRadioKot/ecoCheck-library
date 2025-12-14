#ifndef ECOCHECK_H
#define ECOCHECK_H

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <EEPROM.h>

// Структура для хранения конфигурации WiFi
struct WiFiConfig {
  char ssid[32];
  char password[64];
  bool configured;
};

// Класс библиотеки ecoCheck
class EcoCheck {
public:
  // Конструктор
  EcoCheck(const char* serverIP, int serverPort, const char* deviceID = "esp01");
  
  // Инициализация библиотеки
  void begin();
  
  // Основной цикл (должен вызываться в loop())
  void loop();
  
  // Установка данных датчиков
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
  
  // Отправка данных на сервер (ручная)
  bool sendData();
  
  // Автоматическая отправка данных (включить/выключить)
  void setAutoSend(bool enabled, unsigned long interval = 10000);
  
  // Получение статуса WiFi
  bool isWiFiConnected();
  
  // Получение IP адреса
  IPAddress getLocalIP();
  
  // Сброс конфигурации WiFi
  void resetWiFiConfig();
  
  // Установка имени устройства для AP режима
  void setAPName(const char* apSSID, const char* apPassword = "12345678");

private:
  // Настройки сервера
  const char* _serverIP;
  int _serverPort;
  const char* _deviceID;
  
  // Настройки AP
  const char* _apSSID;
  const char* _apPassword;
  
  // Сервер для конфигурации
  ESP8266WebServer* _server;
  
  // Конфигурация WiFi
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

