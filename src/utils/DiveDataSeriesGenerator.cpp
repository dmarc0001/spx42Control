#include "DiveDataSeriesGenerator.hpp"

namespace spx42
{
  DiveDataSeriesGenerator::DiveDataSeriesGenerator( std::shared_ptr<Logger> logger, std::shared_ptr<SPX42Config> spxCfg) :
    IDataSeriesGenerator(logger, spxCfg)
  {

  }

  DiveDataSeriesGenerator::~DiveDataSeriesGenerator()
  {

  }
}
