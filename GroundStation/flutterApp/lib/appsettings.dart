import 'dart:convert';

import 'package:shared_preferences/shared_preferences.dart';

import 'common/settings/camerasettings.dart';

class IcaroSettings {
  static late final SharedPreferences _prefs;

  IcaroSettings._();

  static Future<void> init() async {
    _prefs = await SharedPreferences.getInstance();
  }

  // Getters and Setters

  // Camera settings
  static final _cameraKey     = "camera";

  static CameraSettings getCameraSettings()
  {
    String? cameraStr = _prefs.getString(_cameraKey);
    if (cameraStr != null)
    {
      final cameraJson = jsonDecode(cameraStr);
      return CameraSettings.fromJson(cameraJson);
    }

    final defaultHost   = "localhost";
    final defaultPort   = 48485;
    final defaultName   = "icaro_camera";
    return CameraSettings(host: defaultHost, port: defaultPort, name: defaultName);
  }

  static Future<void> setCameraSettings(CameraSettings settings) async
  {
    final cameraJson = settings.toMap();
    final cameraStr = jsonEncode(cameraJson);
    await _prefs.setString(_cameraKey, cameraStr);
  }

}
