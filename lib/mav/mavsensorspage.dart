import 'dart:async';

import 'package:flutter/material.dart';

import 'package:icaro_app/common/services/mavservice.dart';
import 'package:icaro_app/common/widgets/humiditywidget.dart';
import 'package:icaro_app/common/widgets/powerwidget.dart';
import 'package:icaro_app/common/widgets/summarywidget.dart';
import 'package:icaro_app/common/widgets/temperaturewidget.dart';
import 'package:icaro_app/common/widgets/threeaxiswidget.dart';

class IcaroMAVSensorsPage extends StatefulWidget {
  const IcaroMAVSensorsPage({super.key});
  static const String pageTitle = "Icaro MAV Sensors page";

  @override
  State<IcaroMAVSensorsPage> createState() => _IcaroMAVSensorsPageState();
}

class _IcaroMAVSensorsPageState extends State<IcaroMAVSensorsPage> {
  late StreamSubscription<double>           _mavCoverageSub;
  late StreamSubscription<MAVGPSStatus>     _mavGPSStatusSub;
  late StreamSubscription<MAVIMUStatus>     _mavIMUStatusSub;
  late StreamSubscription<MAVBatteryStatus> _mavBatteryStatusSub;
  late StreamSubscription<MAVSensorsStatus> _mavSensorsStatusSub;

  double           linkCoverage  = 0;
  MAVGPSStatus     gpsStatus     = MAVGPSStatus();
  MAVIMUStatus     imuStatus     = MAVIMUStatus();
  MAVBatteryStatus batteryStatus = MAVBatteryStatus();
  MAVSensorsStatus sensorsStatus = MAVSensorsStatus();

  // Initial state
  @override
  void initState() {
    super.initState();

    // MAV link status monitor
    _mavCoverageSub = MAVService().linkStatusStream.listen((newCoverage) {
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

    // MAV Battery status monitor
    _mavBatteryStatusSub = MAVService().batteryStatusStream.listen((newStatus) {
      setState((){
        batteryStatus = newStatus;
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
    _mavCoverageSub.cancel();
    _mavGPSStatusSub.cancel();
    _mavIMUStatusSub.cancel();
    _mavBatteryStatusSub.cancel();
    _mavSensorsStatusSub.cancel();
    super.dispose();
  }

  Widget buildContent(BuildContext context) {
    final theme = Theme.of(context);
    final style = theme.textTheme.displayMedium!.copyWith(
       color: theme.colorScheme.onPrimary,
       );

    var accel = ThreeAxisWidget(label: "Accelerometer", threeAxis: imuStatus.accel);
    var gyro = ThreeAxisWidget(label: "Gyroscope", threeAxis: imuStatus.gyro);
    
    var humidity = HumidityWidget(label: "Humidity", value: sensorsStatus.humidity);

    var power = Powerwidget(
      label: "Battery", 
      value: batteryStatus.batteryPercentage,
      status3v3: batteryStatus.status3v3,
      status5v0: batteryStatus.status5v0,
    );

    var temperatures = TemperatureWidget(
      temperatures: [
        TemperatureSubject("Internal Temp", sensorsStatus.internalTemp, Colors.red),
        TemperatureSubject("External Temp", sensorsStatus.externalTemp, Colors.blue),
        TemperatureSubject("OnBoard Temp", sensorsStatus.onboardTemp, Colors.grey),
        TemperatureSubject("Humidity", sensorsStatus.humidity, Colors.green),
      ]
    );

    var summary = SummaryWidget(
      batteryPercentage: batteryStatus.batteryPercentage,
      coveragePercentage: linkCoverage,
    );

    return LayoutBuilder(
        builder: (BuildContext context, BoxConstraints constraints) {
          // Check the available width and height in constraints
          double availableWidth = constraints.maxWidth;
          // double availableHeight = constraints.maxHeight;
        
          if (availableWidth >= 735) {
            // For larger screen width (e.g., tablets), show a two-column layout
            // return Text("2 columns");
            return Column(
              mainAxisAlignment: MainAxisAlignment.center,
              children: [
                summary,
                temperatures,
                Row(
                  children: [
                    accel,
                    gyro,
                    humidity,
                    power,
                  ],
                ),
              ],
            );
          } else if (availableWidth >= 450)  {
            // For smaller screen width (e.g., smartphones), show a single-column layout
            // return Text("1 column");
            return Column(
              mainAxisAlignment: MainAxisAlignment.center,
              children: [
                summary,
                temperatures,
                Row(
                  children: [
                    accel,
                    gyro,
                  ],
                ),
                Row(
                  children: [
                    humidity,
                    power,
                  ],
                ),
              ],
            );
          } else {
            // For smaller screen width (e.g., smartphones), show a single-column layout
            // return Text("1 column");
            return Column(
              mainAxisAlignment: MainAxisAlignment.center,
              children: [
                summary,
                temperatures,
                accel,
                gyro,
                humidity,
                power,
              ],
            );
          }
        },
      );


  }


  @override
  Widget build(BuildContext context) {
    final theme = Theme.of(context);
    final style = theme.textTheme.displayMedium!.copyWith(
       color: theme.colorScheme.onPrimary,
       );

    // Title
    var title = Card(
            color: theme.colorScheme.primary,
            elevation: 10,
            child: Padding(
              padding: const EdgeInsets.all(20),
              child: Text(IcaroMAVSensorsPage.pageTitle, style: style,),
            ),
          );

    // Content  
    var content = buildContent(context);
    return Column(
      mainAxisAlignment: MainAxisAlignment.start,
      children: [
        title,
        Expanded(child: SingleChildScrollView(child: content)),
      ]
    );
  }
}