import 'package:flutter/material.dart';

import 'logo.dart';

class IcaroHomePage extends StatelessWidget {
  const IcaroHomePage({super.key});
  static const String pageTitle = "Icaro Home page";

  @override
  Widget build(BuildContext context) {
    final theme = Theme.of(context);
    final style = theme.textTheme.displayMedium!.copyWith(
       color: theme.colorScheme.onPrimary,
       );

    // Title
    var title = Card(
            color: theme.colorScheme.primary,
            elevation: 10,
            child: Padding(
              padding: const EdgeInsets.all(20),
              child: Text(pageTitle, style: style,),
            ),
          );

    var content =  Column(
        mainAxisAlignment: MainAxisAlignment.center,
        children: [
          IcaroLogo()
        ],
      );

    return Column(
      mainAxisAlignment: MainAxisAlignment.start,
      children: [
        title,
        Expanded(child: SingleChildScrollView(child: content)),
      ]);

  }
}