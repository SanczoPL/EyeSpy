
#include <signal.h>
#include <stdio.h>  //printf()
#include <stdlib.h> //exit()
#include <QCoreApplication>
#include <QDebug>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QObject>
#include "../../src/tools.h"
#include "mainloop.h"

int main(int argc, char *argv[])
{
  printf("dasdadsda");

  QCoreApplication application(argc, argv);
  QString configDir{ "config.json" };
  if (argc > 1) configDir = argv[1];

  QFile jConfigFile{ configDir };
  if (!jConfigFile.open(QIODevice::ReadOnly))
  {
    qFatal("Failed to load config.json! :(");
  }

  QJsonDocument jConfigDoc{ QJsonDocument::fromJson((jConfigFile.readAll())) };
  if (!jConfigDoc.isObject())
  {
    qFatal("Invalid json config file!");
  }
  QJsonObject jObject{ jConfigDoc.object() };
  auto messageLevel{ jObject["LogLevel"].toInt() };
  H_Logger->set_level(static_cast<spdlog::level::level_enum>(messageLevel));
  H_Logger->set_pattern("[%Y-%m-%d] [%H:%M:%S.%e] [%t] [%^%l%$] %v");
  H_Logger->debug("start main logger");

  qRegisterMetaType<cv::Mat>("cv::Mat");
  qRegisterMetaType<std::string>("std::string");

  MainLoop o_mainLoop{ jObject };
  return application.exec();
}
