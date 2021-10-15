#include <unistd.h>

#include <iostream>

#include "TCommTPG.hpp"
#include "TDataUploader.hpp"

int main(int argc, char *argv[])
{
  // password is too open
  std::string serverName = "mongodb://172.18.4.56/ELITPC";
  int timeInterval = 10;
  std::string deviceName = "/dev/ttyUSB0";

  for (auto i = 1; i < argc; i++) {
    if (std::string(argv[i]) == "-s") {
      serverName = argv[i + 1];
    } else if (std::string(argv[i]) == "-t") {
      timeInterval = std::stoi(argv[i + 1]);
    } else if (std::string(argv[i]) == "-d") {
      deviceName = argv[i + 1];
    }
  }
  auto tmpName = serverName;
  auto startPos = tmpName.find("//") + 2;
  auto stopPos = tmpName.find("@");
  if (stopPos != std::string::npos)
    tmpName.replace(startPos, stopPos - startPos, "***:***");
  std::cout << "Device name: " << deviceName << std::endl;
  std::cout << "DB Server: " << tmpName << std::endl;
  std::cout << "Time interval: " << timeInterval << std::endl;

  std::cout << "Some Read Timeout are made by baud rate detecting "
            << "during the configuration." << std::endl;
  std::unique_ptr<TCommTPG> tpg{new TCommTPG(deviceName)};
  std::unique_ptr<TDataUploader> uploader{new TDataUploader(serverName)};
  auto last = time(nullptr);
  while (true) {
    auto now = time(nullptr);
    if (now - last >= timeInterval) {
      auto pressure = tpg->ReadSensor(1);
      if (pressure != READ_SENSOR_ERROR) {  // READ_SENSOR_ERROR is -1
        uploader->UploadData("PR1", pressure, now);
      } else {
        ;
      }
      last = now;
    }

    usleep(1000);
  }

  return 0;
}
