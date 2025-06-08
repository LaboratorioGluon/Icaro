import 'dart:async';

import 'package:flutter/material.dart';
import 'package:icaro_app/common/services/mavservice.dart';
import 'package:icaro_app/common/widgets/summarywidget.dart';

class Summary extends StatefulWidget {
  const Summary({super.key});

  @override
  State<Summary> createState() => _SummaryState();
}

class _SummaryState extends State<Summary> {
  late StreamSubscription<double>           _mavCoverageSub;
  late StreamSubscription<MAVBatteryStatus> _mavBatteryStatusSub;
  late StreamSubscription<DateTime>         _mavTimestampStatusSub;

  double           linkCoverage    = 0;
  MAVBatteryStatus batteryStatus   = MAVBatteryStatus();
  DateTime         timestampStatus = DateTime.now();

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

    // MAV Battery status monitor
    _mavBatteryStatusSub = MAVService().batteryStatusStream.listen((newStatus) {
      setState((){
        batteryStatus = newStatus;
      });
    });

    // MAV Timestamp status monitor
    _mavTimestampStatusSub = MAVService().timestampStatusStream.listen((newStatus) {
      setState((){
        timestampStatus = newStatus;
      });
    });
  }

  @override
  void dispose() {
    _mavCoverageSub.cancel();
    _mavBatteryStatusSub.cancel();
    _mavTimestampStatusSub.cancel();
    super.dispose();
  }

  @override
  Widget build(BuildContext context) {
    return SummaryWidget(
      batteryPercentage: batteryStatus.batteryPercentage,
      coveragePercentage: linkCoverage,
      timestamp: timestampStatus,
    );
  }
}