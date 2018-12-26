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
  class LogDetailWriter : public QObject
  {
    Q_OBJECT
    static const qint64 waitTimeout{( 1000 / 50 ) * 2};
    static const qint64 waitFirstTimeout{( 1000 / 50 ) * 10};
    static const qint64 waitUnits{50};
    QQueue< spSingleCommand > detailQueue;      // Liste mit pointern auf die empfangenen Logdetails
    std::shared_ptr< Logger > lg;               // der Logger
    std::shared_ptr< SPX42Database > database;  // Datenbankverbindung
    bool shouldRunning;                         // thread soll laufen und auf daten warten
    int processed;
    int overAll;

    std::shared_ptr< QQueue< SPX42SingleCommand > > logDetailQueue;  // Queue für Logdaten ankommend
    QString logDetailTableName;

    public:
    explicit LogDetailWriter( QObject *parent, std::shared_ptr< Logger > logger, std::shared_ptr< SPX42Database > _database );
    void reset( void );
    void clear();
    void addDetail( spSingleCommand );
    int getProcessed( void );
    int getGlobal( void );
    int getQueueLen( void );
    int writeLogDataToDatabase( const QString &deviceMac, int diveNum );

    signals:
    void onWriteDoneSig( int );

    public slots:
  };
}
#endif  // LOGDETAILWRITER_HPP
