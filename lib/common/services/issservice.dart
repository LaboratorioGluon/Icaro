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
    void dispose() {
    _fetchTimer?.cancel();
    _satelliteController.close();
  }

  // Stream ISS JSON data
  final StreamController<Satellite> _satelliteController = StreamController.broadcast();
  Stream<Satellite> get satelliteStream => _satelliteController.stream;

  static var latitude = 40.0;
  static var longitude = -4.0;

  Timer? _fetchTimer;
  Duration _fetchInterval = const Duration(seconds: 1);
  int _receivedCount = 0;


  Future<Satellite> _fetchISS() async {
    var online = true;
    if (online)
    {
      final response = await http.get(
        Uri.parse('https://api.wheretheiss.at/v1/satellites/25544'),
      );

      if (response.statusCode == 200) {
        return Satellite.fromJson(jsonDecode(response.body) as Map<String, dynamic>);
      } else {
        throw Exception('Failed to load ISS');
      }
    } else {
      latitude = latitude;
      longitude = longitude + 0.1;
      if (longitude > 180.0) {
        longitude = -180.0 + (longitude - 180.0);
      }

      return Satellite(
        name: "simulated",
        id: 0,
        latitude: latitude,
        longitude: longitude,
        altitude: 0.0,
        velocity: 0.0,
        visibility: "",
        footprint: 0.0,
        timestamp: 0,
        daynum: 0.0,
        solarLat: 0.0,
        solarLon: 0.0,
        units: "",
      );
    }
  }

  void _startSatelliteUpdates() {
    _fetchTimer?.cancel(); // Detiene el timer si ya estaba corriendo
    _fetchTimer = Timer.periodic(_fetchInterval, (timer) async {
      Satellite newSatellite = await _fetchISS();
      _satelliteController.add(newSatellite);
      _receivedCount++;

      if (_receivedCount == 2) {
        _fetchInterval = const Duration(seconds: 10); // Cambia a 10 segundos
        _receivedCount = 0; // Reinicia el contador
        _startSatelliteUpdates(); // Reinicia el stream con el nuevo intervalo
      }
    });  }

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

    Stream.periodic(const Duration(milliseconds: 200))
     .asyncMap((_) => _calculatePrecisePosition())
    .listen((newPosition) {
      _precisePositionController.add(newPosition);
    });
  }

  Future<LatLng> _calculatePrecisePosition() async {
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
