# This is a generated file! Please edit source .ksy file and use kaitai-struct-compiler to rebuild

import kaitaistruct
from kaitaistruct import KaitaiStruct, KaitaiStream, BytesIO
import collections


if getattr(kaitaistruct, 'API_VERSION', (0, 9)) < (0, 9):
    raise Exception("Incompatible Kaitai Struct Python API: 0.9 or later is required, but you have %s" % (kaitaistruct.__version__))

class Icaro(KaitaiStruct):
    SEQ_FIELDS = ["timestamp", "bme280", "acc", "gyro", "gps", "pt100_ext", "pt100_int", "vin", "v5", "currentv3", "currentv5"]
    def __init__(self, _io, _parent=None, _root=None):
        self._io = _io
        self._parent = _parent
        self._root = _root if _root else self
        self._debug = collections.defaultdict(dict)

    def _read(self):
        self._debug['timestamp']['start'] = self._io.pos()
        self.timestamp = self._io.read_s8be()
        self._debug['timestamp']['end'] = self._io.pos()
        self._debug['bme280']['start'] = self._io.pos()
        self.bme280 = Icaro.Bme(self._io, self, self._root)
        self.bme280._read()
        self._debug['bme280']['end'] = self._io.pos()
        self._debug['acc']['start'] = self._io.pos()
        self.acc = Icaro.Bmi(self._io, self, self._root)
        self.acc._read()
        self._debug['acc']['end'] = self._io.pos()
        self._debug['gyro']['start'] = self._io.pos()
        self.gyro = Icaro.Bmi(self._io, self, self._root)
        self.gyro._read()
        self._debug['gyro']['end'] = self._io.pos()
        self._debug['gps']['start'] = self._io.pos()
        self.gps = Icaro.Gps(self._io, self, self._root)
        self.gps._read()
        self._debug['gps']['end'] = self._io.pos()
        self._debug['pt100_ext']['start'] = self._io.pos()
        self.pt100_ext = self._io.read_f4be()
        self._debug['pt100_ext']['end'] = self._io.pos()
        self._debug['pt100_int']['start'] = self._io.pos()
        self.pt100_int = self._io.read_f4be()
        self._debug['pt100_int']['end'] = self._io.pos()
        self._debug['vin']['start'] = self._io.pos()
        self.vin = self._io.read_f4be()
        self._debug['vin']['end'] = self._io.pos()
        self._debug['v5']['start'] = self._io.pos()
        self.v5 = self._io.read_f4be()
        self._debug['v5']['end'] = self._io.pos()
        self._debug['currentv3']['start'] = self._io.pos()
        self.currentv3 = self._io.read_f4be()
        self._debug['currentv3']['end'] = self._io.pos()
        self._debug['currentv5']['start'] = self._io.pos()
        self.currentv5 = self._io.read_f4be()
        self._debug['currentv5']['end'] = self._io.pos()

    class Bme(KaitaiStruct):
        SEQ_FIELDS = ["temperature", "pressure", "humidity"]
        def __init__(self, _io, _parent=None, _root=None):
            self._io = _io
            self._parent = _parent
            self._root = _root if _root else self
            self._debug = collections.defaultdict(dict)

        def _read(self):
            self._debug['temperature']['start'] = self._io.pos()
            self.temperature = self._io.read_f8be()
            self._debug['temperature']['end'] = self._io.pos()
            self._debug['pressure']['start'] = self._io.pos()
            self.pressure = self._io.read_f8be()
            self._debug['pressure']['end'] = self._io.pos()
            self._debug['humidity']['start'] = self._io.pos()
            self.humidity = self._io.read_f8be()
            self._debug['humidity']['end'] = self._io.pos()


    class Bmi(KaitaiStruct):
        SEQ_FIELDS = ["x", "y", "z", "time"]
        def __init__(self, _io, _parent=None, _root=None):
            self._io = _io
            self._parent = _parent
            self._root = _root if _root else self
            self._debug = collections.defaultdict(dict)

        def _read(self):
            self._debug['x']['start'] = self._io.pos()
            self.x = self._io.read_f4be()
            self._debug['x']['end'] = self._io.pos()
            self._debug['y']['start'] = self._io.pos()
            self.y = self._io.read_f4be()
            self._debug['y']['end'] = self._io.pos()
            self._debug['z']['start'] = self._io.pos()
            self.z = self._io.read_f4be()
            self._debug['z']['end'] = self._io.pos()
            self._debug['time']['start'] = self._io.pos()
            self.time = self._io.read_f4be()
            self._debug['time']['end'] = self._io.pos()


    class Gps(KaitaiStruct):
        SEQ_FIELDS = ["valid", "year", "month", "day", "latitude", "longitude", "altitude", "hour", "minute", "satellite", "dummy", "speed"]
        def __init__(self, _io, _parent=None, _root=None):
            self._io = _io
            self._parent = _parent
            self._root = _root if _root else self
            self._debug = collections.defaultdict(dict)

        def _read(self):
            self._debug['valid']['start'] = self._io.pos()
            self.valid = self._io.read_u1()
            self._debug['valid']['end'] = self._io.pos()
            self._debug['year']['start'] = self._io.pos()
            self.year = self._io.read_u1()
            self._debug['year']['end'] = self._io.pos()
            self._debug['month']['start'] = self._io.pos()
            self.month = self._io.read_u1()
            self._debug['month']['end'] = self._io.pos()
            self._debug['day']['start'] = self._io.pos()
            self.day = self._io.read_u1()
            self._debug['day']['end'] = self._io.pos()
            self._debug['latitude']['start'] = self._io.pos()
            self.latitude = self._io.read_f4be()
            self._debug['latitude']['end'] = self._io.pos()
            self._debug['longitude']['start'] = self._io.pos()
            self.longitude = self._io.read_f4be()
            self._debug['longitude']['end'] = self._io.pos()
            self._debug['altitude']['start'] = self._io.pos()
            self.altitude = self._io.read_f4be()
            self._debug['altitude']['end'] = self._io.pos()
            self._debug['hour']['start'] = self._io.pos()
            self.hour = self._io.read_u1()
            self._debug['hour']['end'] = self._io.pos()
            self._debug['minute']['start'] = self._io.pos()
            self.minute = self._io.read_u1()
            self._debug['minute']['end'] = self._io.pos()
            self._debug['satellite']['start'] = self._io.pos()
            self.satellite = self._io.read_u1()
            self._debug['satellite']['end'] = self._io.pos()
            self._debug['dummy']['start'] = self._io.pos()
            self.dummy = self._io.read_u1()
            self._debug['dummy']['end'] = self._io.pos()
            self._debug['speed']['start'] = self._io.pos()
            self.speed = self._io.read_f4be()
            self._debug['speed']['end'] = self._io.pos()




if "__main__" == __name__:
    import sys
    if len(sys.argv) != 2:
        print("Usage: python -m test.icaro FILENAME")
        sys.exit(1)

    with open(sys.argv[1], "rb") as f:
        data = f.read()
        io = KaitaiStream(BytesIO(data))
        instance = Icaro(io)
        instance._read()
        print(instance)