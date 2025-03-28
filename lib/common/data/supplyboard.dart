class SupplyBoard {
  final bool s3V;
  final bool s5V;
  final double batteryLevel;
  
  const SupplyBoard ({
          required this.s3V,
          required this.s5V,
          required this.batteryLevel
        });

  factory SupplyBoard.fromJson(Map<String, dynamic> json) {
    return switch (json) {
      {
        's3V': bool s3V,
        's5V': bool s5V,
        'batteryLevel': double batteryLevel,
      } => SupplyBoard(s3V: s3V, 
                       s5V: s5V, 
                       batteryLevel: batteryLevel),
      _ => throw const FormatException('Failed to load SupplyBoard.'),
    };
  }

  Map<String, dynamic> toMap() {
    return {
      "s3V": s3V,
      "s5V": s5V,
      "batteryLevel": batteryLevel,
    };
  }
}

