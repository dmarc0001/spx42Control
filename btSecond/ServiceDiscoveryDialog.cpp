#include "ServiceDiscoveryDialog.hpp"

ServiceDiscoveryDialog::ServiceDiscoveryDialog( std::shared_ptr< Logger > logger,
                                                QString &dname,
                                                QBluetoothAddress &local_addr,
                                                QBluetoothAddress &remote_addr,
                                                QWidget *parent )
    : QDialog( parent )
    , lg( logger )
    , name( dname )
    , laddr( local_addr )
    , raddr( remote_addr )
    , ui( std::unique_ptr< Ui::ServiceDiscoveryDialog >( new Ui::ServiceDiscoveryDialog ) )
{
  ui->setupUi( this );
  //
  // Service discovery Objekt instanzieren
  //
  discoveryAgent = std::unique_ptr< BtServiceDiscover >( new BtServiceDiscover( lg, name, laddr, raddr, this ) );
  lg->debug( QString( "discovery dialog: local adapter addr: " ).append( laddr.toString() ) );
  lg->debug( QString( "discovery dialog: remote adapter addr: " ).append( raddr.toString() ) );
  /*
   * In case of multiple Bluetooth adapters it is possible to
   * set which adapter will be used by providing MAC Address.
   * Example code:
   *
   * QBluetoothAddress adapterAddress("XX:XX:XX:XX:XX:XX");
   * discoveryAgent = new QBluetoothServiceDiscoveryAgent(adapterAddress);
   */
  setWindowTitle( name );
  //
  // signale mit Slots verbinden
  //
  connect( ui->closePushButton, &QPushButton::clicked, this, [=] { this->done( discoveryAgent->servicesDiscovered() ); } );
  connect( discoveryAgent.get(), &BtServiceDiscover::sigDiscoveredService, this, &ServiceDiscoveryDialog::slotDiscoveredService );
  connect( discoveryAgent.get(), &BtServiceDiscover::sigDiscoverScanFinished, ui->status, &QLabel::hide );
  //
  // starte das suchen nach Services
  //
  discoveryAgent->start();
}

ServiceDiscoveryDialog::~ServiceDiscoveryDialog()
{
}

void ServiceDiscoveryDialog::slotDiscoveredService( const QString &name, const QBluetoothServiceInfo &info )
{
  if ( info.serviceName().isEmpty() )
    return;

  QString line = info.serviceName();
  if ( !info.serviceDescription().isEmpty() )
    line.append( "\n\t" + info.serviceDescription() );
  if ( !info.serviceProvider().isEmpty() )
    line.append( "\n\t" + info.serviceProvider() );
  lg->info( QString( "discovered service: %1 on %2" ).arg( line ).arg( name ) );
  ui->list->addItem( line );
  //
  // signalisiere dem interessierten dass ein Service gefunden wurde
  //
  emit sigDiscoveredService( name, info );
}

void ServiceDiscoveryDialog::changeEvent( QEvent *e )
{
  QDialog::changeEvent( e );
  switch ( e->type() )
  {
    case QEvent::LanguageChange:
      ui->retranslateUi( this );
      break;
    default:
      break;
  }
}
