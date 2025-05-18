import 'package:flutter/material.dart';
import 'package:icaro_app/appsettings.dart';
import 'package:icaro_app/common/settings/camerasettings.dart';
import 'package:icaro_app/settingspage/cameraconfig.dart';

class IcaroSettingsPage extends StatelessWidget {
  IcaroSettingsPage({super.key});

  static const String pageTitle = "Icaro Settings page";

  void _handleCameraSave(CameraSettings updatedSettings) {
    IcaroSettings.setCameraSettings(updatedSettings);
    // Mostrar feedback si quieres
  }

  @override
  Widget build(BuildContext context) {
    final theme = Theme.of(context);
    final style = theme.textTheme.displayMedium!.copyWith(
      color: theme.colorScheme.onPrimary,
    );

    final title = Card(
      color: theme.colorScheme.primary,
      elevation: 10,
      child: Padding(
        padding: const EdgeInsets.all(20),
        child: Text(pageTitle, style: style),
      ),
    );

    final cameraConfig = CameraConfig(
      settings: IcaroSettings.getCameraSettings(),
      onSave: _handleCameraSave,
    );

    return Column(
      mainAxisAlignment: MainAxisAlignment.start,
      children: [
        title,
        Expanded(child: SingleChildScrollView(child: cameraConfig)),
      ],
    );
  }
}
