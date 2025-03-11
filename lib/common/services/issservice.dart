import 'dart:async';
import 'dart:convert';

import 'package:latlong2/latlong.dart';
import 'package:http/http.dart' as http;

import 'package:icaro_app/common/satellite.dart';

class ISSservice {
  static final ISSservice _instance = ISSservice._internal();
  factory ISSservice() => _instance;

  ISSservice._internal() {
    _startSatelliteUpdates();
    _startPositionUpdates();
    _startPrecisePositionUpdates();
    _startPrecisePositionUpdates2();
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
    Stream.periodic(const Duration(seconds: 1))
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

    satelliteStream.listen((newSat) {
      prevTime = lastTime;
      prevSat = lastSat;
      lastTime = DateTime.now();
      lastSat = newSat;
    });

    // Ejemplo de escuchar el stream:
    _precisePositionController.stream.listen((LatLng value) {
      // Manejar los valores emitidos
      print(value);
    });
  }

  void _startPrecisePositionUpdates2() { 
    // Definir la lógica del stream que se ejecuta cada 0.1 segundos
      // Stream.periodic(const Duration(milliseconds: 100), (_) {
      print("");
    //   while(true)
    //   {
    //     if (prevSat == null && lastSat == null) {
    //       return;
    //     } else if (prevSat == null && lastSat != null) {
    //       _precisePositionController.add(LatLng(lastSat!.latitude, lastSat!.longitude));
    //     } else {
    //       double vLat = (lastSat!.latitude - prevSat!.latitude) / (lastTime!.difference(prevTime!)).inMilliseconds;
    //       double vLon = (lastSat!.longitude - prevSat!.longitude) / (lastTime!.difference(prevTime!)).inMilliseconds;

    //       DateTime currentTime = DateTime.now();
    //       double nuevaLat = lastSat!.latitude + vLat * (currentTime.difference(lastTime!)).inMilliseconds;
    //       double nuevaLon = lastSat!.longitude + vLon * (currentTime.difference(lastTime!)).inMilliseconds;

    //       LatLng value = LatLng(nuevaLat, nuevaLon);
    //       _precisePositionController.add(value); // Enviar el valor calculado al controller
    //     }
    //   }
    // });

  }
}
