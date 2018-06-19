#ifndef SERVICEDISCOVERYDIALOG_HPP
#define SERVICEDISCOVERYDIALOG_HPP

#include <QBluetoothAddress>
#include <QBluetoothLocalDevice>
#include <QBluetoothServiceDiscoveryAgent>
#include <QDialog>
#include <memory>
#include "bluetooth/BtServiceDiscover.hpp"
#include "logging/Logger.hpp"
#include "ui_ServiceDiscoveryDialog.h"

using namespace spx;

namespace Ui
{
  class ServiceDiscoveryDialog;
}

class ServiceDiscoveryDialog : public QDialog
{
  Q_OBJECT
  private:
  std::shared_ptr< Logger > lg;
  QString &name;
  QBluetoothAddress &addr;
  std::unique_ptr< Ui::ServiceDiscoveryDialog > ui;
  std::unique_ptr< BtServiceDiscover > discoveryAgent;

  public:
  explicit ServiceDiscoveryDialog( std::shared_ptr< Logger > logger,
                                   QString &dname,
                                   QBluetoothAddress &daddr,
                                   QWidget *parent = nullptr );
  ~ServiceDiscoveryDialog();

  protected:
  void changeEvent( QEvent *e );

  signals:
  void sigDiscoveredService( const QString &name, const QBluetoothServiceInfo &info );

  private slots:
  void slotDiscoveredService( const QString &name, const QBluetoothServiceInfo &info );
};

#endif  // SERVICEDISCOVERYDIALOG_HPP
