import 'dart:async';
import 'dart:io';
import 'dart:math';
import 'package:dart_mavlink/dialects/common.dart';
import 'package:dart_mavlink/mavlink.dart';
import 'package:dart_mavlink/types.dart';
import 'package:icaro_app/common/data/threeaxis.dart';

class MAVCameraStatus
{
    int imageCaptureCount            = 0;
    int storedImagesCount            = 0;
    int imageCapturedErrors          = 0;
    int lastImageCapturedIndex       = 0;
    double latitute                  = 0.0;
    double longitude                 = 0.0;
    double altitude                  = 0.0;
    List<char> lastImageCapturedName = [];
}

class MAVGPSStatus
{
  double latitute  = 0.0;
  double longitude = 0.0;
  double altitude  = 0.0;
}

class MAVIMUStatus
{
  ThreeAxis accel = ThreeAxis(x:0.0, y:0.0, z:0.0);
  ThreeAxis gyro  = ThreeAxis(x:0.0, y:0.0, z:0.0);
}

class MAVBatteryStatus
{
  bool status3v3        = false;
  bool status5v0        = false;
  int batteryPercentage = 0;
}

class MAVSensorsStatus
{
  double internalTemp = 0.0;
  double externalTemp = 0.0;
  double onboardTemp  = 0.0;
  double humidity     = 0.0;
}

class MAVService {
  static final MAVService _instance = MAVService._internal();
  factory MAVService() => _instance;

  MAVService._internal() {
    _startMAVServer();
    _startCoverageChecker();
  }

  void dispose() {
    _linkStatusController.close();
  }

  // MAV link status
  final StreamController<int> _linkStatusController = StreamController.broadcast();
  Stream<int> get linkStatusStream => _linkStatusController.stream;

  final _heartbeats = <DateTime>[];

  // Periodic update of MAV messages updates
  void _startCoverageChecker() {
    final hbPeriod = 0.5;
    final hbExpiration = 5;
    final Duration hbWindow = Duration(seconds: hbExpiration);
    Stream.periodic(const Duration(milliseconds: 500))
      .listen((_) {
        // Update heartbeats/coverage
        _heartbeats.removeWhere((time) => DateTime.now().difference(time) > hbWindow);
        final int maxHeartbeats = (hbExpiration / hbPeriod).toInt();
        final int coverage = min(((_heartbeats.length / maxHeartbeats) * 100).toInt(), 100);
        _linkStatusController.add(coverage);

        // Update camera info
        _cameraStatusController.add(cameras);

        // Update gps info
        _gpsStatusController.add(gpsStatus);
        
        // Update imu info
        _imuStatusController.add(imuStatus);

        // Update battery info
        _batteryStatusController.add(batteryStatus);

        // Update sensors info
        _sensorsStatusController.add(sensorsStatus);
      });
  }

  // MAV camera status
  final Map<int, MAVCameraStatus> cameras = {};
  final StreamController<Map<int, MAVCameraStatus>> _cameraStatusController = StreamController.broadcast();
  Stream<Map<int, MAVCameraStatus>> get cameraStatusStream => _cameraStatusController.stream;

  // MAV GPS status
  final gpsStatus = MAVGPSStatus();
  final StreamController<MAVGPSStatus> _gpsStatusController = StreamController.broadcast();
  Stream<MAVGPSStatus> get gpsStatusStream => _gpsStatusController.stream;

  // MAV IMU status
  final imuStatus = MAVIMUStatus();
  final StreamController<MAVIMUStatus> _imuStatusController = StreamController.broadcast();
  Stream<MAVIMUStatus> get imuStatusStream => _imuStatusController.stream;

  // MAV Battery status
  final batteryStatus = MAVBatteryStatus();
  final StreamController<MAVBatteryStatus> _batteryStatusController = StreamController.broadcast();
  Stream<MAVBatteryStatus> get batteryStatusStream => _batteryStatusController.stream;

  // MAV Sensors status
  final sensorsStatus = MAVSensorsStatus();
  final StreamController<MAVSensorsStatus> _sensorsStatusController = StreamController.broadcast();
  Stream<MAVSensorsStatus> get sensorsStatusStream => _sensorsStatusController.stream;


  // MAV message processors
  void _processMAVHearbeat(Heartbeat hb)
  {
    _heartbeats.add(DateTime.now());
    print("Heartbeat received");
  }

  void _processCameraImageCaptured(CameraImageCaptured cic)
  {
    if (!cameras.containsKey(cic.cameraId))
    {
        cameras[cic.cameraId] = MAVCameraStatus();
    }

    var camera = cameras[cic.cameraId];
    if (camera != null)
    {
        camera.imageCaptureCount++;
        camera.lastImageCapturedIndex = cic.imageIndex;
        camera.lastImageCapturedName  = cic.fileUrl;
        if (cic.captureResult != 0)
        {
            camera.storedImagesCount++;
        }
        else
        {
            camera.imageCapturedErrors++;
        }

        camera.latitute  = cic.lat.toDouble() / 1E7;
        camera.longitude = cic.lon.toDouble() / 1E7;
        camera.altitude  = cic.alt.toDouble() / 1E3;
    }
    print("CameraImageCaptured received");
  }

  void _processGlobalPositionInt(GlobalPositionInt gpi)
  {
    final double e7 = 10000000.0;
    final double e3 = 1000.0;
    gpsStatus.latitute  = gpi.lat.toDouble() / e7;
    gpsStatus.longitude = gpi.lon.toDouble() / e7;
    gpsStatus.altitude  = gpi.alt.toDouble() / e3;
  }

  void _processScaledImu(ScaledImu imu)
  {
    imuStatus.accel = ThreeAxis(
      x: imu.xacc.toDouble() / 1000.0,
      y: imu.yacc.toDouble() / 1000.0,
      z: imu.zacc.toDouble() / 1000.0,
    );
    imuStatus.gyro = ThreeAxis(
      x: imu.xgyro.toDouble() / 1000.0,
      y: imu.ygyro.toDouble() / 1000.0,
      z: imu.zgyro.toDouble() / 1000.0,
    );
  }

  void _processRawImu(RawImu imu)
  {
    imuStatus.accel = ThreeAxis(
      x: imu.xacc.toDouble() / 1000.0,
      y: imu.yacc.toDouble() / 1000.0,
      z: imu.zacc.toDouble() / 1000.0,
    );
    imuStatus.gyro = ThreeAxis(
      x: imu.xgyro.toDouble() / 1000.0,
      y: imu.ygyro.toDouble() / 1000.0,
      z: imu.zgyro.toDouble() / 1000.0,
    );
  }

  void _processBatteryStatus(BatteryStatus bs)
  {
    batteryStatus.status3v3 = bs.voltagesExt[0] != 0.0; // Not official purpose
    batteryStatus.status5v0 = bs.voltagesExt[1] != 0.0; // Not official purpose
    batteryStatus.batteryPercentage = bs.batteryRemaining;
  }

  int _getNameLength(List<int> name)
  {
    int firstZeroIndex = name.indexWhere((element) => element == 0);
    if (firstZeroIndex == -1)
    {
      return name.length;
    }
    else
    {
      return firstZeroIndex;
    }
  }

  bool _areNamesEqual(List<int> nameA, List<int> nameB)
  {
    bool areEqual = true;
    int lenA = _getNameLength(nameA);
    int lenB = _getNameLength(nameB);

    if (lenA == lenB)
    {
      for (int i = 0; i < lenA; i++)
      {
        if (nameA[i] != nameB[i])
        {
          areEqual = false;
          break;
        }
      }
    }
    else
    {
      areEqual = false;
    }

    return areEqual; 
  }

  void _processNamedFloat(NamedValueFloat nvf)
  {
    final name = nvf.name.toList();
    print("NamedValueFloat: ${String.fromCharCodes(name)} = ${nvf.value}");
    if (_areNamesEqual(name, "InternalT".codeUnits.toList()))
    {
      sensorsStatus.internalTemp = nvf.value;
    }
    else if (_areNamesEqual(name, "ExternalT".codeUnits.toList()))
    {
      sensorsStatus.externalTemp = nvf.value;
    }
    else if (_areNamesEqual(name, "OnboardT".codeUnits.toList()))
    {
      sensorsStatus.onboardTemp = nvf.value;
    }
    else if (_areNamesEqual(name, "Humidity".codeUnits.toList()))
    {
      sensorsStatus.humidity = nvf.value;
    }
  }

  // MAV server 
  late MavlinkDialectCommon _dialect;

  void _startMAVServer() async {
    _dialect = MavlinkDialectCommon();
    // Start socket listening server
    final server = await ServerSocket.bind(InternetAddress.loopbackIPv4, 48484);
    print('Servidor socket escuchando en ${server.address.address}:${server.port}');
  
    server.listen((Socket cliente) {
      print('Cliente MAV conectado: ${cliente.remoteAddress.address}:${cliente.remotePort}');

      cliente.listen(
        (data) {
          MavlinkParser parser = MavlinkParser(_dialect);

          parser.stream.listen((MavlinkFrame frm) {
            if (frm.message is Heartbeat) {
              var hb = frm.message as Heartbeat;
              _processMAVHearbeat(hb);
            }
            else if  (frm.message is CameraImageCaptured)
            {
              var cic = frm.message as CameraImageCaptured;
              _processCameraImageCaptured(cic);
            }
            else if  (frm.message is GlobalPositionInt)
            {
              var gpi = frm.message as GlobalPositionInt;
              _processGlobalPositionInt(gpi);
            }
            else if  (frm.message is ScaledImu)
            {
              var imu = frm.message as ScaledImu;
              _processScaledImu(imu);
            }
            else if  (frm.message is RawImu)
            {
              // var imu = RawImu.parse(frm.message.serialize());
              final imu = frm.message as RawImu;
              // frm.message

              _processRawImu(imu);
            }
            else if  (frm.message is BatteryStatus)
            {
              var bs = BatteryStatus.parse(frm.message.serialize());
              _processBatteryStatus(bs);
            }
            else if  (frm.message is NamedValueFloat)
            {
              var nvf = NamedValueFloat.parse(frm.message.serialize());
              _processNamedFloat(nvf);
            }
          });
          parser.parse(data);
        },
        onDone: () {
          print('Cliente desconectado.');
        },
        onError: (error) {
          print('Error en cliente: $error');
        },
      );
    });

  }
}