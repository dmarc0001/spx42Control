﻿#ifndef SPX42LOGDIRECTORYENTRY_HPP
#define SPX42LOGDIRECTORYENTRY_HPP

#include <QByteArray>
#include <QDateTime>
#include <QList>
#include <QString>
#include <memory>

namespace spx
{
  class SPX42LogDirectoryEntry;  // Vorwärtsdeklaration
  using SPX42LogDirectoryEntryList = QHash< int, SPX42LogDirectoryEntry >;
  using SPX42LogDirectoryEntryListPtr = std::shared_ptr< SPX42LogDirectoryEntryList >;

  //
  // Beispiel für Log directory entry
  // zahlen in HEX
  //<~41:0:22_4_12_10_42_24.txt:19>
  //<~41:1:22_4_10_22_55_7.txt:19>
  // ...
  //<~41:19:31_7_11_14_13_11.txt:19>
  //
  // verwandle die Dateiangabe in eine lesbare Datumsangabe
  // Format des Strings ist ja (DEZIMAL!)
  // TAG_MONAT_JAHR_STUNDE_MINUTE_SEKUNDE
  // des Beginns der Aufzeichnung
  //
  /*
  day = Integer.parseInt( fields[ 0 ] );
  month = Integer.parseInt( fields[ 1 ] );
  year = Integer.parseInt( fields[ 2 ] ) + 2000;
  hour = Integer.parseInt( fields[ 3 ] );
  minute = Integer.parseInt( fields[ 4 ] );
  second = Integer.parseInt( fields[ 5 ] );
  */
  class SPX42LogDirectoryEntry
  {
    public:
    //! aktuelle Nummer
    int number;
    //! Dateiname auf dem SPX42
    QString fileName;
    //! Tauchzeitpunkt
    QDateTime diveDateTime;
    //! Repräsenttion als String
    QString dateTimeString;
    //! anzahl Tuchgänge, redundant, aber sicher ist sicher
    int maxNumber;
    //! eindeutige Kennung eines Tauchganges. Nummer und zeit/datum sollte einmalig sein
    QString diveId;
    //! flag ob das Ding in der DB ist
    bool inDatabase;
    //
    SPX42LogDirectoryEntry( int num, int max, const QByteArray &cmd, bool _inDatabase = false );
    SPX42LogDirectoryEntry( int num, int max, const QDateTime &dt, bool _inDatabase = false );
    SPX42LogDirectoryEntry();
    QString &getDateTimeStr( void );
    QString &getDiveId( void );
  };
}  // namespace spx
#endif  // SPX42LOGDIRECTORYENTRY_HPP
