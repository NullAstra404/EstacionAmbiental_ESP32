# 🌡️ Sistema de Monitoreo Ambiental con ESP32 y FreeRTOS

Este proyecto implementa un sistema de monitoreo ambiental basado en el microcontrolador ESP32. Utiliza el sistema operativo FreeRTOS para ejecutar tareas concurrentes encargadas de leer temperatura y humedad con un sensor DHT11, mostrar los datos en una pantalla OLED y enviar alertas automáticas a través de un bot de Telegram al superar umbrales críticos.

> Proyecto desarrollado para la materia **Sistemas en Tiempo Real** en la Maestría en Ciencias y Tecnologías de Seguridad, INAOE.

---

## 📌 Características Principales

- Lectura de temperatura y humedad con sensor DHT11.
- Visualización de datos en pantalla OLED SSD1306 vía I2C.
- Envío de alertas por Telegram cuando se superan umbrales de temperatura o humedad.
- Tareas concurrentes manejadas por FreeRTOS.
- Comunicación segura por WiFi.
- Documentación completa en LaTeX.
- Evidencia en video (YouTube) y gráficas de avance.

---

## 📦 Requisitos

- ESP32 DevKit V1
- Sensor DHT11 (KY-015)
- Pantalla OLED 0.96” (I2C)
- Plataforma de desarrollo: VSCode + PlatformIO
- Conexión WiFi
- Cuenta de Telegram + creación de un bot

---

## 🚀 Instalación

1. Clona el repositorio:
   ```bash
   git clone https://github.com/tu_usuario/sistema-monitoreo-esp32.git
   cd sistema-monitoreo-esp32


Abre el proyecto con PlatformIO en VSCode.

Configura tu red WiFi y bot de Telegram en config.h.

Conecta el ESP32 vía USB y sube el firmware:
pio run --target upload


👨‍💻 Autor
Israel Jaudy Pérez Bermúdez
Maestría en Ciencias y Tecnologías de Seguridad
Instituto Nacional de Astrofísica, Óptica y Electrónica (INAOE)
