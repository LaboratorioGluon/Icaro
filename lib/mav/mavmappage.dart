import 'dart:async';

import 'package:flutter/material.dart';
import 'package:flutter_map/flutter_map.dart';
import 'package:flutter_map_animations/flutter_map_animations.dart';
import 'package:icaro_app/common/services/mavservice.dart';
import 'package:latlong2/latlong.dart';

class IcaroMAVMapPage extends StatefulWidget {
  const IcaroMAVMapPage({super.key});
  static const String pageTitle = "Icaro Map page";

  @override
  State<IcaroMAVMapPage> createState() => _IcaroMAVMapPageState();
}

class _IcaroMAVMapPageState extends State<IcaroMAVMapPage> with TickerProviderStateMixin{
  late StreamSubscription<MAVGPSStatus> _mavGPSStatusSub;

  // Initial state
  @override
  void initState() {
    super.initState();

    // MAV GPS status monitor
    _mavGPSStatusSub = MAVService().gpsStatusStream.listen((newStatus) {
      setState((){
        // gpsStatus = newStatus;
        _setPosition(LatLng(newStatus.latitute, newStatus.longitude));
      });
    });
  }

  @override
  void dispose() {
    _mavGPSStatusSub.cancel();
    _animatedMapController.dispose();
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
    return 5.0;
  }
  static getDefaultRotation() {
    return 0.0;
  }

  void _moveMap(LatLng position) {
    _animatedMapController.animateTo(dest: position);
  }

  // Icaro tracking
  bool trackingEnabled = false;
  LatLng icaroPosition = const LatLng(40.44254064814816, -3.952498215412911);
  late Marker icaroMarker;

  void _setTrackingEnable(bool value) {
    setState(() {
      trackingEnabled = value;
    });
  }
  
  void _setPosition(LatLng newPosition) {
    setState(() {
      icaroPosition = newPosition;
      if(trackingEnabled)
      {
        _moveMap(icaroPosition);
      }
    });
  }

  // Widget creation
  @override
  Widget build(BuildContext context) {
    final theme = Theme.of(context);
    final style = theme.textTheme.displayMedium!.copyWith(
       color: theme.colorScheme.onPrimary,
       );

    icaroMarker = Marker(
                      point: icaroPosition, 
                      child: Image.asset("assets/icaro.png")
                    );

    // Tracking switch
    var trackingSwitch = Switch(
      value: trackingEnabled,
      onChanged: (bool value) {
        _setTrackingEnable(value);
      },
      thumbColor: WidgetStateProperty.all(Colors.white),
      trackColor: WidgetStateProperty.all(Colors.white70),
    );

    var trackingCard = Card(
      color: theme.colorScheme.primary,
      elevation: 10,
      child: Padding(
        padding: const EdgeInsets.all(20),
        child: Row( 
          mainAxisSize: MainAxisSize.min,
          children: [
            Text("Enable tracking", style: style,),
            trackingSwitch
          ],
        ),
      )
    );

    // Map widget
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
                icaroMarker,
              ],
            ),
          ],
        );

    // Compose final widget and return
    var pageWidget = Stack(
        children: [
            map,
            Align(
              alignment: Alignment.bottomCenter,
              child: trackingCard
            )
        ],
    );

    return pageWidget;
  }
}
