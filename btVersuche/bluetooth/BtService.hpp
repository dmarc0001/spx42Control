#ifndef BTSERVICES_HPP
#define BTSERVICES_HPP

#include <QBluetoothAddress>
#include <QBluetoothLocalDevice>
#include <QBluetoothServiceDiscoveryAgent>
#include <QBluetoothServiceInfo>
#include <QObject>
#include <logging/Logger.hpp>

class BtService : public QObject
{
  Q_OBJECT

  private:
  std::shared_ptr< Logger > lg;

  public:
  explicit BtService( std::shared_ptr< Logger > logger, const QBluetoothAddress &address, QObject *parent = nullptr );
  ~BtService();

  signals:
  void sigServiceDiscoverStarted( void );
  void sigCanceled( void );
  void sigError( QBluetoothServiceDiscoveryAgent::Error error );
  void sigServiceDiscovered( const QBluetoothServiceInfo &info );
  void sigServiceDiscoverFinished( void );

  private:
  QBluetoothServiceDiscoveryAgent *discoveryAgent;

  private slots:
  void started( void );
  void canceled( void );
  void discoverError( QBluetoothServiceDiscoveryAgent::Error error );
  void addService( const QBluetoothServiceInfo &info );
  void discoverFinished( void );
};

#endif  // BTSERVICES_HPP
