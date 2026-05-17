// ====================================================================
// 03_server_handlers.cpp - Обработчики WebSocket
// Автор: ralevech
// ====================================================================

#include <Arduino.h>
#include "config.h"
#include "common.h"
#include "globals.h"
#include "server_priv.h"
#include "tasks.h"

#if ENABLE_WEBSERVER

// --------------------------------------------------------------------
// onWebSocketEvent() - Обработка входящих WebSocket команд
// --------------------------------------------------------------------
void onWebSocketEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, 
                      AwsEventType type, void *arg, uint8_t *data, size_t len) {
    if (type == WS_EVT_DATA) {
        AwsFrameInfo *info = (AwsFrameInfo*)arg;
        if (info->final && info->index == 0 && info->len == len && len > 0) {
            data[len] = '\0';
            String msg = String((char*)data);
            
            if (msg == "LED_ON") {
                ledState = true;
                digitalWrite(LED_PIN, HIGH);
                ws.textAll("LED:ON");
            }
            else if (msg == "LED_OFF") {
                ledState = false;
                digitalWrite(LED_PIN, LOW);
                ws.textAll("LED:OFF");
            }
            #if ENABLE_AUDIO
            else if (msg.startsWith("VOL:")) {
                int vol = msg.substring(4).toInt();
                setAudioVolume(vol);
                ws.textAll("VOL:" + String(vol));
            }
            else if (msg.startsWith("PLAY:")) {
                String file = msg.substring(5);
                playMP3(file);
            }
            else if (msg == "STOP") {
                stopAudio();
            }
            #endif
        }
    }
}

#endif