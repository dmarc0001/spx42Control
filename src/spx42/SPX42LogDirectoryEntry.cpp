#include "SPX42LogDirectoryEntry.hpp"

#include <QDate>
#include <QTime>
#include <QTimeZone>

namespace spx
{
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

<<<<<<< HEAD
  SPX42LogDirectoryEntry::SPX42LogDirectoryEntry() : number( -1 ), fileName(), diveDateTime(), dateTimeString(), maxNumber( -1 )
  {
  }

  SPX42LogDirectoryEntry::SPX42LogDirectoryEntry( int num, int max, const QByteArray &cmd )
      : number( num ), fileName( cmd ), maxNumber( max )
=======
  SPX42LogDirectoryEntry::SPX42LogDirectoryEntry()
      : number( -1 ), fileName(), diveDateTime(), dateTimeString(), maxNumber( -1 ), inDatabase( false )
  {
  }

  SPX42LogDirectoryEntry::SPX42LogDirectoryEntry( int num, int max, const QByteArray &cmd, bool _inDatabase )
      : number( num ), fileName( cmd ), maxNumber( max ), inDatabase( _inDatabase )
>>>>>>> dev/stabilizing
  {
    int idx = cmd.indexOf( ".txt" );
    QByteArray tmp = cmd.left( idx );
    QList< QByteArray > dateTimeList = tmp.split( '_' );
    QDate diveDate( dateTimeList.at( 2 ).toInt() + 2000, dateTimeList.at( 1 ).toInt(), dateTimeList.at( 0 ).toInt() );
    QTime diveTime( dateTimeList.at( 3 ).toInt(), dateTimeList.at( 4 ).toInt(), dateTimeList.at( 5 ).toInt() );
    diveDateTime = QDateTime( diveDate, diveTime, QTimeZone::systemTimeZone() );
    // TODO: Konfigurierbar via locale!
    dateTimeString = diveDateTime.toString( "yyyy/MM/dd hh:mm:ss" );
    diveId = QString( "%1-%2" ).arg( num, 4, 10, QChar( '0' ) ).arg( diveDateTime.toString( "yyyyMMddhhmmss" ) );
    // dateTimeString = diveDateTime.toString( "dd.MM.YYYY HH:mm:ss " );
  }

<<<<<<< HEAD
=======
  SPX42LogDirectoryEntry::SPX42LogDirectoryEntry( int num, int max, const QDateTime &dt, bool _inDatabase )
      : number( num ), fileName( "" ), maxNumber( max ), inDatabase( _inDatabase )
  {
    diveDateTime = dt;
    // TODO: Konfigurierbar via locale!
    dateTimeString = diveDateTime.toString( "yyyy/MM/dd hh:mm:ss" );
    diveId = QString( "%1-%2" ).arg( num, 4, 10, QChar( '0' ) ).arg( diveDateTime.toString( "yyyyMMddhhmmss" ) );
    // dateTimeString = diveDateTime.toString( "dd.MM.YYYY HH:mm:ss " );
  }

>>>>>>> dev/stabilizing
  QString &SPX42LogDirectoryEntry::getDateTimeStr()
  {
    return ( dateTimeString );
  }

  QString &SPX42LogDirectoryEntry::getDiveId()
  {
    return ( diveId );
  }
<<<<<<< HEAD
}
=======
}  // namespace spx
>>>>>>> dev/stabilizing
