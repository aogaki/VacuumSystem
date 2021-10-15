#include "TDataUploader.hpp"

TDataUploader::TDataUploader()
{
  fPool.reset(new mongocxx::pool(mongocxx::uri("mongodb://localhost/ELITPC")));
}

TDataUploader::TDataUploader(std::string address) { SetServerAddress(address); }

TDataUploader::~TDataUploader() {}

void TDataUploader::SetServerAddress(std::string address)
{
  fPool.reset(new mongocxx::pool(mongocxx::uri(address)));
}

void TDataUploader::UploadData(std::string sensorName, double pressure,
                               long timeStamp)
{
  auto conn = fPool->acquire();
  auto collection = (*conn)["ELITPC"]["VacMon"];

  bsoncxx::builder::stream::document buf{};

  buf << "time" << timeStamp << "pressure" << pressure << "name" << sensorName;
  collection.insert_one(buf.view());
  buf.clear();
}
