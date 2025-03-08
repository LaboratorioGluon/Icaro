import 'package:flutter/material.dart';
import 'package:flutter_map/flutter_map.dart';
import 'package:latlong2/latlong.dart';

class IcaroMapPage extends StatefulWidget {
  const IcaroMapPage({super.key});
  static const String pageTitle = "Icaro Map page";

  @override
  State<IcaroMapPage> createState() => _IcaroMapPageState();
}

class _IcaroMapPageState extends State<IcaroMapPage> {

  // Sheet controls
  bool expanded = false;
  void _toggleHeight() {
    setState(() {
      expanded = !expanded;
    });
  }

  // Map controls
  final mapController = MapController();
  static getDefaultLatLng() {
    return LatLng(40.44254064814816, -3.952498215412911);
  }
  static getDefaultZoom() {
    return 17.5;
  }
  static getDefaultRotation() {
    return 0.0;
  }

  void _resetLocation() {
    setState(() {
      mapController.move(getDefaultLatLng(), getDefaultZoom());
      mapController.rotate(getDefaultRotation());
    });
  }

  @override
  Widget build(BuildContext context) {
    final theme = Theme.of(context);
    final style = theme.textTheme.displayMedium!.copyWith(
            color: theme.colorScheme.primary,
            );

    // Collapsed widget for sheet
    const double collapsedHeight = 100;
    var collapsedWidget = Text("Show Icaro details", style: style,);
    
    // Expanded widget for sheet
    const double expandedHeight  = 250;
    var expandedWidget = Column(
              mainAxisAlignment: MainAxisAlignment.center,
              mainAxisSize: MainAxisSize.min,
              children: <Widget>[
                const Text('TODO: Add info...'),
                ElevatedButton(
                  child: const Text('Reset position'),
                  // onPressed: () => Navigator.pop(context),
                  onPressed: () => {_resetLocation()},
                ),
              ],
            );
    
    // Widget for sheet
    double sheetHeight = (expanded) ? expandedHeight : collapsedHeight;
    Widget sheetWidget = (expanded) ? expandedWidget : collapsedWidget;
    var bottomSheet = SizedBox(
          height: sheetHeight,
          child: Center(
            child: sheetWidget,
          ),
        );

    // Map backgrounds
    var map = FlutterMap(
          options: MapOptions(
            initialCenter: getDefaultLatLng(),
            initialZoom: getDefaultZoom(),
            initialRotation: getDefaultRotation(),
          ),
          mapController: mapController,
          children: [
            TileLayer(
                urlTemplate: 'https://tile.openstreetmap.org/{z}/{x}/{y}.png',
                userAgentPackageName: 'com.icaro.app',
            ),
          ],
        );


    var pageWidget = Scaffold (
          body: map,
          bottomSheet: bottomSheet,
          floatingActionButton: FloatingActionButton(
            onPressed: () {
              _toggleHeight();
            },
            child: Image.asset(
              "assets/IcaroPatch.png",
              width: 48,
            ),
          ),
          floatingActionButtonLocation: FloatingActionButtonLocation.centerDocked,
        );

    return pageWidget;
  }
}