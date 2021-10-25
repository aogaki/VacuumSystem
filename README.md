# VacuumSystem
Taking, recording, and plotting data from Pfeiffer TPG 362.  Now, this provides only the data taking part.  The data taking part will be separated from here.  

This software uses, MongoCXX driver, libserial (header files are needed), JSON for Modern C++ (https://github.com/nlohmann/json), influxdb-cpp (https://github.com/orca-zhang/influxdb-cpp).

The supported DB is MongoDB and InfluxDB now.  settings.json is the setting file for the DB server addresses, using sensor, and so on.  
