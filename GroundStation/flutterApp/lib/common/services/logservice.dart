import 'dart:io';
import 'package:logger/logger.dart';
import 'package:path_provider/path_provider.dart';
import 'package:intl/intl.dart';
import 'package:logger_screen/logger_screen.dart';

class LogService {
  static final LogService _instance = LogService._internal();

  late final Logger _widgetLogger;
  late final Logger _fileLogger;
  late final LoggerScreenPrinter printer;
  
  LogService._internal() {
    printer = LoggerScreenPrinter(
      fileName: "logs",
      encryptionKey: "this_is_not_a_secret_key",
      printEmojis: true,
    );

    _widgetLogger = Logger(
      printer: printer, 
    );

    final fileOutput = RotatingFileLogOutput();
    _fileLogger = Logger(
      printer: SimplePrinter(printTime: true), 
      output: fileOutput
    );
  }


  factory LogService() {
    return _instance;
  }

  // static Logger get instance => _instance._logger;

  static LoggerScreenPrinter get screenPrinter => _instance.printer;

  void d(String message){_widgetLogger.d(message, time: DateTime.now());_fileLogger.d(message, time: DateTime.now()); }
  void i(String message){_widgetLogger.i(message, time: DateTime.now());_fileLogger.i(message, time: DateTime.now()); }
  void w(String message){_widgetLogger.w(message, time: DateTime.now());_fileLogger.w(message, time: DateTime.now()); }
  void e(String message, [dynamic error, StackTrace? stackTrace]){_widgetLogger.e(message, time: DateTime.now(), error: error, stackTrace: stackTrace);_fileLogger.e(message, time: DateTime.now(), error: error, stackTrace: stackTrace); }
}

class RotatingFileLogOutput extends LogOutput {
  final int maxFileSizeBytes = 25 * 1024 * 1024; // 25MB
  final int maxFilesPerDay = 100;

  late Directory logDirectory;
  late File currentLogFile;
  int currentIndex = 0;
  late String currentDate;

  bool _initialized = false;

  String _getDateString() {
    final now = DateTime.now();
    return DateFormat('yyyy-MM-dd').format(now);
  }

  String _getLogFileName(int index, String date) {
    return 'app_log_${date}_$index.txt';
  }

  Future<void> _init() async {
    final dir = await getApplicationDocumentsDirectory();
    logDirectory = Directory('${dir.path}/logs');
    if (!await logDirectory.exists()) {
      await logDirectory.create(recursive: true);
    }

    currentDate = _getDateString();

    for (int i = 0; i < maxFilesPerDay; i++) {
      final file = File('${logDirectory.path}/${_getLogFileName(i, currentDate)}');
      if (await file.exists()) {
        final size = await file.length();
        if (size < maxFileSizeBytes) {
          currentIndex = i;
          currentLogFile = file;
          _initialized = true;
          return;
        }
      }
    }

    // Create file if not existing
    currentIndex = 0;
    currentLogFile = File('${logDirectory.path}/${_getLogFileName(currentIndex, currentDate)}');
    await currentLogFile.writeAsString('');
    _initialized = true;
  }

  Future<void> _rotate() async {
    currentDate = _getDateString();
    currentIndex = (currentIndex + 1) % maxFilesPerDay;
    currentLogFile = File('${logDirectory.path}/${_getLogFileName(currentIndex, currentDate)}');
    await currentLogFile.writeAsString('');
  }

  @override
  void output(OutputEvent event) async {
    if (!_initialized) await _init();

    // Restart index if new day
    final today = _getDateString();
    if (today != currentDate) {
      currentDate = today;
      currentIndex = 0;
      currentLogFile = File('${logDirectory.path}/${_getLogFileName(currentIndex, currentDate)}');
      await currentLogFile.writeAsString('');
    }

    for (var line in event.lines) {
    if (await currentLogFile.length() >= maxFileSizeBytes) {
        await _rotate();
    }

    final time = DateFormat.Hms().format(DateTime.now()); // HH:mm:ss
    final formatted = '[$time] $line';

    await currentLogFile.writeAsString('$formatted\n',
        mode: FileMode.append, flush: true);
    }for (var line in event.lines) {
    if (await currentLogFile.length() >= maxFileSizeBytes) {
        await _rotate();
    }

    final time = DateFormat.Hms().format(DateTime.now()); // HH:mm:ss
    final formatted = '[$time] $line';

    await currentLogFile.writeAsString('$formatted\n',
        mode: FileMode.append, flush: true);
    }
  }
}
