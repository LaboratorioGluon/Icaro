import 'gps.dart';
import 'threeaxis.dart';

class SensorBoard {
  final double internalTemp;
  final double externalTemp;
  final double boardTemp;
  final double boardHum;
  final ThreeAxis acceletometer;
  final ThreeAxis gyroscope; 
  final GPS gps;

  const SensorBoard ({
          required this.internalTemp,
          required this.externalTemp,
          required this.boardTemp,
          required this.boardHum,
          required this.acceletometer,
          required this.gyroscope,
          required this.gps,
        });

  factory SensorBoard.fromJson(Map<String, dynamic> json) {
    return switch (json) {
      {
        'internalTemp': double internalTemp,
        'externalTemp': double externalTemp,
        'boardTemp': double boardTemp,
        'boardHum': double boardHum,
        'acceletometer': Map<String, dynamic> acceletometer,
        'gyroscope': Map<String, dynamic> gyroscope,
        'gps': Map<String, dynamic> gps,
      } => SensorBoard(internalTemp: internalTemp,
                       externalTemp: externalTemp,
                       boardTemp: boardTemp,
                       boardHum: boardHum,
                       acceletometer:  ThreeAxis.fromJson(acceletometer),
                       gyroscope: ThreeAxis.fromJson(gyroscope),
                       gps: GPS.fromJson(gps)
                      ),
      _ => throw const FormatException('Failed to load SensorBoard.'),
    };
  }

  Map<String, dynamic> toMap() {
    return {
        'internalTemp': internalTemp,
        'externalTemp': externalTemp,
        'boardTemp': boardTemp,
        'boardHum': boardHum,
        'acceletometer': acceletometer.toMap(),
        'gyroscope': gyroscope.toMap(),
        'gps': gps.toMap(),
    };
  }
}
