import 'package:flutter/material.dart';

import 'logo.dart';

class IcaroLoraPage extends StatelessWidget {
  const IcaroLoraPage({super.key});
  static const String pageTitle = "Icaro Lora page";

  @override
  Widget build(BuildContext context) {
    final theme = Theme.of(context);
    final style = theme.textTheme.displayMedium!.copyWith(
       color: theme.colorScheme.onPrimary,
       );

    return Column(
        mainAxisAlignment: MainAxisAlignment.center,
        children: [
          Card(
            color: theme.colorScheme.primary,
            elevation: 10,
            child: Padding(
              padding: const EdgeInsets.all(20),
              child: Text(pageTitle, style: style,),
            ),
          ),
          IcaroLogo()
        ],
      );
  }
}