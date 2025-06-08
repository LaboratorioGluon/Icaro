class GPS
{
  final double latitude;
  final double longitude;
  final double altitude;

  const GPS ({
          required this.latitude,
          required this.longitude,
          required this.altitude
        });

  factory GPS.fromJson(Map<String, dynamic> json) {
    return switch (json) {
      {
        'latitude': double latitude,
        'longitude': double longitude,
        'altitude': double altitude,
      } => GPS(latitude: latitude, 
               longitude: longitude, 
               altitude: altitude),
      _ => throw const FormatException('Failed to load GPS.'),
    };
  }

  Map<String, dynamic> toMap() {
    return {
      "latitude": latitude,
      "longitude": longitude,
      "altitude": altitude,
    };
  }
}
