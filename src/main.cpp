#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <DHT.h>

// =================== CONFIGURACIÓN ===================

// ⚠️ Coloca tus datos aquí
const char* ssid = "Your SSID-XXX";
const char* password = "XXXXXXXXXXXX";

// Datos de Telegram
const char* botToken = "botxxxxxxxxxx:XXXXXXXXX_XXXXXXXXXXXXXXXXXXXXXXXXX";
const char* chatId = "XXXXXXXXXX";

// Umbrales
#define UMBRAL_TEMP 29.0
#define UMBRAL_HUM  50.0

// ================ PANTALLA OLED =====================

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// ================ SENSOR DHT11 ======================

#define DHTPIN 4
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

// Variables globales compartidas
float temperatura = 0.0;
float humedad = 0.0;

// Mutex para acceso seguro
SemaphoreHandle_t mutexDatos;

// Flags para evitar alertas repetidas
bool alertaTempEnviada = false;
bool alertaHumEnviada = false;

// ================ CONEXIÓN WI-FI =====================

void conectarWiFi() {
  WiFi.begin(ssid, password);
  Serial.print("Conectando a WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\n✅ Conectado a WiFi");
}

// ================ FUNCIONES TELEGRAM =================

void enviarAlertaTelegram(String mensaje) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    String url = "https://api.telegram.org/" + String(botToken) + "/sendMessage?chat_id=" + chatId + "&text=" + mensaje;
    Serial.println("URL completa que se usará:");
    Serial.println(url);  // 👈 imprime la URL
    http.begin(url);
    int httpResponseCode = http.GET();
    if (httpResponseCode > 0) {
      Serial.println("📨 Alerta enviada por Telegram");
    } else {
      Serial.printf("❌ Error al enviar alerta: %d\n", httpResponseCode);
    }
    http.end();
  } else {
    Serial.println("❌ Sin conexión WiFi para enviar alerta");
  }
}

// ================ TAREA: Lectura del sensor =================

void TareaLecturaSensor(void *parameter) {
  for (;;) {
    float t = dht.readTemperature();
    float h = dht.readHumidity();

    if (!isnan(t) && !isnan(h)) {
      xSemaphoreTake(mutexDatos, portMAX_DELAY);
      temperatura = t;
      humedad = h;
      xSemaphoreGive(mutexDatos);
      Serial.printf("[Sensor] T=%.1f°C, H=%.1f%%\n", t, h);
    } else {
      Serial.println("[Sensor] Error en la lectura");
    }
    vTaskDelay(pdMS_TO_TICKS(2000));
  }
}

// ================ TAREA: Mostrar en OLED =================

void TareaActualizarPantalla(void *parameter) {
  for (;;) {
    float t, h;
    xSemaphoreTake(mutexDatos, portMAX_DELAY);
    t = temperatura;
    h = humedad;
    xSemaphoreGive(mutexDatos);

    display.clearDisplay();
    display.setTextSize(2);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 10);
    display.printf("T: %.1f C\n", t);
    display.setCursor(0, 35);
    display.printf("H: %.1f %%", h);
    display.display();

    vTaskDelay(pdMS_TO_TICKS(2000));
  }
}

// ================ TAREA: Alerta por Telegram =================

void TareaAlertaTelegram(void *parameter) {
  for (;;) {
    float t, h;
    xSemaphoreTake(mutexDatos, portMAX_DELAY);
    t = temperatura;
    h = humedad;
    xSemaphoreGive(mutexDatos);

    if (t > UMBRAL_TEMP && !alertaTempEnviada) {
      enviarAlertaTelegram("⚠️ Alerta: Temperatura alta: " + String(t) + " °C");
      alertaTempEnviada = true;
    }
    if (t <= UMBRAL_TEMP) alertaTempEnviada = false;

    if (h > UMBRAL_HUM && !alertaHumEnviada) {
      enviarAlertaTelegram("⚠️ Alerta: Humedad alta: " + String(h) + " %");
      alertaHumEnviada = true;
    }
    if (h <= UMBRAL_HUM) alertaHumEnviada = false;

    vTaskDelay(pdMS_TO_TICKS(5000)); // cada 5s
  }
}

// ================ SETUP =================

void setup() {
  Serial.begin(115200);
  conectarWiFi();
  dht.begin();

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("❌ Pantalla OLED no detectada");
    while (true);
  }

  mutexDatos = xSemaphoreCreateMutex();

  xTaskCreatePinnedToCore(TareaLecturaSensor, "LecturaSensor", 2048, NULL, 1, NULL, 1);
  xTaskCreatePinnedToCore(TareaActualizarPantalla, "ActualizarOLED", 2048, NULL, 1, NULL, 1);
  xTaskCreatePinnedToCore(TareaAlertaTelegram, "AlertaTelegram", 4096, NULL, 1, NULL, 0);
}

void loop() {
  // Nada aquí, usamos FreeRTOS
}
