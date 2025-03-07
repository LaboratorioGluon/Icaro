import 'package:flutter/material.dart';

class IcaroHomePage extends StatelessWidget {
  const IcaroHomePage({super.key});
  static const String logoAsset = "assets/IcaroPatch.png";

  @override
  Widget build(BuildContext context) {

    var icaroLogo = Image.asset(logoAsset);

    return Center(
      child: icaroLogo
    );
  }
}