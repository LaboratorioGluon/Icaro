
import 'package:flutter/material.dart';
import 'package:flutter/services.dart';

import 'configurationset.dart';
import '../common/settings/camerasettings.dart';

class CameraConfig extends StatefulWidget {
  final CameraSettings settings;
  final void Function(CameraSettings updatedSettings) onSave;

  const CameraConfig({
    super.key,
    required this.settings,
    required this.onSave,
  });

  @override
  State<CameraConfig> createState() => _CameraConfigState();
}

class _CameraConfigState extends State<CameraConfig> {
  late final TextEditingController _hostController;
  late final TextEditingController _portController;
  late final TextEditingController _nameController;

  @override
  void initState() {
    super.initState();
    _hostController = TextEditingController(text: widget.settings.host);
    _portController = TextEditingController(text: widget.settings.port.toString());
    _nameController = TextEditingController(text: widget.settings.name);
  }

  @override
  void dispose() {
    _hostController.dispose();
    _portController.dispose();
    _nameController.dispose();
    super.dispose();
  }

  void _save() {
    final updatedSettings = CameraSettings(
      host: _hostController.text,
      port: int.tryParse(_portController.text) ?? 0,
      name: _nameController.text,
    );
    widget.onSave(updatedSettings);
  }

  @override
  Widget build(BuildContext context) {
    return ConfiguracionSetWidget(
      title: "Camera settings",
      child: Column(
        children: [
          TextFormField(
            decoration: InputDecoration(labelText: 'Server address'),
            controller: _hostController,
          ),
          TextFormField(
            decoration: InputDecoration(labelText: 'Server port'),
            controller: _portController,
            keyboardType: TextInputType.number,
            inputFormatters: [FilteringTextInputFormatter.digitsOnly],
          ),
          TextFormField(
            decoration: InputDecoration(labelText: 'Camera name'),
            controller: _nameController,
          ),
          const SizedBox(height: 20),
          ElevatedButton(
            onPressed: _save,
            child: const Text("Save"),
          ),
        ],
      ),
    );
  }
}
