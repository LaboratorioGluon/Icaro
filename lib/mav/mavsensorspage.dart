import 'dart:async';

import 'package:flutter/material.dart';

import 'package:icaro_app/common/services/mavservice.dart';
import 'package:icaro_app/sensorspage/threeaxiswidget.dart';

class IcaroMAVSensorsPage extends StatefulWidget {
  const IcaroMAVSensorsPage({super.key});
  static const String pageTitle = "Icaro MAV page";

  @override
  State<IcaroMAVSensorsPage> createState() => _IcaroMAVSensorsPageState();
}

class _IcaroMAVSensorsPageState extends State<IcaroMAVSensorsPage> {
  late StreamSubscription<int>          _mavHeartBeatSub;
  late StreamSubscription<MAVGPSStatus> _mavGPSStatusSub;
  late StreamSubscription<MAVIMUStatus> _mavIMUStatusSub;

  int linkCoverage = 0;
  MAVGPSStatus gpsStatus = MAVGPSStatus();
  MAVIMUStatus imuStatus = MAVIMUStatus();

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
  }

  @override
  void dispose() {
    _mavHeartBeatSub.cancel();
    _mavGPSStatusSub.cancel();
    _mavIMUStatusSub.cancel();
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
          Row(
            children: [
              accel,
              gyro,
            ],
          ),
        ],
      );
  }
}