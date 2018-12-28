#ifndef LOGDETAILWRITER_HPP
#define LOGDETAILWRITER_HPP

#include <QObject>
#include <QQueue>
#include <QString>
#include <memory>
#include "database/SPX42Database.hpp"
#include "spx42/SPX42SingleCommand.hpp"

namespace spx
{
  class LogDetailWalker : public QObject
  {
    Q_OBJECT
    static const qint64 waitTimeout{( 1000 / 50 ) * 20};
    static const qint64 waitUnits{50};
    QQueue< spSingleCommand > detailQueue;      // Liste mit pointern auf die empfangenen Logdetails
    std::shared_ptr< Logger > lg;               // der Logger
    std::shared_ptr< SPX42Database > database;  // Datenbankverbindung
    bool shouldWriterRunning;                   // thread soll laufen und auf daten warten
    int processed;
    int forThisDiveProcessed;
    int overAll;
    qint64 maxTimeoutVal;  // zeit bis zum timeout

    std::shared_ptr< QQueue< SPX42SingleCommand > > logDetailQueue;  // Queue für Logdaten ankommend
    QString logDetailTableName;

    public:
    explicit LogDetailWalker( QObject *parent, std::shared_ptr< Logger > logger, std::shared_ptr< SPX42Database > _database );
    void reset( void );
    void nowait( bool _shouldNoWait = true );  //! nicht mehr warten wenn die queue leer ist
    void addDetail( spSingleCommand );
    int getProcessed( void );
    int getGlobal( void );
    int getQueueLen( void );
    int writeLogDataToDatabase( const QString &deviceMac );
    bool deleteLogDataFromDatabase( const QString &deviceMac, std::shared_ptr< QVector< int > > list );
    bool exportLogDataFromDatabase( const QString &deviceMac, const QString &fileName, std::shared_ptr< QVector< int > > list );

    signals:
    void onNewDiveDoneSig( int savedDiveNum );
    void onWriteDoneSig( int _overallResult );
    void onNewDiveStartSig( int newDiveNum );
    void onDeleteDoneSig( int diveNum );

    public slots:
  };
}
#endif  // LOGDETAILWRITER_HPP
