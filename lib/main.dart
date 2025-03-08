import 'package:flutter/material.dart';
// import 'package:provider/provider.dart';

import 'camerapage.dart';
import 'homepage.dart';
import 'logspage.dart';
import 'lorapage.dart';
import 'mappage/mappage.dart';
import 'powerpage.dart';
import 'sensorspage.dart';
import 'settingspage.dart';
// import 'state.dart';

/* App entry point */
void main() {
  runApp(const ScaffoldExampleApp());
}

class ScaffoldExampleApp extends StatelessWidget {
  const ScaffoldExampleApp({super.key});

  @override
  Widget build(BuildContext context) {
    return const MaterialApp(home: IcaroMain());
  }
}

class IcaroMain extends StatefulWidget {
  const IcaroMain({super.key});

  @override
  State<IcaroMain> createState() => _IcaroMainState();
}

class _IcaroMainState extends State<IcaroMain> {
    static const appTitle = Text("IcaroApp");

    int selectedIndex = 0;
    void updateIndex (int newIndex)
    {
        setState(() {selectedIndex = newIndex;});
    }

    @override
    Widget build(BuildContext context) {

        Widget page;
        switch(selectedIndex){
          case 0:
            page = IcaroHomePage();
          case 1:
            page = IcaroSensorsPage();
          case 2:
            page = IcaroCameraPage();
          case 3:
            page = IcaroMapPage();
          case 4:
            page = IcaroPowerPage();
          case 5:
            page = IcaroLoraPage();
          case 6:
            page = IcaroLogsPage();
          case 7:
            page = IcaroSettingsPage();
          default:
          throw UnimplementedError("No widget for $selectedIndex");
        }
        
        var layout = LayoutBuilder(
            builder: (context, constraints) {
                var destinations = [
                    NavigationRailDestination(
                      icon: Icon(Icons.home),
                      label: Text('Home'),
                    ),
                    NavigationRailDestination(
                      icon: Icon(Icons.sensors),
                      label: Text('Sensors'),
                    ),
                    NavigationRailDestination(
                      icon: Icon(Icons.camera_alt),
                      label: Text('Camera'),
                    ),
                    NavigationRailDestination(
                      icon: Icon(Icons.map),
                      label: Text('Map'),
                    ),
                    NavigationRailDestination(
                      icon: Icon(Icons.power),
                      label: Text('Power'),
                    ),
                    NavigationRailDestination(
                      icon: Icon(Icons.wifi),
                      label: Text('Lora'),
                    ),
                    NavigationRailDestination(
                      icon: Icon(Icons.file_present),
                      label: Text('Logs'),
                    ),
                    NavigationRailDestination(
                      icon: Icon(Icons.settings),
                      label: Text('Settings'),
                    ),
                  ];

                var nav = SingleChildScrollView(
                            child: ConstrainedBox(
                              constraints: BoxConstraints(minHeight: MediaQuery.of(context).size.height),
                                child: IntrinsicHeight(
                                  child: NavigationRail(
                                    extended: constraints.maxWidth >= 800,
                                    selectedIndex: selectedIndex,
                                    onDestinationSelected: (int index) {
                                      updateIndex(index);
                                    },
                                    destinations: destinations,
                                  ),
                                ),
                              ),
                            );

                var center = Expanded(
                                child: Container(
                                    color: Theme.of(context).colorScheme.primaryContainer,
                                    child: page,
                                ),
                              );

                return Scaffold(
                    body: Row(
                        children: [
                            nav,
                            VerticalDivider(thickness: 1, width: 1),
                            center,
                        ],
                    ),
                );
            },
        );

        return layout;
    }
}

