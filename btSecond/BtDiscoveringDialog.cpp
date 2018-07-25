#include "BtDiscoveringDialog.hpp"
#include <QMovie>

namespace spx
{
  /**
   * @brief MainDialog::MainDialog Device scanner
   * @param parent
   */
  BtDiscoveringDialog::BtDiscoveringDialog( QWidget *parent )
      : QDialog( parent )
      , ui( std::unique_ptr< Ui::BtDiscoverDialog >( new Ui::BtDiscoverDialog ) )
      , msgTimer( this )
      , timerCountdowwn( 0 )
  {
    ui->setupUi( this );
    ui->currentTaskLabel->setText( tr( "WAITING..." ) );
    ui->CancelPushButton->setText( tr( "CANCEL" ) );
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
    btDevices = std::unique_ptr< SPX42BtDevices >( new SPX42BtDevices( lg, this ) );
    //
    // add context menu for devices to be able to pair device
    //
    ui->deviceList->setContextMenuPolicy( Qt::CustomContextMenu );
    //
    // Message deactivation Timer vorbereiten
    //
    msgTimer.setInterval( timerInterval );
    //
    // Arbeiter-Animation erzeugen, aber noch nicht starten
    //
    movie = std::unique_ptr< QMovie >( new QMovie( ":/scanner/workerAnimation" ) );
    ui->processLabel->setMovie( movie.get() );
    movie->start();
    movie->stop();
    movie->jumpToFrame( 0 );
    //
    lg->debug( "MainDialog::MainDialog: connect signals..." );
    //
    // Signale zum Slots verbinden
    // GUI
    //
    connect( ui->scanPushButton, &QPushButton::clicked, this, &BtDiscoveringDialog::slotGuiStartScan );
    connect( ui->deviceList, &QListWidget::itemActivated, this, &BtDiscoveringDialog::slotGuiItemActivated );
    connect( ui->deviceList, &QListWidget::customContextMenuRequested, this, &BtDiscoveringDialog::slotGuiDisplayPairingMenu );
    //
    // discovering agent object
    //
    connect( btDevices.get(), &SPX42BtDevices::sigDiscoveredDevice, this, &BtDiscoveringDialog::slotDiscoveredDevice );
    connect( btDevices.get(), &SPX42BtDevices::sigDiscoverScanFinished, this, &BtDiscoveringDialog::slotDiscoverScanFinished );
    connect( btDevices.get(), &SPX42BtDevices::sigDeviceHostModeStateChanged, this,
             &BtDiscoveringDialog::slotDeviceHostModeStateChanged );
    connect( btDevices.get(), &SPX42BtDevices::sigDevicePairingDone, this, &BtDiscoveringDialog::slotDevicePairingDone );
    //
    // timer füer messages verbinden
    //
    connect( &msgTimer, &QTimer::timeout, this, &BtDiscoveringDialog::slotMessageTimer );
    //
    setMessage( tr( "SCANNER READY..." ) );
    lg->debug( "MainDialog::MainDialog: connect signals...OK" );
  }

  /**
   * @brief MainDialog::~MainDialog
   */
  BtDiscoveringDialog::~BtDiscoveringDialog()
  {
    lg->debug( "MainDialog::~MainDialog..." );
    lg->shutdown();
  }

  void BtDiscoveringDialog::setMessage( const QString &msg )
  {
    if ( ui->currentTaskLabel )
    {
      ui->currentTaskLabel->setText( msg );
      //
      // timer Timeout setzen;
      //
      timerCountdowwn = timerStartValue;
      //
      // timer starten, wenn nicht geschehen
      //
      if ( !msgTimer.isActive() )
        msgTimer.start();
    }
  }

  /**
   * @brief MainDialog::slotDiscoveredDevice
   * @param info
   */
  void BtDiscoveringDialog::slotDiscoveredDevice( const QBluetoothDeviceInfo &info )
  {
    //
    // finde exakt den Eintrag, welcher angeklickt wurde
    //
    QString label = QString( "%1 %2" ).arg( info.address().toString() ).arg( info.name() );
    QList< QListWidgetItem * > items = ui->deviceList->findItems( label, Qt::MatchExactly );
    //
    // sollte keiner gefunden sein, dann einen dazu tragen
    //
    if ( items.empty() )
    {
      //
      // erzeuge einien Eintrag und fülle diesen mit Info
      //
      QListWidgetItem *item = new QListWidgetItem( label );
      QBluetoothLocalDevice::Pairing pairingStatus = btDevices->getPairingStatus( info.address() );
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
      ui->deviceList->addItem( item );
      setMessage( tr( "FOUND DEVICE: %1..." ).arg( info.address().toString() ) );
    }
  }

  /**
   * @brief MainDialog::slotGuiStartScan
   */
  void BtDiscoveringDialog::slotGuiStartScan()
  {
    lg->debug( "MainDialog::slotGuiStartScan..." );
    btDevices->setHostDiscoverable( true );
    btDevices->setHostPower( true );
    btDevices->setInquiryGeneralUnlimited( true );
    btDevices->startDiscoverDevices();
    ui->scanPushButton->setEnabled( false );
    ui->deviceList->clear();
    ui->CancelPushButton->setText( tr( "CANCEL" ) );
    setMessage( tr( "START SCAN..." ) );
    movie->start();
  }

  /**
   * @brief MainDialog::slotDiscoverScanFinished
   */
  void BtDiscoveringDialog::slotDiscoverScanFinished()
  {
    lg->debug( "MainDialog::slotDiscoverScanFinished..." );
    ui->scanPushButton->setEnabled( true );
    setMessage( tr( "SCAN FINISHED..." ) );
    ui->CancelPushButton->setText( tr( "DONE" ) );
    movie->stop();
    movie->jumpToFrame( 0 );
  }

  /**
   * @brief MainDialog::slotGuiItemActivated
   * @param item
   */

  void BtDiscoveringDialog::slotGuiItemActivated( QListWidgetItem *item )
  {
    QString text = item->text();
    int index = text.indexOf( ' ' );
    //
    // ist die auswahl gültig?
    //
    if ( index == -1 )
      return;
    lg->debug( "MainDialog::slotGuiItemActivated..." );
    setMessage( tr( "DEVICE %1 ACTIVETED..." ).arg( text ) );
    //
    // Adresse des Gerätes (MAC)
    //
    /*
    QBluetoothAddress address( text.left( index ) );
    QString name( text.mid( index + 1 ) );

    //
    // erzeuge und starte den Dialog zum Service discovering
    //
    QBluetoothAddress l_addr = btDevices->getLocalDevice()->address();
    ServiceDiscoveryDialog d( lg, name, l_addr, address, this );
    lg->debug( "MainDialog::slotGuiItemActivated: open service discovering dialog and start service discovering..." );
    index = d.exec();
    lg->debug( QString( "MainDialog::slotGuiItemActivated: service discovering dialog end with %1" ).arg( index ) );
    */
  }

  /**
   * @brief MainDialog::slotGuiPowerClicked
   * @param clicked
   */
  void BtDiscoveringDialog::slotGuiPowerClicked( bool clicked )
  {
    lg->debug( QString( "MainDialog::slotGuiPowerClicked: %1" ).arg( clicked ) );
    //
    btDevices->setHostPower( clicked );
  }

  /**
   * @brief MainDialog::slotDeviceHostModeStateChanged
   * @param mode
   */
  void BtDiscoveringDialog::slotDeviceHostModeStateChanged( QBluetoothLocalDevice::HostMode mode )
  {
    if ( mode != QBluetoothLocalDevice::HostPoweredOff )
    {
      lg->info( QString( "MainDialog::slotDeviceHostModeStateChanged: host power off: true" ) );
      setMessage( tr( "BT HOST POWERED ON..." ) );
    }
    else
    {
      lg->info( QString( "MainDialog::slotDeviceHostModeStateChanged: host power off: false" ) );
      setMessage( tr( "BT HOST POWERED OFF..." ) );
    }

    if ( mode == QBluetoothLocalDevice::HostDiscoverable )
    {
      lg->info( QString( "MainDialog::slotDeviceHostModeStateChanged: host discoverable: true" ) );
    }
    else
    {
      lg->info( QString( "MainDialog::slotDeviceHostModeStateChanged: host discoverable: false" ) );
    }
    //
    // ist der Host nun an?
    //
    bool on = !( mode == QBluetoothLocalDevice::HostPoweredOff );
    //
    // gui richten
    //
    ui->scanPushButton->setEnabled( on );
  }

  /**
   * @brief MainDialog::slotGuiDisplayPairingMenu
   * @param pos
   */
  void BtDiscoveringDialog::slotGuiDisplayPairingMenu( const QPoint &pos )
  {
    //
    // Gültigkeit des Eintrages testen
    //
    if ( ui->deviceList->count() == 0 )
      return;
    lg->info( "MainDialog::slotGuiDisplayPairingMenu: display pairing menu..." );
    //
    QMenu menu( this );
    QAction *pairAction = menu.addAction( "Pair" );
    QAction *removePairAction = menu.addAction( "Remove Pairing" );
    QAction *chosenAction = menu.exec( ui->deviceList->viewport()->mapToGlobal( pos ) );
    QListWidgetItem *currentItem = ui->deviceList->currentItem();
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
  void BtDiscoveringDialog::slotDevicePairingDone( const QBluetoothAddress &address, QBluetoothLocalDevice::Pairing pairing )
  {
    lg->info( QString( "MainDialog::slotDevicePairingDone: device: %1" ).arg( address.toString() ) );
    //
    QList< QListWidgetItem * > items = ui->deviceList->findItems( address.toString(), Qt::MatchContains );

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

  void BtDiscoveringDialog::slotMessageTimer( void )
  {
    //
    // wenn die Message noch da ist, langsam runterzählen
    //
    if ( --timerCountdowwn < 0 )
    {
      if ( ui->currentTaskLabel )
      {
        ui->currentTaskLabel->clear();
        msgTimer.stop();
      }
    }
  }

  /**
   * @brief MainDialog::changeEvent
   * @param e
   */
  void BtDiscoveringDialog::changeEvent( QEvent *e )
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
}
