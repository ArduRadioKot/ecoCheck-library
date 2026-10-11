import React, { useState, useEffect } from 'react';
import {
  View,
  StyleSheet,
  ScrollView,
  Text,
  ActivityIndicator,
} from 'react-native';
import { Card, Title, Paragraph } from 'react-native-paper';
import { MaterialCommunityIcons as Icon } from '@expo/vector-icons';
import { LineChart } from 'react-native-chart-kit';
import { Dimensions } from 'react-native';
import { getSensorData } from '../services/sensorService';

const SensorDetailScreen = ({ route }) => {
  const { sensor } = route.params;
  const [history, setHistory] = useState([]);
  const [currentValue, setCurrentValue] = useState(sensor.value);
  const [loading, setLoading] = useState(true);

  useEffect(() => {
    loadData();
    const interval = setInterval(loadData, 5000);
    return () => clearInterval(interval);
  }, []);

  const loadData = async () => {
    try {
      const data = await getSensorData('esp01');
      if (data && data.data) {
        const hasValue = (v) => v !== undefined && v !== null && v !== '';

        // Маппинг ID сенсора на ключ в данных сервера
        const dataKeyMap = {
          temperature: ['temperature'],
          humidity: ['humidity'],
          co2: ['co2', 'co2_real'],
          eco2: ['eco2'],
          pm25: ['pm25', 'dust'],
          pm10: ['pm10'],
          pressure: ['pressure'],
          voc: ['voc'],
          tvoc: ['tvoc'],
          aqi: ['aqi'],
          ammonia: ['ammonia', 'nh4'],
          co: ['co'],
          alcohol: ['alcohol'],
          toluene: ['toluene'],
          acetone: ['acetone'],
          dust_density: ['dust_density'],
          calc_voltage: ['calc_voltage'],
          raw_value: ['raw_value'],
          uv_index: ['uv_index', 'uv'],
          nox: ['nox'],
          benzene: ['benzene'],
          free_memory: ['free_memory'],
        };
        
        const keys = dataKeyMap[sensor.id] || [sensor.id];
        const rawValue = keys.map((k) => data.data[k]).find((v) => hasValue(v));
        const value = hasValue(rawValue) ? parseFloat(rawValue) : sensor.value;
        
        setCurrentValue(value);
        // Генерируем историю для графика
        generateHistory(value);
      }
    } catch (error) {
      console.error('Error loading sensor detail:', error);
    } finally {
      setLoading(false);
    }
  };

  const generateHistory = (currentVal) => {
    // Генерируем историю на основе текущего значения
    const historyData = [];
    for (let i = 7; i >= 0; i--) {
      const variation = (Math.random() - 0.5) * (currentVal * 0.1);
      historyData.push(Math.max(0, currentVal + variation));
    }
    setHistory(historyData);
  };

  const getIconName = (iconType) => {
    const iconMap = {
      thermometer: 'thermometer',
      water: 'water',
      cloud: 'cloud',
      wind: 'weather-windy',
      gauge: 'gauge',
      flask: 'flask',
      atom: 'atom',
      factory: 'factory',
      vial: 'test-tube',
      sun: 'weather-sunny',
      leaf: 'leaf',
      'bottle-soda': 'bottle-soda',
      'chart-bar': 'chart-bar',
      flash: 'flash',
      chip: 'cpu-64-bit',
      memory: 'memory',
    };
    return iconMap[iconType] || 'chart-line';
  };

  const screenWidth = Dimensions.get('window').width;

  const chartData = {
    labels: ['00:00', '03:00', '06:00', '09:00', '12:00', '15:00', '18:00', '21:00'],
    datasets: [
      {
        data: history.length > 0 ? history : [currentValue],
        color: (opacity = 1) => sensor.color,
        strokeWidth: 2,
      },
    ],
  };

  const chartConfig = {
    backgroundColor: '#ffffff',
    backgroundGradientFrom: '#ffffff',
    backgroundGradientTo: '#ffffff',
    decimalPlaces: sensor.unit === '°C' || sensor.unit === 'hPa' ? 1 : 0,
    color: (opacity = 1) => sensor.color,
    labelColor: (opacity = 1) => `rgba(0, 0, 0, ${opacity})`,
    style: {
      borderRadius: 16,
    },
    propsForDots: {
      r: '4',
      strokeWidth: '2',
      stroke: sensor.color,
    },
  };

  if (loading) {
    return (
      <View style={styles.centerContainer}>
        <ActivityIndicator size="large" color="#30AC97" />
      </View>
    );
  }

  return (
    <ScrollView style={styles.container} contentContainerStyle={styles.contentContainer}>
      <Card style={styles.mainCard}>
        <Card.Content>
          <View style={styles.header}>
            <View style={[styles.iconContainer, { backgroundColor: sensor.color + '20' }]}>
              <Icon
                name={getIconName(sensor.icon)}
                size={48}
                color={sensor.color}
              />
            </View>
            <View style={styles.headerText}>
              <Title style={styles.sensorTitle}>{sensor.name}</Title>
              <View style={styles.valueRow}>
                <Text style={[styles.mainValue, { color: sensor.color }]}>
                  {currentValue.toFixed(sensor.unit === '°C' || sensor.unit === 'hPa' || sensor.unit === 'ppm' ? 1 : 0)}
                </Text>
                <Text style={styles.mainUnit}>{sensor.unit}</Text>
              </View>
            </View>
          </View>
        </Card.Content>
      </Card>

      <Card style={styles.chartCard}>
        <Card.Content>
          <Title style={styles.chartTitle}>График за последние 24 часа</Title>
          {history.length > 0 ? (
            <LineChart
              data={chartData}
              width={screenWidth - 64}
              height={220}
              chartConfig={chartConfig}
              bezier
              style={styles.chart}
            />
          ) : (
            <View style={styles.chartPlaceholder}>
              <Text style={styles.chartPlaceholderText}>Загрузка данных графика...</Text>
            </View>
          )}
        </Card.Content>
      </Card>

      <Card style={styles.infoCard}>
        <Card.Content>
          <Title style={styles.infoTitle}>Информация</Title>
          <View style={styles.infoRow}>
            <Text style={styles.infoLabel}>Текущее значение:</Text>
            <Text style={styles.infoValue}>
              {currentValue.toFixed(sensor.unit === '°C' || sensor.unit === 'hPa' || sensor.unit === 'ppm' ? 1 : 0)} {sensor.unit}
            </Text>
          </View>
          {history.length > 0 && (
            <>
              <View style={styles.infoRow}>
                <Text style={styles.infoLabel}>Максимум:</Text>
                <Text style={styles.infoValue}>
                  {Math.max(...history).toFixed(sensor.unit === '°C' || sensor.unit === 'hPa' || sensor.unit === 'ppm' ? 1 : 0)} {sensor.unit}
                </Text>
              </View>
              <View style={styles.infoRow}>
                <Text style={styles.infoLabel}>Минимум:</Text>
                <Text style={styles.infoValue}>
                  {Math.min(...history).toFixed(sensor.unit === '°C' || sensor.unit === 'hPa' || sensor.unit === 'ppm' ? 1 : 0)} {sensor.unit}
                </Text>
              </View>
              <View style={styles.infoRow}>
                <Text style={styles.infoLabel}>Среднее:</Text>
                <Text style={styles.infoValue}>
                  {(history.reduce((a, b) => a + b, 0) / history.length).toFixed(sensor.unit === '°C' || sensor.unit === 'hPa' || sensor.unit === 'ppm' ? 1 : 0)} {sensor.unit}
                </Text>
              </View>
            </>
          )}
        </Card.Content>
      </Card>
    </ScrollView>
  );
};

const styles = StyleSheet.create({
  container: {
    flex: 1,
    backgroundColor: '#f5f5f5',
  },
  contentContainer: {
    padding: 16,
  },
  centerContainer: {
    flex: 1,
    justifyContent: 'center',
    alignItems: 'center',
  },
  mainCard: {
    marginBottom: 16,
    backgroundColor: '#fff',
    elevation: 3,
  },
  header: {
    flexDirection: 'row',
    alignItems: 'center',
  },
  iconContainer: {
    width: 80,
    height: 80,
    borderRadius: 40,
    justifyContent: 'center',
    alignItems: 'center',
    marginRight: 16,
  },
  headerText: {
    flex: 1,
  },
  sensorTitle: {
    fontSize: 20,
    fontWeight: 'bold',
    color: '#17827E',
    marginBottom: 8,
  },
  valueRow: {
    flexDirection: 'row',
    alignItems: 'baseline',
  },
  mainValue: {
    fontSize: 36,
    fontWeight: 'bold',
    marginRight: 8,
  },
  mainUnit: {
    fontSize: 18,
    color: '#999',
  },
  chartCard: {
    marginBottom: 16,
    backgroundColor: '#fff',
    elevation: 3,
  },
  chartTitle: {
    fontSize: 16,
    fontWeight: 'bold',
    color: '#17827E',
    marginBottom: 16,
  },
  chart: {
    marginVertical: 8,
    borderRadius: 16,
  },
  chartPlaceholder: {
    height: 220,
    justifyContent: 'center',
    alignItems: 'center',
  },
  chartPlaceholderText: {
    color: '#999',
    fontSize: 14,
  },
  infoCard: {
    backgroundColor: '#fff',
    elevation: 3,
  },
  infoTitle: {
    fontSize: 16,
    fontWeight: 'bold',
    color: '#17827E',
    marginBottom: 16,
  },
  infoRow: {
    flexDirection: 'row',
    justifyContent: 'space-between',
    paddingVertical: 8,
    borderBottomWidth: 1,
    borderBottomColor: '#f0f0f0',
  },
  infoLabel: {
    fontSize: 14,
    color: '#666',
  },
  infoValue: {
    fontSize: 14,
    fontWeight: '600',
    color: '#17827E',
  },
});

export default SensorDetailScreen;
