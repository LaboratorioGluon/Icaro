# Icaro Sensor Box

## Diseño

Las tarjetas tienes que estar comunicadas todas con todas a través de un conector común. Las dos opciones más comunes son:
- **Sandwich**: apilando todas las tarjetas del proyecto [Issue [#1]](../issues/1)
- **Backplane**: estilo placa base, donde una tarjeta maestra tiene los conectores para enchufar el resto de tarjetas como si de una placa base se tratase.

En cualquier caso, el conector y pinout será compartido por todas las tarjetas que montemos en el proyecto.

## Módulos

- **Sensor Board** [**[+Info]**](../Modules/SensorBoard/Doc/SensorBoard.md): Tarjeta con los sensores básicos de telemetría.
- **Supply Board** [**[+Info]**](../Modules/SupplyBoard/Doc/SupplyBoard.md): Encargada de convertir el voltaje de la batería en 3.3 V y 5 V.
- **Camera Board** [**[+Info]**](../Modules/CamBoard/Doc/CameraBoard.md): Encargada de convertir el voltaje de la batería en 3.3 V y 5 V.

## Comunicación

Para comunicar las tarjetas se usará el protocolo _TBD_, y a ese protocolo se conectarán todas las tarjetas. El protocolo de comunicación será estándar y compartido entre todos mediante una librería agnóstica al HW, por lo que se necesitará una capa HAL de acceso al uC.

<div style="text-align: center;">
    <img width="80%" src="Software.png">
</div>