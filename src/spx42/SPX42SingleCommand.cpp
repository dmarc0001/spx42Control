#include "SPX42SingleCommand.hpp"

namespace spx
{
  /**
   * @brief SPX42SingleCommand::SPX42SingleCommand
   * @param cmd
   * @param params
   */
  SPX42SingleCommand::SPX42SingleCommand( char cmd, const QList< QByteArray > &params, int tag, int seq )
      : command( cmd ), params( params ), diveNum( tag ), sequence( seq )
  {
  }

  /**
   * @brief SPX42SingleCommand::getCommand
   * @return
   */
  char SPX42SingleCommand::getCommand()
  {
    return ( command );
  }

  /**
   * @brief SPX42SingleCommand::getParamAt
   * @param idx
   * @return parameterwert
   */
  QByteArray SPX42SingleCommand::getParamAt( int idx )
  {
    //
    // sicherstellen, dass nur gültige Werte zurück kommen
    //
    if ( idx > -1 && idx < params.count() )
    {
      return ( params.at( idx ) );
    }
    return ( QByteArray() );
  }

  /**
   * @brief SPX42SingleCommand::getValueFromHexAt
   * @param idx
   * @return
   */
  uint SPX42SingleCommand::getValueFromHexAt( int idx )
  {
    if ( idx > -1 && idx < params.count() )
    {
      uint value = params.at( idx ).toUInt( nullptr, 16 );
      return ( value );
    }
    // wenn das komisch ist, MAX zurückgeben
    return ( static_cast< uint >( std::numeric_limits< unsigned int >::max() ) );
  }

  uint SPX42SingleCommand::getValueAt( int idx )
  {
    if ( idx > -1 && idx < params.count() )
    {
      uint value = params.at( idx ).toUInt( nullptr, 10 );
      return ( value );
    }
    // wenn das komisch ist, MAX zurückgeben
    return ( static_cast< uint >( std::numeric_limits< unsigned int >::max() ) );
  }

  double SPX42SingleCommand::getDoubleValueAt( int idx )
  {
    if ( idx > -1 && idx < params.count() )
    {
      double value = params.at( idx ).toDouble();
      return ( value );
    }
    // wenn das komisch ist, MAX zurückgeben
    return ( static_cast< double >( std::numeric_limits< double >::max() ) );
  }

  int SPX42SingleCommand::getDiveNum()
  {
    return ( diveNum );
  }

  int SPX42SingleCommand::getSequence()
  {
    return ( sequence );
  }
}
