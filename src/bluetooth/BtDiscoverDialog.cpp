#include "BtDiscoverDialog.hpp"
#include <QMovie>

namespace spx
{
  /**
   * @brief  BtDiscoverDialog::BtDiscoverDialog Device scanner
   * @param parent
   */
  BtDiscoverDialog::BtDiscoverDialog( std::shared_ptr< Logger > logger,
                                      std::shared_ptr< SPX42Database > spx42Database,
                                      QWidget *parent )
      : QDialog( parent )
      , lg( logger )
      , database( spx42Database )
      , ui( std::unique_ptr< Ui::BtDiscoverDialog >( new Ui::BtDiscoverDialog ) )
      , timerCountdowwn( 0 )
  {
    selectedDeviceAddr.clear();
    ui->setupUi( this );
    ui->currentTaskLabel->setText( tr( "WAITING..." ) );
    ui->CancelPushButton->setText( tr( "CANCEL" ) );
    //
    lg->debug( "BtDiscoverDialog::BtDiscoverDialog..." );
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
    lg->debug( " BtDiscoverDialog::BtDiscoverDialog -> create device discovering object..." );
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
    lg->debug( " BtDiscoverDialog::BtDiscoverDialog -> connect signals..." );
    //
    // Signale zum Slots verbinden
    // GUI
    //
    connect( ui->scanPushButton, &QPushButton::clicked, this, &BtDiscoverDialog::slotGuiStartScan );
    // momentan nicht möglich
    connect( ui->deviceList, &QListWidget::itemClicked, this, &BtDiscoverDialog::itemClicked );
    // connect( ui->deviceList, &QListWidget::customContextMenuRequested, this, &BtDiscoverDialog::slotGuiDisplayPairingMenu );
    //
    // discovering agent object
    //
    connect( btDevices.get(), &SPX42BtDevices::sigDiscoveredDevice, this, &BtDiscoverDialog::slotDiscoveredDevice );
    connect( btDevices.get(), &SPX42BtDevices::sigDiscoverScanFinished, this, &BtDiscoverDialog::slotDiscoverScanFinished );
    connect( btDevices.get(), &SPX42BtDevices::sigDeviceHostModeStateChanged, this,
             &BtDiscoverDialog::slotDeviceHostModeStateChanged );
    connect( btDevices.get(), &SPX42BtDevices::sigDevicePairingDone, this, &BtDiscoverDialog::slotDevicePairingDone );
    connect( ui->CancelPushButton, &QPushButton::clicked, this, &BtDiscoverDialog::reject );
    //
    // timer füer messages verbinden
    //
    connect( &msgTimer, &QTimer::timeout, this, &BtDiscoverDialog::slotMessageTimer );
    //
    setMessage( tr( "SCANNER READY..." ) );
    lg->debug( " BtDiscoverDialog::BtDiscoverDialog -> connect signals...OK" );
  }

  /**
   * @brief BtDiscoverDialog::~BtDiscoverDialog
   */
  BtDiscoverDialog::~BtDiscoverDialog()
  {
    lg->debug( "BtDiscoverDialog::~BtDiscoverDialog..." );
  }

  /**
   * @brief BtDiscoverDialog::getSPX42Devices
   * @return
   */
  SPXDeviceList BtDiscoverDialog::getSPX42Devices( void ) const
  {
    return ( btDevices->getSPX42Devices() );
  }

  /**
   * @brief BtDiscoverDialog::getActivatedDevice
   * @return
   */
  QBluetoothDeviceInfo BtDiscoverDialog::getActivatedDevice( void ) const
  {
    //
    // ist die Auswahl nicht leer?
    //
    if ( !selectedDeviceAddr.isEmpty() )
    {
      //
      // existiert ein Schlüssel "selectedDeviceAddr" im hash?
      //
      if ( btDevices->getSPX42Devices().contains( selectedDeviceAddr ) )
      {
        //
        // die Info zurück geben
        //
        return ( btDevices->getSPX42Devices().value( selectedDeviceAddr ) );
      }
    }
    //
    // ungültiges Objekt zurückgeben
    //
    return ( QBluetoothDeviceInfo() );
  }

  /**
   * @brief BtDiscoverDialog::setMessage
   * @param msg
   */
  void BtDiscoverDialog::setMessage( const QString &msg )
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
   * @brief BtDiscoverDialog::slotDiscoveredDevice
   * @param info
   */
  void BtDiscoverDialog::slotDiscoveredDevice( const QBluetoothDeviceInfo &info )
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
      lg->debug( QString( "BtDiscoverDialog::slotDiscoveredDevice -> addr: %1, name %2, pairing: %3" )
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
   * @brief BtDiscoverDialog::slotGuiStartScan
   */
  void BtDiscoverDialog::slotGuiStartScan()
  {
    lg->debug( "BtDiscoverDialog::slotGuiStartScan..." );
    btDevices->setHostDiscoverable( true );
    btDevices->setHostPower( true );
    btDevices->setInquiryGeneralUnlimited( true );
    btDevices->startDiscoverDevices();
    ui->scanPushButton->setEnabled( false );
    ui->deviceList->clear();
    ui->CancelPushButton->setText( tr( "CANCEL" ) );
    disconnect( ui->CancelPushButton );
    connect( ui->CancelPushButton, &QPushButton::clicked, this, &BtDiscoverDialog::reject );
    setMessage( tr( "START SCAN..." ) );
    movie->start();
  }

  /**
   * @brief BtDiscoverDialog::slotDiscoverScanFinished
   */
  void BtDiscoverDialog::slotDiscoverScanFinished()
  {
    lg->debug( "BtDiscoverDialog::slotDiscoverScanFinished..." );
    ui->scanPushButton->setEnabled( true );
    setMessage( tr( "SCAN FINISHED..." ) );
    disconnect( ui->CancelPushButton );
    connect( ui->CancelPushButton, &QPushButton::clicked, this, &BtDiscoverDialog::accept );
    ui->CancelPushButton->setText( tr( "DONE" ) );
    movie->stop();
    movie->jumpToFrame( 0 );
  }

  /**
   * @brief BtDiscoverDialog::slotGuiItemActivated
   * @param item
   */
  void BtDiscoverDialog::itemClicked( QListWidgetItem *item )
  {
    QString text = item->text();
    int index = text.indexOf( ' ' );
    //
    // ist die auswahl gültig?
    //
    if ( index == -1 )
      return;
    setMessage( tr( "DEVICE %1 ACTIVATED..." ).arg( text ) );
    //
    // Adresse des Gerätes (MAC) als String für Marker, welches Gerät ausgewählt ist
    //
    selectedDeviceAddr = text.left( index ).trimmed();
    lg->debug( QString( "BtDiscoverDialog::slotGuiItemActivated -> device <%1>..." ).arg( selectedDeviceAddr ) );
  }

  /**
   * @brief BtDiscoverDialog::slotDeviceHostModeStateChanged
   * @param mode
   */
  void BtDiscoverDialog::slotDeviceHostModeStateChanged( QBluetoothLocalDevice::HostMode mode )
  {
    if ( mode != QBluetoothLocalDevice::HostPoweredOff )
    {
      lg->info( QString( "BtDiscoverDialog::slotDeviceHostModeStateChanged -> host power off: true" ) );
      setMessage( tr( "BT HOST POWERED ON..." ) );
    }
    else
    {
      lg->info( QString( "BtDiscoverDialog::slotDeviceHostModeStateChanged -> host power off: false" ) );
      setMessage( tr( "BT HOST POWERED OFF..." ) );
    }

    if ( mode == QBluetoothLocalDevice::HostDiscoverable )
    {
      lg->info( QString( "BtDiscoverDialog::slotDeviceHostModeStateChanged -> host discoverable: true" ) );
    }
    else
    {
      lg->info( QString( "BtDiscoverDialog::slotDeviceHostModeStateChanged -> host discoverable: false" ) );
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
   * @brief BtDiscoverDialog::slotGuiDisplayPairingMenu
   * @param pos
   */
  void BtDiscoverDialog::slotGuiDisplayPairingMenu( const QPoint &pos )
  {
    //
    // Gültigkeit des Eintrages testen
    //
    if ( ui->deviceList->count() == 0 )
      return;
    lg->info( "BtDiscoverDialog::slotGuiDisplayPairingMenu -> display pairing menu..." );
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
   * @brief BtDiscoverDialog::slotDevicePairingDone
   * @param address
   * @param pairing
   */
  void BtDiscoverDialog::slotDevicePairingDone( const QBluetoothAddress &address, QBluetoothLocalDevice::Pairing pairing )
  {
    lg->info( QString( "BtDiscoverDialog::slotDevicePairingDone -> device: %1" ).arg( address.toString() ) );
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

  /**
   * @brief BtDiscoverDialog::slotMessageTimer
   */
  void BtDiscoverDialog::slotMessageTimer( void )
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
   * @brief BtDiscoverDialog::changeEvent
   * @param e
   */
  void BtDiscoverDialog::changeEvent( QEvent *e )
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
