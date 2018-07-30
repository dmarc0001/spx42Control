#include "IFragmentInterface.hpp"

namespace spx
{
  IFragmentInterface::IFragmentInterface( std::shared_ptr< Logger > logger,
                                          std::shared_ptr< SPX42Database > spx42Database,
                                          std::shared_ptr< SPX42Config > spxCfg )
      : lg( logger ), database( spx42Database ), spxConfig( spxCfg )
  {
  }
}
