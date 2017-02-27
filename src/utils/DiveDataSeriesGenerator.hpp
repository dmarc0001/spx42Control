#ifndef DIVEDATASERIESGENERATOR_HPP
#define DIVEDATASERIESGENERATOR_HPP

#include <memory>

#include <QtCharts>

#include "../logging/Logger.hpp"
#include "../utils/SPX42Config.hpp"
#include "../config/SPX42Defs.hpp"
#include "../config/ProjectConst.hpp"
#include "IDataSeriesGenerator.hpp"


namespace spx42
{
  class DiveDataSeriesGenerator : public IDataSeriesGenerator
  {
    private:
      std::shared_ptr<Logger> lg;                               //! Zeiger auf das Log-Objekt
      std::shared_ptr<SPX42Config> spxConfig;                   //! Zeiger auf das SPX42 Config Objekt
    public:
      explicit DiveDataSeriesGenerator( std::shared_ptr<Logger> logger, std::shared_ptr<SPX42Config> spxCfg);
      virtual ~DiveDataSeriesGenerator();
      virtual QLineSeries* makeDepthSerie( int diveNum ) = 0;
      virtual QLineSeries* makeTempSerie( int diveNum ) = 0;
      virtual QLineSeries* makePPOSerie( int diveNum ) = 0;
      virtual QLineSeries* makeSensor1Serie( int diveNum ) = 0;
      virtual QLineSeries* makeSensor2Serie( int diveNum ) = 0;
      virtual QLineSeries* makeSensor3Serie( int diveNum ) = 0;
  };
}
#endif // DIVEDATASERIESGENERATOR_HPP
