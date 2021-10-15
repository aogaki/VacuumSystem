#include <unistd.h>

#include <array>
#include <iostream>
#include <sstream>

#include "TCommTPG.hpp"

TCommTPG::TCommTPG()
{
  OpenDevice();
  Config();
}

TCommTPG::TCommTPG(std::string portName) : fPortName(portName)
{
  OpenDevice();
  Config();
}

TCommTPG::~TCommTPG() { CloseDevice(); }

void TCommTPG::OpenDevice()
{
  try {
    // Open the Serial Ports at the desired hardware devices.
    fSerialPort.Open(fPortName);
  } catch (const std::exception &e) {
    std::cerr << "The serial ports did not open correctly: " << e.what()
              << std::endl;
  }
}

void TCommTPG::CloseDevice()
{
  try {
    fSerialPort.Close();
  } catch (const std::exception &e) {
    std::cerr << "The serial ports did not close correctly: " << e.what()
              << std::endl;
  }
}

void TCommTPG::Config()
{
  using namespace LibSerial;

  for (auto i = 0; i < 5; i++) {
    // Blute force checking baud rate.  And set max speed
    std::array<LibSerial::BaudRate, 5> ratesList{
        LibSerial::BaudRate::BAUD_9600, LibSerial::BaudRate::BAUD_19200,
        LibSerial::BaudRate::BAUD_38400, LibSerial::BaudRate::BAUD_57600,
        LibSerial::BaudRate::BAUD_115200};
    auto commCheck = 0;
    for (unsigned int iRate = 0; iRate < ratesList.size(); iRate++) {
      fSerialPort.SetBaudRate(ratesList[iRate]);
      auto ret = CommDevice("BAU,4");
      commCheck += ret.size();
    }

    if (commCheck > 0)
      break;
    else {
      if (i == 4) {
        std::cout << "Can not set the baud rate as 115200.\n"
                  << "Set the baud rate 115200 by manualy." << std::endl;
        exit(1);
      }
      std::cout << "Setting baud rate now." << std::endl;
      usleep(1000000);
    }
  }

  fSerialPort.SetBaudRate(LibSerial::BaudRate::BAUD_115200);
  fSerialPort.SetCharacterSize(CharacterSize::CHAR_SIZE_8);
  fSerialPort.SetFlowControl(FlowControl::FLOW_CONTROL_NONE);
  fSerialPort.SetParity(Parity::PARITY_NONE);
  fSerialPort.SetStopBits(StopBits::STOP_BITS_1);

  std::cout << "Configure done" << std::endl;
}

double TCommTPG::ReadSensor(int id)
{
  if (id != 1 && id != 2) {
    std::cerr << "Sensor id can be 1 or 2 (NOT 0 or 1).\n"
              << "Now reading sensor 1 instead of " << id << std::endl;
    id = 1;
  }

  auto cmd = "PR" + std::to_string(id);
  auto response = CommDevice(cmd);

  auto pressure = READ_SENSOR_ERROR;

  if (response.size() == 2) {
    if (response[0] == "0") {
      // Data is OK.
      pressure = std::stod(response[1]);
    } else {
      std::cerr << "Sensor" + std::to_string(id) + ": "
                << StatusCodeMap.at(std::stoi(response[0])) << std::endl;
      pressure = READ_SENSOR_ERROR;
    }
  }

  return pressure;
}

std::vector<std::string> TCommTPG::CommDevice(std::string cmd)
{
  constexpr size_t timeout = 25;  // in ms

  constexpr char ENQ = 0x05;
  constexpr char ACK = 0x06;
  constexpr char LF = 0x0A;
  constexpr char CR = 0x0D;
  constexpr char NAK = 0x15;

  std::vector<std::string> retVec;

  // TPG series retrun <ACK><CR><LF> to our request,
  // after get the response, we send <ENQ> and receive
  // the any data from TPG 361/362
  bool commFlag = true;
  try {
    std::string buf;
    fSerialPort.Write(cmd);
    fSerialPort.WriteByte(CR);
    fSerialPort.DrainWriteBuffer();
    fSerialPort.Read(buf, 3, timeout);
    // std::cout << std::hex << (int)buf[0] << "\t" << (int)buf[1] << "\t"
    //           << (int)buf[2] << std::endl;
    if (buf[0] == NAK) {  // try again
      fSerialPort.Write(cmd);
      fSerialPort.WriteByte(CR);
      fSerialPort.DrainWriteBuffer();
      fSerialPort.Read(buf, 3, timeout);
      // std::cout << std::hex << (int)buf[0] << "\t" << (int)buf[1] << "\t"
      //           << (int)buf[2] << std::endl;
    }

    commFlag = ((buf[0] == ACK) && (buf[1] == CR) && (buf[2] == LF));

  } catch (const LibSerial::ReadTimeout &timeOut) {
    std::cerr << "Read Timeout" << std::endl;
    commFlag = false;

  } catch (const std::exception &e) {
    std::cerr << "Read problem by " << e.what() << std::endl;
  }

  if (commFlag == false) {
    return retVec;
  }

  fSerialPort.WriteByte(ENQ);
  fSerialPort.WriteByte(CR);
  fSerialPort.DrainWriteBuffer();
  std::string retBuf;
  try {
    // I can not know the received size.
    // 256 is enough big.  Expecting timeout.
    fSerialPort.Read(retBuf, 256, timeout);
  } catch (const LibSerial::ReadTimeout &timeOut) {
    ;
  }

  // If retBuf has no CR, it means retBuf has problems
  auto responseSize = retBuf.find(CR);
  if (responseSize != std::string::npos) retBuf.resize(responseSize);
  // Splitting by ','.
  std::stringstream ss{retBuf};
  std::string ele;
  while (std::getline(ss, ele, ',')) {
    retVec.push_back(ele);
  }

  return retVec;
}
