#ifndef LOGDETAILWRITER_HPP
#define LOGDETAILWRITER_HPP

#include <QMutex>
#include <QMutexLocker>
#include <QObject>
#include <QQueue>
#include <QString>
#include <QWaitCondition>
#include <memory>
#include "database/SPX42Database.hpp"
#include "spx42/SPX42Config.hpp"
#include "spx42/SPX42SingleCommand.hpp"

namespace spx
{
  using LogDetailSetQueue = QQueue< spSingleCommand >;  //! Queue an details für einen Tauchgang
  using LogDetailsQueue = QQueue< LogDetailSetQueue >;  //! Queue für LogDertailSets

  enum LOGWRITEERR : int
  {
    LOGWR_ERR_CANT_DELETE_DIVE,
    LOGWR_ERR_CANT_INSERT_DIVE,
    LOGWR_ERR_CANT_READ_NEW_DIVEID,
    LOGWR_ERR_CANT_INSERT_LOGDETAIL,
    LOGWR_ERR_xx
  };

  class LogDetailWalker : public QThread
  {
    Q_OBJECT
    private:
    std::shared_ptr< Logger > lg;                //! der Logger
    std::shared_ptr< SPX42Database > database;   //! Datenbankverbindung
    std::shared_ptr< SPX42Config > spx42Config;  //! die Konfiguration des SQX
    QString deviceMac;                           //! das genutzte Gerät
    bool threadShouldRun;                        //! thread soll laufen und auf daten warten
    LogDetailsQueue logDetailQueue;              //! Queue mit Sets von Datenlogs
    QMutex queueMutex;                           //! Mutex zum locken der Queue
    QMutex conditionMutex;                       //! Mutex zum locken der Queue
    QWaitCondition queueCondiotion;              //! wenn die Queue leeer ist, warte auf Inhalt
    int processed;                               //! anzahl bearbeiteter Datensätze
    bool isSleeping{false};                      //! ist der Thread am schlummern

    public:
    explicit LogDetailWalker( QObject *parent,
                              std::shared_ptr< Logger > logger,
                              std::shared_ptr< SPX42Database > _database,
                              std::shared_ptr< SPX42Config > spxCfg,
                              const QString &devMac );
    void run() override;                          //! die Thread Hauptschleife
    bool setThreadEnd( bool _should );            //! zeige an ob er Thread enden soll
    void setDeviceName( const QString &devMac );  //! wenn sich das ändert, möglichkeit zum setzen
    int addLogQueue( LogDetailSetQueue logSet );  //! füge einen Tauchgang hinzu
    bool isThreadSleeping();                      //! ist der Thread am schlummern

    private:
    bool writeOneDataset( LogDetailSetQueue divelogSet, int diveNum, int dive_id );

    signals:
    void onWriteDiveStartSig( int diveNum );   //! Starte sicheren eines Tauchganges
    void onWriteDiveDoneSig( int countSets );  //! Tauchgang sichern fertig
    void onWriteFinishedSig( int count );      //! Alle Tauchgänge in der Queue fertig
    void onWriteCritSig( LOGWRITEERR err );    //! Kritischer Fehler

    public slots:
  };
}  // namespace spx
#endif  // LOGDETAILWRITER_HPP
