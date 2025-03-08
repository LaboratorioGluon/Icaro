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
  bool trackIss = false;

  void _setTracking(bool value) {
    setState(() {
      trackIss = value;
    });
  }

  // Widget creation
  @override
  Widget build(BuildContext context) {
    final theme = Theme.of(context);
    final style = theme.textTheme.displayMedium!.copyWith(
            color: theme.colorScheme.primary,
            );
    final smallStyle = theme.textTheme.displaySmall!.copyWith(
            color: theme.colorScheme.primary,
            fontSize: 16
            );

    var issStream = Stream.periodic(
                        Duration(seconds: 1)
                      ).asyncMap((i) => fetchISS());

    // Widget for sheet
    double sheetHeight = 100;
    Widget sheetWidget = ConstrainedBox(
        constraints: BoxConstraints(
          minWidth: 0, 
          minHeight: 100, 
        ),
        child: Row(
          mainAxisAlignment: MainAxisAlignment.center,
          children: [
            Expanded(       
              child: Text("Show details", 
                  textAlign: TextAlign.center, 
                  style: style,
                ),
            ),
            Divider(),
            Column(
              mainAxisAlignment: MainAxisAlignment.center, 
              crossAxisAlignment: CrossAxisAlignment.center, 
              children: [
                Text("Follow", style: smallStyle), // Etiqueta del switch
                Switch(
                  value: trackIss,
                  onChanged: (bool value) {_setTracking(value);},
                ),
              ],    
            ),
            Padding(padding: EdgeInsets.all(10))
          ],
        ),
    );
        
    if (expanded)
    {
      var issTable = Center(child:                
                      StreamBuilder(
                        stream: issStream,
                        builder: (context, snapshot) {
                          if (snapshot.hasData) {
                            return SatelliteDataTable(sat: snapshot.requireData);
                          } else if (snapshot.hasError) {
                            return Text('${snapshot.error}');
                          }
                          return const CircularProgressIndicator();
                        },
                      ),
                  );

      // Expand widget for sheet
      sheetHeight = 350;
      sheetWidget = Column(
                      mainAxisAlignment: MainAxisAlignment.center,
                      mainAxisSize: MainAxisSize.min,
                      children: <Widget>[
                        sheetWidget,
                        // ElevatedButton(
                        //   child: const Text('Reset position'),
                        //   // onPressed: () => Navigator.pop(context),
                        //   onPressed: () => {_resetLocation()},
                        // ),
                        Expanded (child: issTable),
                      ],
                    );
    }

    // Widget for sheet
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

class SatelliteDataTable extends StatelessWidget{
  const SatelliteDataTable({super.key, required this.sat});

  final Satellite sat;

    TableRow _buildRow(String label, String value) {
      return TableRow(
        children: [
          Padding(
            padding: const EdgeInsets.all(5.0),
            child: Text(
              label,
              style: TextStyle(fontWeight: FontWeight.bold, fontSize: 16),
            ),
          ),
          Padding(
            padding: const EdgeInsets.all(5.0),
            child: Text(
              value,
              style: TextStyle(fontSize: 16),
            ),
          ),
        ],
      );    }

  @override
  Widget build(BuildContext context) {

    String altUnit = (sat.units == "kilometers") ? " km" : "";
    String velUnit = (sat.units == "kilometers") ? " km/s" : "";

    return SingleChildScrollView(
      scrollDirection: Axis.vertical,
      child: Padding(
        padding: const EdgeInsets.all(30.0), // Agrega espacio alrededor de la tabla
        child: Table(
          columnWidths: const {
            0: FlexColumnWidth(1),
            1: FlexColumnWidth(3),
          },  
          children: [
            _buildRow("Name" , sat.name),
            _buildRow("Latitude" , sat.latitude.toString()),
            _buildRow("Longitude" , sat.longitude.toString()),
            _buildRow("Altitude" , sat.altitude.toStringAsFixed(4) + altUnit),
            _buildRow("Velocity" , sat.velocity.toStringAsFixed(4) + velUnit),
          ],
        ),
      ),

    );
  }
}