import 'dart:async';

import 'package:flutter/material.dart';

import '../logo.dart';

import 'package:icaro_app/common/services/mavservice.dart';

class IcaroMAVPage extends StatefulWidget {
  const IcaroMAVPage({super.key});
  static const String pageTitle = "Icaro MAV page";

  @override
  State<IcaroMAVPage> createState() => _IcaroMAVPageState();
}

class _IcaroMAVPageState extends State<IcaroMAVPage> {
  late StreamSubscription<bool> _mavSub;

  bool linkStatus = false;

  // Initial state
  @override
  void initState() {
    super.initState();

    // Initialize ISS data subscription for tracking
    _mavSub = MAVService().linkStatusStream.listen((newLinkStatus) {
      setState((){
        linkStatus = newLinkStatus;
      });
    });
  }

  @override
  void dispose() {
    _mavSub.cancel();
    super.dispose();
  }

  @override
  Widget build(BuildContext context) {
    final theme = Theme.of(context);
    final style = theme.textTheme.displayMedium!.copyWith(
       color: theme.colorScheme.onPrimary,
       );

    var activeText = (linkStatus) ? "Is active" : "Is not active";
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
          IcaroLogo()
        ],
      );
  }
}