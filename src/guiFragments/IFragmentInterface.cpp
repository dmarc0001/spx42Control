#include "IFragmentInterface.hpp"

namespace spx
{
  IFragmentInterface::IFragmentInterface( std::shared_ptr< Logger > logger,
                                          std::shared_ptr< SPX42Database > spx42Database,
                                          std::shared_ptr< SPX42Config > spxCfg,
                                          std::shared_ptr< SPX42RemotBtDevice > remSPX42,
                                          std::shared_ptr< SPX42Commands > spxCmds )
      : lg( logger ), database( spx42Database ), spxConfig( spxCfg ), remoteSPX42( remSPX42 ), spxCommands( spxCmds ), ackuVal( 0.0 )
  {
  }
}
