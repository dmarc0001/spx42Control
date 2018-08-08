/*#############################################################################
#### Kapselung der verschiedenen Kommandos und deren Auswertung zur        ####
#### besseren Wiederverwendbarkeit in späteren Projekten                   ####
#############################################################################*/

#include "Spx42Commands.hpp"
#include <stdio.h>
#include <QByteArray>

namespace spx
{
  Spx42Commands::Spx42Commands()
  {
  }

  void Spx42Commands::makeSipleCommand( char commandChar )
  {
    QByteArray code;
    cmd.clear();
    cmd.append( &SpxCommandDef::STX, 1 );
    cmd.append( "~" );
    code.append( &commandChar, 1 );
    cmd.append( code.toHex() );
    cmd.append( &SpxCommandDef::ETX, 1 );
  }

  QByteArray &Spx42Commands::sendManufacturers( void )
  {
    makeSipleCommand( SpxCommandDef::SPX_MANUFACTURERS );
    return ( cmd );
  }

  QByteArray &Spx42Commands::sendSerialNumber( void )
  {
    makeSipleCommand( SpxCommandDef::SPX_SERIAL_NUMBER );
    return ( cmd );
  }

  QByteArray &Spx42Commands::sendAliveSignal( void )
  {
    makeSipleCommand( SpxCommandDef::SPX_ALIVE );
    return ( cmd );
  }

  QByteArray &Spx42Commands::sendFirmwareVersion( void )
  {
    makeSipleCommand( SpxCommandDef::SPX_APPLICATION_ID );
    return ( cmd );
  }

  QByteArray &Spx42Commands::sendStartYModem( void )
  {
    makeSipleCommand( SpxCommandDef::SPX_START_YMODEM );
    return ( cmd );
  }

  QByteArray &Spx42Commands::sendStartProgramming( void )
  {
    makeSipleCommand( SpxCommandDef::SPX_START_FW_PROGRAMM );
    return ( cmd );
  }

  QByteArray &Spx42Commands::shutdownSPX42( void )
  {
    makeSipleCommand( SpxCommandDef::SPX_SHUTDOWN );
    return ( cmd );
  }

  char Spx42Commands::decodeCommand( const QByteArray &pdu )
  {
    QByteArray cmd;
    char retVal = 0;
    //
    params.clear();
    if ( pdu[ 0 ] == '~' )
    {
      // OK, das ist ein Teil vom SPX
      params = pdu.split( ':' );
      cmd = params.at( 0 );
      cmd = cmd.remove( 0, 1 );
      retVal = static_cast< char >( cmd.toUInt( nullptr, 16 ) & 0xff );
    }
    return ( retVal );
  }

  QByteArray Spx42Commands::getParameter( int index )
  {
    if ( params.length() > index )
    {
      return ( params[ index ] );
    }
    return ( nullptr );
  }
}
