/*
  ====================================================================
  script.js - Клиентская логика веб-интерфейса ESP32
  ====================================================================
  Описание: Управление LED, получение статуса, обработка ответов сервера
  Автор: ralevech
  ====================================================================
*/

/**
 * Глобальное состояние LED
 * @type {boolean} false - выключен, true - включен
 */
let ledState = false;

/**
 * DOM элементы
 */
const ledButton = document.getElementById('ledButton');
const statusButton = document.getElementById('statusButton');
const statusDiv = document.getElementById('status');
const ledStateDiv = document.getElementById('ledState');

/**
 * Переключение состояния LED
 * Отправляет запрос на сервер и обновляет интерфейс
 */
async function toggleLed() {
    const targetState = !ledState;
    const url = targetState ? '/api/led/on' : '/api/led/off';
    
    // Визуальная обратная связь
    setButtonLoading(ledButton, true, 'Отправка...');
    setStatusWaiting('Отправка запроса...');
    
    try {
        const response = await fetch(url);
        const data = await response.text();
        
        // Обновляем состояние
        ledState = targetState;
        updateLedButtonStyle();
        
        // Успешный ответ
        setStatusSuccess(`Ответ: ${data}`);
        updateLedStateDisplay();
        
    } catch (error) {
        // Обработка ошибки
        setStatusError(`Ошибка: ${error}`);
    }
    
    // Сброс состояния кнопки
    setButtonLoading(ledButton, false, '');
    
    // Автоматический сброс статуса через 3 секунды
    resetStatusAfterDelay();
}

/**
 * Получение статуса с сервера
 * Запрашивает uptime, heap, состояние WiFi и LED
 */
async function getStatus() {
    setButtonLoading(statusButton, true, 'Запрос...');
    setStatusWaiting('Запрос статуса...');
    
    try {
        const response = await fetch('/api/status');
        const data = await response.text();
        
        // Парсим JSON ответ
        try {
            const json = JSON.parse(data);
            
            // Синхронизируем состояние LED
            if (json.led_state !== undefined) {
                ledState = json.led_state;
                updateLedButtonStyle();
                updateLedStateDisplay();
            }
            
            // Формируем читаемый статус
            const wifiStatus = json.wifi === 'connected' ? 'подключен' : 'отключен';
            setStatusSuccess(
                `Статус: uptime=${json.uptime}с, ` +
                `heap=${json.heap} байт, ` +
                `WiFi=${wifiStatus}`
            );
            
        } catch (e) {
            // Если ответ не JSON
            setStatusSuccess(`Ответ: ${data}`);
        }
        
    } catch (error) {
        setStatusError(`Ошибка: ${error}`);
    }
    
    setButtonLoading(statusButton, false, '');
    resetStatusAfterDelay();
}

/**
 * Обновление стиля кнопки LED в зависимости от состояния
 */
function updateLedButtonStyle() {
    if (ledState) {
        ledButton.classList.add('on');
        ledButton.textContent = 'Выключить LED';
    } else {
        ledButton.classList.remove('on');
        ledButton.textContent = 'Включить LED';
    }
}

/**
 * Обновление текста состояния LED
 */
function updateLedStateDisplay() {
    if (ledState) {
        ledStateDiv.innerHTML = 'Состояние LED: Включен';
    } else {
        ledStateDiv.innerHTML = 'Состояние LED: Выключен';
    }
}

/**
 * Установка состояния загрузки для кнопки
 * @param {HTMLElement} button - кнопка
 * @param {boolean} isLoading - состояние загрузки
 * @param {string} loadingText - текст во время загрузки
 */
function setButtonLoading(button, isLoading, loadingText) {
    if (isLoading) {
        button.classList.add('loading');
        button.disabled = true;
        if (loadingText) button.innerHTML = loadingText;
    } else {
        button.classList.remove('loading');
        button.disabled = false;
        if (loadingText) button.innerHTML = loadingText;
    }
}

/**
 * Установка статуса ожидания
 * @param {string} message - сообщение
 */
function setStatusWaiting(message) {
    statusDiv.classList.add('status-update');
    statusDiv.classList.remove('status-success', 'status-error');
    statusDiv.innerHTML = message;
}

/**
 * Установка статуса успеха
 * @param {string} message - сообщение
 */
function setStatusSuccess(message) {
    statusDiv.innerHTML = message;
    statusDiv.classList.add('status-success');
}

/**
 * Установка статуса ошибки
 * @param {string} message - сообщение
 */
function setStatusError(message) {
    statusDiv.innerHTML = message;
    statusDiv.classList.add('status-error');
}

/**
 * Автоматический сброс статуса через 3 секунды
 */
function resetStatusAfterDelay() {
    setTimeout(() => {
        statusDiv.classList.remove('status-update', 'status-success', 'status-error');
        
        // Через секунду возвращаем стандартное сообщение
        setTimeout(() => {
            if (statusDiv.innerHTML !== 'Ожидание команды...') {
                statusDiv.innerHTML = 'Ожидание команды...';
                statusDiv.style.backgroundColor = '';
            }
        }, 1000);
    }, 3000);
}

/**
 * Назначение обработчиков событий
 */
function initEventHandlers() {
    ledButton.addEventListener('click', toggleLed);
    statusButton.addEventListener('click', getStatus);
}

/**
 * Инициализация при загрузке страницы
 */
function init() {
    initEventHandlers();
    getStatus();  // Запрашиваем начальное состояние
}

// Запуск инициализации после загрузки DOM
document.addEventListener('DOMContentLoaded', init);