#ifndef LOGDETAILWRITER_HPP
#define LOGDETAILWRITER_HPP

#include <QMutex>
#include <QMutexLocker>
#include <QObject>
#include <QQueue>
#include <QString>
#include <memory>
#include "database/SPX42Database.hpp"
#include "spx42/SPX42Config.hpp"
#include "spx42/SPX42SingleCommand.hpp"

namespace spx
{
  class LogDetailWalker : public QObject
  {
    Q_OBJECT
    //! Maximale Wartezeit beim schreiben
    static const qint64 waitTimeout{( 1000 / 50 ) * 20};
    static const qint64 waitUnits{50};
    //! Queue mit pointern auf die empfangenen Logdetails
    QQueue< spSingleCommand > detailQueue;
    //! der Logger
    std::shared_ptr< Logger > lg;
    //! Datenbankverbindung
    std::shared_ptr< SPX42Database > database;
    //! die Konfioguration
    std::shared_ptr< SPX42Config > spx42Config;
    //! Mutex zum locken der Queue
    QMutex queueMutex;
    //! thread soll laufen und auf daten warten
    bool shouldWriterRunning;
    int processed;
    int forThisDiveProcessed;
    int overAll;
    // zeit bis zum timeout
    qint64 maxTimeoutVal;
    //! Queue für Logdaten ankommend
    std::shared_ptr< QQueue< SPX42SingleCommand > > logDetailQueue;
    QString logDetailTableName;

    public:
    explicit LogDetailWalker( QObject *parent,
                              std::shared_ptr< Logger > logger,
                              std::shared_ptr< SPX42Database > _database,
                              std::shared_ptr< SPX42Config > spxCfg );
    void reset( void );
    //! nicht mehr warten wenn die queue leer ist
    void nowait( bool _shouldNoWait = true );
    void addDetail( spSingleCommand );
    int getProcessed( void );
    int getGlobal( void );
    int getQueueLen( void );
    int writeLogDataToDatabase( const QString &deviceMac );
    bool deleteLogDataFromDatabase( const QString &deviceMac, std::shared_ptr< QVector< int > > list );
    bool exportLogDataFromDatabase( const QString &deviceMac, const QString &fileName, std::shared_ptr< QVector< int > > list );

    private:
    spSingleCommand dequeueDetail( void );

    signals:
    void onNewDiveDoneSig( int savedDiveNum );
    void onWriteDoneSig( int _overallResult );
    void onNewDiveStartSig( int newDiveNum );
    void onDeleteDoneSig( int diveNum );

    public slots:
  };
}  // namespace spx
#endif  // LOGDETAILWRITER_HPP
