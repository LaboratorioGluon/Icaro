import 'dart:convert';

import 'package:flutter/material.dart';
import 'package:flutter_map/flutter_map.dart';
import 'package:latlong2/latlong.dart';
import 'package:http/http.dart' as http;

import 'package:icaro_app/common/satellite.dart';

class IcaroMapPage extends StatefulWidget {
  const IcaroMapPage({super.key});
  static const String pageTitle = "Icaro Map page";

  @override
  State<IcaroMapPage> createState() => _IcaroMapPageState();
}

class _IcaroMapPageState extends State<IcaroMapPage> {

  // Initial state
  @override
  void initState() {
    super.initState();
    iss = fetchISS();
  }

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
    _setLocation(getDefaultLatLng(), getDefaultZoom(), getDefaultRotation());
  }

  void _setLocation(LatLng position, double zoom, double rotation) {
    setState(() {
      mapController.move(position, zoom);
      mapController.rotate(rotation);
    });
  }

  // ISS tracking
  late Future<Satellite> iss;

  // Widget creation
  @override
  Widget build(BuildContext context) {
    final theme = Theme.of(context);
    final style = theme.textTheme.displayMedium!.copyWith(
            color: theme.colorScheme.primary,
            );

    var issData = Center(child:                
                    FutureBuilder<Satellite>(
                      future: iss,
                      builder: (context, snapshot) {
                        if (snapshot.hasData) {
                          return SatelliteDataWidget(sat: snapshot.requireData);
                        } else if (snapshot.hasError) {
                          return Text('${snapshot.error}');
                        }

                        // By default, show a loading spinner.
                        return const CircularProgressIndicator();
                      },
                    ),
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
                issData
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

Future<Satellite> fetchISS() async {
  final response = await http.get(
    Uri.parse('https://api.wheretheiss.at/v1/satellites/25544'),
  );

  if (response.statusCode == 200) {
    return Satellite.fromJson(jsonDecode(response.body) as Map<String, dynamic>);
  } else {
    throw Exception('Failed to load ISS');
  }
}

class SatelliteDataWidget extends StatelessWidget{
  const SatelliteDataWidget({super.key, required this.sat});

  final Satellite sat;

  @override
  Widget build(BuildContext context) {
    return Text(sat.name.toUpperCase());
  }

}