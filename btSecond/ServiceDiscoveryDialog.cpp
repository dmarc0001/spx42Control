#include "ServiceDiscoveryDialog.hpp"

ServiceDiscoveryDialog::ServiceDiscoveryDialog( std::shared_ptr< Logger > logger,
                                                QString &dname,
                                                QBluetoothAddress &daddr,
                                                QWidget *parent )
    : QDialog( parent ), lg( logger ), name( dname ), addr( daddr ), ui( new Ui::ServiceDiscoveryDialog ), servicesDiscovered( 0 )
{
  ui->setupUi( this );
  //
  // default Bluetooth adapter
  //
  QBluetoothLocalDevice localDevice;
  QBluetoothAddress adapterAddress = localDevice.address();
  lg->debug( QString( "discovery dialog: local adapter addr: " ).append( adapterAddress.toString() ) );
  lg->debug( QString( "discovery dialog: remote adapter addr: " ).append( addr.toString() ) );
  /*
   * In case of multiple Bluetooth adapters it is possible to
   * set which adapter will be used by providing MAC Address.
   * Example code:
   *
   * QBluetoothAddress adapterAddress("XX:XX:XX:XX:XX:XX");
   * discoveryAgent = new QBluetoothServiceDiscoveryAgent(adapterAddress);
   */
  discoveryAgent = new QBluetoothServiceDiscoveryAgent( adapterAddress );
  discoveryAgent->setRemoteAddress( addr );
  setWindowTitle( name );
  //
  // signale mit Slots verbinden
  //
  connect( ui->closePushButton, &QPushButton::clicked, this, [=] { this->done( servicesDiscovered ); } );
  connect( discoveryAgent, &QBluetoothServiceDiscoveryAgent::serviceDiscovered, this, &ServiceDiscoveryDialog::slotDiscoveredService );
  connect( discoveryAgent, &QBluetoothServiceDiscoveryAgent::finished, ui->status, &QLabel::hide );
  //
  // starte das suchen nach Services
  //
  discoveryAgent->start();
}

ServiceDiscoveryDialog::~ServiceDiscoveryDialog()
{
  delete ui;
}

void ServiceDiscoveryDialog::slotDiscoveredService( const QBluetoothServiceInfo &info )
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
  servicesDiscovered++;
}

void ServiceDiscoveryDialog::slotDialogClose( void )
{
  done( servicesDiscovered );
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
