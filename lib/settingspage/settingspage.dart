import 'package:flutter/material.dart';
import 'package:icaro_app/appsettings.dart';
import 'package:icaro_app/logo.dart';
import 'package:icaro_app/settingspage/cameraconfig.dart';

class IcaroSettingsPage extends StatelessWidget {

  IcaroSettingsPage({super.key});
  static const String pageTitle = "Icaro Settings page";

  late CameraConfig cameraConfig;

  void saveSettings()
  {
    // Store camera settings
    IcaroSettings.setCameraSettings(cameraConfig.getSettings());
  }

  @override
  Widget build(BuildContext context) {
    final theme = Theme.of(context);
    final style = theme.textTheme.displayMedium!.copyWith(
       color: theme.colorScheme.onPrimary,
       );

    var  saveButton = ElevatedButton(
      onPressed: saveSettings, 
      child: Text ("Save")
    );

    cameraConfig = CameraConfig(settings: IcaroSettings.getCameraSettings());

    // Title 
    var title = Card(
      color: theme.colorScheme.primary,
      elevation: 10,
      child: Padding(
        padding: const EdgeInsets.all(20),
        child: Text(IcaroSettingsPage.pageTitle, style: style,),
      ),
    );

    // Content
    var content = Column(
      mainAxisAlignment: MainAxisAlignment.center,
      children: [
        cameraConfig,
        saveButton
      ]   
    );

    return Column(
      mainAxisAlignment: MainAxisAlignment.start,
      children: [
        title,
        Expanded(child: SingleChildScrollView(child: content)),
      ]
    );
  }
}