#include "DiveDataSeriesGenerator.hpp"

namespace spx
{
  DiveDataSeriesGenerator::DiveDataSeriesGenerator( std::shared_ptr< Logger > logger, std::shared_ptr< SPX42Config > spxCfg )
      : IDataSeriesGenerator( logger, spxCfg )
  {
  }
}
