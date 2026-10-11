from flask import Flask, render_template, request, jsonify
import os
import datetime
import threading
from collections import deque

# Импорт для Telegram бота
import sys
# Добавляем путь к корневой папке проекта для импорта
project_root = os.path.join(os.path.dirname(__file__), '..')
sys.path.insert(0, project_root)
try:
    from bot.bot import init_bot
    TELEBOT_AVAILABLE = True
except ImportError as e:
    TELEBOT_AVAILABLE = False
    print(f"Warning: Cannot import bot module: {e}. Bot functionality will be disabled.")


app = Flask(__name__)

# Хранилище данных сенсоров
sensor_data_history = deque(maxlen=1000)  

class SensorDataStore:
    def __init__(self):
        self.sensor_data = {}
        self.lock = threading.Lock()
    
    def add_sensor_data(self, device, sensor_data):
        with self.lock:
            timestamp = datetime.datetime.now().strftime("%Y-%m-%d %H:%M:%S")
            record = {
                'device': device,
                'timestamp': timestamp,
                'sensor_data': sensor_data
            }
            sensor_data_history.append(record)
            # Сохраняем последние данные для каждого устройства
            self.sensor_data[device] = {
                'timestamp': timestamp,
                'data': sensor_data
            }
    
    def get_latest_data(self, device=None):
        with self.lock:
            if device:
                return self.sensor_data.get(device, None)
            return self.sensor_data
    
    def get_all_data(self):
        with self.lock:
            return list(sensor_data_history)

sensor_store = SensorDataStore()

@app.route('/')
def index():
    return render_template('index.html')

@app.route('/dashboard')
def dashboard():
    return render_template('dashboard.html')

# API для приема данных от ESP8266
@app.route('/data', methods=['POST'])
def receive_data():
    try:
        device = request.form.get('device', 'unknown')
        
        # Получаем ВСЕ возможные данные сенсоров из формы
        # Основные данные
        sensor_data = {
            # AHT21
            'temperature': request.form.get('temperature'),
            'humidity': request.form.get('humidity'),
            
            # ENS160
            'aqi': request.form.get('aqi'),
            'tvoc': request.form.get('tvoc'),
            'eco2': request.form.get('eco2'),
            
            # MQ-135
            'co': request.form.get('co'),
            'alcohol': request.form.get('alcohol'),
            'co2_real': request.form.get('co2_real'),
            'co2': request.form.get('co2'),  # Альтернативное имя
            'toluene': request.form.get('toluene'),
            'ammonia': request.form.get('ammonia'),
            'nh4': request.form.get('nh4'),  # Альтернативное имя для аммиака
            'acetone': request.form.get('acetone'),
            
            # Dust sensor
            'dust': request.form.get('dust'),
            'pm25': request.form.get('pm25'),
            'pm10': request.form.get('pm10'),
            'dust_density': request.form.get('dust_density'),
            'calc_voltage': request.form.get('calc_voltage'),
            'raw_value': request.form.get('raw_value'),
            
            # UV sensor
            'uv': request.form.get('uv'),
            'uv_index': request.form.get('uv_index'),  # Альтернативное имя
            
            # Статус и системная информация
            'status': request.form.get('status', 'online'),
            'free_memory': request.form.get('free_memory', ''),
            
            # Дополнительные поля из старой версии
            'pressure': request.form.get('pressure'),
            'voc': request.form.get('voc'),
            'nox': request.form.get('nox'),
            'benzene': request.form.get('benzene'),
        }
        
        # Удаляем None значения
        sensor_data = {k: v for k, v in sensor_data.items() if v is not None}
        
        # Нормализуем некоторые значения
        if 'nh4' in sensor_data and 'ammonia' not in sensor_data:
            sensor_data['ammonia'] = sensor_data['nh4']
        if 'uv' in sensor_data and 'uv_index' not in sensor_data:
            sensor_data['uv_index'] = sensor_data['uv']
        if 'co2_real' in sensor_data and 'co2' not in sensor_data:
            sensor_data['co2'] = sensor_data['co2_real']
        if 'dust' in sensor_data and 'pm25' not in sensor_data:
            sensor_data['pm25'] = sensor_data['dust']
        
        sensor_store.add_sensor_data(device, sensor_data)
        
        print(f"Received sensor data from {device}: {sensor_data}")
        return jsonify({'status': 'success'}), 200
    
    except Exception as e:
        print(f"Error processing sensor data: {e}")
        return jsonify({'status': 'error', 'message': str(e)}), 400

# API для получения данных сенсоров
@app.route('/api/sensor-data')
def get_sensor_data():
    device = request.args.get('device', None)
    latest_data = sensor_store.get_latest_data(device)
    
    if device and latest_data:
        return jsonify(latest_data)
    elif device:
        return jsonify({'error': 'Device not found'}), 404
    else:
        return jsonify({
            'devices': sensor_store.get_latest_data(),
            'history': sensor_store.get_all_data()[-100:]  # последние 100 записей
        })

# API для мобильного приложения
@app.route('/api/mobile/sensor-data')
def get_mobile_sensor_data():
    device = request.args.get('device', 'esp01')
    latest_data = sensor_store.get_latest_data(device)
    
    if latest_data:
        return jsonify(latest_data)
    else:
        # Возвращаем пустую структуру вместо 404, чтобы клиент мог обработать отсутствие данных
        return jsonify({
            'device': device,
            'timestamp': None,
            'data': {}
        })

# Функция для запуска Telegram бота
def init_telegram_bot():
    """Инициализация и запуск Telegram бота в отдельном потоке"""
    if not TELEBOT_AVAILABLE:
        print("Telegram bot is not available")
        return
    
    # Передаем sensor_store в функцию инициализации бота
    init_bot(sensor_store)

if __name__ == '__main__':
    # Запускаем Telegram бота
    init_telegram_bot()
    
    # Запускаем Flask приложение
    app.run(host='0.0.0.0', port=5002, debug=True)
