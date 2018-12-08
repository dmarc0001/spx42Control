/*#############################################################################
#### Kapselung der verschiedenen Kommandos und deren Auswertung zur        ####
#### besseren Wiederverwendbarkeit in späteren Projekten                   ####
#############################################################################*/

#include "SPX42Commands.hpp"
#include <QByteArray>
#include <cstdio>

namespace spx
{
  CmdMarker::CmdMarker( char _cmd, bool _waitFor, bool _wasSend ) : cmd( _cmd ), waitFor( _waitFor ), wasSend( _wasSend ), sendTime()
  {
  }

  void SPX42Commands::makeSipleCommand( char commandChar, QByteArray &cmd )
  {
    QByteArray code;
    cmd.clear();
    cmd.append( &SPX42CommandDef::STX, 1 );
    cmd.append( "~" );
    code.append( &commandChar, 1 );
    cmd.append( code.toHex() );
    cmd.append( &SPX42CommandDef::ETX, 1 );
  }

  SendListEntry SPX42Commands::askForManufacturers()
  {
    QByteArray cmd;
    makeSipleCommand( SPX42CommandDef::SPX_MANUFACTURERS, cmd );
    return ( SendListEntry( CmdMarker( SPX42CommandDef::SPX_MANUFACTURERS ), cmd ) );
  }

  SendListEntry SPX42Commands::askForSerialNumber()
  {
    QByteArray cmd;
    makeSipleCommand( SPX42CommandDef::SPX_SERIAL_NUMBER, cmd );
    return ( SendListEntry( CmdMarker( SPX42CommandDef::SPX_SERIAL_NUMBER ), cmd ) );
  }

  SendListEntry SPX42Commands::aksForAliveSignal()
  {
    QByteArray cmd;
    makeSipleCommand( SPX42CommandDef::SPX_ALIVE, cmd );
    return ( SendListEntry( CmdMarker( SPX42CommandDef::SPX_ALIVE ), cmd ) );
  }

  SendListEntry SPX42Commands::askForFirmwareVersion()
  {
    QByteArray cmd;
    makeSipleCommand( SPX42CommandDef::SPX_APPLICATION_ID, cmd );
    return ( SendListEntry( CmdMarker( SPX42CommandDef::SPX_APPLICATION_ID ), cmd ) );
  }

  SendListEntry SPX42Commands::askForLicenseState()
  {
    QByteArray cmd;
    makeSipleCommand( SPX42CommandDef::SPX_LICENSE_STATE, cmd );
    return ( SendListEntry( CmdMarker( SPX42CommandDef::SPX_LICENSE_STATE ), cmd ) );
  }

  SendListEntry SPX42Commands::askForConfig()
  {
    QByteArray code;
    QByteArray cmd;
    // Prefix
    cmd.append( &SPX42CommandDef::STX, 1 );
    cmd.append( "~" );
    //
    code.append( &SPX42CommandDef::SPX_GET_SETUP_DEKO, 1 );
    cmd.append( code.toHex() );
    //
    cmd.append( "~" );
    code.clear();
    code.append( &SPX42CommandDef::SPX_GET_SETUP_SETPOINT, 1 );
    cmd.append( code.toHex() );
    //
    cmd.append( "~" );
    code.clear();
    code.append( &SPX42CommandDef::SPX_GET_SETUP_DISPLAYSETTINGS, 1 );
    cmd.append( code.toHex() );
    //
    cmd.append( "~" );
    code.clear();
    code.append( &SPX42CommandDef::SPX_GET_SETUP_UNITS, 1 );
    cmd.append( code.toHex() );
    //
    cmd.append( "~" );
    code.clear();
    code.append( &SPX42CommandDef::SPX_GET_SETUP_INDIVIDUAL, 1 );
    cmd.append( code.toHex() );
    //
    cmd.append( "~" );
    code.clear();
    code.append( &SPX42CommandDef::SPX_LICENSE_STATE, 1 );
    cmd.append( code.toHex() );
    //
    cmd.append( "~" );
    code.clear();
    code.append( &SPX42CommandDef::SPX_ALIVE, 1 );
    cmd.append( code.toHex() );
    // Postfix
    cmd.append( &SPX42CommandDef::ETX, 1 );
    return ( SendListEntry( CmdMarker( SPX42CommandDef::SPX_GET_SETUP_DEKO ), cmd ) );
  }

  SendListEntry SPX42Commands::askForGasList()
  {
    QByteArray cmd;
    makeSipleCommand( SPX42CommandDef::SPX_GET_SETUP_GASLIST, cmd );
    return ( SendListEntry( CmdMarker( SPX42CommandDef::SPX_GET_SETUP_GASLIST ), cmd ) );
  }

  SendListEntry SPX42Commands::setDateTime( const QDateTime &nowDateTime )
  {
    //
    // kann der SPX das überhaupt? Das klärt der Caller!
    //
    QByteArray code;
    QByteArray cmd;
    cmd.append( &SPX42CommandDef::STX, 1 );
    cmd.append( "~" );
    code.append( &SPX42CommandDef::SPX_DATETIME, 1 );
    cmd.append( code.toHex() );
    cmd.append( nowDateTime.toString( ":hh:mm:dd:MM:yy" ).toLatin1() );
    cmd.append( &SPX42CommandDef::ETX, 1 );
    // erwarte Quittung! (true)
    return ( SendListEntry( CmdMarker( SPX42CommandDef::SPX_DATETIME, true ), cmd ) );
  }

  /*
  SendListEntry SPX42Commands::sendStartYModem()
  {
    makeSipleCommand( SPX42CommandDef::SPX_START_YMODEM );
    return ( cmd );
  }


  SendListEntry &SPX42Commands::sendStartProgramming()
  {
    makeSipleCommand( SPX42CommandDef::SPX_START_FW_PROGRAMM );
    return ( cmd );
  }
  */

  SendListEntry SPX42Commands::shutdownSPX42()
  {
    QByteArray cmd;
    makeSipleCommand( SPX42CommandDef::SPX_SHUTDOWN, cmd );
    return ( SendListEntry( CmdMarker( SPX42CommandDef::SPX_SHUTDOWN ), cmd ) );
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

  SendListEntry SPX42Commands::sendDecoParams( SPX42Config &cfg )
  {
    QByteArray code;
    QString cmdStr;
    QByteArray cmd;
    // notwendig weil alter Firmware einen Fehler beim high gradient hat :-(
    DecoGradient gradient = cfg.getCurrentDecoGradientValue();
    cmd.append( &SPX42CommandDef::STX, 1 );
    cmd.append( "~" );
    code.append( &SPX42CommandDef::SPX_SET_SETUP_DEKO, 1 );
    cmd.append( code.toHex() );
    if ( cfg.getIsOldParamSorting() )
    {
      // ~29:GH:GL:LS:DY:DS
      // GH = Gradient HIGH
      // GL = Gradient LOW
      // LS = Last Stop 0=>6m 1=>3m
      // DY = Dynamische gradienten 0->off 1->on
      // DS = Deepstops 0=> enabled, 1=>disabled
      cmdStr = ":%1:%2:%3:%4:%5";
      // gradient.first += 1;  // Firmware bugfix workarround :-(
      // gradient.second += 1;
    }
    else
    {
      // Kommando SPX_SET_SETUP_DEKO
      // ~29:GL:GH:DS:DY:LS
      // GL=GF-Low, GH=GF-High,
      // DS=Deepstops (0/1)
      // DY=Dynamische Gradienten (0/1)
      // LS=Last Decostop (0=3 Meter/1=6 Meter)
      cmdStr = ":%2:%1:%5:%4:%3";
    }
    cmd.append( cmdStr.arg( gradient.second, 2, 16, QChar( '0' ) )
                    .arg( gradient.first, 2, 16, QChar( '0' ) )
                    .arg( ( cfg.getLastDecoStop() == DecoLastStop::LAST_STOP_ON_3 ) ? 0 : 1 )
                    .arg( ( cfg.getIsDecoDynamicGradients() == DecompressionDynamicGradient::DYNAMIC_GRADIENT_ON ) ? 1 : 0 )
                    .arg( ( cfg.getIstDeepstopsEnabled() == DecompressionDeepstops::DEEPSTOPS_ENABLED ? 1 : 0 ) )
                    .toLatin1() );
    cmd.append( &SPX42CommandDef::ETX, 1 );
    // erwarte Quittung! (true)
    return ( SendListEntry( CmdMarker( SPX42CommandDef::SPX_SET_SETUP_DEKO, true ), cmd ) );
  }

  SendListEntry SPX42Commands::sendSetpointParams( SPX42Config &cfg )
  {
    QByteArray code;
    QString cmdStr;
    QByteArray cmd;
    cmd.append( &SPX42CommandDef::STX, 1 );
    cmd.append( "~" );
    code.append( &SPX42CommandDef::SPX_SET_SETUP_SETPOINT, 1 );
    cmd.append( code.toHex() );
    if ( cfg.getIsOldParamSorting() )
    {
      //
      // Kommando SPX_SET_SETUP_SETPOINT
      // ~30:P:A
      // P = Partialdruck (0..4) 1.0 .. 1.4
      // A = Setpoint bei (0,1,2,3,4) = (0,5,15,20,25)
      cmdStr = ":%2:%1";
    }
    else
    {
      // ~30:A:P
      // A = Setpoint bei (0,1,2,3,4) = (0,5/6,15,20,25)
      // P = Partialdruck (0..4) 1.0 .. 1.4
      cmdStr = ":%1:%2";
    }
    cmd.append( cmdStr.arg( static_cast< int >( cfg.getSetpointAuto() ), 1, 16 )
                    .arg( static_cast< int >( cfg.getSetpointValue() ), 1, 16 )
                    .toLatin1() );
    cmd.append( &SPX42CommandDef::ETX, 1 );
    // erwarte Quittung! (true)
    return ( SendListEntry( CmdMarker( SPX42CommandDef::SPX_SET_SETUP_SETPOINT, true ), cmd ) );
  }

  SendListEntry SPX42Commands::sendDisplayParams( SPX42Config &cfg )
  {
    QByteArray code;
    QByteArray cmd;
    cmd.append( &SPX42CommandDef::STX, 1 );
    cmd.append( "~" );
    code.append( &SPX42CommandDef::SPX_SET_SETUP_DISPLAYSETTINGS, 1 );
    cmd.append( code.toHex() );
    // ~31:D:A
    // Alte Settings D= 0->10&, 1->50%, 2->100%
    // Neuere Settings 0->20%, 1->40%, 2->60%, 3->80%, 4->100%
    // A= 0->Landscape 1->180Grad
    // Display setzen
    cmd.append( QString( ":%1:%2" )
                    .arg( static_cast< int >( cfg.getDisplayBrightness() ), 1, 16 )
                    .arg( static_cast< int >( cfg.getDisplayOrientation() ), 1, 16 )
                    .toLatin1() );
    cmd.append( &SPX42CommandDef::ETX, 1 );
    // erwarte Quittung! (true)
    return ( SendListEntry( CmdMarker( SPX42CommandDef::SPX_SET_SETUP_DISPLAYSETTINGS, true ), cmd ) );
  }

  SendListEntry SPX42Commands::sendUnitsParams( SPX42Config &cfg )
  {
    QByteArray code;
    QByteArray cmd;
    cmd.append( &SPX42CommandDef::STX, 1 );
    cmd.append( "~" );
    code.append( &SPX42CommandDef::SPX_SET_SETUP_UNITS, 1 );
    cmd.append( code.toHex() );
    // ~32:UD:UL:UW
    // UD= Fahrenheit/Celsius => immer 0 in der aktuellen Firmware 2.6.7.7_U
    // UL= 0=>metrisch 1=>imperial
    // UW= 0->Salzwasser 1->Süßwasser
    cmd.append( QString( ":%1:%2:%3" )
                    .arg( static_cast< int >( cfg.getUnitsTemperatur() ), 1, 16 )
                    .arg( static_cast< int >( cfg.getUnitsLength() ), 1, 16 )
                    .arg( static_cast< int >( cfg.getUnitsWaterType() ), 1, 16 )
                    .toLatin1() );
    cmd.append( &SPX42CommandDef::ETX, 1 );
    // erwarte Quittung! (true)
    return ( SendListEntry( CmdMarker( SPX42CommandDef::SPX_SET_SETUP_UNITS, true ), cmd ) );
  }

  SendListEntry SPX42Commands::sendCustomParams( SPX42Config &cfg )
  {
    QByteArray code;
    QByteArray cmd;
    cmd.append( &SPX42CommandDef::STX, 1 );
    cmd.append( "~" );
    code.append( &SPX42CommandDef::SPX_SET_SETUP_INDIVIDUAL, 1 );
    cmd.append( code.toHex() );
    // Kommando SPX_SET_SETUP_INDIVIDUAL
    // ~33:SM:PS:SC:AC:LT:TS
    // SM = 0-> Sensoren ON, 1-> No Sensor
    // PS = PSCR Mode 0->off; 1->ON (sollte eigentlich immer off (0 ) sein)
    // SC = SensorsCount 0->1 Sensor, 1->2 sensoren, 2->3 Sensoren
    // AC = acoustic 0->off, 1->on
    // LT = Logbook Timeinterval 0->10s, 1->30s, 2->60s
    // Ab Version 2.7_H_R_83ce
    // TS : TempStick == 0
    if ( cfg.getHasSixValuesIndividual() )
    {
      //
      // ab version 2.7_H_R_83ce sechs parameter (TEMPSTICK)
      //
      cmd.append( QString( ":%1:%2:%3:%4:%5:%6" )
                      .arg( static_cast< int >( cfg.getIndividualSensorsOn() ), 1, 16 )
                      .arg( static_cast< int >( cfg.getIndividualPscrMode() ), 1, 16 )
                      .arg( static_cast< int >( cfg.getIndividualSensorsCount() ), 1, 16 )
                      .arg( static_cast< int >( cfg.getIndividualAcoustic() ), 1, 16 )
                      .arg( static_cast< int >( cfg.getIndividualLogInterval() ), 1, 16 )
                      .arg( static_cast< int >( cfg.getIndividualTempStick() ), 1, 16 )
                      .toLatin1() );
    }
    else
    {
      //
      // fünf Parameter
      //
      cmd.append( QString( ":%1:%2:%3:%4:%5" )
                      .arg( static_cast< int >( cfg.getIndividualSensorsOn() ), 1, 16 )
                      .arg( static_cast< int >( cfg.getIndividualPscrMode() ), 1, 16 )
                      .arg( static_cast< int >( cfg.getIndividualSensorsCount() ), 1, 16 )
                      .arg( static_cast< int >( cfg.getIndividualAcoustic() ), 1, 16 )
                      .arg( static_cast< int >( cfg.getIndividualLogInterval() ), 1, 16 )
                      .toLatin1() );
    }

    cmd.append( &SPX42CommandDef::ETX, 1 );
    // erwarte Quittung! (true)
    return ( SendListEntry( CmdMarker( SPX42CommandDef::SPX_SET_SETUP_INDIVIDUAL, true ), cmd ) );
  }

  SendListEntry SPX42Commands::sendGas( int gasNum, SPX42Config &cfg )
  {
    QByteArray code;
    QByteArray cmd;
    SPX42Gas cGas = cfg.getGasAt( gasNum );
    cmd.append( &SPX42CommandDef::STX, 1 );
    cmd.append( "~" );
    code.append( &SPX42CommandDef::SPX_SET_SETUP_GASLIST, 1 );
    cmd.append( code.toHex() );
    // ############ Alte Parameter Reihenfolge
    // Kommando SPX_SET_SETUP_GASLIST
    // ~40:NR:HE:N2:BO:DI:CU
    // NR -> Gas Nummer
    // HE -> Heliumanteil
    // N2 -> Stickstoffanteil
    // BO -> Bailoutgas? (3?)
    // DI -> Diluent ( 0, 1 oder 2 )
    // CU Current Gas (0 oder 1)
    // ############ NEUE Parameter Reihenfolge
    // Kommando SPX_SET_SETUP_GASLIST
    // ~40:NR:N2:HE:BO:DI:CU
    // NR: Nummer des Gases 0..7
    // N2: Sticksoff in %
    // HE: Heluim in %
    // BO: Bailout (Werte 0,1 und 3 gefunden, 0 kein BO, 3 BO Wert 1 unbekannt?)
    // DI: Diluent 1 oder 2
    // CU: Current Gas
    //
    if ( cfg.getIsOldParamSorting() )
    {
      //
      // Alte Parameterreihenfolge
      //
      cmd.append( QString( ":%1:%3:%2:%4:%5:%6" )
                      .arg( static_cast< int >( gasNum ), 1, 16 )
                      .arg( static_cast< int >( cGas.getN2() ), 1, 16 )
                      .arg( static_cast< int >( cGas.getHe() ), 1, 16 )
                      .arg( cGas.getBailout() ? "1" : "0" )
                      .arg( static_cast< int >( cGas.getDiluentType() ), 1, 16 )
                      .arg( 0 ) );
    }
    else
    {
      //
      // Neue Parameter reiehenfolge
      //
      cmd.append( QString( ":%1:%2:%3:%4:%5:%6" )
                      .arg( static_cast< int >( gasNum ), 1, 16 )
                      .arg( static_cast< int >( cGas.getN2() ), 1, 16 )
                      .arg( static_cast< int >( cGas.getHe() ), 1, 16 )
                      .arg( cGas.getBailout() ? "1" : "0" )
                      .arg( static_cast< int >( cGas.getDiluentType() ), 1, 16 )
                      .arg( 0 ) );
    }
    cmd.append( &SPX42CommandDef::ETX, 1 );
    // erwarte Quittung! (true)
    return ( SendListEntry( CmdMarker( SPX42CommandDef::SPX_SET_SETUP_GASLIST, true ), cmd ) );
  }
}
