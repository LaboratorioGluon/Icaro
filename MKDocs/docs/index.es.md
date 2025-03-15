# Ícaro's Sensor Box

## Introducción
El proyecto **Ícaro** busca lanzar un globo estratosférico (hasta unos 20-30 km de altura), incluyendo varios equipos, entre ellos:
- Un equipo de radioaficionados con APRS y Repetidor U/V.
- **Nuestro equipo de sensores.**

Este proyecto es parte de una iniciativa organizada y retransmitida por _[Araba_ST](https://twitch.com/Araba_ST)_ y _[Labgluon](https://www.twitch.tv/labgluon)_ en Twitch.

!!! info
    Nuestro objetivo es desarrollar equipos más complejos, con la dificultad adicional de ser el primer proyecto realizado en comunidad con la gente de Twitch. También enfrentamos el reto de gestionar equipos remotos que no han trabajado juntos antes.

## Objetivos del Proyecto

- **Prueba de comunicaciones WiFi** con ESP32-CAM (o equivalente) para determinar el máximo rango.
  - Mejorar antenas de 2.4 GHz.
  - Implementar el código para la lectura y envío de vídeo por WiFi en el ESP32.
- **Registro de datos de telemetría** de la subida y transmisión mediante LoRa.
- **Diseño de PCBs modulares** para:
  - Lectura de sensores GPS/IMU/Temp/Barómetro, etc.
  - Regulación de voltajes.

## Otras Ideas

- Implementar un sistema de orientación automática de la antena hacia el globo.
- Integrar paneles solares.
- Exponer un ESP32 a la intemperie para estudiar su resistencia.

## Estructura del Proyecto

```plaintext
Icaro
├── Docs/                    # Documentación general
│   ├── NamingAndCoding.md   # Normas para nombrar ficheros/variables...
│   └── Design.md            # Diseño a alto nivel
├── IcaroSW/                 # Software principal
├── Mechanical/              # Diseños 3D y mecánicos
├── Modules/                 # Módulos de hardware
│   ├── <Module>/            # Carpeta por tarjeta
│   │   ├── Doc/             # Documentación
│   │   ├── PCB/             # Diseño de la PCB
│   │   ├── Software/        # Software asociado
│   │   └── Library/         # Librería para el módulo
│   └── ...
```
Cada tarjeta diseñada contará con una librería de software específica, mientras que en _IcaroSW_ se desarrollará el software principal de la misión.
