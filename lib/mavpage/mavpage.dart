import 'dart:async';

import 'package:flutter/material.dart';

import 'package:icaro_app/common/services/mavservice.dart';

class IcaroMAVPage extends StatefulWidget {
  const IcaroMAVPage({super.key});
  static const String pageTitle = "Icaro MAV page";

  @override
  State<IcaroMAVPage> createState() => _IcaroMAVPageState();
}

class _IcaroMAVPageState extends State<IcaroMAVPage> {
  late StreamSubscription<int> _mavStatusSub;
  late StreamSubscription<Map<int, MAVCameraStatus>> _mavCameraSub;

  int linkCoverage = 0;
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

    var activeText = "Coverage $linkCoverage";
    var activeCard = Card(
            color: theme.colorScheme.primary,
            elevation: 10,
            child: Padding(
              padding: const EdgeInsets.all(20),
              child: Text(activeText, style: style,),
            ),
          );

    return Column(
        mainAxisAlignment: MainAxisAlignment.center,
        children: [
          activeCard,
          Card(
            color: theme.colorScheme.primary,
            elevation: 10,
            child: Padding(
              padding: const EdgeInsets.all(20),
              child: Text(IcaroMAVPage.pageTitle, style: style,),
            ),
          ),
          buildCameraStatusList(cameras),
        ],
      );
  }

  Widget buildCameraStatusList(Map<int, MAVCameraStatus> cameras) {
    
    final entries = cameras.entries.toList();
    return SizedBox(
        height: 300,
        width: 333,
        child:ListView.builder(
            
      itemCount: entries.length,
      itemBuilder: (context, index) {
        final entry = entries[index];
        return ListTile(
          title: Text(entry.key.toString()),
          subtitle: Text(entry.value.imageCaptureCount.toString()),
        );
      },
    )
    );
    // }
  }
}