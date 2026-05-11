/*
  ====================================================================
  script.js - Клиентская логика веб-интерфейса ESP32
  ====================================================================
  Описание: 
    - Управление LED (HTTP API)
    - Получение статуса (HTTP API + WebSocket)
    - Управление аудио через WebSocket (громкость, воспроизведение, стоп)
    - WebSocket для мгновенного обмена данными
  Автор: ralevech
  ====================================================================
*/

// ===== ГЛОБАЛЬНЫЕ ПЕРЕМЕННЫЕ =====

let ledState = false;          // Текущее состояние LED (false = выкл, true = вкл)
let socket;                    // WebSocket соединение

// ===== DOM ЭЛЕМЕНТЫ =====

const ledButton = document.getElementById('ledButton');
const statusButton = document.getElementById('statusButton');
const statusDiv = document.getElementById('status');
const ledStateDiv = document.getElementById('ledState');
const volumeSlider = document.getElementById('volumeSlider');
const volumeValue = document.getElementById('volumeValue');
const playBtn = document.getElementById('playBtn');
const stopBtn = document.getElementById('stopBtn');
const audioStatus = document.getElementById('audioStatus');

// ===== WEBSOCKET ФУНКЦИИ =====

// Подключение к WebSocket серверу ESP32
function connectWebSocket() {
    const wsUrl = `ws://${window.location.host}/ws`;
    socket = new WebSocket(wsUrl);
    
    socket.onopen = function() {
        console.log('WebSocket подключен');
        setStatusSuccess('WebSocket подключен');
    };
    
    // Обработка входящих сообщений от ESP32
    socket.onmessage = function(event) {
        const message = event.data;
        console.log('Получено:', message);
        
        // Команда: изменение громкости
        if (message.startsWith('VOL:')) {
            const vol = message.substring(4);
            volumeSlider.value = vol;
            volumeValue.textContent = vol;
        }
        // Команда: изменение состояния LED
        else if (message.startsWith('LED:')) {
            const state = message.substring(4);
            ledState = (state === 'ON');
            updateLedButtonStyle();
            updateLedStateDisplay();
        }
        // Команда: статус аудио
        else if (message.startsWith('AUDIO:')) {
            const status = message.substring(6);
            audioStatus.textContent = status;
        }
        // Команда: общий статус
        else if (message.startsWith('STATUS:')) {
            const status = message.substring(7);
            setStatusSuccess(status);
        }
    };
    
    socket.onerror = function(error) {
        console.error('WebSocket ошибка:', error);
        setStatusError('Ошибка WebSocket');
    };
    
    // Автоматическое переподключение при разрыве
    socket.onclose = function() {
        console.log('WebSocket отключен, переподключение через 3 секунды');
        setTimeout(connectWebSocket, 3000);
    };
}

// Отправка команды через WebSocket
function sendCommand(command) {
    if (socket && socket.readyState === WebSocket.OPEN) {
        socket.send(command);
    } else {
        console.warn('WebSocket не подключен');
        setStatusError('Нет соединения');
    }
}

// ===== УПРАВЛЕНИЕ ГРОМКОСТЬЮ =====

// Настройка слайдера громкости
function setupVolumeControl() {
    volumeSlider.addEventListener('input', function() {
        const vol = this.value;
        volumeValue.textContent = vol;
        sendCommand(`VOL:${vol}`);
    });
}

// ===== УПРАВЛЕНИЕ АУДИО =====

// Воспроизведение MP3 (имя файла из LittleFS)
function setupPlayControl() {
    const filename = 'ksyusha.mp3';
    playBtn.addEventListener('click', function() {
        sendCommand(`PLAY:${filename}`);
        audioStatus.textContent = 'Загрузка...';
    });
}

// Остановка воспроизведения
function setupStopControl() {
    stopBtn.addEventListener('click', function() {
        sendCommand('STOP');
    });
}

// ===== УПРАВЛЕНИЕ LED (HTTP API) =====

// Переключение состояния LED
async function toggleLed() {
    const targetState = !ledState;
    const url = targetState ? '/api/led/on' : '/api/led/off';
    
    setButtonLoading(ledButton, true, 'Отправка...');
    setStatusWaiting('Отправка запроса...');
    
    try {
        const response = await fetch(url);
        const data = await response.text();
        
        ledState = targetState;
        updateLedButtonStyle();
        updateLedStateDisplay();
        
        setStatusSuccess(`Ответ: ${data}`);
    } catch (error) {
        setStatusError(`Ошибка: ${error}`);
    }
    
    setButtonLoading(ledButton, false, '');
    resetStatusAfterDelay();
}

// Получение общего статуса системы
async function getStatus() {
    setButtonLoading(statusButton, true, 'Запрос...');
    setStatusWaiting('Запрос статуса...');
    
    try {
        const response = await fetch('/api/status');
        const data = await response.text();
        
        try {
            const json = JSON.parse(data);
            if (json.led_state !== undefined) {
                ledState = json.led_state;
                updateLedButtonStyle();
                updateLedStateDisplay();
            }
            setStatusSuccess(`Статус OK: uptime=${json.uptime}с`);
        } catch (e) {
            setStatusSuccess(`Ответ: ${data}`);
        }
    } catch (error) {
        setStatusError(`Ошибка: ${error}`);
    }
    
    setButtonLoading(statusButton, false, '');
    resetStatusAfterDelay();
}

// ===== ВСПОМОГАТЕЛЬНЫЕ ФУНКЦИИ =====

// Обновление стиля кнопки LED
function updateLedButtonStyle() {
    if (ledState) {
        ledButton.classList.add('on');
        ledButton.textContent = 'Выключить LED';
    } else {
        ledButton.classList.remove('on');
        ledButton.textContent = 'Включить LED';
    }
}

// Обновление текста состояния LED
function updateLedStateDisplay() {
    ledStateDiv.innerHTML = ledState ? 'Состояние LED: Включен' : 'Состояние LED: Выключен';
}

// Установка состояния загрузки для кнопки
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

// Установка статуса ожидания
function setStatusWaiting(message) {
    statusDiv.classList.add('status-update');
    statusDiv.classList.remove('status-success', 'status-error');
    statusDiv.innerHTML = message;
}

// Установка статуса успеха
function setStatusSuccess(message) {
    statusDiv.innerHTML = message;
    statusDiv.classList.add('status-success');
}

// Установка статуса ошибки
function setStatusError(message) {
    statusDiv.innerHTML = message;
    statusDiv.classList.add('status-error');
}

// Автоматический сброс статуса через 3 секунды
function resetStatusAfterDelay() {
    setTimeout(() => {
        statusDiv.classList.remove('status-update', 'status-success', 'status-error');
        setTimeout(() => {
            if (statusDiv.innerHTML !== 'Ожидание команды...') {
                statusDiv.innerHTML = 'Ожидание команды...';
            }
        }, 1000);
    }, 3000);
}

// Назначение обработчиков событий
function initEventHandlers() {
    ledButton.addEventListener('click', toggleLed);
    statusButton.addEventListener('click', getStatus);
}

// ===== ИНИЦИАЛИЗАЦИЯ =====

function init() {
    initEventHandlers();
    setupVolumeControl();
    setupPlayControl();
    setupStopControl();
    connectWebSocket();      // Подключение WebSocket
    getStatus();             // Запрашиваем начальное состояние
}

// Запуск инициализации после загрузки DOM
document.addEventListener('DOMContentLoaded', init);