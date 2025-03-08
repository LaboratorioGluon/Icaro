class Satellite {
  final String name;                //	"iss"
  final int id;                     //	25544
  final double latitude;            //	-27.720568104868
  final double longitude;           //	-13.460969859136
  final double altitude;            //	428.66758810474
  final double velocity;            //	27548.842184164
  final String visibility;          //	"daylight"
  final double footprint;           //	4551.2999767041
  final int timestamp;              //	1741425089
  final double daynum;              //	2460742.8829745
  final double solarLat;            //	-4.7240914065878
  final double solarLon;            //	44.81079404436
  final String units;               //	"kilometers"

  const Satellite ({
          required this.name,
          required this.id,
          required this.latitude,
          required this.longitude,
          required this.altitude,
          required this.velocity,
          required this.visibility,
          required this.footprint,
          required this.timestamp,
          required this.daynum,
          required this.solarLat ,
          required this.solarLon,
          required this.units,
        });

  factory Satellite.fromJson(Map<String, dynamic> json) {
    return switch (json) {
      {
        'name': String name,
        'id': int id,
        'latitude': double latitude,
        'longitude': double longitude,
        'altitude': double altitude,
        'velocity': double velocity,
        'visibility': String visibility,
        'footprint': double footprint,
        'timestamp': int timestamp,
        'daynum': double daynum,
        'solar_lat': double solarLat,
        'solar_lon': double solarLon,
        'units': String units,
      } => Satellite(name: name, 
                     id: id, 
                     latitude: latitude, 
                     longitude: longitude,
                     altitude: altitude,
                     velocity: velocity,
                     visibility: visibility, 
                     footprint: footprint, 
                     timestamp: timestamp, 
                     daynum: daynum, 
                     solarLat: solarLat, 
                     solarLon: solarLon, 
                     units: units),
      _ => throw const FormatException('Faild to load satellite.'),
    };
  }
}
