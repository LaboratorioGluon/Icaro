import 'package:flutter/material.dart';

class IcaroLogo extends StatelessWidget {
  const IcaroLogo({super.key});
  static const String logoAsset = "assets/IcaroPatch.png";

  @override
  Widget build(BuildContext context) {

    var icaroLogo = Image.asset(logoAsset);

    return Center(
      child: icaroLogo
    );
  }
}