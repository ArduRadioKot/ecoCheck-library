# ecoCheck Library

Библиотека для ESP8266 для отправки данных с датчиков на сервер. Поддерживает автоматическую и ручную отправку данных, работу с WiFi через точку доступа для конфигурации.

## Возможности

- ✅ Автоматическая отправка данных на сервер с настраиваемым интервалом
- ✅ Ручная отправка данных по требованию
- ✅ Поддержка стандартных датчиков (температура, влажность, AQI, CO, пыль, UV и др.)
- ✅ WiFi конфигурация через веб-интерфейс (точка доступа)
- ✅ Сохранение конфигурации WiFi в EEPROM
- ✅ Автоматическое переподключение при потере соединения
- ✅ Настраиваемые параметры сервера (IP и порт)

## Установка

1. Скопируйте папку `ecoCheck` в папку `libraries` вашего проекта Arduino или в стандартную папку библиотек Arduino IDE
2. Откройте Arduino IDE
3. Выберите пример из меню: `Файл -> Примеры -> ecoCheck`

## Быстрый старт

```cpp
#include <ecoCheck.h>

// Настройки сервера
const char* SERVER_IP = "82.202.142.35";
const int SERVER_PORT = 8080;
const char* DEVICE_ID = "esp01";

// Создаем экземпляр библиотеки
EcoCheck ecoCheck(SERVER_IP, SERVER_PORT, DEVICE_ID);

void setup() {
  ecoCheck.begin();
  ecoCheck.setAutoSend(true, 10000); // Автоотправка каждые 10 секунд
}

void loop() {
  ecoCheck.loop();
  
  // Установка данных датчиков
  ecoCheck.setTemperature(23.5);
  ecoCheck.setHumidity(55.0);
  ecoCheck.setAQI(50);
  
  delay(5000);
}
```

## API Документация

### Конструктор

```cpp
EcoCheck(const char* serverIP, int serverPort, const char* deviceID = "esp01")
```

Создает экземпляр библиотеки с указанием IP адреса и порта сервера.

**Параметры:**
- `serverIP` - IP адрес сервера (например, "192.168.1.100")
- `serverPort` - Порт сервера (например, 8080)
- `deviceID` - Идентификатор устройства (по умолчанию "esp01")

### Инициализация

```cpp
void begin()
```

Инициализирует библиотеку, загружает конфигурацию WiFi из EEPROM и подключается к сети или запускает точку доступа для конфигурации.

### Основной цикл

```cpp
void loop()
```

**Обязательно** должен вызываться в функции `loop()` вашего скетча. Обрабатывает WiFi подключение и автоматическую отправку данных.

### Стандартные датчики

Библиотека поддерживает следующие стандартные функции для установки данных датчиков:

```cpp
void setTemperature(float value);      // Температура (°C)
void setHumidity(float value);          // Влажность (%)
void setAQI(int value);                 // Индекс качества воздуха
void setTVOC(float value);              // TVOC (ppb)
void setECO2(float value);              // eCO2 (ppm)
void setCO(float value);                // CO (ppm)
void setAlcohol(float value);           // Alcohol (ppm)
void setCO2(float value);               // CO2 (ppm)
void setToluene(float value);           // Toluene (ppm)
void setAmmonia(float value);           // Ammonia (ppm)
void setAcetone(float value);           // Acetone (ppm)
void setPM25(float value);              // PM2.5 (µg/m³)
void setPM10(float value);              // PM10 (µg/m³)
void setDustDensity(float value);       // Плотность пыли (µg/m³)
void setUVIndex(float value);           // UV индекс
```

### Отправка данных

```cpp
// Ручная отправка данных
bool sendData();

// Настройка автоматической отправки
void setAutoSend(bool enabled, unsigned long interval = 10000);
```

**Примеры:**

```cpp
// Отключить автоматическую отправку
ecoCheck.setAutoSend(false);

// Включить автоматическую отправку каждые 5 секунд
ecoCheck.setAutoSend(true, 5000);

// Ручная отправка данных
if (someCondition) {
  ecoCheck.sendData();
}
```

### WiFi функции

```cpp
// Проверка подключения WiFi
bool isWiFiConnected();

// Получение локального IP адреса
IPAddress getLocalIP();

// Сброс конфигурации WiFi
void resetWiFiConfig();

// Установка имени точки доступа
void setAPName(const char* apSSID, const char* apPassword = "12345678");
```

## Примеры использования

### Пример 1: Базовое использование

См. `examples/BasicExample/BasicExample.ino`

### Пример 2: Ручная отправка

См. `examples/ManualSend/ManualSend.ino`

### Пример 3: Полный пример

См. `examples/FullExample/FullExample.ino`

## Конфигурация WiFi

При первом запуске устройство создаст точку доступа:

- **SSID:** `ESP8266_Config` (можно изменить через `setAPName()`)
- **Пароль:** `12345678` (можно изменить через `setAPName()`)

1. Подключитесь к точке доступа
2. Откройте браузер и перейдите на `192.168.4.1`
3. Введите SSID и пароль вашей WiFi сети
4. Нажмите "Save & Connect"
5. Устройство перезагрузится и подключится к WiFi

Конфигурация сохраняется в EEPROM и загружается при следующем запуске.

## Формат данных на сервере

Библиотека отправляет POST запрос на `/data` с данными в формате `application/x-www-form-urlencoded`.

**Важно:** На сервер отправляются **только те параметры, которые вы явно установили** через функции `set*()`. Если вы не вызвали, например, `setTemperature()`, то параметр `temperature` не будет включен в запрос.

Пример запроса (если установлены только температура, влажность и AQI):

```
device=esp01
status=online
free_memory=12345
temperature=23.5
humidity=55.0
aqi=50
```

Если установлены все параметры:

```
device=esp01
status=online
free_memory=12345
temperature=23.5
humidity=55.0
aqi=50
tvoc=100
eco2=400
co=0.5
pm25=15.2
uv_index=3.5
...
```


## Требования

- ESP8266 (NodeMCU, Wemos D1 Mini и др.)
- Arduino IDE с поддержкой ESP8266
- WiFi сеть для подключения

## Лицензия

Свободное использование в личных и коммерческих проектах.

## Поддержка

При возникновении проблем проверьте:
1. Правильность IP адреса и порта сервера
2. Подключение к WiFi сети
3. Доступность сервера из сети
4. Серийный монитор для отладочной информации

