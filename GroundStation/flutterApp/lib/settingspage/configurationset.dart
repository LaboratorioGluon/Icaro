import 'package:flutter/material.dart';

class ConfiguracionSetWidget extends StatelessWidget {
  final String title;
  final Widget child;

  ConfiguracionSetWidget({
    required this.title,
    required this.child,
  });

  @override
  Widget build(BuildContext context) {
    return Padding(
      padding: const EdgeInsets.all(16.0),
      child: Stack(
        clipBehavior: Clip.none,
        children: [
          Text( title,
                style: TextStyle(
                  // color: Colors.blue,
                  fontWeight: FontWeight.bold,
                  fontSize: 18,
                  decoration: TextDecoration.underline,
                ),
              ),
          Container(
            padding: const EdgeInsets.only(top: 24.0),
            child: child,
          ),
        ],
      ),
    );
  }
}
