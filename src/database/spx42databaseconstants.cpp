#include "spx42databaseconstants.hpp"
namespace spx
{
  const QString SPX42DatabaseConstants::createVersionTable{"create table 'version' ( version INTEGER default 0 )"};

  const QString SPX42DatabaseConstants::createDeviceTable{
      " \
CREATE TABLE 'devices' ( \
  device_id INTEGER PRIMARY KEY \
                    NOT NULL, \
  mac       TEXT    NOT NULL, \
  name      TEXT    NOT NULL, \
  alias     TEXT    DEFAULT NULL, \
  last      INTEGER DEFAULT 0 \
)"};

  const QString SPX42DatabaseConstants::createDiveLogDirectoryTable{
      "\
CREATE TABLE 'detaildir' ( \
  detail_id    INTEGER PRIMARY KEY \
                       NOT NULL, \
  device_id    INTEGER, \
  divenum      INTEGER DEFAULT 0, \
  ux_timestamp INTEGER DEFAULT 0, \
  detail_count INTEGER DEFAULT 0, \
  max_depth    INTEGER DEFAULT 0, \
  FOREIGN KEY ( \
               device_id \
              ) \
  REFERENCES 'devices' (device_id) DEFERRABLE INITIALLY DEFERRED \
)"};

  const QString SPX42DatabaseConstants::createDiveLogTableIndex01{
      "CREATE INDEX 'idx_detaildir_device_id' ON 'detaildir' ( device_id ASC )"};

  const QString SPX42DatabaseConstants::createDiveLogTableIndex02{
      "CREATE INDEX 'idx_detaildir_devid_divenum' ON 'detaildir'( detail_id ASC, device_id ASC )"};

  const QString SPX42DatabaseConstants::createDiveLogDetailsTable{
      "\
CREATE TABLE 'logdata' \
( \
    detail_id INTEGER, \
    lfnr INTEGER NOT NULL, \
    pressure INTEGER DEFAULT 0, \
    depth INTEGER DEFAULT 0, \
    temperature INTEGER DEFAULT 0, \
    acku REAL DEFAULT 0, \
    ppo2 REAL DEFAULT 0, \
    ppo2_1 DEFAULT 0, \
    ppo2_2 DEFAULT 0, \
    ppo2_3 DEFAULT 0, \
    setpoint INTEGER DEFAULT 0, \
    n2 INTEGER DEFAULT 78, \
    he INTEGER DEFAULT 0, \
    zerotime INTEGER DEFAULT 0, \
    next_step INTEGER DEFAULT 60, \
    FOREIGN KEY( detail_id ) REFERENCES 'detaildir'( detail_id ) DEFERRABLE INITIALLY DEFERRED \
) "};

  const QString SPX42DatabaseConstants::createDiveLogDetailsTableIndex01{
      "CREATE INDEX idx_logdata_detail ON 'logdata'( detail_id ASC )"};

  const QString SPX42DatabaseConstants::createDiveLogDetailsTableIndex02{
      "CREATE INDEX idx_logdata_detail_lfdnr ON 'logdata'( detail_id ASC, lfnr ASC )"};

  const QString SPX42DatabaseConstants::loglineInsertTemplate{
      "insert into 'logdata' \n"
      "(detail_id,lfnr,pressure,depth,temperature,acku,ppo2,ppo2_1,ppo2_2,ppo2_3,setpoint,n2,he,zerotime,next_step)\n"
      "values\n"
      "(%1,%2,%3,%4,%5,%6,%7,%8,%9,%10,%11,%12,%13,%14,%15)"};

}  // namespace spx
