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
}
