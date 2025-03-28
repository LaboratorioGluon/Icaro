import 'package:flutter/material.dart';
import 'package:syncfusion_flutter_gauges/gauges.dart';

class HumidityWidget extends StatelessWidget {

  final String label;
  final double value;

  const HumidityWidget({super.key, required this.label, required this.value});

  Widget _generatePointerWidget(LinearGaugeOrientation orientation) {

    const humColors = [Colors.grey, Colors.blue, Color(0xFF0D47A1)];
    var humPointer = LinearBarPointer(
      value: value,
      color: Colors.white, // El color base no importa, ya que será cubierto por el shader
      position: LinearElementPosition.outside,
      thickness: 30,
      offset: 2,
      animationDuration: 0,
    );
    
    LinearGradient gradient;
    if (orientation == LinearGaugeOrientation.horizontal)
    {
      gradient = const LinearGradient(
          colors: humColors,
          begin: Alignment.centerLeft, 
          end: Alignment.centerRight
        );
    } else {
      gradient = const LinearGradient(
          colors: humColors,
          begin: Alignment.bottomCenter, 
          end: Alignment.topCenter
        );
    }

    var pointerWidget = ShaderMask(
      shaderCallback: (bounds) {
          return gradient.createShader(bounds);
        },
        child: SfLinearGauge(
          orientation: orientation,
          isMirrored: true,
          minimum: 0,
          maximum: 100,
          barPointers: [
            humPointer
          ],
        ),
      );

    return pointerWidget;
  }

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

          var graph = _generatePointerWidget(LinearGaugeOrientation.vertical);

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
