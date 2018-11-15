﻿/*#############################################################################
#### Kapselung der verschiedenen Kommandos und deren Auswertung zur        ####
#### besseren Wiederverwendbarkeit in späteren Projekten                   ####
#############################################################################*/

#include "SPX42Commands.hpp"
#include <QByteArray>
#include <cstdio>

namespace spx
{
  void SPX42Commands::makeSipleCommand( char commandChar )
  {
    QByteArray code;
    cmd.clear();
    cmd.append( &SPX42CommandDef::STX, 1 );
    cmd.append( "~" );
    code.append( &commandChar, 1 );
    cmd.append( code.toHex() );
    cmd.append( &SPX42CommandDef::ETX, 1 );
  }

  QByteArray &SPX42Commands::sendManufacturers()
  {
    makeSipleCommand( SPX42CommandDef::SPX_MANUFACTURERS );
    return ( cmd );
  }

  QByteArray &SPX42Commands::sendSerialNumber()
  {
    makeSipleCommand( SPX42CommandDef::SPX_SERIAL_NUMBER );
    return ( cmd );
  }

  QByteArray &SPX42Commands::sendAliveSignal()
  {
    makeSipleCommand( SPX42CommandDef::SPX_ALIVE );
    return ( cmd );
  }

  QByteArray &SPX42Commands::sendFirmwareVersion()
  {
    makeSipleCommand( SPX42CommandDef::SPX_APPLICATION_ID );
    return ( cmd );
  }

  QByteArray &SPX42Commands::sendStartYModem()
  {
    makeSipleCommand( SPX42CommandDef::SPX_START_YMODEM );
    return ( cmd );
  }

  QByteArray &SPX42Commands::sendStartProgramming()
  {
    makeSipleCommand( SPX42CommandDef::SPX_START_FW_PROGRAMM );
    return ( cmd );
  }

  QByteArray &SPX42Commands::shutdownSPX42()
  {
    makeSipleCommand( SPX42CommandDef::SPX_SHUTDOWN );
    return ( cmd );
  }

  char SPX42Commands::decodeCommand( const QByteArray &pdu )
  {
    QByteArray cmd;
    char retVal = 0;
    //
    params.clear();
    if ( pdu[ 0 ] == '~' )
    {
      // OK, das ist ein Datagramm vom SPX
      params = pdu.split( ':' );
      cmd = params.at( 0 );
      cmd = cmd.remove( 0, 1 );
      retVal = static_cast< char >( cmd.toUInt( nullptr, 16 ) & 0xff );
    }
    return ( retVal );
  }

  QByteArray SPX42Commands::getParameter( int index )
  {
    if ( params.length() > index )
    {
      return ( params[ index ] );
    }
    return ( nullptr );
  }
}