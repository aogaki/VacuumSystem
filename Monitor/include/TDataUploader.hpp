#ifndef TDataUploader_hpp
#define TDataUploader_hpp 1

#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/builder/stream/helpers.hpp>
#include <bsoncxx/json.hpp>
#include <influxdb.hpp>
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
  // TDataUploader(std::string address);
  ~TDataUploader();

  // Make different claases?
  void SetMongoDB(std::string address, std::string dbName,
                  std::string collection);
  void SetInfluxDB(std::string address, std::string dbName,
                   std::string measurement);

  void UploadData(std::string sensorName, double pressure, long timeStamp);

 private:
  void EnableMongoDB() { fUseMongoDB = true; };
  bool fUseMongoDB;
  std::unique_ptr<mongocxx::pool> fMongoPool;
  std::string fMongoDBName;
  std::string fMongoCollection;

  void EnableInfluxDN() { fUseInfluxDB = true; };
  bool fUseInfluxDB;
  influxdb_cpp::server_info fInfluxServer;
  std::string fMeasurement;
};

#endif
