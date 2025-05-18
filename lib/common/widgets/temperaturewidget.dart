import 'package:flutter/material.dart';
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

  Widget _generateGraphWidget ()
  {
    var tempColors = <Color>[];
    var tempSplines = <SplineSeries<TemperaturePoint, num>>[];
    
    // Generate data for each temperature given
    for(var temp in widget.temperatures)
    {
      // Generate color
      tempColors.add(temp.color);
    
      // Generate spline serie
      tempSplines.add(
          SplineSeries<TemperaturePoint, num>(
            name: temp.label,
            dataSource: [...history[temp.label]!],
            xValueMapper: (TemperaturePoint data, int index) => data.x,
            yValueMapper: (TemperaturePoint data, int index) => data.y,
            animationDuration: 0,
            color: temp.color,
          )
        );

    }

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

    return graphWidget;
  }


  Widget _generatePointerWidget(LinearGaugeOrientation orientation) {

    var tempColors = <Color>[];
    var tempPointers = <LinearShapePointer>[];
    
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
    }

    LinearGradient gradient;
    if (orientation == LinearGaugeOrientation.horizontal)
    {
      gradient = LinearGradient(
          colors: [Colors.blue, Colors.red],
          begin: Alignment.centerLeft, 
          end: Alignment.centerRight
        );
    } else {
      gradient = LinearGradient(
          colors: [Colors.blue, Colors.red],
          begin: Alignment.bottomCenter, 
          end: Alignment.topCenter
        );
    }

    var pointerWidget = SfLinearGauge(
        minimum: -100.0,
        maximum: 100.0,
        orientation: orientation,
        axisTrackStyle: LinearAxisTrackStyle(
          gradient: gradient
        ),
        isAxisInversed: false,
        ranges: [],
        markerPointers: tempPointers,
      );

    return pointerWidget;
  }

  Widget _buildTwoColumnLayout() {

    var pointerWidget = _generatePointerWidget(LinearGaugeOrientation.vertical);
    var graphWidget = _generateGraphWidget();

    return Row(
      children: [
        Expanded(child: graphWidget,),
        pointerWidget,
      ],
    );
  }

  Widget _buildSingleColumnLayout() {

    var pointerWidget = _generatePointerWidget(LinearGaugeOrientation.horizontal);
    var graphWidget = _generateGraphWidget();

    return Column(
      children: [
        graphWidget,
        pointerWidget,
      ],
    );
  }
}
