import 'dart:async';

import 'package:flutter/material.dart';
import 'package:icaro_app/common/data/icarosensors.dart';
import 'package:icaro_app/common/services/simulated/sensorsservicesim.dart';
import 'package:syncfusion_flutter_charts/charts.dart';
import 'package:syncfusion_flutter_gauges/gauges.dart';

class TemperatureWidget extends StatefulWidget {

  @override
  State<TemperatureWidget> createState() => _TemperatureWidgetState();
}

class TemperaturePoint{
  final int x;
  final double y;

  TemperaturePoint({required this.x, required this.y});
}

class _TemperatureWidgetState extends State<TemperatureWidget> {
  late StreamSubscription<IcaroSensors> _sensorsSub;

  static const Color warmColor = Colors.red;
  static const Color coldColor = Colors.blue;

  late double internalTemp;
  late double externalTemp;

  late List<TemperaturePoint>? internalTempData;
  late List<TemperaturePoint>? externalTempData;
  int? counter;

  int maxElems = 10;

  // Initial state
  @override
  void initState() {
    super.initState();
    // Initialize state
    internalTemp = 0.0;
    externalTemp = 0.0;

    internalTempData = <TemperaturePoint>[];
    externalTempData = <TemperaturePoint>[];
    counter = 0;

    // Initialize ISS data subscription for tracking
    _sensorsSub = SensorsServiceSim().sensorsStream.listen((newData) {
      _setData(newData);
    });
  }

  @override
  void dispose() {
    _sensorsSub.cancel();
    internalTempData!.clear();
    externalTempData!.clear();
    super.dispose();
  }

  void _setData(IcaroSensors newData) {
    setState(() {
      internalTemp = newData.sensorBoard.internalTemp;
      externalTemp = newData.sensorBoard.externalTemp;

      if (internalTempData!.length >= 10) {
        internalTempData!.removeAt(0);
      }
      internalTempData!.add(TemperaturePoint(x: counter!, y: internalTemp));

      if (externalTempData!.length >= 10) {
        externalTempData!.removeAt(0);
      }
      externalTempData!.add(TemperaturePoint(x: counter!, y: externalTemp));

      counter = counter! + 1;
    });
  }


  @override
  Widget build(BuildContext context) {

    return LayoutBuilder(
        builder: (BuildContext context, BoxConstraints constraints) {
          // Check the available width and height in constraints
          double availableWidth = constraints.maxWidth;
          // double availableHeight = constraints.maxHeight;
        
          if (availableWidth >= 512) {
            // For larger screen width (e.g., tablets), show a two-column layout
            // return Text("2 columns");
            return _buildTwoColumnLayout();
          } else {
            // For smaller screen width (e.g., smartphones), show a single-column layout
            // return Text("1 column");
            return _buildSingleColumnLayout();
          }
        },
      );
  }

  Widget _buildTwoColumnLayout() {

    var currentTemps = SfLinearGauge(
        minimum: -100.0,
        maximum: 100.0,
        orientation: LinearGaugeOrientation.vertical,
        axisTrackStyle: LinearAxisTrackStyle(
          gradient: LinearGradient(
            colors: [warmColor, Colors.grey, coldColor],
            begin: Alignment.topCenter, 
            end: Alignment.bottomCenter
          )
        ),
        isAxisInversed: false,
        ranges: [],
        markerPointers: [
          LinearShapePointer(
            value: externalTemp,
            color: coldColor
          ),
          LinearShapePointer(
            value: internalTemp,
            color: warmColor
          ),
        ],
        // barPointers: [LinearBarPointer(value: 20)],
      );

      var splineSeries = <SplineSeries<TemperaturePoint, num>>[
        SplineSeries<TemperaturePoint, num>(
          name: "InternalTemp",
          dataSource: [...internalTempData!],
          xValueMapper: (TemperaturePoint data, int index) => data.x,
          yValueMapper: (TemperaturePoint data, int index) => data.y,
          animationDuration: 0,
          color: warmColor,
        ),
        SplineSeries<TemperaturePoint, num>(
          name: "ExternalTemp",
          dataSource: [...externalTempData!],
          xValueMapper: (TemperaturePoint data, int index) => data.x,
          yValueMapper: (TemperaturePoint data, int index) => data.y,
          animationDuration: 0,
          color: coldColor,
        )
      ];

      var splineLegend = Legend(
        isVisible: true,
        position: LegendPosition.bottom,
        alignment: ChartAlignment.center
      );

      var graph = SfCartesianChart(
        plotAreaBorderWidth: 0,
        primaryXAxis: const NumericAxis(
          majorGridLines: MajorGridLines(width: 0),
        ),
        primaryYAxis: const NumericAxis(
          minimum: -125,
          maximum: 125,
          axisLine: AxisLine(width: 0),
          majorTickLines: MajorTickLines(size: 0),
        ),
        series: splineSeries,
        legend: splineLegend,
      );

    return Row(
      children: [
        Expanded(child: graph,),
        currentTemps,
      ],
    );
  }

  Widget _buildSingleColumnLayout() {

    var currentTemps = SfLinearGauge(
        minimum: -100.0,
        maximum: 100.0,
        orientation: LinearGaugeOrientation.horizontal,
        axisTrackStyle: LinearAxisTrackStyle(
          gradient: LinearGradient(
            colors: [warmColor, Colors.grey, coldColor],
            begin: Alignment.centerRight, 
            end: Alignment.centerLeft
          )
        ),
        isAxisInversed: false,
        ranges: [],
        markerPointers: [
          LinearShapePointer(
            value: externalTemp,
            color: coldColor
          ),
          LinearShapePointer(
            value: internalTemp,
            color: warmColor
          ),
        ],
        // barPointers: [LinearBarPointer(value: 20)],
      );

      var splineSeries = <SplineSeries<TemperaturePoint, num>>[
        SplineSeries<TemperaturePoint, num>(
          dataSource: [...internalTempData!],
          xValueMapper: (TemperaturePoint data, int index) => data.x,
          yValueMapper: (TemperaturePoint data, int index) => data.y,
          animationDuration: 5000,
          color: warmColor,
        ),
        SplineSeries<TemperaturePoint, num>(
          dataSource: [...externalTempData!],
          xValueMapper: (TemperaturePoint data, int index) => data.x,
          yValueMapper: (TemperaturePoint data, int index) => data.y,
          animationDuration: 5000,
          color: coldColor,
        )
      ];

      var graph = SfCartesianChart(
        plotAreaBorderWidth: 0,
        primaryXAxis: const NumericAxis(
          majorGridLines: MajorGridLines(width: 0),
        ),
        primaryYAxis: const NumericAxis(
          minimum: -125,
          maximum: 125,
          axisLine: AxisLine(width: 0),
          majorTickLines: MajorTickLines(size: 0),
        ),
        series: splineSeries,
      );

    return Column(
      children: [
        graph,
        currentTemps,
      ],
    );
  }
}
