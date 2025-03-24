import 'dart:async';
import 'dart:convert';

import 'package:flutter/material.dart';
import 'package:based_battery_indicator/based_battery_indicator.dart';

import 'common/services/simulated/sensorsservicesim.dart';
import 'common/data/icarosensors.dart';

class IcaroSensorsPage extends StatefulWidget {
  const IcaroSensorsPage({super.key});
  static const String pageTitle = "Icaro Sensors page";

  @override
  State<IcaroSensorsPage> createState() => _IcaroSensorsPageState();
}

class _IcaroSensorsPageState extends State<IcaroSensorsPage> {
  late StreamSubscription<IcaroSensors> _sensorsSub;

  late IcaroSensors data;
  late String dataStr;

  late BasedBatteryStatus batteryStatus;




  // Initial state
  @override
  void initState() {
    super.initState();
    // Initialize state
    dataStr = "NoData";
    batteryStatus = BasedBatteryStatus(value: 0, type: BasedBatteryStatusType.normal);

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

    return Column(
        mainAxisAlignment: MainAxisAlignment.start,
        children: [
          Card(
            color: theme.colorScheme.primary,
            elevation: 10,
            child: Padding(
              padding: const EdgeInsets.all(20),
              child: Text(IcaroSensorsPage.pageTitle, style: style,),
            ),
          ),

          Text(dataStr),
          battery,
        ],
      );
  }
}