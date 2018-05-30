#ifndef DEBUGDATASERIESGENERATOR_HPP
#define DEBUGDATASERIESGENERATOR_HPP

#include <memory>
#include <random>

#include <QtCharts>

#include "../logging/Logger.hpp"
#include "../utils/SPX42Config.hpp"
#include "../config/SPX42Defs.hpp"
#include "../config/ProjectConst.hpp"
#include "IDataSeriesGenerator.hpp"

namespace spx
{
  class DebugDataSeriesGenerator : public IDataSeriesGenerator
  {
    private:
      std::random_device rd;                                        //! DEBUG Generator für DataSerien
      float maxdepth;
      int samples;
      std::mt19937 gen;

    public:
      explicit DebugDataSeriesGenerator( std::shared_ptr<Logger> logger, std::shared_ptr<SPX42Config> spxCfg );
      virtual ~DebugDataSeriesGenerator();
      virtual void setDeviceId( int deviceId );
      virtual QtCharts::QLineSeries* makeDepthSerie( int diveNum );
      virtual QtCharts::QLineSeries* makeTempSerie( int diveNum );
      virtual QtCharts::QLineSeries* makePPOSerie( int diveNum );
      virtual QtCharts::QLineSeries* makeSensor1Serie( int diveNum );
      virtual QtCharts::QLineSeries* makeSensor2Serie( int diveNum );
      virtual QtCharts::QLineSeries* makeSensor3Serie( int diveNum );

  };
}
#endif // DEBUGDATASERIESGENERATOR_HPP
