import 'package:flutter/material.dart';
import 'package:material_design_icons_flutter/material_design_icons_flutter.dart';

class SummaryWidget extends StatelessWidget {

  final double batteryPercentage;
  final double coveragePercentage;
  final DateTime timestamp;

  const SummaryWidget({super.key, required this.batteryPercentage, required this.coveragePercentage, required this.timestamp});

  Widget getBatteryIcon(double battery)
  {
    var icon = Icon(Icons.battery_full);
    if (battery >= 95.0)
    {
      icon = Icon(MdiIcons.battery);
    }
    else if (battery >= 85.0)
    {
      icon = Icon(MdiIcons.battery90);
    }
    else if (battery >= 75.0)
    {
      icon = Icon(MdiIcons.battery80);
    }
    else if (battery >= 65.0)
    {
      icon = Icon(MdiIcons.battery70);
    }
    else if (battery >= 55.0)
    {
      icon = Icon(MdiIcons.battery60);
    }
    else if (battery >= 45.0)
    {
      icon = Icon(MdiIcons.battery50);
    }
    else if (battery >= 35.0)
    {
      icon = Icon(MdiIcons.battery40);
    }
    else if (battery >= 25.0)
    {
      icon = Icon(MdiIcons.battery30);
    }
    else if (battery >= 15.0)
    {
      icon = Icon(MdiIcons.battery20);
    }
    else if (battery >= 5.0)
    {
      icon = Icon(MdiIcons.battery10);
    }
    else
    {
      icon = Icon(Icons.battery_0_bar);
    }
    return RotatedBox(quarterTurns: 1, child: icon,);
  }

  Icon getCoverageIcon(double coverage)
  {
    if (coverage >= 80.0)
    {
      return Icon(MdiIcons.wifiStrength4);
    }
    else if (coverage >= 60.0)
    {
      return Icon(MdiIcons.wifiStrength3);
    }
    else if (coverage >= 40.0)
    {
      return Icon(MdiIcons.wifiStrength2);
    }
    else if (coverage >= 20.0)
    {
      return Icon(MdiIcons.wifiStrength1);
    }
    else
    {
      return Icon(MdiIcons.wifiStrengthOff);
    }
  }

  @override
  Widget build(BuildContext context) {

    var dateSummary = Text(
      "${timestamp.day.toString().padLeft(2, '0')}/${timestamp.month.toString().padLeft(2, '0')}/${timestamp.year}",
    );

    var timeSummary = Text(
      "${timestamp.hour.toString().padLeft(2, '0')}:${timestamp.minute.toString().padLeft(2, '0')}",
    );

    var batterySummary = Row(
      mainAxisAlignment: MainAxisAlignment.end,
      children: [
        Text('${batteryPercentage.toStringAsFixed(0)}%'),
        getBatteryIcon(batteryPercentage),
      ],
    );

    var coverageSummary = Row(
      mainAxisAlignment: MainAxisAlignment.end,
      children: [
        Text('${coveragePercentage.toStringAsFixed(0)}%'),
        getCoverageIcon(coveragePercentage),
      ],
    );

    return Column(
      crossAxisAlignment: CrossAxisAlignment.end,
      mainAxisAlignment: MainAxisAlignment.end,
      children: [
        timeSummary,
        dateSummary,
        batterySummary,
        coverageSummary,
      ],
    );
  }

}
