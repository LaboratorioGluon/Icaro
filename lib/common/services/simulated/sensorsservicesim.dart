import 'dart:async';
// import 'dart:convert';

import 'package:icaro_app/common/data/cameraboard.dart';
import 'package:icaro_app/common/data/gps.dart';
import 'package:icaro_app/common/data/icarosensors.dart';
import 'package:icaro_app/common/data/sensorboard.dart';
import 'package:icaro_app/common/data/supplyboard.dart';
import 'package:icaro_app/common/data/threeaxis.dart';

class SensorsServiceSim {
  static final SensorsServiceSim _instance = SensorsServiceSim._internal();
  factory SensorsServiceSim() => _instance;

  // Static modifiable status
  static var internalTemp = 0.0;
  static var externalTemp = 0.0;
  static var boardTemp = 0.0;
  static var boardHum = 0.0;

  static var accgyr = 0.0;
  static var acceletometer = ThreeAxis(x: accgyr, y: accgyr, z: accgyr);
  static var gyroscope = ThreeAxis(x: accgyr, y: accgyr, z: accgyr);

  static var latitude = 40.0;
  static var longitude = -4.0;
  static var altitude = 0.0;

  static var s3V = false;
  static var s5V = false;
  static var batteryLevel = 0.0;

  SensorsServiceSim._internal() {
    _startSensorsUpdates();
  }

  // Stream Serial JSON data
  final StreamController<IcaroSensors> _sensorsController = StreamController.broadcast();
  Stream<IcaroSensors> get sensorsStream => _sensorsController.stream;

  Future<IcaroSensors> _fetchSensorsSim() async {
    // TODO: Receive data from real device 

    accgyr = (accgyr >= 1.0) ? -1.0 : accgyr + 0.01;
    
    acceletometer = ThreeAxis(
      x: accgyr, 
      y: accgyr, 
      z: accgyr
    );

    gyroscope = ThreeAxis(
      x: accgyr, 
      y: accgyr, 
      z: accgyr
    );

    latitude = latitude;
    longitude = longitude + 0.1;
    if (longitude > 180.0) {
      longitude = -180.0 + (longitude - 180.0);
    }
    altitude = 0;
    
    GPS gps = GPS(
      latitude: latitude,
      longitude: longitude,
      altitude: altitude,
    );

    internalTemp = (internalTemp >= 100.0) ? -100.0 : internalTemp + 2.0;
    externalTemp = (externalTemp <= -100.0) ? 100.0 : externalTemp - 2.0;
    boardTemp = (boardTemp >= 30.0) ? -30.0 : boardTemp + 0.5;
    boardHum = (boardHum >= 100.0) ? 0.0 : boardHum + 1.0;

    SensorBoard sensorBoard = SensorBoard(
      internalTemp: internalTemp,
      externalTemp: externalTemp,
      boardTemp: boardTemp,
      boardHum: boardHum,
      acceletometer: acceletometer,
      gyroscope: gyroscope,
      gps: gps,
    );

    batteryLevel = (batteryLevel >= 100.0) ? 0.0 : batteryLevel + 1.0;
    s3V = (batteryLevel >= 30);
    s5V = (batteryLevel >= 50);

    SupplyBoard supplyBoard = SupplyBoard(
      s3V: s3V,
      s5V: s5V,
      batteryLevel: batteryLevel,
    );

    CameraBoard cameraBoard = CameraBoard();

    IcaroSensors icaroSensors = IcaroSensors(
      sensorBoard: sensorBoard,
      supplyBoard: supplyBoard,
      cameraBoard: cameraBoard,
    );

    // var json = jsonEncode(icaroSensors.toMap());
    // return IcaroSensors.fromJson(jsonDecode(json) as Map<String, dynamic>);
    return IcaroSensors.fromJson(icaroSensors.toMap());
  }

  void _startSensorsUpdates() {
    Stream.periodic(const Duration(milliseconds: 50))
     .asyncMap((_) => _fetchSensorsSim())
    .listen((icaroSensors) {
      // TODO: Publish data independently?
      _sensorsController.add(icaroSensors);
    });
  }
}
