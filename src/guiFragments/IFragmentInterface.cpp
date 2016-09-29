#include "IFragmentInterface.hpp"


namespace spx42
{
  IFragmentInterface::IFragmentInterface(Logger *logger, SPX42Config *spxCfg) :
    lg( logger ),
    spxConfig( spxCfg )
  {

  }
  IFragmentInterface::~IFragmentInterface(void)
  {

  }

}
