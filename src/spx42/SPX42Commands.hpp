#ifndef SPX42COMMANDS_HPP
#define SPX42COMMANDS_HPP

#include <QByteArray>
#include <QDateTime>
#include <QList>
#include "SPX42CommandDef.hpp"
#include "config/ProjectConst.hpp"

namespace spx
{
  class SPX42Commands
  {
    private:
    QByteArray cmd;
    void makeSipleCommand( char commandChar );

    protected:
    QList< QByteArray > params;

    public:
    SPX42Commands() = default;
    // Kommandos an den SPX zusammenstellen
    QByteArray &askForManufacturers( void );                  //! Hersteller anfragen 0x01
    QByteArray &askForSerialNumber( void );                   //! Noch nicht ganz klar, was das ist (Daniel fragen) 0x02
    QByteArray &aksForAliveSignal( void );                    //! Sende ein Lebenszeichen 0x03
    QByteArray &askForFirmwareVersion( void );                //! Firmwareversion erfragen 0x04
    QByteArray &askForLicenseState( void );                   //! erfrage Lizenzstatus 0x46
    QByteArray &askForConfig( void );                         //! erfrage die Konfiguration des SPX42
    QByteArray &setDateTime( const QDateTime &nowDateTime );  //! setzte im SPX DAtum und Zeit, sofern seine Firmware das kann
    QByteArray &sendStartYModem( void );                      //! Schalte SPX auf YMODEM-Betrieb!
    QByteArray &sendStartProgramming( void );                 //! sag dem SPX42, er soll die Firmware flashen
    QByteArray &shutdownSPX42( void );                        //! fahre den SPX42 runter
    // empfangene Daten auf Kommandos hin untersuchen
    char decodeCommand( const QByteArray &pdu );  //! erkenne die PDU, lagere Parameter hier im Objekt
    QByteArray getParameter( int index );         //! gib Parameter mit Index index zurück, 0 == Command
  };
}
#endif  // SPX42COMMANDS_HPP
