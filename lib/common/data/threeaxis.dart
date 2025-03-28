class ThreeAxis
{
  final double x;
  final double y;
  final double z;

  const ThreeAxis ({
          required this.x,
          required this.y,
          required this.z
        });

  factory ThreeAxis.fromJson(Map<String, dynamic> json) {
    return switch (json) {
      {
        'x': double x,
        'y': double y,
        'z': double z,
      } => ThreeAxis(x: x, 
                     y: y, 
                     z: z),
      _ => throw const FormatException('Failed to load ThreeAxis.'),
    };
  }

  Map<String, dynamic> toMap() {
    return {
      "x": x,
      "y": y,
      "z": z,
    };
  }
}
