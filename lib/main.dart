import 'package:flutter/material.dart';
import 'package:provider/provider.dart';

import 'homepage.dart';
import 'state.dart';

/* App entry point */
void main() {
  runApp(const IcaroApp());
}

class IcaroApp extends StatelessWidget {
  const IcaroApp({super.key});
  static const String appTitle = "Icaro";

  @override
  Widget build(BuildContext context) {
    return ChangeNotifierProvider(
      create: (context) => IcaroState(),
      child: MaterialApp(
        title: appTitle,
        theme: ThemeData(
          useMaterial3: true,
          colorScheme: ColorScheme.fromSeed(seedColor: Colors.blue),
        ),
        home: IcaroHomePage(),
      )
    );
  }
}
