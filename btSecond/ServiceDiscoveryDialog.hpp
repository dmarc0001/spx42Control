#ifndef SERVICEDISCOVERYDIALOG_HPP
#define SERVICEDISCOVERYDIALOG_HPP

#include <QBluetoothAddress>
#include <QBluetoothLocalDevice>
#include <QBluetoothServiceDiscoveryAgent>
#include <QDialog>
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
  Ui::ServiceDiscoveryDialog *ui;
  int servicesDiscovered;
  QBluetoothServiceDiscoveryAgent *discoveryAgent;

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
  void slotDiscoveredService( const QBluetoothServiceInfo &info );
  void slotDialogClose( void );
};

#endif  // SERVICEDISCOVERYDIALOG_HPP
