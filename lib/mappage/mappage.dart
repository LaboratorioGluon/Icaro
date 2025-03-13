import 'dart:async';

import 'package:flutter/foundation.dart';
import 'package:flutter/material.dart';
import 'package:flutter_map/flutter_map.dart';
import 'package:flutter_map_animations/flutter_map_animations.dart';
import 'package:latlong2/latlong.dart';

import 'package:icaro_app/common/satellite.dart';
import 'package:icaro_app/common/services/issservice.dart';

class IcaroMapPage extends StatefulWidget {
  const IcaroMapPage({super.key});
  static const String pageTitle = "Icaro Map page";

  @override
  State<IcaroMapPage> createState() => _IcaroMapPageState();
}

class _IcaroMapPageState extends State<IcaroMapPage> with TickerProviderStateMixin{
  late StreamSubscription<LatLng> _issPositionSub;
  late StreamSubscription<Satellite> _issSatelliteSub;

  // Initial state
  @override
  void initState() {
    super.initState();

    // Initialize ISS data subscription for tracking
    _issSatelliteSub = ISSservice().satelliteStream.listen((newData) {
      _setIss(newData);
    });

    _issPositionSub = ISSservice().preciseLocationStream.listen((newPosition) {
      _setIssPosition(newPosition);
    });
  }

  @override
  void dispose() {
    _issPositionSub.cancel();
    _issSatelliteSub.cancel();
    super.dispose();
  }

  // Map controls
  late final _animatedMapController = AnimatedMapController(
                                        vsync: this, 
                                        duration: const Duration(milliseconds: 500),
                                        curve: Curves.easeInOut);

  static getDefaultLatLng() {
    return LatLng(40.44254064814816, -3.952498215412911);
  }
  static getDefaultZoom() {
    return 5;
  }
  static getDefaultRotation() {
    return 0.0;
  }

  void _moveMap(LatLng position) {
    _animatedMapController.animateTo(dest: position);
  }

  // ISS tracking
  late Satellite iss;
  bool issTrackEnable = true;
  LatLng issPosition = const LatLng(40.44254064814816, -3.952498215412911);
  late Marker issMarker;

  void _setIssTrackingEnable(bool value) {
    setState(() {
      issTrackEnable = value;
    });
  }
  
  void _setIss(Satellite newData) {
    setState(() {
      iss = newData;
    });
  }

  void _setIssPosition(LatLng newPosition) {
    setState(() {
      issPosition = newPosition;
      if(issTrackEnable)
      {
        _moveMap(issPosition);
      }
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

    issMarker = Marker(
                      point: issPosition, 
                      child: Image.asset("assets/icaro.png")
                    );

    // Widgets for detail sheet
    var header = ConstrainedBox(
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
                Text("Track", style: smallStyle), // Etiqueta del switch
                Switch(
                  value: issTrackEnable,
                  onChanged: (bool value) {_setIssTrackingEnable(value);},
                ),
              ],    
            ),
            Padding(padding: EdgeInsets.all(10))
          ],
        ),
    );
        
    var satTable = Center(child:                
        StreamBuilder(
          stream: ISSservice().satelliteStream,
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

    var details = DetailsBottomSheet(
      header: header,
      body: satTable);

    // Map widget for background
    var map = FlutterMap(
          options: MapOptions(
            initialCenter: getDefaultLatLng(),
            initialZoom: getDefaultZoom(),
            initialRotation: getDefaultRotation(),
          ),
          mapController: _animatedMapController.mapController,
          children: [
            TileLayer(
                urlTemplate: 'https://tile.openstreetmap.org/{z}/{x}/{y}.png',
                userAgentPackageName: 'com.icaro.app',
            ),
            MarkerLayer(
              markers: [
                issMarker,
              ],
            ),
          ],
        );

    // Compose final widget and return
    var pageWidget = Stack(
        children: [
            map,
            details
        ],
    );

    return pageWidget;
  }
}

// Detail section
class DetailsBottomSheet extends StatefulWidget {
  final Widget header;
  final Widget body;

  const DetailsBottomSheet ({ super.key, required this.header,required this.body });

  @override
  State<DetailsBottomSheet> createState() => _DetailsBottomSheetState();
}

class _DetailsBottomSheetState extends State<DetailsBottomSheet> {

  final DraggableScrollableController sheetController = DraggableScrollableController();

  double _sheetPosition = 0.15;
  final double _dragSensitivity = 600;

  bool get _isOnDesktopAndWeb =>
      kIsWeb ||
      switch (defaultTargetPlatform) {
        TargetPlatform.macOS ||
        TargetPlatform.linux ||
        TargetPlatform.windows => true,
        TargetPlatform.android ||
        TargetPlatform.iOS ||
        TargetPlatform.fuchsia => false,
      };

  @override
  Widget build(BuildContext context) {
    // final ColorScheme colorScheme = Theme.of(context).colorScheme;

    var sheet = DraggableScrollableSheet(
      minChildSize: 0.15,
      maxChildSize: 0.5,
      initialChildSize: _sheetPosition,
      builder: (BuildContext context, ScrollController scrollController) {
        var grabber = Grabber(
          onVerticalDragUpdate: (DragUpdateDetails details) {
            setState(() {
              _sheetPosition -= details.delta.dy / _dragSensitivity;
              if (_sheetPosition < 0.15) {
                _sheetPosition = 0.15;
              }
              if (_sheetPosition > 0.5) {
                _sheetPosition = 0.5;
              }
            });
          },
          isOnDesktopAndWeb: _isOnDesktopAndWeb,
        );

        var realHeader = widget.header;

        var realBody = SingleChildScrollView (
          scrollDirection: Axis.vertical,
          child: widget.body,
        );

        return Container(
            clipBehavior: Clip.hardEdge,
            decoration: BoxDecoration(
              color: Theme.of(context).canvasColor,
              borderRadius: const BorderRadius.only(
                topLeft: Radius.circular(25),
                topRight: Radius.circular(25),
              ),
            ),
            child: SingleChildScrollView(
                  controller: scrollController,
            child: Column(
              children: <Widget>[
                grabber,
                realHeader,
                realBody,
              ],
            ),
            ),
          );
      },
    );

    return Align (
        alignment: Alignment.bottomCenter,
        child: ConstrainedBox (
          constraints: BoxConstraints (
            maxWidth: 1000,
            maxHeight: 700,
          ),
          child: FractionallySizedBox (
            alignment: Alignment.center, 
            widthFactor: 0.5,
            child: sheet,
          ),
        ),
    );
  }
}

// Widget to display detail information
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

// This widget is needed for PC/Web
class Grabber extends StatelessWidget {
  const Grabber({
    super.key,
    required this.onVerticalDragUpdate,
    required this.isOnDesktopAndWeb,
  });

  final ValueChanged<DragUpdateDetails> onVerticalDragUpdate;
  final bool isOnDesktopAndWeb;

  @override
  Widget build(BuildContext context) {
    if (!isOnDesktopAndWeb) {
      return const SizedBox.shrink();
    }
    final ColorScheme colorScheme = Theme.of(context).colorScheme;

    return GestureDetector(
      onVerticalDragUpdate: onVerticalDragUpdate,
      child: Container(
        width: double.infinity,
        color: colorScheme.primary,
        child: Align(
          alignment: Alignment.topCenter,
          child: Container(
            margin: const EdgeInsets.symmetric(vertical: 8.0),
            width: 32.0,
            height: 4.0,
            decoration: BoxDecoration(
              color: colorScheme.surfaceContainerHighest,
              borderRadius: BorderRadius.circular(8.0),
            ),
          ),
        ),
      ),
    );
  }
}
