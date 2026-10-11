// Конфигурация API
// Можно переопределить через переменную окружения EXPO_PUBLIC_API_BASE_URL
const API_BASE_URL =
  process.env.EXPO_PUBLIC_API_BASE_URL ||
  'http://82.202.142.35:5002'; // дефолт — порт Flask

export const API_ENDPOINTS = {
  SENSOR_DATA: `${API_BASE_URL}/api/mobile/sensor-data`,
};

export default API_BASE_URL;
