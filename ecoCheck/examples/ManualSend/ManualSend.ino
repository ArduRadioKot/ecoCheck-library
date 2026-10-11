/*
 * ManualSend - Пример ручной отправки данных с библиотекой ecoCheck
 * 
 * Этот пример демонстрирует:
 * - Отключение автоматической отправки
 * - Ручную отправку данных по требованию
 * - Контроль момента отправки данных
 */

#include <ecoCheck.h>

// Настройки сервера (укажите свои значения)
const char* SERVER_IP = "82.202.142.35";
const int SERVER_PORT = 8080;
const char* DEVICE_ID = "esp01_manual";

// Создаем экземпляр библиотеки
EcoCheck ecoCheck(SERVER_IP, SERVER_PORT, DEVICE_ID);

// Переменные для управления отправкой
unsigned long lastManualSend = 0;
const unsigned long MANUAL_SEND_INTERVAL = 30000; // Отправка каждые 30 секунд

void setup() {
  // Инициализация библиотеки
  ecoCheck.begin();
  
  // ОТКЛЮЧАЕМ автоматическую отправку
  ecoCheck.setAutoSend(false);
  
  Serial.println("EcoCheck Manual Send Example");
  Serial.println("Automatic send is DISABLED");
  Serial.println("Data will be sent manually every 30 seconds");
}

void loop() {
  // Обязательно вызывать в loop()
  ecoCheck.loop();
  
  // Чтение данных с датчиков (симуляция)
  float temperature = 22.0 + (random(0, 100) / 10.0);
  float humidity = 50.0 + (random(0, 50) / 10.0);
  int aqi = random(0, 150);
  
  // Установка данных
  ecoCheck.setTemperature(temperature);
  ecoCheck.setHumidity(humidity);
  ecoCheck.setAQI(aqi);
  
  // Дополнительные данные датчиков
  ecoCheck.setPM25(15.5);
  ecoCheck.setUVIndex(3.2);
  
  // Проверяем, нужно ли отправить данные
  if (millis() - lastManualSend > MANUAL_SEND_INTERVAL) {
    Serial.println("Sending data manually...");
    
    // Проверяем подключение WiFi перед отправкой
    if (ecoCheck.isWiFiConnected()) {
      Serial.print("WiFi connected. IP: ");
      Serial.println(ecoCheck.getLocalIP());
      
      // Ручная отправка данных
      bool success = ecoCheck.sendData();
      
      if (success) {
        Serial.println("Data sent successfully!");
      } else {
        Serial.println("Failed to send data");
      }
    } else {
      Serial.println("WiFi not connected. Cannot send data.");
    }
    
    lastManualSend = millis();
  }
  
  // Небольшая задержка
  delay(1000);
}

/*
 * Дополнительные примеры использования:
 * 
 * // Отправка данных по нажатию кнопки
 * if (digitalRead(BUTTON_PIN) == LOW) {
 *   ecoCheck.sendData();
 * }
 * 
 * // Отправка данных при определенном условии
 * if (temperature > 30.0) {
 *   ecoCheck.sendData();
 * }
 * 
 * // Включение/выключение автоматической отправки динамически
 * if (someCondition) {
 *   ecoCheck.setAutoSend(true, 5000);  // Включить с интервалом 5 сек
 * } else {
 *   ecoCheck.setAutoSend(false);       // Выключить
 * }
 */

