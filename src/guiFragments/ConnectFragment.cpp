#include "ConnectFragment.hpp"
#include "ui_ConnectFragment.h"

namespace spx
{
  SPXDeviceList ConnectFragment::spx42Devices{SPXDeviceList()};

  /**
   * @brief ConnectFragment::ConnectFragment
   * @param parent
   * @param logger
   * @param spx42Database
   * @param spxCfg
   * @param remSPX42
   */
  ConnectFragment::ConnectFragment( QWidget *parent,
                                    std::shared_ptr< Logger > logger,
                                    std::shared_ptr< SPX42Database > spx42Database,
                                    std::shared_ptr< SPX42Config > spxCfg,
                                    std::shared_ptr< SPX42RemotBtDevice > remSPX42,
                                    AppConfigClass *appCfg )
      : QWidget( parent )
      , IFragmentInterface( logger, spx42Database, spxCfg, remSPX42, appCfg )
      , ui( new Ui::connectForm )
      , errMsg( tr( "CONNECTION BLUETHOOTH ERROR: %1" ) )
      , discoverObj( std::unique_ptr< BtDiscoverRemoteDevice >( new BtDiscoverRemoteDevice( lg ) ) )
      , normalSpx( ":/images/spx42-normal" )
      , busySpx( new QMovie( ":/images/spx42-busy" ) )
      , connectedSpx( ":/images/spx42-connected" )
      , disConnectedSpx( ":/images/spx42-no-connection" )
      , errorSpx( ":/images/spx42-error" )
  {
    // merke mir ob das Programm das zum ersten mal macht...
    static bool isFirstStart{true};
    *lg << LDEBUG << "ConnectFragment::ConnectFragment..." << Qt::endl;
    //
    // Fragmenttitel Musterstring erzeugen
    //
    fragmentTitlePattern = tr( "CONNECTSTATE SPX42 Serial [%1] LIC: %2" );
    //
    // GUI machen
    //
    ui->setupUi( this );
    ui->connectProgressBar->setVisible( false );
    ui->spx42ImageLabel->setVisible( true );
    ui->spx42ImageLabel->setPixmap( normalSpx );
    ui->editAliasesTableWidget->setVisible( false );
    ui->editAliasesTableWidget->setColumnCount( 2 );
    ui->editAliasesTableWidget->setHorizontalHeaderItem( 0, new QTableWidgetItem( tr( "DEVICE" ) ) );
    ui->editAliasesTableWidget->setHorizontalHeaderItem( 1, new QTableWidgetItem( tr( "DEVICE ALIAS" ) ) );
    ui->editAliasesTableWidget->horizontalHeader()->setMinimumSectionSize( 220 );
    ui->editAliasesTableWidget->horizontalHeader()->setStretchLastSection( true );
    //
    // geräte einlesen und combo liste füllen
    // hashliste mac, <name,alias>
    //
    spxDevicesAliasHash = database->getDeviceAliasHash();
    //
    onConfLicChangedSlot();
    //
    // GUI Signale
    //
    connect( spxConfig.get(), &SPX42Config::licenseChangedSig, this, &ConnectFragment::onConfLicChangedSlot );
    connect( ui->connectButton, &QPushButton::clicked, this, &ConnectFragment::onConnectButtonSlot );
    connect( ui->propertyPushButton, &QPushButton::clicked, this, &ConnectFragment::onPropertyButtonSlot );
    connect( ui->discoverPushButton, &QPushButton::clicked, this, &ConnectFragment::onDiscoverButtonSlot );
    connect( ui->deviceComboBox, static_cast< void ( QComboBox::* )( int ) >( &QComboBox::currentIndexChanged ), this,
             &ConnectFragment::onCurrentIndexChangedSlot );
    connect( discoverObj.get(), &BtDiscoverRemoteDevice::onDiscoveredDeviceSig, this, &ConnectFragment::onDiscoveredDeviceSlot );
    connect( discoverObj.get(), &BtDiscoverRemoteDevice::onDiscoverScanFinishedSig, this,
             &ConnectFragment::onDiscoverScanFinishedSlot );
    connect( remoteSPX42.get(), &SPX42RemotBtDevice::onStateChangedSig, this, &ConnectFragment::onOnlineStatusChangedSlot );
    connect( remoteSPX42.get(), &SPX42RemotBtDevice::onSocketErrorSig, this, &ConnectFragment::onSocketErrorSlot );
    connect( remoteSPX42.get(), &SPX42RemotBtDevice::onCommandRecivedSig, this, &ConnectFragment::onCommandRecivedSlot );
    //
    // setzte den Connectionsstatus
    //
    setGuiConnected( remoteSPX42->getConnectionStatus() == SPX42RemotBtDevice::SPX42_CONNECTED );
    // nach 200 ms discover starten, falls das das erste mal ist
    if ( isFirstStart )
    {
      QTimer::singleShot( 200, this, [=]() { this->onDiscoverButtonSlot(); } );
      isFirstStart = false;
    }
    else
    {
      fillDeviceCombo();
    }
  }

  /**
   * @brief ConnectFragment::~ConnectFragment
   */
  ConnectFragment::~ConnectFragment()
  {
    *lg << LDEBUG << "ConnectFragment::~ConnectFragment..." << Qt::endl;
    ui->spx42ImageLabel->setText( "" );
    delete busySpx;
    spxConfig->disconnect( this );
    remoteSPX42->disconnect( this );
    discoverObj->disconnect( this );
    *lg << LDEBUG << "ConnectFragment::~ConnectFragment...OK" << Qt::endl;
  }

  /**
   * @brief ConnectFragment::onOnlineStatusChangedSlot
   */
  void ConnectFragment::onOnlineStatusChangedSlot( bool )
  {
    setGuiConnected( ( remoteSPX42->getConnectionStatus() == SPX42RemotBtDevice::SPX42_CONNECTED ) ||
                     remoteSPX42->getConnectionStatus() == SPX42RemotBtDevice::SPX42_CONNECTING );
    //
    // gibt es den verbundenen Computer schon in der Datenbank
    //
    QString addr( remoteSPX42->getRemoteConnected() );
    if ( remoteSPX42->getConnectionStatus() == SPX42RemotBtDevice::SPX42_CONNECTED )
    {
      //
      // wenn der alias nicht in der Liste ist, in die Datenbank damit
      //
      if ( !spxDevicesAliasHash.contains( addr ) )
      {
        *lg << LDEBUG << "ConnectFragment::onOnlineStatusChangedSlot -> connected, add device " << addr << " in database" << Qt::endl;
        auto descr = ConnectFragment::spx42Devices.value( addr );
        //
        // und rein damit
        //
        database->addAlias( descr.first, descr.second, descr.second, true );
        //
        // ein kleiner einzeiler zum update der Device-Box
        //
        QTimer::singleShot( 50, [this] {
          this->spxDevicesAliasHash = this->database->getDeviceAliasHash();
          fillDeviceCombo();
        } );
      }
      else
      {
        //
        // der ist schon in der Datenbank
        // setzte als letzte Verbindung
        //
        database->setLastConnected( addr );
      }
    }
    // TODO: evtl mehr machen
  }

  /**
   * @brief ConnectFragment::onSocketErrorSlot
   * @param btErr
   */
  void ConnectFragment::onSocketErrorSlot( QBluetoothSocket::SocketError btErr )
  {
    QString sendMsg;

    switch ( btErr )
    {
      case QBluetoothSocket::UnknownSocketError:
        sendMsg = errMsg.arg( tr( "UNKNOWN ERROR" ) );
        break;
      case QBluetoothSocket::NoSocketError:
        sendMsg = errMsg.arg( tr( "NO BT SOCKET FOUND" ) );
        break;
      case QBluetoothSocket::HostNotFoundError:
        sendMsg = errMsg.arg( tr( "REMOTE HOST NOT FOUND" ) );
        break;
      case QBluetoothSocket::RemoteHostClosedError:
        sendMsg = errMsg.arg( tr( "REMOTE HOST CLOSED CONNECTION" ) );
        break;
      default:
        sendMsg = errMsg.arg( tr( "COMMUNICATION ERROR" ) );
    }
    *lg << LDEBUG << "ConnectFragment::onSocketErrorSlot -> send error message <" << sendMsg << "> to main window..." << Qt::endl;
    emit onErrorgMessageSig( sendMsg, true );
  }

  /**
   * @brief ConnectFragment::onConfLicChangedSlot
   */
  void ConnectFragment::onConfLicChangedSlot()
  {
    setGuiConnected( remoteSPX42->getConnectionStatus() == SPX42RemotBtDevice::SPX42_CONNECTED );
  }

  /**
   * @brief ConnectFragment::onCloseDatabaseSlot
   */
  void ConnectFragment::onCloseDatabaseSlot()
  {
    // TODO: implementieren
  }

  /**
   * @brief ConnectFragment::onConnectButtonSlot
   */
  void ConnectFragment::onConnectButtonSlot()
  {
    bool isConnected = remoteSPX42->getConnectionStatus() == SPX42RemotBtDevice::SPX42_CONNECTED;
    //
    *lg << LDEBUG << "ConnectFragment::onConnectButtonSlot -> connect/disconnect button clicked." << Qt::endl;
    //
    // je nachdem ob das gerät verbunden oder getrennt ist
    //
    if ( isConnected )
    {
      *lg << LDEBUG << "ConnectFragment::onConnectButtonSlot -> disconnect current connection" << Qt::endl;
      remoteSPX42->endConnection();
    }
    else
    {
      //
      // stelle sicher, dass ein Eintrag ausgewählt ist
      //
      if ( ui->deviceComboBox->currentIndex() < 0 )
      {
        *lg << LWARN << "ConnectFragment::onConnectButtonSlot -> not an device delected!" << Qt::endl;
        emit onWarningMessageSig( tr( "NOT SELECTED A DEVICE TO CONNECT" ), true );
        return;
      }
      QString remoteMac = ui->deviceComboBox->itemData( ui->deviceComboBox->currentIndex() ).toString();
      *lg << LDEBUG << "ConnectFragment::onConnectButtonSlot -> connect to device <" << remoteMac << ">" << Qt::endl;
      //
      // die busy anzeige werkeln lassen
      //
      ui->connectProgressBar->setRange( 0, 0 );
      ui->connectProgressBar->setVisible( true );
      //
      // blockiere mal die GUI solange
      // setGuiConnected( true );
      //
      // jetzt das remote objekt arbeiten lassen
      //
      // programm schmiert ab, ursache noch nicht geklärt
      // discoverObj->stopDiscover();
      remoteSPX42->endConnection();
      remoteSPX42->startConnection( remoteMac );
    }
  }

  /**
   * @brief ConnectFragment::onPropertyButtonSlot
   */
  void ConnectFragment::onPropertyButtonSlot()
  {
    *lg << LDEBUG << "ConnectFragment::onPropertyButtonSlot -> property button clicked." << Qt::endl;
    if ( remoteSPX42->getConnectionStatus() != SPX42RemotBtDevice::SPX42_CONNECTED )
    {
      ui->editAliasesTableWidget->setVisible( !ui->editAliasesTableWidget->isVisible() );
      ui->spx42ImageLabel->setVisible( !ui->editAliasesTableWidget->isVisible() );
      if ( ui->editAliasesTableWidget->isVisible() )
      {
        //
        // größe etwas eindämmen
        //
        int edWith = ui->editorWidget->width();
        if ( edWith > 500 )
        {
          int margins = edWith - 500;
          if ( margins > 400 )
            margins = 400;
          int left, top, right, bottom;
          ui->editorLayout->getContentsMargins( &left, &top, &right, &bottom );
          ui->editorLayout->setContentsMargins( margins / 4, top, margins / 4, bottom );
        }
        //
        // mit datenbankeinträgen füllen
        //
        DeviceAliasHash aliasHash = database->getDeviceAliasHash();
        disconnect( ui->editAliasesTableWidget, &QTableWidget::itemChanged, nullptr, nullptr );
        ui->editAliasesTableWidget->setRowCount( 0 );
        int currentRow = 0;
        for ( auto key : aliasHash.keys() )
        {
          auto deviceAlias = aliasHash.value( key );
          // column 0
          auto *item0 = new QTableWidgetItem( deviceAlias.mac );
          item0->setFlags( item0->flags() & ~Qt::ItemIsEditable );
          // column 1
          auto *item1 = new QTableWidgetItem( deviceAlias.alias );
          // einfügen
          ui->editAliasesTableWidget->insertRow( currentRow );
          *lg << LDEBUG << "insert row <" << currentRow << ">, " << item0->text() << ", " << item1->text() << Qt::endl;
          ui->editAliasesTableWidget->setItem( currentRow, 0, item0 );
          ui->editAliasesTableWidget->setItem( currentRow, 1, item1 );
          ui->editAliasesTableWidget->repaint();
          currentRow++;
        }
        connect( ui->editAliasesTableWidget, &QTableWidget::itemChanged, this, &ConnectFragment::onAliasEditItemChanged );
      }
    }
  }

  void ConnectFragment::onAliasEditItemChanged( QTableWidgetItem *edItem )
  {
    int currentRow = edItem->row();
    QString alias = edItem->text();
    QString mac = ui->editAliasesTableWidget->item( currentRow, 0 )->text();
    *lg << LDEBUG << "ConnectFragment::onAliasEditItemChanged -> entry <" << mac << "> to new Alias: <" << alias << ">" << Qt::endl;
    if ( database->setAliasForMac( mac, alias ) )
    {
      //
      // die combobox neu füllen
      //
      spxDevicesAliasHash = database->getDeviceAliasHash();
      fillDeviceCombo();
      *lg << LDEBUG << "ConnectFragment::onAliasEditItemChanged -> OK" << Qt::endl;
      return;
    }
    *lg << LWARN << "ConnectFragment::onAliasEditItemChanged -> NOT OK - show previous messages" << Qt::endl;
  }

  /**
   * @brief ConnectFragment::onDiscoverButtonSlot
   */
  void ConnectFragment::onDiscoverButtonSlot()
  {
    *lg << LDEBUG << "ConnectFragment::onDiscoverButtonSlot -> discover button clicked." << Qt::endl;
    //
    // Liste löschen
    //
    ui->deviceComboBox->clear();
    ConnectFragment::spx42Devices.clear();
    ui->discoverPushButton->setEnabled( false );
    //
    // das Discovering starten
    //
    discoverObj->startDiscover();
  }

  /**
   * @brief ConnectFragment::onDiscoveredDeviceSlot
   * @param deviceInfo
   */
  void ConnectFragment::onDiscoveredDeviceSlot( const SPXDeviceDescr &deviceInfo )
  {
    *lg << LDEBUG << "ConnectFragment::onDiscoveredDeviceSlot-> device found, try inserting in combo..." << Qt::endl;
    //
    // Gerät zuerst in die Liste einfügen
    // dabei gehe ich davon aus das die discoverroutine auf doppelte Geräte schon
    // geprüft hat
    //
    addDeviceComboEntry( deviceInfo );
    trySetIndex();
  }

  /**
   * @brief ConnectFragment::onDiscoverScanFinishedSlot
   */
  void ConnectFragment::onDiscoverScanFinishedSlot()
  {
    *lg << LDEBUG << "ConnectFragment::onDiscoverScanFinishedSlot-> discovering finished..." << Qt::endl;
    ui->discoverPushButton->setEnabled( true );
    trySetIndex();
  }

  /**
   * @brief ConnectFragment::onCurrentIndexChangedSlot
   * @param index
   */
  void ConnectFragment::onCurrentIndexChangedSlot( int index )
  {
    *lg << LDEBUG
        << QString( "ConnectFragment::onCurrentIndexChangedSlot -> index changed to <%1>. addr: <%2>" )
               .arg( index, 2, 10, QChar( '0' ) )
               .arg( ui->deviceComboBox->itemData( index ).toString() )
        << Qt::endl;
  }

  /**
   * @brief ConnectFragment::onCommandRecivedSlot
   */
  void ConnectFragment::onCommandRecivedSlot()
  {
    spSingleCommand recCommand;
    QDateTime nowDateTime;
    char kdo;
    //
    *lg << LDEBUG << "ConnectFragment::onDatagramRecivedSlot..." << Qt::endl;
    //
    // alle abholen...
    //
    while ( ( recCommand = remoteSPX42->getNextRecCommand() ) )
    {
      // ja, es gab ein Datagram zum abholen
      kdo = recCommand->getCommand();
      switch ( kdo )
      {
        case SPX42CommandDef::SPX_ALIVE:
          // Kommando ALIVE liefert zurück:
          // ~03:PW
          // PX => Angabe HEX in Milivolt vom Akku
          *lg << LDEBUG << "ConnectFragment::onDatagramRecivedSlot -> alive/acku..." << Qt::endl;
          ackuVal = ( recCommand->getValueFromHexAt( SPXCmdParam::ALIVE_POWER ) / 100.0 );
          emit onAkkuValueChangedSig( ackuVal );
          if ( ui->connectProgressBar->isVisible() )
          {
            ui->connectProgressBar->setVisible( false );
            //
            // TODO: die tabs, welche nur inline sinnvoll sind entsperren
            //
          }
          break;
        case SPX42CommandDef::SPX_APPLICATION_ID:
          // Kommando APPLICATION_ID (VERSION)
          // ~04:NR -> VERSION als String
          *lg << LDEBUG << "ConnectFragment::onDatagramRecivedSlot -> firmwareversion..." << Qt::endl;
          // Setzte die Version in die Config
          spxConfig->setSpxFirmwareVersion( recCommand->getParamAt( SPXCmdParam::FIRMWARE_VERSION ) );
          spxConfig->freezeConfigs( SPX42ConfigClass::CF_CLASS_SPX );
          // Geht das Datum zu setzen?
          if ( spxConfig->getCanSetDate() )
          {
            // ja der kann das Datum online setzten
            nowDateTime = QDateTime::currentDateTime();
            // sende das Datum an den SPX
            remoteSPX42->setDateTime( nowDateTime );
          }
          break;
        case SPX42CommandDef::SPX_SERIAL_NUMBER:
          // Kommando SERIAL NUMBER
          // ~07:XXX -> Seriennummer als String
          *lg << LDEBUG << "ConnectFragment::onDatagramRecivedSlot -> serialnumber..." << Qt::endl;
          spxConfig->setSerialNumber( recCommand->getParamAt( SPXCmdParam::SERIAL_NUMBER ) );
          spxConfig->freezeConfigs( SPX42ConfigClass::CF_CLASS_SPX );
          setGuiConnected( remoteSPX42->getConnectionStatus() == SPX42RemotBtDevice::SPX42_CONNECTED );
          break;
        case SPX42CommandDef::SPX_LICENSE_STATE:
          // Kommando SPX_LICENSE_STATE
          // komplett: <~45:LS:CE>
          // übergeben LS,CE
          // LS : License State 0=Nitrox,1=Normoxic Trimix,2=Full Trimix
          // CE : Custom Enabled 0= disabled, 1=enabled
          *lg << LDEBUG << "ConnectFragment::onDatagramRecivedSlot -> license state..." << Qt::endl;
          spxConfig->setLicense( recCommand->getParamAt( SPXCmdParam::LICENSE_STATE ),
                                 recCommand->getParamAt( SPXCmdParam::LICENSE_INDIVIDUAL ) );
          spxConfig->freezeConfigs( SPX42ConfigClass::CF_CLASS_SPX );
          setGuiConnected( remoteSPX42->getConnectionStatus() == SPX42RemotBtDevice::SPX42_CONNECTED );
          break;
      }
      //
      // falls es mehr gibt, lass dem Rest der App auch eine Chance
      //
      QCoreApplication::processEvents();
    }
  }

  // ##########################################################################
  // PRIVATE Funktionen
  // ##########################################################################

  /**
   * @brief ConnectFragment::setGuiConnected
   * @param isConnected
   */
  void ConnectFragment::setGuiConnected( bool isConnected )
  {
    //
    // setzte die GUI Elemente entsprechend des Online Status
    //
    isConnected ? ui->connectButton->setText( tr( "DISCONNECT DEVICE" ) ) : ui->connectButton->setText( tr( "CONNECT DEVICE" ) );
    ui->deviceComboBox->setEnabled( !isConnected );
    ui->propertyPushButton->setEnabled( !isConnected );
    ui->discoverPushButton->setEnabled( !isConnected );
    if ( isConnected )
    {
      ui->spx42ImageLabel->setVisible( true );
      if ( remoteSPX42->getConnectionStatus() == SPX42RemotBtDevice::SPX42_CONNECTING )
      {
        ui->spx42ImageLabel->setMovie( busySpx );
        busySpx->start();
      }
      else
        ui->spx42ImageLabel->setPixmap( connectedSpx );
      ui->editAliasesTableWidget->setVisible( false );
    }
    else
    {
      if ( remoteSPX42->getConnectionStatus() == SPX42RemotBtDevice::SPX42_ERROR )
      {
        ui->spx42ImageLabel->setPixmap( errorSpx );
      }
      else
      {
        ui->spx42ImageLabel->setPixmap( disConnectedSpx );
      }
    }
    ui->tabHeaderLabel->setText( fragmentTitlePattern.arg( spxConfig->getSerialNumber() ).arg( spxConfig->getLicName() ) );
  }

  /**
   * @brief ConnectFragment::addDeviceComboEntry
   * @param deviceInfo
   */
  void ConnectFragment::addDeviceComboEntry( const SPXDeviceDescr &deviceInfo )
  {
    QString addr = deviceInfo.first;
    ConnectFragment::spx42Devices.insert( addr, deviceInfo );
    //
    // und nun eine vernünftige Anzeige machen und die MAC als Schlüssel lassen
    //
    if ( spxDevicesAliasHash.contains( addr ) )
    {
      //
      // ein alias ist vorhanden!
      //
      auto localSpxDevice = spxDevicesAliasHash.value( addr );
      QString title = QString( "%1 (%2)" ).arg( localSpxDevice.alias ).arg( localSpxDevice.name );
      ui->deviceComboBox->addItem( title, addr );
    }
    else
    {
      //
      // es gibt (noch) keinen Alias, baue den Eintrag aus Name und MAC
      //
      QString title = QString( "%1 (%2)" ).arg( deviceInfo.second ).arg( addr );
      ui->deviceComboBox->addItem( title, addr );
    }
  }

  void ConnectFragment::fillDeviceCombo()
  {
    //
    ui->deviceComboBox->clear();
    for ( auto deviceInfo : ConnectFragment::spx42Devices )
    {
      addDeviceComboEntry( deviceInfo );
    }
    trySetIndex();
  }

  void ConnectFragment::trySetIndex()
  {
    //
    // wer ist verbunden
    //
    QString mac;
    *lg << LDEBUG << "ConnectFragment::trySetIndex..." << Qt::endl;
    mac = remoteSPX42->getRemoteConnected();
    if ( mac.isEmpty() )
    {
      //
      // nichts verbunden
      // gucke ob ich das zulettz verbundene Gerät finde
      //
      *lg << LDEBUG << "ConnectFragment::trySetIndex -> not connected, try last connected..." << Qt::endl;
      mac = database->getLastConnected();
    }
    if ( !mac.isEmpty() )
    {
      *lg << LDEBUG << "ConnectFragment::trySetIndex -> last connected was: " << mac << Qt::endl;
      //
      // verbunden oder nicht, versuche etwas zu selektiern
      //
      // suche nach diesem Eintrag...
      //
      int index = ui->deviceComboBox->findData( mac );
      if ( index != ui->deviceComboBox->currentIndex() && index != -1 )
      {
        *lg << LDEBUG << "ConnectFragment::trySetIndex -> found at idx " << index << ", set to idx" << Qt::endl;
        //
        // -1 for not found
        // und der index ist nicht schon auf diesen Wert gesetzt
        // also, wenn gefunden, selektiere diesen Eintrag
        //
        ui->deviceComboBox->setCurrentIndex( index );
      }
    }
    *lg << LDEBUG << "ConnectFragment::trySetIndex...OK" << Qt::endl;
  }
}  // namespace spx
