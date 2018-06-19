#include "MainDialog.hpp"

using namespace spx;

/**
 * @brief MainDialog::MainDialog Device scanner
 * @param parent
 */
MainDialog::MainDialog( QWidget *parent ) : QDialog( parent ), ui( std::unique_ptr< Ui::MainDialog >( new Ui::MainDialog ) )
{
  ui->setupUi( this );
  lg = std::shared_ptr< Logger >( new Logger() );
  lg->startLogging( LG_DEBUG, "btsecond.log" );
  lg->debug( "MainDialog::MainDialog" );
  /*
   * In case of multiple Bluetooth adapters it is possible to set adapter
   * which will be used. Example code:
   *
   * QBluetoothAddress address("XX:XX:XX:XX:XX:XX");
   * discoveryAgent = new QBluetoothDeviceDiscoveryAgent(address);
   *
   **/
  //
  // Geräte Discovering Objekt erschaffen
  //
  lg->debug( "MainDialog::MainDialog: create device discovering object..." );
  btDevices = std::unique_ptr< BtLocalDevicesManager >( new BtLocalDevicesManager( lg, this ) );
  //
  // add context menu for devices to be able to pair device
  //
  ui->list->setContextMenuPolicy( Qt::CustomContextMenu );
  //
  lg->debug( "MainDialog::MainDialog: connect signals..." );
  //
  // Signale zum Slots verbinden
  // GUI
  //
  connect( ui->inquiryType, &QCheckBox::toggled, this, &MainDialog::slotGuiSetGeneralUnlimited );
  connect( ui->scan, &QPushButton::clicked, this, &MainDialog::slotGuiStartScan );
  connect( ui->list, &QListWidget::itemActivated, this, &MainDialog::slotGuiItemActivated );
  connect( ui->list, &QListWidget::customContextMenuRequested, this, &MainDialog::slotGuiDisplayPairingMenu );
  //
  // discovering agent object
  //
  connect( btDevices.get(), &BtLocalDevicesManager::sigDiscoveredDevice, this, &MainDialog::slotDiscoveredDevice );
  connect( btDevices.get(), &BtLocalDevicesManager::sigDiscoverScanFinished, this, &MainDialog::slotDiscoverScanFinished );
  connect( btDevices.get(), &BtLocalDevicesManager::sigDeviceHostModeStateChanged, this, &MainDialog::slotDeviceHostModeStateChanged );
  connect( btDevices.get(), &BtLocalDevicesManager::sigDevicePairingDone, this, &MainDialog::slotDevicePairingDone );
  //
  lg->debug( "MainDialog::MainDialog: connect signals...OK" );
  //
  // initialisierung durchführen
  //
  btDevices->init();
}

/**
 * @brief MainDialog::~MainDialog
 */
MainDialog::~MainDialog()
{
  lg->debug( "MainDialog::~MainDialog..." );
  lg->shutdown();
}

/**
 * @brief MainDialog::slotDiscoveredDevice
 * @param info
 */
void MainDialog::slotDiscoveredDevice( const QBluetoothDeviceInfo &info )
{
  //
  // finde exakt den Eintrag, welcher angeklickt wurde
  //
  QString label = QString( "%1 %2" ).arg( info.address().toString() ).arg( info.name() );
  QList< QListWidgetItem * > items = ui->list->findItems( label, Qt::MatchExactly );
  lg->debug( QString( "MainDialog::slotDiscoveredDevice: addr: %1, name %2" ).arg( info.address().toString() ).arg( info.name() ) );
  //
  // sollte keiner gefunden sein, dann einen dazu tragen
  //
  if ( items.empty() )
  {
    //
    // erzeuge einien Eintrag und fülle diesen mit Info
    //
    QListWidgetItem *item = new QListWidgetItem( label );
    QBluetoothLocalDevice::Pairing pairingStatus = btDevices->getLocalDevice()->pairingStatus( info.address() );
    //
    // unterscheide den Pairing status
    //
    if ( pairingStatus == QBluetoothLocalDevice::Paired || pairingStatus == QBluetoothLocalDevice::AuthorizedPaired )
      item->setTextColor( QColor( Qt::green ) );
    else
      item->setTextColor( QColor( Qt::black ) );
    lg->debug( QString( "MainDialog::slotDiscoveredDevice: addr: %1, name %2, pairing: %3" )
                   .arg( info.address().toString() )
                   .arg( info.name() )
                   .arg( pairingStatus ) );
    //
    // Eintrag in die Liste setzen
    //
    ui->list->addItem( item );
  }
}

/**
 * @brief MainDialog::slotGuiStartScan
 */
void MainDialog::slotGuiStartScan()
{
  lg->debug( "MainDialog::slotGuiStartScan..." );
  btDevices->startDiscoverDevices();
  ui->scan->setEnabled( false );
  ui->inquiryType->setEnabled( false );
}

/**
 * @brief MainDialog::slotDiscoverScanFinished
 */
void MainDialog::slotDiscoverScanFinished()
{
  lg->debug( "MainDialog::slotDiscoverScanFinished..." );
  ui->scan->setEnabled( true );
  ui->inquiryType->setEnabled( true );
}

/**
 * @brief MainDialog::slotGuiSetGeneralUnlimited
 * @param unlimited
 */
void MainDialog::slotGuiSetGeneralUnlimited( bool unlimited )
{
  lg->debug( QString( "MainDialog::slotGuiSetGeneralUnlimited: %1" ).arg( unlimited ) );
  //
  btDevices->setInquiryGeneralUnlimited( unlimited );
}

/**
 * @brief MainDialog::slotGuiItemActivated
 * @param item
 */
void MainDialog::slotGuiItemActivated( QListWidgetItem *item )
{
  QString text = item->text();
  int index = text.indexOf( ' ' );
  //
  // ist die auswahl gültig?
  //
  if ( index == -1 )
    return;
  lg->debug( "MainDialog::slotGuiItemActivated..." );
  //
  // Adresse des Gerätes (MAC)
  //
  QBluetoothAddress address( text.left( index ) );
  QString name( text.mid( index + 1 ) );

  //
  // erzeuge und starte den Dialog zum Service discovering
  //
  ServiceDiscoveryDialog d( lg, name, address );
  lg->debug( "MainDialog::slotGuiItemActivated: open service discovering dialog and start service discovering..." );
  index = d.exec();
  lg->debug( QString( "MainDialog::slotGuiItemActivated: service discovering dialog end with %1" ).arg( index ) );
}

/**
 * @brief MainDialog::slotGuiDiscoverableClicked
 * @param clicked
 */
void MainDialog::slotGuiDiscoverableClicked( bool clicked )
{
  lg->debug( QString( "MainDialog::slotGuiDiscoverableClicked: %1" ).arg( clicked ) );
  //
  btDevices->setHostDiscoverable( clicked );
}

/**
 * @brief MainDialog::slotGuiPowerClicked
 * @param clicked
 */
void MainDialog::slotGuiPowerClicked( bool clicked )
{
  lg->debug( QString( "MainDialog::slotGuiPowerClicked: %1" ).arg( clicked ) );
  //
  btDevices->setHostPower( clicked );
}

/**
 * @brief MainDialog::slotDeviceHostModeStateChanged
 * @param mode
 */
void MainDialog::slotDeviceHostModeStateChanged( QBluetoothLocalDevice::HostMode mode )
{
  if ( mode != QBluetoothLocalDevice::HostPoweredOff )
  {
    lg->info( QString( "MainDialog::slotDeviceHostModeStateChanged: host power off: true" ) );
    ui->power->setChecked( true );
  }
  else
  {
    lg->info( QString( "MainDialog::slotDeviceHostModeStateChanged: host power off: false" ) );
    ui->power->setChecked( false );
  }

  if ( mode == QBluetoothLocalDevice::HostDiscoverable )
  {
    lg->info( QString( "MainDialog::slotDeviceHostModeStateChanged: host discoverable: true" ) );
    ui->discoverable->setChecked( true );
  }
  else
  {
    lg->info( QString( "MainDialog::slotDeviceHostModeStateChanged: host discoverable: false" ) );
    ui->discoverable->setChecked( false );
  }
  //
  // ist der Host nun an?
  //
  bool on = !( mode == QBluetoothLocalDevice::HostPoweredOff );
  //
  // gui richten
  //
  ui->scan->setEnabled( on );
  ui->discoverable->setEnabled( on );
}

/**
 * @brief MainDialog::slotGuiDisplayPairingMenu
 * @param pos
 */
void MainDialog::slotGuiDisplayPairingMenu( const QPoint &pos )
{
  //
  // Gültigkeit des Eintrages testen
  //
  if ( ui->list->count() == 0 )
    return;
  lg->info( "MainDialog::slotGuiDisplayPairingMenu: display pairing menu..." );
  //
  QMenu menu( this );
  QAction *pairAction = menu.addAction( "Pair" );
  QAction *removePairAction = menu.addAction( "Remove Pairing" );
  QAction *chosenAction = menu.exec( ui->list->viewport()->mapToGlobal( pos ) );
  QListWidgetItem *currentItem = ui->list->currentItem();
  //
  QString text = currentItem->text();
  int index = text.indexOf( ' ' );
  //
  // ungültiger Index beendet funktion
  //
  if ( index == -1 )
    return;
  //
  // mit wem paaren?
  //
  QBluetoothAddress address( text.left( index ) );
  if ( chosenAction == pairAction )
  {
    btDevices->requestPairing( address, QBluetoothLocalDevice::Paired );
  }
  else if ( chosenAction == removePairAction )
  {
    btDevices->requestPairing( address, QBluetoothLocalDevice::Unpaired );
  }
}

/**
 * @brief MainDialog::slotDevicePairingDone
 * @param address
 * @param pairing
 */
void MainDialog::slotDevicePairingDone( const QBluetoothAddress &address, QBluetoothLocalDevice::Pairing pairing )
{
  lg->info( QString( "MainDialog::slotDevicePairingDone: device: %1" ).arg( address.toString() ) );
  //
  QList< QListWidgetItem * > items = ui->list->findItems( address.toString(), Qt::MatchContains );

  if ( pairing == QBluetoothLocalDevice::Paired || pairing == QBluetoothLocalDevice::AuthorizedPaired )
  {
    for ( int var = 0; var < items.count(); ++var )
    {
      QListWidgetItem *item = items.at( var );
      item->setTextColor( QColor( Qt::green ) );
    }
  }
  else
  {
    for ( int var = 0; var < items.count(); ++var )
    {
      QListWidgetItem *item = items.at( var );
      item->setTextColor( QColor( Qt::red ) );
    }
  }
}

/**
 * @brief MainDialog::changeEvent
 * @param e
 */
void MainDialog::changeEvent( QEvent *e )
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
