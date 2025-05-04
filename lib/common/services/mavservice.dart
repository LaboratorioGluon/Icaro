import 'dart:async';
import 'dart:io';
import 'dart:typed_data';
import 'package:dart_mavlink/dialects/common.dart';
import 'package:dart_mavlink/mavlink.dart';

class MAVService {
  static final MAVService _instance = MAVService._internal();
  factory MAVService() => _instance;

  MAVService._internal() {
    _startMAVServer();
    _startHBWatchdog();
    _lastHB = DateTime.now();
  }

  void dispose() {
  }

  // MAV link status
  final StreamController<bool> _linkStatusController = StreamController.broadcast();
  Stream<bool> get linkStatusStream => _linkStatusController.stream;

  late DateTime _lastHB;

  void _startHBWatchdog() {
    Stream.periodic(const Duration(seconds: 1))
      .listen((_) {
        if (DateTime.now().difference(_lastHB).inSeconds >= 5)
        {
          _linkStatusController.add(false);
        }
      });
  }

  // MAV message processors
  void _processMAVHearbeat(Heartbeat hb)
  {
    _lastHB = DateTime.now();
    print("Heartbeat received");
    _linkStatusController.add(true);
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
    });

    // Start socket listening server
    final server = await ServerSocket.bind(InternetAddress.loopbackIPv4, 48484);
    print('Servidor socket escuchando en ${server.address.address}:${server.port}');
  
    server.listen((Socket cliente) {
      print('Cliente conectado: ${cliente.remoteAddress.address}:${cliente.remotePort}');

      cliente.listen(
        (data) {
          _parser.parse(data);

          // Enviar respuesta
          final mensaje = data;
          cliente.write('Echo: $mensaje\n');
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