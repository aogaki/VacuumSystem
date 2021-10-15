#ifndef TDataUploader_hpp
#define TDataUploader_hpp 1

#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/builder/stream/helpers.hpp>
#include <bsoncxx/json.hpp>
#include <memory>
#include <mongocxx/client.hpp>
#include <mongocxx/pool.hpp>
#include <mongocxx/stdx.hpp>
#include <mongocxx/uri.hpp>
#include <string>

class TDataUploader
{
 public:
  TDataUploader();
  TDataUploader(std::string address);
  ~TDataUploader();

  void SetServerAddress(std::string address);

  void UploadData(std::string sensorName, double pressure, long timeStamp);

 private:
  std::unique_ptr<mongocxx::pool> fPool;
};

#endif
