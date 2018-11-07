#ifndef SPX42REMOTBTDEVICE_HPP
#define SPX42REMOTBTDEVICE_HPP

#include <QBluetoothSocket>
#include <memory>
#include "config/ProjectConst.hpp"
#include "logging/Logger.hpp"

namespace spx
{
  class SPX42RemotBtDevice : public QObject
  {
    Q_OBJECT
    private:
    std::shared_ptr< Logger > lg;                //! Zeiger auf Loggerobjekt
    std::unique_ptr< QBluetoothSocket > socket;  //! Zeiger auf einen Socket
    QBluetoothServiceInfo remoteService;         //! Zeiger auf den zu verbindenen Service

    public:
    explicit SPX42RemotBtDevice( std::shared_ptr< Logger > logger, QObject *parent = nullptr );
    ~SPX42RemotBtDevice();
    void startConnection( const QBluetoothServiceInfo &remService );

    signals:

    public slots:
  };
}
#endif  // SPX42REMOTBTDEVICE_HPP
