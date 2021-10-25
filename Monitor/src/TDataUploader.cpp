#include <iostream>

#include "TDataUploader.hpp"

TDataUploader::TDataUploader()
    : fUseMongoDB(false), fUseInfluxDB(false), fInfluxServer("", 0)
{
  fMongoPool.reset(
      new mongocxx::pool(mongocxx::uri("mongodb://localhost/ELITPC")));
}

TDataUploader::~TDataUploader() {}

void TDataUploader::SetMongoDB(std::string address, std::string dbName,
                               std::string collection)
{
  fMongoDBName = dbName;
  fMongoCollection = collection;
  auto uri = "mongodb://" + address + "/" + dbName;
  fMongoPool.reset(new mongocxx::pool(mongocxx::uri(uri)));
  EnableMongoDB();

  std::cout << "\nMongoDB server: " << uri << "\n"
            << "Collection name: " << fMongoCollection << std::endl;
}

void TDataUploader::SetInfluxDB(std::string address, std::string dbName,
                                std::string measurement)
{
  fMeasurement = measurement;
  fInfluxServer = influxdb_cpp::server_info(address, 8086, dbName);
  EnableInfluxDN();

  std::cout << "\nInfluxDB server: " << address << "\n"
            << "DB name: " << dbName << "\n"
            << "Measurement: " << fMeasurement << std::endl;
}

void TDataUploader::UploadData(std::string sensorName, double pressure,
                               long timeStamp)
{
  if (fUseMongoDB) {
    auto conn = fMongoPool->acquire();
    auto collection = (*conn)[fMongoDBName][fMongoCollection];

    bsoncxx::builder::stream::document buf{};

    buf << "time" << timeStamp << "pressure" << pressure << "sensor"
        << sensorName;
    collection.insert_one(buf.view());
    buf.clear();
  }

  if (fUseInfluxDB) {
    influxdb_cpp::builder()
        .meas(fMeasurement)
        .tag("sensor", sensorName)
        .field("pressure", pressure, 10)
        .timestamp(timeStamp * 1000000000)  // sec -> ns
        .post_http(fInfluxServer);
  }
}
