import 'dart:async';
import 'dart:convert';

import 'package:latlong2/latlong.dart';
import 'package:http/http.dart' as http;

import 'package:icaro_app/common/data/satellite.dart';

class ISSservice {
  static final ISSservice _instance = ISSservice._internal();
  factory ISSservice() => _instance;

  ISSservice._internal() {
    _startSatelliteUpdates();
    _startPositionUpdates();
    _startPrecisePositionUpdates();
  }

  // Stream ISS JSON data
  final StreamController<Satellite> _satelliteController = StreamController.broadcast();
  Stream<Satellite> get satelliteStream => _satelliteController.stream;

  Future<Satellite> _fetchISS() async {
    final response = await http.get(
      Uri.parse('https://api.wheretheiss.at/v1/satellites/25544'),
    );

    if (response.statusCode == 200) {
      return Satellite.fromJson(jsonDecode(response.body) as Map<String, dynamic>);
    } else {
      throw Exception('Failed to load ISS');
    }
  }

  void _startSatelliteUpdates() {
    Stream.periodic(const Duration(seconds: 5))
     .asyncMap((_) => _fetchISS())
    .listen((newSatellite) {
      _satelliteController.add(newSatellite);
    });
  }

  // Stream ISS Position data
  final StreamController<LatLng> _positionController = StreamController.broadcast();
  Stream<LatLng> get locationStream => _positionController.stream;

  void _startPositionUpdates() {
    satelliteStream.listen((newSatellite) {
      _positionController.add(LatLng(newSatellite.latitude, newSatellite.longitude));
    });
  }

  // Stream ISS Predicted position data
  final StreamController<LatLng> _precisePositionController = StreamController.broadcast();
  Stream<LatLng> get preciseLocationStream => _precisePositionController.stream;

  DateTime? prevTime;
  Satellite? prevSat;  
  DateTime? lastTime;
  Satellite? lastSat;  

  void _startPrecisePositionUpdates() {

    satelliteStream.listen((newSatellite) {
      prevTime = lastTime;
      prevSat = lastSat;
      lastTime = DateTime.now();
      lastSat = newSatellite;
    });

    Stream.periodic(const Duration(milliseconds: 150))
     .asyncMap((_) => _calculatePrecisePosition())
    .listen((newPosition) {
      _precisePositionController.add(newPosition);
    });

    // preciseLocationStream.listen((LatLng value) {
    //   // Manejar los valores emitidos
    //   print(value);
    // });
  }

  Future<LatLng> _calculatePrecisePosition() async {
  // void _calculatePrecisePosition() { 
    // Definir la lógica del stream que se ejecuta cada 0.1 segundos
    // Stream.periodic(const Duration(milliseconds: 100), (_) {
    if (prevSat == null && lastSat == null) {
      return LatLng(40.44254064814816, -3.952498215412911);
    } else if (prevSat == null && lastSat != null) {
      return LatLng(lastSat!.latitude, lastSat!.longitude);
    } else {
      double vLat = (lastSat!.latitude - prevSat!.latitude) / (lastTime!.difference(prevTime!)).inMilliseconds;
      double vLon = (lastSat!.longitude - prevSat!.longitude) / (lastTime!.difference(prevTime!)).inMilliseconds;

      DateTime currentTime = DateTime.now();
      double nuevaLat = lastSat!.latitude + vLat * (currentTime.difference(lastTime!)).inMilliseconds;
      double nuevaLon = lastSat!.longitude + vLon * (currentTime.difference(lastTime!)).inMilliseconds;

      LatLng value = LatLng(nuevaLat, nuevaLon);
      return (value); // Enviar el valor calculado al controller
    }
  }
}
