import 'dart:async';

import 'package:flutter/material.dart';
import 'package:icaro_app/common/data/icarosensors.dart';
import 'package:syncfusion_flutter_charts/charts.dart';
import 'package:syncfusion_flutter_gauges/gauges.dart';

class TemperatureSubject{
  final String label;
  final double value;
  final Color color;

  TemperatureSubject(this.label, this.value, this.color);
}

class TemperaturePoint{
  final int x;
  final double y;

  TemperaturePoint({required this.x, required this.y});
}

class TemperatureWidget extends StatefulWidget {

  final List<TemperatureSubject> temperatures;

  const TemperatureWidget({super.key, required this.temperatures});

  @override
  State<TemperatureWidget> createState() => _TemperatureWidgetState();
}

class _TemperatureWidgetState extends State<TemperatureWidget> {
  late StreamSubscription<IcaroSensors> _sensorsSub;

  final Map<String, List<TemperaturePoint>> history = {};

  final int maxElems = 50;
  int elemCounter = 0;

  // Initial state
  @override
  void initState() {
    super.initState();
    // Initialize state
    for (var temp in widget.temperatures) {
      if (!history.containsKey(temp.label)) {
        history[temp.label] = <TemperaturePoint>[];
      }
    }
  }

  @override
  void dispose() {
    _sensorsSub.cancel();
    for (var temp in widget.temperatures) {
      if (!history.containsKey(temp.label)) {
        history[temp.label]!.clear();
      }
    }
    history.clear();
    super.dispose();
  }

  @override
  void didUpdateWidget(TemperatureWidget oldWidget) {
    super.didUpdateWidget(oldWidget);

    elemCounter = elemCounter + 1;

    for (var temp in widget.temperatures) {
      if (history.containsKey(temp.label)) {
        history[temp.label]!.add(TemperaturePoint(x: elemCounter, y: temp.value));
        if (history[temp.label]!.length >= maxElems) {
          history[temp.label]!.removeAt(0);
        }
      }
    }

    setState(() {});
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

    var tempColors = <Color>[];
    var tempPointers = <LinearShapePointer>[];
    var tempSplines = <SplineSeries<TemperaturePoint, num>>[];
    
    // Generate data for each temperature given
    for(var temp in widget.temperatures)
    {
      // Generate color
      tempColors.add(temp.color);
    
      // Generate pointer
      tempPointers.add(
          LinearShapePointer(
            value: temp.value, 
            color: temp.color,
          )
        );

      // Generate spline serie
      tempSplines.add(
          SplineSeries<TemperaturePoint, num>(
            name: "InternalTemp",
            dataSource: [...history[temp.label]!],
            xValueMapper: (TemperaturePoint data, int index) => data.x,
            yValueMapper: (TemperaturePoint data, int index) => data.y,
            animationDuration: 0,
            color: temp.color,
          )
        );

    }

    var pointerWidget = SfLinearGauge(
        minimum: -100.0,
        maximum: 100.0,
        orientation: LinearGaugeOrientation.vertical,
        axisTrackStyle: LinearAxisTrackStyle(
          gradient: LinearGradient(
            colors: tempColors,
            begin: Alignment.topCenter, 
            end: Alignment.bottomCenter
          )
        ),
        isAxisInversed: false,
        ranges: [],
        markerPointers: tempPointers,
      );

      var splineLegend = Legend(
        isVisible: true,
        position: LegendPosition.bottom,
        alignment: ChartAlignment.center
      );

      var graphWidget = SfCartesianChart(
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
        series: tempSplines,
        legend: splineLegend,
      );

    return Row(
      children: [
        Expanded(child: graphWidget,),
        pointerWidget,
      ],
    );
  }

  Widget _buildSingleColumnLayout() {

    // var currentTemps = SfLinearGauge(
    //     minimum: -100.0,
    //     maximum: 100.0,
    //     orientation: LinearGaugeOrientation.horizontal,
    //     axisTrackStyle: LinearAxisTrackStyle(
    //       gradient: LinearGradient(
    //         colors: [warmColor, Colors.grey, coldColor],
    //         begin: Alignment.centerRight, 
    //         end: Alignment.centerLeft
    //       )
    //     ),
    //     isAxisInversed: false,
    //     ranges: [],
    //     markerPointers: [
    //       LinearShapePointer(
    //         value: externalTemp,
    //         color: coldColor
    //       ),
    //       LinearShapePointer(
    //         value: internalTemp,
    //         color: warmColor
    //       ),
    //     ],
    //     // barPointers: [LinearBarPointer(value: 20)],
    //   );

    //   var splineSeries = <SplineSeries<TemperaturePoint, num>>[
    //     SplineSeries<TemperaturePoint, num>(
    //       dataSource: [...internalTempData!],
    //       xValueMapper: (TemperaturePoint data, int index) => data.x,
    //       yValueMapper: (TemperaturePoint data, int index) => data.y,
    //       animationDuration: 5000,
    //       color: warmColor,
    //     ),
    //     SplineSeries<TemperaturePoint, num>(
    //       dataSource: [...externalTempData!],
    //       xValueMapper: (TemperaturePoint data, int index) => data.x,
    //       yValueMapper: (TemperaturePoint data, int index) => data.y,
    //       animationDuration: 5000,
    //       color: coldColor,
    //     )
    //   ];

    //   var graph = SfCartesianChart(
    //     plotAreaBorderWidth: 0,
    //     primaryXAxis: const NumericAxis(
    //       majorGridLines: MajorGridLines(width: 0),
    //     ),
    //     primaryYAxis: const NumericAxis(
    //       minimum: -125,
    //       maximum: 125,
    //       axisLine: AxisLine(width: 0),
    //       majorTickLines: MajorTickLines(size: 0),
    //     ),
    //     series: splineSeries,
    //   );

    return Column(
      children: [
        // graph,
        // currentTemps,
      ],
    );
  }
}
