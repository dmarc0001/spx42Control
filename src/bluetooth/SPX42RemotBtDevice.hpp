#ifndef SPX42REMOTBTDEVICE_HPP
#define SPX42REMOTBTDEVICE_HPP

#include <QBluetoothSocket>
#include <QByteArray>
#include <memory>
#include "config/ProjectConst.hpp"
#include "logging/Logger.hpp"

namespace spx
{
  class SPX42RemotBtDevice : public QObject
  {
    Q_OBJECT
    public:
    enum SPX42ConnectStatus : qint8
    {
      SPX42_DISCONNECTED,
      SPX42_CONNECTING,
      SPX42_CONNECTED,
      SPX42_DISCONNECTING,
      SPX42_ERROR
    };

    private:
    std::shared_ptr< Logger > lg;                //! Zeiger auf Loggerobjekt
    std::unique_ptr< QBluetoothSocket > socket;  //! Zeiger auf einen Socket
    QBluetoothServiceInfo remoteService;         //! Zeiger auf den zu verbindenen Service

    public:
    explicit SPX42RemotBtDevice( std::shared_ptr< Logger > logger, QObject *parent = nullptr );
    ~SPX42RemotBtDevice();
    void startConnection( const QBluetoothServiceInfo &remService );
    void startConnection( void );
    void endConnection( void );
    SPX42ConnectStatus getConnectionStatus( void );

    signals:
    void onConnectSig( void );
    void onDisconnectSig( void );
    void onDatagramSig( const QByteArray &datagram );
    void onLogentrySig( const QByteArray &datagram );

    public slots:

    private slots:
    void onReadSocketSlot( void );
    void onConnectSlot( void );
    void onDisconnectSlot( void );
  };
}
#endif  // SPX42REMOTBTDEVICE_HPP
