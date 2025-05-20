import 'package:flutter/material.dart';
import 'package:icaro_app/common/services/logservice.dart';
import 'package:intl/intl.dart';
import 'package:logger/logger.dart';
import 'package:logger_screen/logger_screen.dart';


class IcaroLogsPage extends StatelessWidget {
  const IcaroLogsPage({super.key});
  static const String pageTitle = "Logs";

  @override
  Widget build(BuildContext context) {
    final theme = Theme.of(context);
    final style = theme.textTheme.displayMedium!.copyWith(
       color: theme.colorScheme.onPrimary,
       );

    var loggerWidget = LoggerScreen(
        printer: LogService.screenPrinter,
        itemBuilder: (context, log, index) {
          return ListTile(
            title: Text("${log.message}"),
            subtitle: Text("[${log.level}] ${DateFormat('yyyy-MM-dd HH:mm:ss').format(log.timestamp)}"),
          );
        },
        numberOfColumns: 1,
        title: "",
        trailingActions: [
          IconButton(
            icon: Icon(Icons.clear_all),
            onPressed: () {
              LogService.screenPrinter.clear();
            },
          ),
        ],
      );

    // Page composition
    var title = Card(
      color: theme.colorScheme.primary,
      elevation: 10,
      child: Padding(
        padding: const EdgeInsets.all(20),
        child: Text(IcaroLogsPage.pageTitle, style: style,),
      ),
    );

    var content = Container(
      decoration: BoxDecoration(
        color: Color(0xfffef7ff), // Forced color from loggerWidget
        borderRadius: BorderRadius.circular(20.0),
      ),
      child: Padding(
        padding: const EdgeInsets.all(6),
        child: loggerWidget,
      ),
    );

    var page = Container(
      // color: Colors.brown,
      child: Column(
        children: [
          title,
          Expanded(
            child: content,
          ),
        ],
      ),
    );

    return page;
  }
}