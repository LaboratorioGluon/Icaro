import 'dart:async';
import 'dart:typed_data';
import 'package:icaro_app/appsettings.dart';
import 'package:icaro_app/common/services/mavservice.dart';
import 'package:material_design_icons_flutter/material_design_icons_flutter.dart';
import 'package:mjpeg_stream/mjpeg_stream.dart';
import 'package:flutter/material.dart';


class IcaroCameraPage extends StatefulWidget {
  const IcaroCameraPage({super.key});
  static const String pageTitle = "Icaro Camera page";

  @override
  State<IcaroCameraPage> createState() => _IcaroCameraPageState();
}

class _IcaroCameraPageState extends State<IcaroCameraPage> {
  Uint8List? imageBytes;
  late StreamSubscription<double>                    _mavStatusSub;
  late StreamSubscription<Map<int, MAVCameraStatus>> _mavCameraSub;

  double linkCoverage               = 0;
  Map<int, MAVCameraStatus> cameras = {};

  // Initial state
  @override
  void initState() {
    super.initState();
    
    // MAV link status monitor
    _mavStatusSub = MAVService().linkStatusStream.listen((newCoverage) {
      setState((){
        linkCoverage = newCoverage;
      });
    });

    // MAV link cameras data
    _mavCameraSub = MAVService().cameraStatusStream.listen((newCameras) {
      setState((){
        cameras = newCameras;
      });
    });
  }

  @override
  void dispose() {
    _mavStatusSub.cancel();
    _mavCameraSub.cancel();
    super.dispose();
  }

  @override
  Widget build(BuildContext context) {
    final theme = Theme.of(context);
    final style = theme.textTheme.displayMedium!.copyWith(
       color: theme.colorScheme.onPrimary,
       );

    final panelBorderRadius = 15.0;

    final cameraSettings = IcaroSettings.getCameraSettings();

    String streamUrl = "http://${cameraSettings.host}:${cameraSettings.port}/${cameraSettings.name}";

    // Right panel
    var stream = Padding(
      padding: const EdgeInsets.all(8.0),
      child: MJPEGStreamScreen(
        streamUrl: streamUrl,
        height: double.infinity,
        width: double.infinity,
        borderRadius: panelBorderRadius,
        showLiveIcon: true,
        watermarkText: "IcaroView",
        showWatermark: true,
        blurSensitiveContent: true,
      ),
    );

    var rigthPanel = Container(
      // color: Colors.amber,
      child: stream
    );

    // Left panel
    var coverageSymbol = getCoverageIcon(linkCoverage);
    
    var coverageBar = Row(
      children: [
        coverageSymbol,
        Padding(
          padding: EdgeInsets.symmetric(vertical: 5, horizontal: 10), 
          child: Text("Coverage"), 
        ),
      ],
    );

    final entries = cameras.entries.toList();
    
    var cameraList = Container(
      // color: Colors.cyan,
      height: 120.0 * entries.length,
      width: 200.0,
      child: ListView.builder(        
        itemCount: entries.length,
        itemBuilder: (context, index) {
          final entry = entries[index];
          return getCameraInfoBox(context, entry.key, entry.value);
        },
      )
    );

    var leftPanel = Padding ( 
      padding: const EdgeInsets.all(8.0),
      child: Container(
        decoration: BoxDecoration(
          border: Border.all(
            color: theme.colorScheme.primary,
            width: 5.0 
          ),
          borderRadius: BorderRadius.circular(panelBorderRadius),
        ),
        child: Column(
          children: [
            coverageBar,
            SingleChildScrollView(
              child: cameraList
            )
          ]
        )
      )
    );

    // Page composition
    var title = Card(
      color: theme.colorScheme.primary,
      elevation: 10,
      child: Padding(
        padding: const EdgeInsets.all(20),
        child: Text(IcaroCameraPage.pageTitle, style: style,),
      ),
    );

    var content = Container(
      // color: Colors.purple,
      child: Row(
        children: [
          leftPanel,
          // rigthPanel,
          Expanded(child: rigthPanel,),
        ],
      ),
    );

    var page = Container(
      // color: Colors.brown,
      child: Column(
        children: [
          title,
          Expanded(
            child: content,
          ),
        ],
      ),
    );

    return page;
  }

  Icon getCoverageIcon(double coverage)
  {
    if (coverage >= 80.0)
    {
      return Icon(MdiIcons.wifiStrength4);
    }
    else if (coverage >= 60.0)
    {
      return Icon(MdiIcons.wifiStrength3);
    }
    else if (coverage >= 40.0)
    {
      return Icon(MdiIcons.wifiStrength2);
    }
    else if (coverage >= 20.0)
    {
      return Icon(MdiIcons.wifiStrength1);
    }
    else
    {
      return Icon(MdiIcons.wifiStrengthOff);
    }
  }

  Widget getCameraInfoBox(BuildContext context, int cameraID, MAVCameraStatus cameraInfo)
  {
    final theme = Theme.of(context);
    final style = theme.textTheme.displayMedium!.copyWith(
       color: theme.colorScheme.onPrimary,
       );

    String cameraName;
    switch(cameraID)
    {
      case 0:  cameraName = "StorageCam";
      case 1:  cameraName = "StreamingCam";
      default: cameraName = "Unknown";
    }


    // Position widgets
    List<Widget> posWidgets = [];
    if (cameraInfo.latitude  != 0.0 &&
        cameraInfo.longitude != 0.0 && 
        cameraInfo.altitude  != 0.0)
    {
      posWidgets = [
        Text("Latitude: ${cameraInfo.latitude.toString()}"),
        Text("Longitude: ${cameraInfo.longitude.toString()}"),
        Text("Altitude: ${cameraInfo.altitude.toString()}"),
      ];
    }

    List<Widget> widgets = [
      Text("Updates received: ${cameraInfo.imageCaptureCount.toString()}"),
      Text("Image index: ${cameraInfo.lastImageCapturedIndex.toString()}"),
      Text("Error count: ${cameraInfo.imageCapturedErrors.toString()}"),
    ];
    widgets.addAll(posWidgets);

    return ListTile(
        title: Text(cameraName),
        subtitle: Container (
          // color: Colors.yellow,
          child: Column(
          crossAxisAlignment: CrossAxisAlignment.start,
          children: widgets
        )
      ));
  }
}