import 'dart:async';

import 'package:flutter/material.dart';

import 'package:icaro_app/common/services/mavservice.dart';
import 'package:icaro_app/sensorspage/humiditywidget.dart';
import 'package:icaro_app/sensorspage/temperaturewidget.dart';
import 'package:icaro_app/sensorspage/threeaxiswidget.dart';

class IcaroMAVSensorsPage extends StatefulWidget {
  const IcaroMAVSensorsPage({super.key});
  static const String pageTitle = "Icaro MAV page";

  @override
  State<IcaroMAVSensorsPage> createState() => _IcaroMAVSensorsPageState();
}

class _IcaroMAVSensorsPageState extends State<IcaroMAVSensorsPage> {
  late StreamSubscription<int>              _mavHeartBeatSub;
  late StreamSubscription<MAVGPSStatus>     _mavGPSStatusSub;
  late StreamSubscription<MAVIMUStatus>     _mavIMUStatusSub;
  late StreamSubscription<MAVSensorsStatus> _mavSensorsStatusSub;

  int              linkCoverage  = 0;
  MAVGPSStatus     gpsStatus     = MAVGPSStatus();
  MAVIMUStatus     imuStatus     = MAVIMUStatus();
  MAVSensorsStatus sensorsStatus = MAVSensorsStatus();

  // Initial state
  @override
  void initState() {
    super.initState();

    // MAV link status monitor
    _mavHeartBeatSub = MAVService().linkStatusStream.listen((newCoverage) {
      setState((){
        linkCoverage = newCoverage;
      });
    });

    // MAV GPS status monitor
    _mavGPSStatusSub = MAVService().gpsStatusStream.listen((newStatus) {
      setState((){
        gpsStatus = newStatus;
      });
    });

    // MAV IMU status monitor
    _mavIMUStatusSub = MAVService().imuStatusStream.listen((newStatus) {
      setState((){
        imuStatus = newStatus;
      });
    });

    // MAV Sensors status monitor
    _mavSensorsStatusSub = MAVService().sensorsStatusStream.listen((newStatus) {
      setState((){
        sensorsStatus = newStatus;
      });
    });
  }

  @override
  void dispose() {
    _mavHeartBeatSub.cancel();
    _mavGPSStatusSub.cancel();
    _mavIMUStatusSub.cancel();
    _mavSensorsStatusSub.cancel();
    super.dispose();
  }

  @override
  Widget build(BuildContext context) {
    final theme = Theme.of(context);
    final style = theme.textTheme.displayMedium!.copyWith(
       color: theme.colorScheme.onPrimary,
       );

    var activeText = "Coverage $linkCoverage";
    var activeCard = Card(
            color: theme.colorScheme.primary,
            elevation: 10,
            child: Padding(
              padding: const EdgeInsets.all(20),
              child: Text(activeText, style: style,),
            ),
          );

    var accel = ThreeAxisWidget(label: "Accelerometer", threeAxis: imuStatus.accel);
    var gyro = ThreeAxisWidget(label: "Gyroscope", threeAxis: imuStatus.gyro);
    
    var humidity = HumidityWidget(label: "Humidity", value: sensorsStatus.humidity);

    var temperatures = TemperatureWidget(
      temperatures: [
        TemperatureSubject("Internal Temp", sensorsStatus.internalTemp, Colors.red),
        TemperatureSubject("External Temp", sensorsStatus.externalTemp, Colors.blue),
        TemperatureSubject("OnBoard Temp", sensorsStatus.onboardTemp, Colors.grey),
        TemperatureSubject("Humidity", sensorsStatus.humidity, Colors.green),
        ]);

    return Column(
        mainAxisAlignment: MainAxisAlignment.center,
        children: [
          activeCard,
          Card(
            color: theme.colorScheme.primary,
            elevation: 10,
            child: Padding(
              padding: const EdgeInsets.all(20),
              child: Text(IcaroMAVSensorsPage.pageTitle, style: style,),
            ),
          ),
          Column( 
            children: [
              temperatures,
              Row(
                children: [
                  accel,
                  gyro,
                  humidity,
                ],
              ),
            ],
          ),
        ],
      );
  }
}