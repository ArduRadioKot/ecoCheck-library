#include "ecoCheck.h"

const char* configPage = R"rawliteral(
<!DOCTYPE HTML>
<html>
<head>
  <title>ESP8266 WiFi Configuration</title>
  <meta charset="utf-8">
  <style>
    body { font-family: Arial; margin: 40px; }
    .container { max-width: 400px; margin: 0 auto; }
    input { width: 100%; padding: 10px; margin: 8px 0; box-sizing: border-box; }
    button { width: 100%; padding: 12px; background: #4CAF50; color: white; border: none; cursor: pointer; }
    button:hover { background: #45a049; }
  </style>
</head>
<body>
  <div class="container">
    <h2>WiFi Configuration</h2>
    <form action="/save" method="POST">
      <input type="text" name="ssid" placeholder="WiFi SSID" required>
      <input type="password" name="password" placeholder="WiFi Password" required>
      <button type="submit">Save & Connect</button>
    </form>
  </div>
</body>
</html>
)rawliteral";


EcoCheck::EcoCheck(const char* serverIP, int serverPort, const char* deviceID) 
  : _serverIP(serverIP), _serverPort(serverPort), _deviceID(deviceID),
    _apSSID("ESP8266_Config"), _apPassword("12345678"),
    _isConfigured(false), _shouldReboot(false), 
    _lastDataSend(0), _dataInterval(10000), _autoSendEnabled(true) {
  _server = new ESP8266WebServer(80);
  
  // Инициализация данных датчиков
  memset(&_currentData, 0, sizeof(_currentData));
  memset(&_sensorFlags, 0, sizeof(_sensorFlags));
}

// Инициализация библиотеки
void EcoCheck::begin() {
  Serial.begin(115200);
  EEPROM.begin(512);
  
  // Загрузка конфигурации из EEPROM
  loadConfig();
  
  if (_wifiConfig.configured) {
    // Подключение к WiFi
    connectToWiFi();
  } else {
    // Режим точки доступа для конфигурации
    setupAP();
  }
}

// Основной цикл
void EcoCheck::loop() {
  if (_shouldReboot) {
    delay(1000);
    ESP.restart();
  }
  
  if (_isConfigured) {
    // Режим клиента - отправка данных
    _server->handleClient();
    
    if (WiFi.status() == WL_CONNECTED) {
      // Автоматическая отправка данных
      if (_autoSendEnabled && (millis() - _lastDataSend > _dataInterval)) {
        sendDataToServer();
        _lastDataSend = millis();
      }
    } else {
      // Переподключение при потере соединения
      connectToWiFi();
    }
  } else {
    // Режим точки доступа
    _server->handleClient();
  }
}

// Настройка точки доступа
void EcoCheck::setupAP() {
  WiFi.mode(WIFI_AP);
  WiFi.softAP(_apSSID, _apPassword);
  
  // Настройка сервера
  _server->on("/", HTTP_GET, [this]() {
    _server->send(200, "text/html", configPage);
  });
  
  _server->on("/save", HTTP_POST, [this]() {
    handleSaveConfig();
  });
  
  _server->begin();
  Serial.println("AP Mode started");
  Serial.print("AP IP: ");
  Serial.println(WiFi.softAPIP());
}

// Обработка сохранения конфигурации
void EcoCheck::handleSaveConfig() {
  if (_server->hasArg("ssid") && _server->hasArg("password")) {
    String ssid = _server->arg("ssid");
    String password = _server->arg("password");
    
    // Сохранение в EEPROM
    strncpy(_wifiConfig.ssid, ssid.c_str(), sizeof(_wifiConfig.ssid) - 1);
    strncpy(_wifiConfig.password, password.c_str(), sizeof(_wifiConfig.password) - 1);
    _wifiConfig.ssid[sizeof(_wifiConfig.ssid) - 1] = '\0';
    _wifiConfig.password[sizeof(_wifiConfig.password) - 1] = '\0';
    _wifiConfig.configured = true;
    
    saveConfig();
    
    _server->send(200, "text/html", 
      "<html><body><h2>Configuration saved!</h2><p>Device will restart and connect to WiFi.</p></body></html>");
    
    _shouldReboot = true;
  } else {
    _server->send(400, "text/plain", "Missing SSID or Password");
  }
}

// Загрузка конфигурации из EEPROM
void EcoCheck::loadConfig() {
  EEPROM.get(0, _wifiConfig);
  if (_wifiConfig.configured) {
    _isConfigured = true;
  }
}

// Сохранение конфигурации в EEPROM
void EcoCheck::saveConfig() {
  EEPROM.put(0, _wifiConfig);
  EEPROM.commit();
}

// Подключение к WiFi
void EcoCheck::connectToWiFi() {
  Serial.print("Connecting to ");
  Serial.println(_wifiConfig.ssid);
  
  WiFi.mode(WIFI_STA);
  WiFi.begin(_wifiConfig.ssid, _wifiConfig.password);
  
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    delay(500);
    Serial.print(".");
    attempts++;
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nWiFi connected!");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("\nFailed to connect to WiFi");
    // Возврат в режим AP при неудаче
    setupAP();
    _isConfigured = false;
  }
}

// Установка температуры
void EcoCheck::setTemperature(float value) {
  _currentData.temperature = value;
  _sensorFlags.temperature = true;
  _currentData.dataValid = true;
  _currentData.lastUpdate = millis();
}

// Установка влажности
void EcoCheck::setHumidity(float value) {
  _currentData.humidity = value;
  _sensorFlags.humidity = true;
  _currentData.dataValid = true;
  _currentData.lastUpdate = millis();
}

// Установка AQI
void EcoCheck::setAQI(int value) {
  _currentData.aqi = value;
  _sensorFlags.aqi = true;
  _currentData.dataValid = true;
  _currentData.lastUpdate = millis();
}

// Установка TVOC
void EcoCheck::setTVOC(float value) {
  _currentData.tvoc = value;
  _sensorFlags.tvoc = true;
  _currentData.dataValid = true;
  _currentData.lastUpdate = millis();
}

// Установка eCO2
void EcoCheck::setECO2(float value) {
  _currentData.eco2 = value;
  _sensorFlags.eco2 = true;
  _currentData.dataValid = true;
  _currentData.lastUpdate = millis();
}

// Установка CO
void EcoCheck::setCO(float value) {
  _currentData.co = value;
  _sensorFlags.co = true;
  _currentData.dataValid = true;
  _currentData.lastUpdate = millis();
}

// Установка Alcohol
void EcoCheck::setAlcohol(float value) {
  _currentData.alcohol = value;
  _sensorFlags.alcohol = true;
  _currentData.dataValid = true;
  _currentData.lastUpdate = millis();
}

// Установка CO2
void EcoCheck::setCO2(float value) {
  _currentData.co2 = value;
  _sensorFlags.co2 = true;
  _currentData.dataValid = true;
  _currentData.lastUpdate = millis();
}

// Установка Toluene
void EcoCheck::setToluene(float value) {
  _currentData.toluene = value;
  _sensorFlags.toluene = true;
  _currentData.dataValid = true;
  _currentData.lastUpdate = millis();
}

// Установка Ammonia
void EcoCheck::setAmmonia(float value) {
  _currentData.nh4 = value;
  _sensorFlags.nh4 = true;
  _currentData.dataValid = true;
  _currentData.lastUpdate = millis();
}

// Установка Acetone
void EcoCheck::setAcetone(float value) {
  _currentData.acetone = value;
  _sensorFlags.acetone = true;
  _currentData.dataValid = true;
  _currentData.lastUpdate = millis();
}

// Установка PM2.5
void EcoCheck::setPM25(float value) {
  _currentData.pm25 = value;
  _sensorFlags.pm25 = true;
  _currentData.dataValid = true;
  _currentData.lastUpdate = millis();
}

// Установка PM10
void EcoCheck::setPM10(float value) {
  _currentData.pm10 = value;
  _sensorFlags.pm10 = true;
  _currentData.dataValid = true;
  _currentData.lastUpdate = millis();
}

// Установка Dust Density
void EcoCheck::setDustDensity(float value) {
  _currentData.dust_density = value;
  _sensorFlags.dust_density = true;
  _currentData.dataValid = true;
  _currentData.lastUpdate = millis();
}

// Установка UV Index
void EcoCheck::setUVIndex(float value) {
  _currentData.uv_index = value;
  _sensorFlags.uv_index = true;
  _currentData.dataValid = true;
  _currentData.lastUpdate = millis();
}

// Отправка данных на сервер (ручная)
bool EcoCheck::sendData() {
  return sendDataToServer();
}

// Отправка данных на сервер
bool EcoCheck::sendDataToServer() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi not connected");
    return false;
  }
  
  WiFiClient client;
  
  Serial.print("Connecting to server: ");
  Serial.print(_serverIP);
  Serial.print(":");
  Serial.println(_serverPort);
  
  if (!client.connect(_serverIP, _serverPort)) {
    Serial.println("Connection to server failed");
    return false;
  }
  
  // Проверяем, есть ли актуальные данные (получены менее 15 секунд назад)
  bool hasRecentData = (millis() - _currentData.lastUpdate < 15000);
  
  // Формирование POST данных
  String postData = "device=" + String(_deviceID);
  postData += "&status=" + String(hasRecentData ? "online" : "no_data");
  postData += "&free_memory=" + String(ESP.getFreeHeap());
  
  // Отправляем только те параметры, которые были установлены пользователем
  if (_sensorFlags.temperature) {
    postData += "&temperature=" + String(_currentData.temperature, 1);
  }
  if (_sensorFlags.humidity) {
    postData += "&humidity=" + String(_currentData.humidity, 1);
  }
  if (_sensorFlags.aqi) {
    postData += "&aqi=" + String(_currentData.aqi);
  }
  if (_sensorFlags.tvoc) {
    postData += "&tvoc=" + String(_currentData.tvoc, 0);
  }
  if (_sensorFlags.eco2) {
    postData += "&eco2=" + String(_currentData.eco2, 0);
  }
  if (_sensorFlags.co) {
    postData += "&co=" + String(_currentData.co, 2);
  }
  if (_sensorFlags.alcohol) {
    postData += "&alcohol=" + String(_currentData.alcohol, 2);
  }
  if (_sensorFlags.co2) {
    postData += "&co2_real=" + String(_currentData.co2, 0);
  }
  if (_sensorFlags.toluene) {
    postData += "&toluene=" + String(_currentData.toluene, 2);
  }
  if (_sensorFlags.nh4) {
    postData += "&ammonia=" + String(_currentData.nh4, 2);
  }
  if (_sensorFlags.acetone) {
    postData += "&acetone=" + String(_currentData.acetone, 2);
  }
  if (_sensorFlags.pm25) {
    postData += "&pm25=" + String(_currentData.pm25, 1);
  }
  if (_sensorFlags.pm10) {
    postData += "&pm10=" + String(_currentData.pm10, 1);
  }
  if (_sensorFlags.dust_density) {
    postData += "&dust_density=" + String(_currentData.dust_density, 1);
  }
  if (_sensorFlags.uv_index) {
    postData += "&uv_index=" + String(_currentData.uv_index, 1);
  }
  
  // Отправка HTTP запроса
  client.println("POST /data HTTP/1.1");
  client.println("Host: " + String(_serverIP) + ":" + String(_serverPort));
  client.println("Content-Type: application/x-www-form-urlencoded");
  client.println("Connection: close");
  client.print("Content-Length: ");
  client.println(postData.length());
  client.println();
  client.println(postData);
  
  // Ждем ответа
  delay(100);
  unsigned long timeout = millis();
  while (client.available() == 0) {
    if (millis() - timeout > 5000) {
      Serial.println("Client timeout");
      client.stop();
      return false;
    }
  }
  
  // Читаем ответ
  while (client.available()) {
    String line = client.readStringUntil('\r');
    Serial.print(line);
  }
  
  client.stop();
  Serial.println("\nData sent to server successfully");
  _lastDataSend = millis();
  return true;
}

// Установка автоматической отправки
void EcoCheck::setAutoSend(bool enabled, unsigned long interval) {
  _autoSendEnabled = enabled;
  _dataInterval = interval;
}

// Проверка подключения WiFi
bool EcoCheck::isWiFiConnected() {
  return WiFi.status() == WL_CONNECTED;
}

// Получение локального IP
IPAddress EcoCheck::getLocalIP() {
  return WiFi.localIP();
}

// Сброс конфигурации WiFi
void EcoCheck::resetWiFiConfig() {
  _wifiConfig.configured = false;
  saveConfig();
  _shouldReboot = true;
}

// Установка имени AP
void EcoCheck::setAPName(const char* apSSID, const char* apPassword) {
  _apSSID = apSSID;
  _apPassword = apPassword;
}

