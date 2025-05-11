
import 'package:flutter/material.dart';
import 'package:flutter/services.dart';

import 'configurationset.dart';
import '../common/settings/camerasettings.dart';

class CameraConfig extends StatelessWidget {

  late final CameraSettings _settings;
  CameraConfig({super.key, required CameraSettings settings})
  {
    _settings = settings;
  }

  final _hostController = TextEditingController();
  final _portController = TextEditingController();
  final _nameController = TextEditingController();

  CameraSettings getSettings()
  {
    return CameraSettings(
      host: _hostController.text, 
      port: int.parse(_portController.text),
      name: _nameController.text,
    );
  }

  @override
  Widget build(BuildContext context) {
    final theme = Theme.of(context);
    final style = theme.textTheme.displayMedium!.copyWith(
       color: theme.colorScheme.onPrimary,
       );

    // Camera configuration
    _hostController.text = _settings.host;
    _portController.text = _settings.port.toString();
    _nameController.text = _settings.name;

    // URL
    var url = TextFormField(
      decoration: InputDecoration(labelText: 'Server address'),
      controller: _hostController,
    );

    // Port
    var port = TextFormField(
      decoration: InputDecoration(labelText: 'Server port'),
      controller: _portController,
      keyboardType: TextInputType.number,
      inputFormatters: [FilteringTextInputFormatter.digitsOnly],
      validator: (value) {
        if (value == null || value.isEmpty) {
          return 'Please enter a valid port (Default: 48485)';
        }
        else 
        {
          int? portNum = int.tryParse(value);
          if (portNum == null)
          {
            return 'Invalid port number';
          } 
          else if (portNum <= 0 || portNum >= 65535)
          {
            return 'Port out of range (0 <= port <= 65535)';
          }
        }
        return null;
      },
    );

    // URL
    var name = TextFormField(
      decoration: InputDecoration(labelText: 'Camera name'),
      controller: _nameController,
    );

    // Configuration set
    var configs = Form(
      child: Column(
        children: [
          url,
          port,
          name,
        ],
      )
    );

    return ConfiguracionSetWidget(
      title: "Camera settings", 
      child: configs
    );
  }
}