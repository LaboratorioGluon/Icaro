import 'package:flutter/material.dart';

class IcaroLogoPage extends StatelessWidget {
  const IcaroLogoPage({super.key});
  static const String logoAsset = "assets/IcaroPatch.png";

  @override
  Widget build(BuildContext context) {

    var icaroLogo = Image.asset(logoAsset);

    return Center(
      child: icaroLogo
    );
  }
}