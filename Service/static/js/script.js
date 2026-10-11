AOS.init({
  duration: 1000,
  once: true,
  offset: 100
});

// FAQ toggle
document.querySelectorAll('.faq-item button').forEach(btn => {
  btn.addEventListener('click', () => {
    btn.parentElement.classList.toggle('active');
  });
});

// Плавная прокрутка для якорных ссылок
document.querySelectorAll('a[href^="#"]').forEach(anchor => {
  anchor.addEventListener('click', function (e) {
    e.preventDefault();
    document.querySelector(this.getAttribute('href')).scrollIntoView({
      behavior: 'smooth'
    });
  });
});

// Инициализация карты мира
function initMap() {
  const map = L.map('globalMap').setView([20, 0], 2);

  // Добавляем слой карты
  L.tileLayer('https://{s}.tile.openstreetmap.org/{z}/{x}/{y}.png', {
    attribution: '© OpenStreetMap contributors'
  }).addTo(map);

  // Добавляем сообщение о том, что сенсоров пока нет
  const noSensorsDiv = L.DomUtil.create('div', 'empty-state');
  noSensorsDiv.innerHTML = `
    <i class="fas fa-map-marker-alt"></i>
    <h4>Сенсоры скоро появятся!</h4>
    <p>Мы активно работаем над развертыванием нашей глобальной сети мониторинга. Первые сенсоры появятся в ближайшие месяцы.</p>
    <button style="background: var(--teal); color: white; border: none; padding: 0.8rem 1.5rem; border-radius: 25px; cursor: pointer; font-weight: 600;">
      Узнать о запуске
    </button>
  `;

  const noSensorsOverlay = L.popup()
    .setLatLng([20, 0])
    .setContent(noSensorsDiv)
    .addTo(map);

  // Открываем попап по центру карты
  noSensorsOverlay.openOn(map);

  // Добавляем несколько маркеров для демонстрации будущих локаций
  const futureLocations = [
    {
      lat: 55.7558,
      lng: 37.6173,
      city: 'Москва',
      status: 'Запуск: Q2 2025'
    },
    {
      lat: 59.9343,
      lng: 30.3351,
      city: 'Санкт-Петербург',
      status: 'Запуск: Q3 2025'
    },
    {
      lat: 40.7128,
      lng: -74.0060,
      city: 'Нью-Йорк',
      status: 'Запуск: Q4 2025'
    },
    {
      lat: 51.5074,
      lng: -0.1278,
      city: 'Лондон',
      status: 'Запуск: Q1 2026'
    },
    {
      lat: 35.6762,
      lng: 139.6503,
      city: 'Токио',
      status: 'Запуск: Q2 2026'
    }
  ];

  futureLocations.forEach(location => {
    const marker = L.marker([location.lat, location.lng]).addTo(map);
    
    const popupContent = `
      <div class="custom-popup">
        <div class="popup-title">${location.city}</div>
        <div class="popup-status">${location.status}</div>
        <p>Сенсоры появятся здесь в указанный квартал</p>
      </div>
    `;
    
    marker.bindPopup(popupContent);
  });
}

// Инициализируем карту после загрузки страницы
document.addEventListener('DOMContentLoaded', initMap);