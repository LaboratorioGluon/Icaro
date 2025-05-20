import 'package:flutter/material.dart';
import 'package:material_design_icons_flutter/material_design_icons_flutter.dart';
import 'package:syncfusion_flutter_gauges/gauges.dart';

class Powerwidget extends StatelessWidget {

  final String label;
  final double value;
  final bool status3v3;
  final bool status5v0;

  const Powerwidget({super.key, required this.label, required this.value, required this.status3v3, required this.status5v0});

  Widget _generatePointerWidget(LinearGaugeOrientation orientation) {

    const batteryColors = [Colors.red, Colors.yellow, Colors.green];
    var batteryPointer = LinearBarPointer(
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
          colors: batteryColors,
          begin: Alignment.centerLeft, 
          end: Alignment.centerRight
        );
    } else {
      gradient = const LinearGradient(
          colors: batteryColors,
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
            batteryPointer
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
          
          var wstatus3v3 = Row(
            children: [
              Text("Voltage 3.3V: "),
              Icon(status3v3 ? MdiIcons.check : MdiIcons.close, 
                color: status3v3 ? Colors.green : Colors.red,
              ),
            ],
          );
          var wstatus5v0 = Row(
            children: [
              Text("Voltage 5.0V: "),
              Icon(status5v0 ? MdiIcons.check : MdiIcons.close, 
                color: status5v0 ? Colors.green : Colors.red,
              ),
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

          var voltages = Column(
            mainAxisAlignment: MainAxisAlignment.start,
            crossAxisAlignment: CrossAxisAlignment.start,
            children: [
              wstatus3v3,
              wstatus5v0,
            ],
          );

          var grid = Row(
            mainAxisAlignment: MainAxisAlignment.start,
            crossAxisAlignment: CrossAxisAlignment.start,
            children: [
              box,
              voltages
            ],
          );

          // var title = Expanded(child: Text(label),);
          return Column(children: [Text(label), grid,],);

        },
      );
  }

}
