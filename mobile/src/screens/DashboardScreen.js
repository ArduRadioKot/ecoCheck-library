import React, { useState, useEffect, useCallback } from 'react';
import {
  View,
  StyleSheet,
  ScrollView,
  RefreshControl,
  Text,
  ActivityIndicator,
} from 'react-native';
import { Card, Title, Paragraph } from 'react-native-paper';
import { getSensorData, formatSensorData } from '../services/sensorService';
import SensorCard from '../components/SensorCard';

const DashboardScreen = ({ navigation }) => {
  const [sensors, setSensors] = useState([]);
  const [loading, setLoading] = useState(true);
  const [refreshing, setRefreshing] = useState(false);
  const [error, setError] = useState(null);
  const [lastUpdate, setLastUpdate] = useState(null);

  const loadSensorData = useCallback(async () => {
    try {
      setError(null);
      const data = await getSensorData('esp01');
      const formattedSensors = formatSensorData(data);
      setSensors(formattedSensors);
      setLastUpdate(new Date());
    } catch (err) {
      console.error('Failed to load sensor data:', err);
      setError('Не удалось загрузить данные. Проверьте подключение к серверу.');
    } finally {
      setLoading(false);
      setRefreshing(false);
    }
  }, []);

  useEffect(() => {
    loadSensorData();
    // Автообновление каждые 5 секунд
    const interval = setInterval(loadSensorData, 5000);
    return () => clearInterval(interval);
  }, [loadSensorData]);

  const onRefresh = useCallback(() => {
    setRefreshing(true);
    loadSensorData();
  }, [loadSensorData]);

  const handleSensorPress = (sensor) => {
    navigation.navigate('SensorDetail', { sensor });
  };

  if (loading) {
    return (
      <View style={styles.centerContainer}>
        <ActivityIndicator size="large" color="#30AC97" />
        <Text style={styles.loadingText}>Загрузка данных...</Text>
      </View>
    );
  }

  if (error) {
    return (
      <ScrollView
        contentContainerStyle={styles.centerContainer}
        refreshControl={
          <RefreshControl refreshing={refreshing} onRefresh={onRefresh} />
        }
      >
        <Text style={styles.errorText}>{error}</Text>
        <Text style={styles.errorHint}>Потяните вниз для обновления</Text>
      </ScrollView>
    );
  }

  return (
    <ScrollView
      style={styles.container}
      contentContainerStyle={styles.contentContainer}
      refreshControl={
        <RefreshControl refreshing={refreshing} onRefresh={onRefresh} />
      }
    >
      <Card style={styles.statusCard}>
        <Card.Content>
          <Title style={styles.statusTitle}>Статус подключения</Title>
          <Paragraph style={styles.statusText}>
            {sensors.length > 0 ? '🟢 Онлайн' : '🔴 Офлайн'}
          </Paragraph>
          {lastUpdate && (
            <Paragraph style={styles.updateText}>
              Обновлено: {lastUpdate.toLocaleTimeString('ru-RU')}
            </Paragraph>
          )}
        </Card.Content>
      </Card>

      {sensors.length === 0 ? (
        <Card style={styles.emptyCard}>
          <Card.Content>
            <Text style={styles.emptyText}>
              Данные сенсоров недоступны
            </Text>
            <Text style={styles.emptyHint}>
              Убедитесь, что датчик подключен и передает данные
            </Text>
          </Card.Content>
        </Card>
      ) : (
        <View style={styles.sensorsGrid}>
          {sensors.map((sensor) => (
            <SensorCard
              key={sensor.id}
              sensor={sensor}
              onPress={() => handleSensorPress(sensor)}
            />
          ))}
        </View>
      )}
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
    padding: 20,
  },
  loadingText: {
    marginTop: 16,
    fontSize: 16,
    color: '#666',
  },
  errorText: {
    fontSize: 16,
    color: '#e74c3c',
    textAlign: 'center',
    marginBottom: 8,
  },
  errorHint: {
    fontSize: 14,
    color: '#999',
    textAlign: 'center',
  },
  statusCard: {
    marginBottom: 16,
    backgroundColor: '#fff',
    elevation: 2,
  },
  statusTitle: {
    fontSize: 18,
    fontWeight: 'bold',
    color: '#17827E',
    marginBottom: 8,
  },
  statusText: {
    fontSize: 16,
    marginBottom: 4,
  },
  updateText: {
    fontSize: 12,
    color: '#999',
    marginTop: 4,
  },
  sensorsGrid: {
    flexDirection: 'row',
    flexWrap: 'wrap',
    justifyContent: 'space-between',
  },
  emptyCard: {
    backgroundColor: '#fff',
    elevation: 2,
  },
  emptyText: {
    fontSize: 16,
    color: '#666',
    textAlign: 'center',
    marginBottom: 8,
  },
  emptyHint: {
    fontSize: 14,
    color: '#999',
    textAlign: 'center',
  },
});

export default DashboardScreen;
