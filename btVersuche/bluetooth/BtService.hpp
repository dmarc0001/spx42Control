#ifndef BTSERVICES_HPP
#define BTSERVICES_HPP

#include <QBluetoothAddress>
#include <QBluetoothLocalDevice>
#include <QBluetoothServiceDiscoveryAgent>
#include <QBluetoothServiceInfo>
#include <QObject>
#include <QTimer>
#include <logging/Logger.hpp>

QT_USE_NAMESPACE

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
  void slotDiscoverCanceled( void );
  void slotDiscoverError( QBluetoothServiceDiscoveryAgent::Error error );
  void slotDiscoveredService( const QBluetoothServiceInfo &info );
  void slotDiscoverFinished( void );
};

#endif  // BTSERVICES_HPP
