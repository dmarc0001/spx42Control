#include "IDataSeriesGenerator.hpp"

namespace spx42
{
  IDataSeriesGenerator::IDataSeriesGenerator( std::shared_ptr<Logger> logger, std::shared_ptr<SPX42Config> spxCfg) :
    lg( logger ),
    spxConfig( spxCfg ),
    deviceId( -1 ),
    dataAvaivible( false )
  {

  }

  IDataSeriesGenerator::~IDataSeriesGenerator()
  {

  }

  /**
   * @brief Geräte id setzen
   * @param deviceId gerätenummer in der Datenbank
   */
  void IDataSeriesGenerator::setDeviceId( int deviceId )
  {
    this->deviceId = deviceId;
  }

}
