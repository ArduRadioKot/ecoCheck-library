/*
 * BasicExample - Базовый пример использования библиотеки ecoCheck
 * 
 * Этот пример демонстрирует базовое использование библиотеки:
 * - Инициализация с указанием IP и порта сервера
 * - Установка данных стандартных датчиков
 * - Автоматическая отправка данных на сервер
 */

#include <ecoCheck.h>

// Настройки сервера (укажите свои значения)
const char* SERVER_IP = "82.202.142.35";
const int SERVER_PORT = 8080;
const char* DEVICE_ID = "esp01_basic";

// Создаем экземпляр библиотеки
EcoCheck ecoCheck(SERVER_IP, SERVER_PORT, DEVICE_ID);

void setup() {
  // Инициализация библиотеки
  ecoCheck.begin();
  
  // Настройка автоматической отправки данных каждые 10 секунд
  ecoCheck.setAutoSend(true, 10000);
  
  Serial.println("EcoCheck library initialized");
  Serial.println("If WiFi is not configured, connect to AP: ESP8266_Config");
  Serial.println("Password: 12345678");
}

void loop() {
  // Обязательно вызывать в loop()
  ecoCheck.loop();
  
  // Симуляция чтения данных с датчиков
  // В реальном проекте здесь будет код чтения с ваших датчиков
  
  // Пример: установка данных температуры и влажности
  float temperature = 23.5 + (random(0, 50) / 10.0); // Симуляция: 23.5-28.5°C
  float humidity = 45.0 + (random(0, 30) / 10.0);    // Симуляция: 45-48%
  
  ecoCheck.setTemperature(temperature);
  ecoCheck.setHumidity(humidity);
  
  // Пример: установка данных качества воздуха
  int aqi = random(0, 200);
  float tvoc = random(0, 500);
  float eco2 = random(400, 1000);
  
  ecoCheck.setAQI(aqi);
  ecoCheck.setTVOC(tvoc);
  ecoCheck.setECO2(eco2);
  
  // Пример: установка данных MQ-135
  float co = random(0, 50) / 10.0;
  float co2 = random(400, 1000);
  
  ecoCheck.setCO(co);
  ecoCheck.setCO2(co2);
  
  // Пример: установка данных пыли
  float pm25 = random(0, 100) / 10.0;
  ecoCheck.setPM25(pm25);
  ecoCheck.setPM10(pm25 * 1.5);
  
  // Задержка перед следующим обновлением
  delay(5000);
}

