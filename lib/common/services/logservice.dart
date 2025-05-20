import 'package:intl/intl.dart';
import 'package:logger/logger.dart';
import 'package:logger_screen/logger_screen.dart';

class LogService {
  // Instancia privada del singleton
  static final LogService _instance = LogService._internal();

  // Instancia pública del logger
  late final Logger _logger;

  // Printer para mostrar los logs en pantalla
  late final LoggerScreenPrinter printer;

  // Constructor privado
  LogService._internal() {
    printer = LoggerScreenPrinter(
      fileName: "logs",
      encryptionKey: "this_is_not_a_secret_key",
      printEmojis: true,
    );
    _logger = Logger(printer: printer);
  }

  // Fábrica para retornar la instancia
  factory LogService() {
    return _instance;
  }

  // Método para acceder al logger
  static Logger get instance => _instance._logger;

  // Método para acceder al printer
  static LoggerScreenPrinter get screenPrinter => _instance.printer;

  void d(String message) => _logger.d(message, time: DateTime.now());
  void i(String message) => _logger.i(message, time: DateTime.now());
  void w(String message) => _logger.w(message, time: DateTime.now());
  void e(String message, [dynamic error, StackTrace? stackTrace]) =>
      _logger.e(message, time: DateTime.now(), error: error, stackTrace: stackTrace);
}