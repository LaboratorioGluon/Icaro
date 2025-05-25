import 'package:logger/logger.dart';
import 'package:logger_screen/logger_screen.dart';

class LogService {
  static final LogService _instance = LogService._internal();

  late final Logger _logger;

  late final LoggerScreenPrinter printer;

  LogService._internal() {
    printer = LoggerScreenPrinter(
      fileName: "logs",
      encryptionKey: "this_is_not_a_secret_key",
      printEmojis: true,
    );
    _logger = Logger(printer: printer);
  }

  factory LogService() {
    return _instance;
  }

  static Logger get instance => _instance._logger;

  static LoggerScreenPrinter get screenPrinter => _instance.printer;

  void d(String message) => _logger.d(message, time: DateTime.now());
  void i(String message) => _logger.i(message, time: DateTime.now());
  void w(String message) => _logger.w(message, time: DateTime.now());
  void e(String message, [dynamic error, StackTrace? stackTrace]) =>
      _logger.e(message, time: DateTime.now(), error: error, stackTrace: stackTrace);
}