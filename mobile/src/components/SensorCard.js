import React from 'react';
import { View, Text, StyleSheet, TouchableOpacity } from 'react-native';
import { MaterialCommunityIcons as Icon } from '@expo/vector-icons';

const SensorCard = ({ sensor, onPress }) => {
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

  return (
    <TouchableOpacity
      style={[styles.card, { borderLeftColor: sensor.color }]}
      onPress={onPress}
      activeOpacity={0.7}
    >
      <View style={styles.cardHeader}>
        <View style={[styles.iconContainer, { backgroundColor: sensor.color + '20' }]}>
          <Icon
            name={getIconName(sensor.icon)}
            size={28}
            color={sensor.color}
          />
        </View>
      </View>
      <View style={styles.cardContent}>
        <Text style={styles.sensorName}>{sensor.name}</Text>
        <View style={styles.valueContainer}>
          <Text style={[styles.value, { color: sensor.color }]}>
            {sensor.value.toFixed(sensor.unit === '°C' || sensor.unit === 'hPa' || sensor.unit === 'ppm' ? 1 : 0)}
          </Text>
          <Text style={styles.unit}>{sensor.unit}</Text>
        </View>
      </View>
    </TouchableOpacity>
  );
};

const styles = StyleSheet.create({
  card: {
    width: '48%',
    backgroundColor: '#fff',
    borderRadius: 12,
    padding: 16,
    marginBottom: 16,
    elevation: 3,
    shadowColor: '#000',
    shadowOffset: { width: 0, height: 2 },
    shadowOpacity: 0.1,
    shadowRadius: 4,
    borderLeftWidth: 4,
  },
  cardHeader: {
    marginBottom: 12,
  },
  iconContainer: {
    width: 50,
    height: 50,
    borderRadius: 25,
    justifyContent: 'center',
    alignItems: 'center',
  },
  cardContent: {
    flex: 1,
  },
  sensorName: {
    fontSize: 14,
    color: '#666',
    marginBottom: 8,
    fontWeight: '500',
  },
  valueContainer: {
    flexDirection: 'row',
    alignItems: 'baseline',
  },
  value: {
    fontSize: 24,
    fontWeight: 'bold',
    marginRight: 4,
  },
  unit: {
    fontSize: 14,
    color: '#999',
  },
});

export default SensorCard;
