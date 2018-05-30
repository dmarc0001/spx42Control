#ifndef SPX42COMMANDS_HPP
#define SPX42COMMANDS_HPP

#include <QList>
#include <QByteArray>

#include "../config/ProjectConst.hpp"
#include "SpxCommandDef.hpp"

namespace spx
{
  class Spx42Commands
  {
    private:
      QByteArray cmd;
      QList<QByteArray> params;
      void makeSipleCommand( char commandChar );

    public:
      Spx42Commands();
      // Kommandos an den SPX zusammenstellen
      QByteArray& sendManufacturers(void);                 //! Hersteller anfragen 0x01
      QByteArray& sendSerialNumber(void);                  //! Noch nicht ganz klar, was das ist (Daniel fragen) 0x02
      QByteArray& sendAliveSignal(void);                   //! Sende ein Lebenszeichen 0x03
      QByteArray& sendFirmwareVersion(void);               //! Firmwareversion erfragen 0x04
      QByteArray& sendStartYModem(void);                   //! Schalte SPX auf YMODEM-Betrieb!
      QByteArray& sendStartProgramming(void);              //! sag dem SPX42, er soll die Firmware flashen
      QByteArray& shutdownSPX42(void);                     //! fahre den SPX42 runter
      // empfangene Daten auf Kommandos hin untersuchen
      char decodeCommand( const QByteArray& pdu );         //! erkenne die PDU, lagere Parameter hier im Objekt
      QByteArray getParameter( int index );                //! gib Parameter mit Index index zurück, 0 == Command
  };
}
#endif // SPX42COMMANDS_HPP
