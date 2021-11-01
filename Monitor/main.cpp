#include <unistd.h>

#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>

#include "TCommTPG.hpp"
#include "TDataUploader.hpp"
#include "influxdb.hpp"

void PrintUsage()
{
  std::cout << "-t: Time interval of measurement in sec.\n"
            << "-d: Device name e.g. /dev/ttyUSB0\n"
            << "All settings (settings.json) are overwritten by command line.\n"
            << std::endl;
}

int main(int argc, char *argv[])
{
  bool useMongoDB = true;
  std::string mongoAddress = "172.18.7.140";
  std::string mongoName = "ELIADE";
  std::string collection = "VacMon";

  bool useInfluxDB = true;
  std::string influxAddress = "172.18.7.140";
  std::string influxName = "ELIADE";
  std::string measurement = "vacuum-monitor";

  int timeInterval = 10;
  std::string deviceName = "/dev/ttyUSB0";
  bool useSensor1 = false;
  bool useSensor2 = false;

  std::ifstream fin("settings.json");
  nlohmann::json jsonSettings;
  fin >> jsonSettings;

  useMongoDB = jsonSettings["useMongoDB"];
  mongoAddress = jsonSettings["mongoAddress"];
  mongoName = jsonSettings["mongoName"];
  collection = jsonSettings["collection"];

  useInfluxDB = jsonSettings["useInfluxDB"];
  influxAddress = jsonSettings["influxAddress"];
  influxName = jsonSettings["influxName"];
  measurement = jsonSettings["measurement"];

  deviceName = jsonSettings["deviceName"];
  timeInterval = jsonSettings["timeInterval"];

  useSensor1 = jsonSettings["sensor1"];
  useSensor2 = jsonSettings["sensor2"];

  for (auto i = 1; i < argc; i++) {
    if (std::string(argv[i]) == "-t") {
      timeInterval = std::stoi(argv[i + 1]);
    } else if (std::string(argv[i]) == "-d") {
      deviceName = argv[i + 1];
    } else if (std::string(argv[i]) == "-h") {
      PrintUsage();
      exit(0);
    }
  }

  auto tmpName = mongoAddress;
  auto startPos = tmpName.find("//") + 2;
  auto stopPos = tmpName.find("@");
  if (stopPos != std::string::npos)
    tmpName.replace(startPos, stopPos - startPos, "***:***");
  std::cout << "Device name: " << deviceName << "\n"
            << "Time interval: " << timeInterval << std::endl;
  std::cout << "Some Read Timeout are made by baud rate detecting "
            << "during the configuration." << std::endl;
  std::unique_ptr<TCommTPG> tpg{new TCommTPG(deviceName)};

  std::unique_ptr<TDataUploader> uploader{new TDataUploader()};
  if (useMongoDB) uploader->SetMongoDB(mongoAddress, mongoName, collection);
  if (useInfluxDB)
    uploader->SetInfluxDB(influxAddress, influxName, measurement);

  auto last = time(nullptr);
  while (true) {
    auto now = time(nullptr);
    if (now - last >= timeInterval) {
      auto pressure1 = tpg->ReadSensor(1);
      auto pressure2 = tpg->ReadSensor(2);
      std::cout << "PR1: " << pressure1 << "\tPR2: " << pressure2 << std::endl;

      if (useSensor1 && pressure1 != READ_SENSOR_ERROR) {
        uploader->UploadData("PR1", pressure1, now);
      }
      if (useSensor2 && pressure2 != READ_SENSOR_ERROR) {
        uploader->UploadData("PR2", pressure2, now);
      }

      if (pressure1 != READ_SENSOR_ERROR && pressure2 != READ_SENSOR_ERROR)
        last = now;
    }

    usleep(1000);
  }

  return 0;
}
