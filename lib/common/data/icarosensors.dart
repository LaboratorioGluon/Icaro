import 'sensorboard.dart';
import 'supplyboard.dart';
import 'cameraboard.dart';

class IcaroSensors {
  final SensorBoard sensorBoard;
  final SupplyBoard supplyBoard;
  final CameraBoard cameraBoard;

  const IcaroSensors ({
          required this.sensorBoard,
          required this.supplyBoard,
          required this.cameraBoard
        });

  factory IcaroSensors.fromJson(Map<String, dynamic> json) {
    return switch (json) {
      {
        'sensorBoard': Map<String, dynamic> sensorBoard,
        'supplyBoard': Map<String, dynamic> supplyBoard,
        'cameraBoard': Map<String, dynamic> cameraBoard,
      } => IcaroSensors(
                       sensorBoard: SensorBoard.fromJson(sensorBoard),
                       supplyBoard: SupplyBoard.fromJson(supplyBoard),
                       cameraBoard: CameraBoard.fromJson(cameraBoard),
                       ),
      _ => throw const FormatException('Failed to load SensorBoard.'),
    };
  }

  Map<String, dynamic> toMap() {
    return {     
        'sensorBoard': sensorBoard.toMap(),
        'supplyBoard': supplyBoard.toMap(),
        'cameraBoard': cameraBoard.toMap(),
        };
  }
}
