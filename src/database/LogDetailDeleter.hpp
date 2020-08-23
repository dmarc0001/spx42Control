#ifndef LOGDETAILDELETER_HPP
#define LOGDETAILDELETER_HPP

#include <QObject>
#include <QThread>
#include <memory>
#include "database/SPX42Database.hpp"
#include "spx42/SPX42Config.hpp"

namespace spx
{
  class LogDetailDeleter : public QThread
  {
    Q_OBJECT
    std::shared_ptr< Logger > lg;                //! der Logger
    std::shared_ptr< SPX42Database > database;   //! Datenbankverbindung
    std::shared_ptr< SPX42Config > spx42Config;  //! die Konfiguration des SQX
    QString deviceMac;                           //! das genutzte Gerät
    std::shared_ptr< QVector< int > > delList;   //! Liste der hzu löschenden Tauchgänge

    public:
    explicit LogDetailDeleter( QObject *parent,
                               std::shared_ptr< Logger > logger,
                               std::shared_ptr< SPX42Database > _database,
                               std::shared_ptr< SPX42Config > spxCfg,
                               const QString &devMac,
                               std::shared_ptr< QVector< int > > list );
    void run() override;  //! die Thread Hauptschleife

    signals:
    void onDeleteDoneSig( int diveNum );
    void onDeleteCritSig();
  };
}  // namespace spx
#endif  // LOGDETAILDELETER_HPP
