# Ícaro's Sensor Box

Desarrollo de uno de los equipos que van a ser lanzados en el proyecto Ícaro, un proyecto organizado y retransmitido por _@Araba_ST_ y _@Labgluon_ en Twitch.

> [!TIP]
> El proyecto **Ícaro** busca lanzar un globo estratosférico (hasta unos 20-30km de altura), subiendo varios equipos, entre ellos: Uno de radioafionados con APRS y Repetidor U/V, y nuestro equipo de sensores.

Nuestro objetivo es aprender a desarrollar equipos más complejos, a lo que sumamos la dificultad de ser el primer proyecto realizado en comunidad con la gente de _www.twitch.tv/labgluon_. Añadiendo la incertidumbre de gestionar equipos remotos y que no han trabajado nunca juntos..

---
Dentro de las misiones concretas está:
- Prueba de comunicaciones Wifi con ESP32-CAM (o equivalente) y ver máximo rango.
    - Mejorar antenas 2.4Ghz
    - Implementar código de lectura y envío de vídeo por wifi en el ESP32.
- Registro de datos de telemetría de la súbida, y comunicación de estos mediante LoRa.
- Creación de PCBs modulares para:
    - La lectura de sensores GPS/IMU/Temp/Barómetro/...
    - Regulación de voltajes.

**Otras ideas** que se han planteado:
- Añadir un control a la antena para que apunte siempre al globo.
- Añadir panel solar 
- Poner un ESP32 a la intemperie y ver cuando deja de funcionar.

## Estructura de carpetas

```
Icaro
├── Docs                    # Documentacion general
│   ├── NamingAndCoding.md  # Normas para nombrar ficheros/variables ...
│   └── Design.md           # Diseño a alto nivel
├── IcaroSW/                # Carpeta donde integramos el software final
│   └── ... 
├── Mechanical/             # Diseños 3D y mecánicos
│   └── ... 
└── Modules/                  
│   ├── <Module>             # Una carpeta por tarjeta
│       ├── Doc             # Documentación de la tarjeta
│       ├── PCB             # Diseño de la PCB
│       ├── Software        # Software si el módulo es uno completo
│       └── Library         # Librería SW para manejar la tarjeta.
|   └── ...
```

La idea es que por cada tarjeta que diseñemos tengamos una librería de software pensada para usar los componentes de esa tarjeta. Y que luego, en _IcaroSW_ se haga el software que se considera el principal. 