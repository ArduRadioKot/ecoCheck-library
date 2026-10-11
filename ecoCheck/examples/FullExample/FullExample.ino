/*
 * FullExample - Полный пример использования всех возможностей библиотеки ecoCheck
 * 
 * Этот пример демонстрирует:
 * - Все стандартные функции для работы с датчиками
 * - Использование пользовательских параметров
 * - Управление автоматической отправкой
 * - Ручную отправку данных
 * - Работу с WiFi конфигурацией
 */

#include <ecoCheck.h>

// Настройки сервера (укажите свои значения)
const char* SERVER_IP = "82.202.142.35";
const int SERVER_PORT = 8080;
const char* DEVICE_ID = "esp01_full";

// Создаем экземпляр библиотеки
EcoCheck ecoCheck(SERVER_IP, SERVER_PORT, DEVICE_ID);

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  // Настройка имени точки доступа (опционально)
  ecoCheck.setAPName("EcoCheck_Config", "12345678");
  
  // Инициализация библиотеки
  ecoCheck.begin();
  
  // Настройка автоматической отправки каждые 10 секунд
  ecoCheck.setAutoSend(true, 10000);
  
  Serial.println("=================================");
  Serial.println("EcoCheck Full Example");
  Serial.println("=================================");
  Serial.println("Server: " + String(SERVER_IP) + ":" + String(SERVER_PORT));
  Serial.println("Device ID: " + String(DEVICE_ID));
  Serial.println("=================================");
}

void loop() {
  // Обязательно вызывать в loop()
  ecoCheck.loop();
  
  // ============================================
  // СТАНДАРТНЫЕ ДАТЧИКИ
  // ============================================
  
  // Температура и влажность (AHT21)
  float temperature = 23.5 + (random(0, 50) / 10.0);
  float humidity = 45.0 + (random(0, 30) / 10.0);
  ecoCheck.setTemperature(temperature);
  ecoCheck.setHumidity(humidity);
  
  // Качество воздуха (ENS160)
  int aqi = random(0, 200);
  float tvoc = random(0, 500);
  float eco2 = random(400, 1000);
  ecoCheck.setAQI(aqi);
  ecoCheck.setTVOC(tvoc);
  ecoCheck.setECO2(eco2);
  
  // Газы (MQ-135)
  float co = random(0, 50) / 10.0;
  float alcohol = random(0, 30) / 10.0;
  float co2 = random(400, 1000);
  float toluene = random(0, 20) / 10.0;
  float ammonia = random(0, 15) / 10.0;
  float acetone = random(0, 10) / 10.0;
  
  ecoCheck.setCO(co);
  ecoCheck.setAlcohol(alcohol);
  ecoCheck.setCO2(co2);
  ecoCheck.setToluene(toluene);
  ecoCheck.setAmmonia(ammonia);
  ecoCheck.setAcetone(acetone);
  
  // Пыль
  float pm25 = random(0, 100) / 10.0;
  float pm10 = pm25 * 1.5;
  float dustDensity = pm25;
  ecoCheck.setPM25(pm25);
  ecoCheck.setPM10(pm10);
  ecoCheck.setDustDensity(dustDensity);
  
  // Ультрафиолет
  float uvIndex = random(0, 11);
  ecoCheck.setUVIndex(uvIndex);
  
  // ============================================
  // ДОПОЛНИТЕЛЬНЫЕ ВОЗМОЖНОСТИ
  // ============================================
  
  // Проверка статуса WiFi
  if (ecoCheck.isWiFiConnected()) {
    IPAddress ip = ecoCheck.getLocalIP();
    Serial.print("WiFi connected. IP: ");
    Serial.println(ip);
  } else {
    Serial.println("WiFi not connected");
  }
  
  // Пример: ручная отправка данных при определенном условии
  static unsigned long lastManualSend = 0;
  if (millis() - lastManualSend > 60000) { // Каждую минуту
    if (temperature > 25.0) {
      Serial.println("High temperature detected! Sending data manually...");
      ecoCheck.sendData();
      lastManualSend = millis();
    }
  }
  
  // Пример: динамическое изменение интервала отправки
  if (aqi > 150) {
    // При плохом качестве воздуха отправляем чаще
    ecoCheck.setAutoSend(true, 5000);
  } else {
    // При нормальном качестве воздуха отправляем реже
    ecoCheck.setAutoSend(true, 10000);
  }
  
  // Вывод информации о данных
  Serial.println("--- Sensor Data ---");
  Serial.print("Temperature: "); Serial.print(temperature); Serial.println("°C");
  Serial.print("Humidity: "); Serial.print(humidity); Serial.println("%");
  Serial.print("AQI: "); Serial.println(aqi);
  Serial.print("CO: "); Serial.print(co); Serial.println("ppm");
  Serial.print("PM2.5: "); Serial.print(pm25); Serial.println("µg/m³");
  Serial.println("------------------");
  
  delay(5000);
}

/*
 * ДОПОЛНИТЕЛЬНЫЕ ФУНКЦИИ:
 * 
 * // Сброс конфигурации WiFi (если нужно переподключиться)
 * // ecoCheck.resetWiFiConfig();
 * 
 * // Временное отключение автоматической отправки
 * // ecoCheck.setAutoSend(false);
 * 
 * // Включение автоматической отправки с новым интервалом
 * // ecoCheck.setAutoSend(true, 15000); // 15 секунд
 */

