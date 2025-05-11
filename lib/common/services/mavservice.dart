import 'dart:async';
import 'dart:io';
import 'dart:math';
import 'package:dart_mavlink/dialects/common.dart';
import 'package:dart_mavlink/mavlink.dart';
import 'package:dart_mavlink/types.dart';

class MAVCameraStatus
{
    int imageCaptureCount        = 0;
    int storedImagesCount        = 0;
    int imageCapturedErrors      = 0;
    int lastImageCapturedIndex   = 0;
    double latitute              = 0.0;
    double longitude             = 0.0;
    double altitude              = 0.0;
    List<char> lastImageCapturedName = [];
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
      });
  }

  // MAV camera status
  final Map<int, MAVCameraStatus> cameras = {};
  final StreamController<Map<int, MAVCameraStatus>> _cameraStatusController = StreamController.broadcast();
  Stream<Map<int, MAVCameraStatus>> get cameraStatusStream => _cameraStatusController.stream;

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

  // MAV server 
  late MavlinkDialectCommon _dialect;
  late MavlinkParser _parser;

  void _startMAVServer() async {
    _dialect = MavlinkDialectCommon();
    _parser = MavlinkParser(_dialect);

    _parser.stream.listen((MavlinkFrame frm) {
      if (frm.message is Heartbeat) {
        var hb = frm.message as Heartbeat;
        _processMAVHearbeat(hb);
      }
      else if  (frm.message is CameraImageCaptured)
      {
        var cic = frm.message as CameraImageCaptured;
        _processCameraImageCaptured(cic);
      }
    });

    // Start socket listening server
    final server = await ServerSocket.bind(InternetAddress.loopbackIPv4, 48484);
    print('Servidor socket escuchando en ${server.address.address}:${server.port}');
  
    server.listen((Socket cliente) {
      print('Cliente MAV conectado: ${cliente.remoteAddress.address}:${cliente.remotePort}');

      cliente.listen(
        (data) {
          _parser.parse(data);
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