class CameraSettings
{
  String host;
  int    port;
  String name;

  CameraSettings ({
          required this.host,
          required this.port,
          required this.name,
        });

  factory CameraSettings.fromJson(Map<String, dynamic> json) {
    return switch (json) {
      {
        'host': String host,
        'port': int    port,
        'name': String name,
      } => CameraSettings(host: host, 
                          port: port, 
                          name: name), 
      _ => throw const FormatException('Failed to load CameraSettings.'),
    };
  }

  Map<String, dynamic> toMap() {
    return {
        "host": host,
        "port": port,
        "name": name,
    };
  }


}
