import axios from 'axios';
import { API_ENDPOINTS } from '../config/api';

/**
 * Получение данных сенсора
 * @param {string} device - ID устройства (по умолчанию 'esp01')
 * @returns {Promise<Object>} Данные сенсора
 */
export const getSensorData = async (device = 'esp01') => {
  try {
    const response = await axios.get(API_ENDPOINTS.SENSOR_DATA, {
      params: { device },
      timeout: 10000,
    });
    return response.data;
  } catch (error) {
    console.error('Error fetching sensor data:', error);
    throw error;
  }
};

/**
 * Преобразование данных сенсора в удобный формат
 * @param {Object} serverData - Данные с сервера
 * @returns {Array} Массив данных сенсоров
 */
export const formatSensorData = (serverData) => {
  if (!serverData || !serverData.data) {
    return [];
  }

  const data = serverData.data;
  const sensors = [];

  const hasValue = (v) => v !== undefined && v !== null && v !== '';
  const asFloat = (v) => {
    const num = parseFloat(v);
    return Number.isNaN(num) ? null : num;
  };
  const asInt = (v) => {
    const num = parseInt(v, 10);
    return Number.isNaN(num) ? null : num;
  };

  const addSensor = ({
    key,
    altKey,
    id,
    name,
    unit = '',
    icon = 'gauge',
    color = '#17827E',
    parse = asFloat,
  }) => {
    const raw = hasValue(data[key]) ? data[key] : altKey ? data[altKey] : undefined;
    if (!hasValue(raw)) return;
    const val = parse(raw);
    if (val === null) return;

    sensors.push({
      id: id || key,
      name,
      value: val,
      unit,
      icon,
      color,
    });
  };

  const co2Value = hasValue(data.co2) ? data.co2 : data.co2_real;
  const uvValue = hasValue(data.uv_index) ? data.uv_index : data.uv;
  const ammoniaValue = hasValue(data.ammonia) ? data.ammonia : data.nh4;
  const pm25Value = hasValue(data.pm25) ? data.pm25 : data.dust;

  addSensor({
    key: 'temperature',
    name: 'Температура',
    unit: '°C',
    icon: 'thermometer',
    color: '#e74c3c',
  });

  addSensor({
    key: 'humidity',
    name: 'Влажность',
    unit: '%',
    icon: 'water',
    color: '#3498db',
  });

  addSensor({
    key: 'aqi',
    name: 'AQI',
    unit: 'индекс',
    icon: 'leaf',
    color: '#2ecc71',
    parse: asInt,
  });

  addSensor({
    key: 'tvoc',
    id: 'tvoc',
    name: 'TVOC',
    unit: 'ppb',
    icon: 'flask',
    color: '#e67e22',
    parse: asInt,
  });

  addSensor({
    key: 'voc',
    name: 'ЛОС',
    unit: 'ppb',
    icon: 'flask',
    color: '#e67e22',
    parse: asInt,
  });

  addSensor({
    key: 'eco2',
    id: 'eco2',
    name: 'eCO₂',
    unit: 'ppm',
    icon: 'cloud',
    color: '#9b59b6',
    parse: asInt,
  });

  addSensor({
    key: 'co2',
    altKey: 'co2_real',
    id: 'co2',
    name: 'CO₂',
    unit: 'ppm',
    icon: 'cloud',
    color: '#9b59b6',
    parse: asInt,
  });

  addSensor({
    key: 'pm25',
    altKey: 'dust',
    id: 'pm25',
    name: 'PM2.5',
    unit: 'µg/m³',
    icon: 'wind',
    color: '#f39c12',
    parse: asFloat,
  });

  addSensor({
    key: 'pm10',
    name: 'PM10',
    unit: 'µg/m³',
    icon: 'cloud',
    color: '#d35400',
    parse: asFloat,
  });

  addSensor({
    key: 'pressure',
    name: 'Давление',
    unit: 'hPa',
    icon: 'gauge',
    color: '#16a085',
  });

  addSensor({
    key: 'ammonia',
    altKey: 'nh4',
    name: 'Амиак (NH₃)',
    unit: 'ppm',
    icon: 'atom',
    color: '#27ae60',
  });

  addSensor({
    key: 'co',
    name: 'CO',
    unit: 'ppm',
    icon: 'factory',
    color: '#e74c3c',
    parse: asFloat,
  });

  addSensor({
    key: 'alcohol',
    name: 'Алкоголь',
    unit: 'ppm',
    icon: 'bottle-soda',
    color: '#8e44ad',
    parse: asFloat,
  });

  addSensor({
    key: 'toluene',
    name: 'Толуол',
    unit: 'ppm',
    icon: 'flask',
    color: '#c0392b',
    parse: asFloat,
  });

  addSensor({
    key: 'acetone',
    name: 'Ацетон',
    unit: 'ppm',
    icon: 'vial',
    color: '#8e44ad',
    parse: asFloat,
  });

  addSensor({
    key: 'dust_density',
    name: 'Плотность пыли',
    unit: 'µg/m³',
    icon: 'chart-bar',
    color: '#7f8c8d',
    parse: asFloat,
  });

  addSensor({
    key: 'calc_voltage',
    name: 'Напряжение датчика',
    unit: 'V',
    icon: 'flash',
    color: '#2980b9',
    parse: asFloat,
  });

  addSensor({
    key: 'raw_value',
    name: 'ADC Raw',
    unit: '',
    icon: 'chip',
    color: '#2c3e50',
    parse: asInt,
  });

  addSensor({
    key: 'uv_index',
    altKey: 'uv',
    id: 'uv_index',
    name: 'УФ-индекс',
    unit: '',
    icon: 'sun',
    color: '#f1c40f',
    parse: asFloat,
  });

  addSensor({
    key: 'nox',
    name: 'Оксиды азота (NOₓ)',
    unit: 'ppb',
    icon: 'factory',
    color: '#c0392b',
    parse: asInt,
  });

  addSensor({
    key: 'benzene',
    name: 'Бензол (C₆H₆)',
    unit: 'ppb',
    icon: 'vial',
    color: '#8e44ad',
    parse: asFloat,
  });

  addSensor({
    key: 'free_memory',
    name: 'Свободная память',
    unit: 'байт',
    icon: 'memory',
    color: '#27ae60',
    parse: asInt,
  });

  return sensors;
};
