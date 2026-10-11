import telebot
import os
import threading
import time

def init_bot(sensor_store, bot_token=None):

    if bot_token is None:
        bot_token = os.getenv('TELEGRAM_BOT_TOKEN', '')
    
    if not bot_token:
        print("Warning: TELEGRAM_BOT_TOKEN not set. Bot will not start.")
        return None
    
    bot = telebot.TeleBot(bot_token)
    
    @bot.message_handler(commands=['start', 'help'])
    def send_welcome(message):
        help_text = (
            "Привет! Я бот для мониторинга параметров воздуха.\n\n"
            "Доступные команды:\n"
            "/refresh - получить текущие данные сенсоров\n"
            "/help - показать это сообщение"
        )
        bot.reply_to(message, help_text)
    
    @bot.message_handler(commands=['refresh'])
    def send_sensor_data(message):
        """Отправка данных сенсоров пользователю"""
        device = 'esp01'  # Используем устройство по умолчанию
        latest_data = sensor_store.get_latest_data(device)
        
        if not latest_data or not latest_data.get('data'):
            bot.reply_to(
                message, 
                "Данные сенсоров пока не получены. Убедитесь, что ESP8266 отправляет данные на сервер."
            )
            return
        
        data = latest_data['data']
        timestamp = latest_data.get('timestamp', 'Неизвестно')
        
        # Формируем сообщение с данными
        msg_parts = [f"📊 Параметры воздуха (обновлено: {timestamp})\n"]
        
        if data.get('temperature'):
            msg_parts.append(f"Температура: {data['temperature']}°C")
        
        if data.get('humidity'):
            msg_parts.append(f"Влажность: {data['humidity']}%")
        
        if data.get('pressure'):
            pressure_hpa = float(data['pressure'])
            pressure_mmhg = round(pressure_hpa * 0.750062, 2)
            msg_parts.append(f"Атмосферное давление: {pressure_hpa} ГПа ({pressure_mmhg} мм рт.ст.)")
        
        if data.get('ammonia'):
            msg_parts.append(f"Аммиак (NH₃): {data['ammonia']} ppm")
        
        if data.get('nox'):
            msg_parts.append(f"Оксиды азота (NOₓ): {data['nox']} ppb")
        
        if data.get('co2'):
            msg_parts.append(f"Диоксид углерода (CO₂): {data['co2']} ppm")
        
        if data.get('pm25'):
            msg_parts.append(f"PM2.5: {data['pm25']} µg/m³")
        
        if data.get('pm10'):
            msg_parts.append(f"PM10: {data['pm10']} µg/m³")
        
        if data.get('voc'):
            msg_parts.append(f"ЛОС (VOC): {data['voc']} ppb")
        
        if data.get('benzene'):
            msg_parts.append(f"Бензол (C₆H₆): {data['benzene']} ppb")
        
        if data.get('uv_index'):
            msg_parts.append(f"УФ-индекс: {data['uv_index']}")
        
        if len(msg_parts) == 1:
            msg_parts.append("Нет доступных данных сенсоров.")
        
        msg = "\n".join(msg_parts)
        bot.reply_to(message, msg)
    
    @bot.message_handler(func=lambda message: True)
    def handle_unknown(message):
        bot.reply_to(
            message, 
            "Я тебя не понимаю. Напиши /help для списка команд."
        )
    
    def run_bot():
        """Запуск бота в бесконечном цикле"""
        try:
            print("Starting Telegram bot...")
            bot.polling(none_stop=True, interval=0, timeout=20)
        except Exception as e:
            print(f"Error in Telegram bot: {e}")
            # Перезапуск бота через 5 секунд при ошибке
            time.sleep(5)
            run_bot()
    
    # Запускаем бота в отдельном потоке
    bot_thread = threading.Thread(target=run_bot, daemon=True)
    bot_thread.start()
    print("Telegram bot thread started")
    
    return bot
