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

  SPX42LogDirectoryEntry::SPX42LogDirectoryEntry() : number( -1 ), fileName(), diveDateTime(), dateTimeString(), maxNumber( -1 )
  {
  }

  SPX42LogDirectoryEntry::SPX42LogDirectoryEntry( int num, int max, const QByteArray &cmd )
      : number( num ), fileName( cmd ), maxNumber( max )
  {
    int idx = cmd.indexOf( ".txt" );
    QByteArray tmp = cmd.left( idx );
    QList< QByteArray > dateTimeList = tmp.split( '_' );
    QDate diveDate( dateTimeList.at( 2 ).toInt() + 2000, dateTimeList.at( 1 ).toInt(), dateTimeList.at( 0 ).toInt() );
    QTime diveTime( dateTimeList.at( 3 ).toInt(), dateTimeList.at( 4 ).toInt(), dateTimeList.at( 5 ).toInt() );
    diveDateTime = QDateTime( diveDate, diveTime, QTimeZone::systemTimeZone() );
    // TODO: Konfigurierbar via locale!
    dateTimeString = diveDateTime.toString( "YYYY/MM/dd hh:mm:ss" );
    // dateTimeString = diveDateTime.toString( "dd.MM.YYYY HH:mm:ss " );
  }

  QString &SPX42LogDirectoryEntry::getDateTimeStr( void )
  {
    return ( dateTimeString );
  }
}
