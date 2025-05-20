import 'dart:async';
import 'dart:convert';

import 'package:flutter/material.dart';
import 'package:based_battery_indicator/based_battery_indicator.dart';
import 'package:icaro_app/common/widgets/humiditywidget.dart';

import '../common/services/simulated/sensorsservicesim.dart';
import '../common/data/icarosensors.dart';
import '../common/data/threeaxis.dart';

import '../common/widgets/temperaturewidget.dart';
import '../common/widgets/threeaxiswidget.dart';

class IcaroSensorsPage extends StatefulWidget {
  const IcaroSensorsPage({super.key});
  static const String pageTitle = "Icaro Sensors page";

  @override
  State<IcaroSensorsPage> createState() => _IcaroSensorsPageState();
}

class _IcaroSensorsPageState extends State<IcaroSensorsPage> {
  late StreamSubscription<IcaroSensors> _sensorsSub;

  late IcaroSensors data;
  var gyroscope = ThreeAxis(x: 0, y: 0, z: 0);
  var accelerometer = ThreeAxis(x: 0, y: 0, z: 0);

  var internalTemp = 0.0;
  var externalTemp = 0.0;
  var boardTemp = 0.0;
  var boardHum = 0.0;

  late String dataStr;

  late BasedBatteryStatus batteryStatus;

  // Initial state
  @override
  void initState() {
    super.initState();
    // Initialize state
    dataStr = "NoData";
    batteryStatus = BasedBatteryStatus(value: 0, type: BasedBatteryStatusType.normal);

    internalTemp = 0.0;
    externalTemp = 0.0;
    boardTemp = 0.0;
    boardHum = 0.0;

    gyroscope = ThreeAxis(x: 0, y: 0, z: 0);
    accelerometer = ThreeAxis(x: 0, y: 0, z: 0);

    // Initialize ISS data subscription for tracking
    _sensorsSub = SensorsServiceSim().sensorsStream.listen((newData) {
      _setData(newData);
    });
  }

  @override
  void dispose() {
    _sensorsSub.cancel();
    super.dispose();
  }

  BasedBatteryStatus getBatteryStatus(int value) {
    const lowBatteryThreshold = 40;
    BasedBatteryStatusType type = (value >= lowBatteryThreshold) ? BasedBatteryStatusType.normal : BasedBatteryStatusType.low;

    var status = BasedBatteryStatus(
                    value: value, 
                    type: type,
                  );
    return status;
  }


  void _setData(IcaroSensors newData) {
    setState(() {
      data = newData;
      dataStr = jsonEncode(newData.toMap());
      // dataStr = newData;

      internalTemp = data.sensorBoard.internalTemp;
      externalTemp = data.sensorBoard.externalTemp;
      boardTemp = data.sensorBoard.boardTemp;
      boardHum = data.sensorBoard.boardHum;

      gyroscope = data.sensorBoard.gyroscope;
      accelerometer = data.sensorBoard.acceletometer;

      batteryStatus = BasedBatteryStatus(
                        value: newData.supplyBoard.batteryLevel.toInt(), 
                        type: BasedBatteryStatusType.normal,
                      );
    });
  }

  @override
  Widget build(BuildContext context) {
    final theme = Theme.of(context);
    final style = theme.textTheme.displayMedium!.copyWith(
       color: theme.colorScheme.onPrimary,
       );

    var battery = BasedBatteryIndicator(
        status: batteryStatus,
        trackHeight: 100.0,
        trackAspectRatio: 2.0,
        curve: Curves.ease,
        duration: const Duration(seconds: 1),
        );

    var tempWidget = TemperatureWidget(
      temperatures: [
        TemperatureSubject("Internal Temp", internalTemp, Colors.red),
        TemperatureSubject("External Temp", externalTemp, Colors.blue),
        TemperatureSubject("OnBoard Temp", boardTemp, Colors.grey),
        TemperatureSubject("Humidity", boardHum, Colors.green),
        ]);

    var accel = ThreeAxisWidget(label: "Accelerometer", threeAxis: accelerometer);
    var gyro = ThreeAxisWidget(label: "Gyroscope", threeAxis: gyroscope);
    var humWidget = HumidityWidget(label: "Humidity", value: boardHum);

    // Title 
    var title = Card(
      color: theme.colorScheme.primary,
      elevation: 10,
      child: Padding(
        padding: const EdgeInsets.all(20),
        child: Text(IcaroSensorsPage.pageTitle, style: style,),
      ),
    );

    // Content
    var content = Column(
      mainAxisAlignment: MainAxisAlignment.start,
      children: [
        battery,
        tempWidget,
        Row( 
          children: 
            [
              accel,
              gyro,
              humWidget,
            ]
        ),
        Text(dataStr),
      ],
    );

    return Column(
      mainAxisAlignment: MainAxisAlignment.start,
      children: [
        title,
        Expanded(child: SingleChildScrollView(child: content)),
      ]
    );
  }
}