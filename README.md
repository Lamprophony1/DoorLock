# DoorLock

Prototipo inicial para abrir una puerta electrónica mediante un relé, controlado por un microcontrolador ESP32. El objetivo es exponer el control como un servicio sencillo para poder invocarlo desde cualquier dispositivo o integrarlo con asistentes y automatizaciones sin tener que modificar el sistema.

## Estado actual (v1 del prototipo)

- **Microcontrolador:** NodeMCU ESP32 WROOM 32.
- **Relé:** 2 Channel 5V Optical Isolated Relay Module.
- **Alimentación:** Transformador de fuente conmutada AC-DC 220V a 5V, 2A.
- **Conectividad:** El ESP32 se conecta al Wi-Fi del hogar.
- **Control local:** Se expone un endpoint local con un token sencillo para ejecutar la apertura de la puerta.

## Objetivo del proyecto

Dejar el sistema expuesto como un servicio para poder:

- Abrir la puerta desde cualquier dispositivo sin necesidad de una app dedicada.
- Integrar con Google Home, Siri u otros asistentes.
- Invocar desde un chatbot o desde una aplicación futura.
- Mantener la base del sistema sin cambios mientras se agregan nuevas integraciones.

## Siguientes pasos (dirección general)

- Definir el mecanismo seguro de exposición remota (tokenización, autenticación y autorización).
- Diseñar una API estable para la apertura/cierre.
- Evaluar compatibilidad con plataformas de automatización y asistentes.

## Nota

Este repositorio refleja el prototipo inicial y su descripción general. El objetivo es documentar el punto de partida y la dirección del proyecto.
