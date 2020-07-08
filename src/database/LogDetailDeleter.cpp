#include "LogDetailDeleter.hpp"

namespace spx
{
  LogDetailDeleter::LogDetailDeleter( QObject *parent,
                                      std::shared_ptr< Logger > logger,
                                      std::shared_ptr< SPX42Database > _database,
                                      std::shared_ptr< SPX42Config > spxCfg,
                                      const QString &devMac,
                                      std::shared_ptr< QVector< int > > list )
      : QThread( parent ), lg( logger ), database( _database ), spx42Config( spxCfg ), deviceMac( devMac ), delList( list )
  {
  }

  void LogDetailDeleter::run()
  {
    *lg << LDEBUG << "LogDetailDeleter::run -> thread started..." << Qt::endl;
    //
    // zuerst: ist die Tabelle da/wurde angelegt?
    //
    for ( int diveNum : *delList )
    {
      if ( !database->delDiveLogFromBase( deviceMac, diveNum ) )
      {
        emit onDeleteCritSig();
      }
      else
      {
        // gib Bescheid, der ist Geschichte...
        emit onDeleteDoneSig( diveNum );
      }
    }
    QThread::msleep( 1200 );
    emit onDeleteDoneSig( -1 );
    *lg << LDEBUG << "LogDetailDeleter::run -> thread finished..." << Qt::endl;
    //
    // das Ding soll sich selber zerstören
    //
    deleteLater();
  }
}  // namespace spx
