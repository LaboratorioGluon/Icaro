import 'package:flutter/material.dart';
import 'package:icaro_app/common/data/threeaxis.dart';
import 'package:syncfusion_flutter_charts/charts.dart';

class _AxisData {
  final String label; // Para el eje X (etiqueta de la barra)
  final double value; // Para el eje Y (altura de la barra)

  _AxisData(this.label, this.value);
}

class ThreeAxisWidget extends StatelessWidget {

  static const Color xColor = Colors.red;
  static const Color yColor = Colors.green;
  static const Color zColor = Colors.blue;

  final String label;
  final ThreeAxis threeAxis;

  const ThreeAxisWidget({super.key, required this.label, required this.threeAxis});

  @override
  Widget build(BuildContext context) {
    return LayoutBuilder(
        builder: (BuildContext context, BoxConstraints constraints) {
          // Check the available width and height in constraints
          // double availableWidth = constraints.maxWidth;
          // double availableHeight = constraints.maxHeight;
          // double availableWidth = 0;
          // var orientation = (availableWidth >= 512) ? 
          //     LinearGaugeOrientation.vertical : LinearGaugeOrientation.horizontal;

          var axisSeries = ColumnSeries<_AxisData, String>(
            
            dataSource: [
              _AxisData('X', threeAxis.x),
              _AxisData('Y', threeAxis.y),
              _AxisData('Z', threeAxis.z),
            ],
            xValueMapper: (_AxisData data, _) => data.label,
            yValueMapper: (_AxisData data, _) => data.value,
            animationDuration: 0,
            width: 0.8,
            pointColorMapper: (_AxisData data, _) {
              if (data.label == 'X') return xColor;
              if (data.label == 'Y') return yColor;
              if (data.label == 'Z') return zColor;
              return Colors.grey;
            },
          );

          var graph = SfCartesianChart(
              enableAxisAnimation: false,
              primaryXAxis: const CategoryAxis(), // Eje X categórico para las etiquetas
              primaryYAxis: const NumericAxis(
                minimum: -1,
                maximum: 1,
                interval: 0.25, // Espaciado entre las líneas
              ),
              series: <CartesianSeries<_AxisData, String>>[
                axisSeries,
              ],
          );

          var box = ConstrainedBox(
            constraints: BoxConstraints(
              minWidth: 100,
              maxWidth: 150,
              minHeight: 100,
              maxHeight: 300,
            ), 
            child: graph,);

          // var title = Expanded(child: Text(label),);
          return Column(children: [Text(label), box],);

        },
      );
  }

}
