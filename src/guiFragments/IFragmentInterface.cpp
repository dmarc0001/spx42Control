#include "IFragmentInterface.hpp"


namespace spx
{
  IFragmentInterface::IFragmentInterface( std::shared_ptr<Logger> logger, std::shared_ptr<SPX42Config> spxCfg) :
    lg( logger ),
    spxConfig( spxCfg )
  {

  }
  IFragmentInterface::~IFragmentInterface(void)
  {

  }

}
