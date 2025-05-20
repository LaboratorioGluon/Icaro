import 'package:flutter/material.dart';

import 'camerapage/camerapage.dart';
import 'homepage.dart';
import 'logspage.dart';
import 'mav/mavsensorspage.dart';
import 'mav/mavmappage.dart';
import 'settingspage/settingspage.dart';
import 'appsettings.dart';
import 'summary.dart';

/* App entry point */
void main() async {
  WidgetsFlutterBinding.ensureInitialized();
  await IcaroSettings.init();
  runApp(const IcaroApp());
}

class IcaroApp extends StatelessWidget {
  const IcaroApp({super.key});

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
  // ignore: unused_field
  static const appTitle = Text("Project Icaro");

  final GlobalKey navKey = GlobalKey();
  bool isExtended  = true;
  var buttonLeft   = 0.0;
  var buttonBottom = 0.0;
  var summaryLeft  = 0.0;
  var summaryTop   = 0.0;

  int selectedIndex = 0;
  void updateIndex (int newIndex)
  {
      setState(() {selectedIndex = newIndex;});
  }

  @override
  Widget build(BuildContext context) {
    final theme = Theme.of(context);
    final style = theme.textTheme.displayMedium!.copyWith(
      color: theme.colorScheme.onPrimary,
    );

    Widget page;
    switch(selectedIndex){
      case 0:
        page = IcaroHomePage();
      case 1:
        page = IcaroMAVSensorsPage();
      case 2:
        page = IcaroCameraPage();
      case 3:
        page = IcaroMAVMapPage();
      case 4:
        page = IcaroLogsPage();
      case 5:
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
                icon: Icon(Icons.wifi_tethering),
                label: Text('MAV Sensors'),
              ),
              NavigationRailDestination(
                icon: Icon(Icons.camera_alt),
                label: Text('MAV Camera'),
              ),
              NavigationRailDestination(
                icon: Icon(Icons.map_outlined),
                label: Text('MAV Map'),
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

          void toggleRail() {
            setState(() {
              isExtended = !isExtended && (constraints.maxWidth >= 600);
            });
          }

          var nav = SingleChildScrollView(
            key: navKey,
            child: ConstrainedBox(
              constraints: BoxConstraints(minHeight: MediaQuery.of(context).size.height),
                child: IntrinsicHeight(
                  child: NavigationRail(
                    extended: isExtended,
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

          var extendButton = FloatingActionButton(
            onPressed: toggleRail,
            mini: true,
            child: Icon(isExtended ? Icons.arrow_back : Icons.arrow_forward),
          );

          var summary = Summary();

          // Position extendedbutton
          WidgetsBinding.instance.addPostFrameCallback((_) {
            final navContext = navKey.currentContext;
            if (navContext != null) {
              setState(() {
                final box = navContext.findRenderObject() as RenderBox;
                final size = box.size;
                buttonLeft   = size.width - 20.0;
                buttonBottom = 20.0;
                summaryLeft = size.width + 20.0;
                summaryTop  = 10.0;
                isExtended = isExtended && (constraints.maxWidth >= 600);
              });
            }
          });

        return Scaffold(
          body: Stack(
            children: [
              Row(
                children: [
                  nav,
                  VerticalDivider(thickness: 1, width: 1),
                  center,
                ]
              ),
              Positioned(
                left: buttonLeft,
                bottom: buttonBottom,
                child: extendButton,
              ),
              Positioned(
                left: summaryLeft,
                top: summaryTop,
                child: summary
              )
            ],
          ),
        );
      },
    );

    return layout;
  }
}

