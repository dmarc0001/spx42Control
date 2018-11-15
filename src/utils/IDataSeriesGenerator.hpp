#ifndef IDATASERIESGENERATOR_HPP
#define IDATASERIESGENERATOR_HPP

#include <memory>

#include "config/SPX42Defs.hpp"
#include "logging/Logger.hpp"
#include "spx42/SPX42Config.hpp"

#include <QtCharts>

namespace spx
{
  class IDataSeriesGenerator
  {
    protected:
    std::shared_ptr< Logger > lg;              //! Zeiger auf das Log-Objekt
    std::shared_ptr< SPX42Config > spxConfig;  //! Zeiger auf das SPX42 Config Objekt
    int deviceId;
    bool dataAvaivible;

    public:
    explicit IDataSeriesGenerator( std::shared_ptr< Logger > logger, std::shared_ptr< SPX42Config > spxCfg );
    virtual ~IDataSeriesGenerator() = default;
    virtual void setDeviceId( int deviceId );
    virtual QtCharts::QLineSeries *makeDepthSerie( int diveNum ) = 0;
    virtual QtCharts::QLineSeries *makeTempSerie( int diveNum ) = 0;
    virtual QtCharts::QLineSeries *makePPOSerie( int diveNum ) = 0;
    virtual QtCharts::QLineSeries *makeSensor1Serie( int diveNum ) = 0;
    virtual QtCharts::QLineSeries *makeSensor2Serie( int diveNum ) = 0;
    virtual QtCharts::QLineSeries *makeSensor3Serie( int diveNum ) = 0;
  };
}

#endif  // IDATASERIESGENERATOR_HPP
