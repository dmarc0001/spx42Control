#ifndef SPX42COMMANDS_HPP
#define SPX42COMMANDS_HPP

#include <QByteArray>
#include <QDateTime>
#include <QList>
#include "SPX42CommandDef.hpp"
#include "SPX42Config.hpp"
#include "config/ProjectConst.hpp"

namespace spx
{
  class CmdMarker
  {
    public:
    char cmd;
    bool waitFor;
    bool wasSend;
    QTime sendTime;
    explicit CmdMarker( char _cmd, bool _waitFor = false, bool _wasSend = false );
  };

  // mach das lesbar im Code
  using SendListEntry = QPair< CmdMarker, QByteArray >;

  class SPX42Commands
  {
    private:
    // CmdMarker marker;
    void makeSipleCommand( char commandChar, QByteArray &cmd );

    protected:
    QList< QByteArray > params;

    public:
    SPX42Commands() = default;
    // Kommandos an den SPX zusammenstellen

    //! Hersteller anfragen 0x01
    SendListEntry askForManufacturers( void );
    //! Noch nicht ganz klar, was das ist (Daniel fragen) 0x02
    SendListEntry askForSerialNumber( void );
    //! Sende ein Lebenszeichen 0x03
    SendListEntry aksForAliveSignal( void );
    //! Firmwareversion erfragen 0x04
    SendListEntry askForFirmwareVersion( void );
    //! erfrage Lizenzstatus 0x46
    SendListEntry askForLicenseState( void );
    //! erfrage die Konfiguration des SPX42
    SendListEntry askForConfig( void );
    //! frage alle Gaase ab
    SendListEntry askForGasList( void );
    //! Erster Kontakt, einstellungen abfragen
    // Frage nach dem Hersteller
    // gleich danach Frage nach der Seriennummer
    // und dann noch Frage nach der Firmwareversion
    // und lizenz nicht vergessen
    SendListEntry askWhileStartup( void );
    //! setzte im SPX Datum und Zeit, sofern seine Firmware das kann
    SendListEntry setDateTime( const QDateTime &nowDateTime );
    // Schalte SPX auf YMODEM-Betrieb!
    // SendListEntry sendStartYModem( void );
    // sag dem SPX42, er soll die Firmware flashen
    // SendListEntry sendStartProgramming( void );
    //! fahre den SPX42 runter
    SendListEntry shutdownSPX42( void );
    //! erkenne die PDU, lagere Parameter hier im Objekt
    char decodeCommand( const QByteArray &pdu );
    //! gib Parameter mit Index index zurück, 0 == Command
    QByteArray getParameter( int index );
    //! send command zum deco setzten
    SendListEntry sendDecoParams( SPX42Config &cfg );
    //! sende Kommando zu setzten der Setpoints
    SendListEntry sendSetpointParams( SPX42Config &cfg );
    //! sende Kommando zu setzten der Displayeinstellungen
    SendListEntry sendDisplayParams( SPX42Config &cfg );
    //! sende Einheiten Einstellungen zum SPX
    SendListEntry sendUnitsParams( SPX42Config &cfg );
    //! sende individual Einstellungen zum SPX
    SendListEntry sendCustomParams( SPX42Config &cfg );
    //! sende gas an den SPX42
    SendListEntry sendGas( int gasNum, SPX42Config &cfg );
  };
}
#endif  // SPX42COMMANDS_HPP
