#ifndef TCommTPG_hpp
#define TCommTPG_hpp 1

#include <libserial/SerialPort.h>

#include <map>
#include <string>
#include <vector>

constexpr double READ_SENSOR_ERROR = -1.;

const static std::map<int, std::string> StatusCodeMap{
    {0, "Measurement data okay"}, {1, "Underrange"}, {2, "Overrange"},
    {3, "Sensor error"},          {4, "Sensor off"}, {5, "No sensor"},
    {6, "Identification error"}};

class TCommTPG
{
 public:
  TCommTPG();
  TCommTPG(std::string portName);
  ~TCommTPG();

  double ReadSensor(int id = 1);

  std::vector<std::string> CommDevice(std::string cmd = "AYT");

 private:
  std::string fPortName = "/dev/ttyUSB0";
  LibSerial::SerialPort fSerialPort;

  void OpenDevice();
  void Config();
  void CloseDevice();
};

#endif
