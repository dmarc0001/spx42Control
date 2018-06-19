#ifndef MAINDAILOG_HPP
#define MAINDAILOG_HPP

#include <QBluetoothDeviceDiscoveryAgent>
#include <QBluetoothLocalDevice>
#include <QDialog>
#include <QMenu>

#include <memory>
#include "ServiceDiscoveryDialog.hpp"
#include "bluetooth/BtLocalDevicesManager.hpp"
#include "logging/Logger.hpp"
#include "ui_MainDialog.h"

using namespace spx;

namespace Ui
{
  class MainDialog;
}

class MainDialog : public QDialog
{
  Q_OBJECT

  private:
  std::shared_ptr< Logger > lg;
  std::unique_ptr< BtLocalDevicesManager > btDevices;
  std::unique_ptr< Ui::MainDialog > ui;

  public:
  explicit MainDialog( QWidget *parent = nullptr );
  ~MainDialog();

  protected:
  void changeEvent( QEvent *e );

  public slots:
  void slotDiscoveredDevice( const QBluetoothDeviceInfo & );
  void slotGuiPowerClicked( bool clicked );
  void slotGuiDiscoverableClicked( bool clicked );
  void slotGuiDisplayPairingMenu( const QPoint &pos );
  void slotDevicePairingDone( const QBluetoothAddress &, QBluetoothLocalDevice::Pairing );
  private slots:
  void slotGuiStartScan();
  void slotDiscoverScanFinished();
  void slotGuiSetGeneralUnlimited( bool unlimited );
  void slotGuiItemActivated( QListWidgetItem *item );
  void slotDeviceHostModeStateChanged( QBluetoothLocalDevice::HostMode );
};

#endif  // MAINDAILOG_HPP
