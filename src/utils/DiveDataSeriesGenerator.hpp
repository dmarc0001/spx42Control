#ifndef DIVEDATASERIESGENERATOR_HPP
#define DIVEDATASERIESGENERATOR_HPP

#include <memory>

#include <QtCharts>

#include "../config/ProjectConst.hpp"
#include "../config/SPX42Defs.hpp"
#include "../logging/Logger.hpp"
#include "../utils/SPX42Config.hpp"
#include "IDataSeriesGenerator.hpp"

namespace spx
{
  class DiveDataSeriesGenerator : public IDataSeriesGenerator
  {
    private:
    public:
    explicit DiveDataSeriesGenerator( std::shared_ptr< Logger > logger, std::shared_ptr< SPX42Config > spxCfg );
    virtual ~DiveDataSeriesGenerator() = default;
    virtual QLineSeries *makeDepthSerie( int diveNum ) = 0;
    virtual QLineSeries *makeTempSerie( int diveNum ) = 0;
    virtual QLineSeries *makePPOSerie( int diveNum ) = 0;
    virtual QLineSeries *makeSensor1Serie( int diveNum ) = 0;
    virtual QLineSeries *makeSensor2Serie( int diveNum ) = 0;
    virtual QLineSeries *makeSensor3Serie( int diveNum ) = 0;
  };
}
#endif  // DIVEDATASERIESGENERATOR_HPP
