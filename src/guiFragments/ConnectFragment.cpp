#include "ConnectFragment.hpp"
#include "ui_ConnectFragment.h"

namespace spx
{
  ConnectFragment::ConnectFragment( QWidget *parent,
                                    std::shared_ptr< Logger > logger,
                                    std::shared_ptr< SPX42Database > spx42Database,
                                    std::shared_ptr< SPX42Config > spxCfg,
                                    std::shared_ptr< SPX42RemotBtDevice > remSPX42 )
      : QWidget( parent )
      , IFragmentInterface( logger, spx42Database, spxCfg, remSPX42 )
      , ui( new Ui::connectForm )
      , errMsg( tr( "CONNECTION BLUETHOOTH ERROR: %1" ) )
  {
    lg->debug( "ConnectFragment::ConnectFragment..." );
    ui->setupUi( this );
    ui->connectProgressBar->setVisible( false );
    //
    // geräte einlesen und combo liste füllen
    // hashliste mac, <name,alias>
    //
    devices = database->getDeviceAliasHash();
    fillDevicesList();
    //
    onConfLicChangedSlot();
    connect( spxConfig.get(), &SPX42Config::licenseChangedSig, this, &ConnectFragment::onConfLicChangedSlot );
    connect( ui->connectButton, &QPushButton::clicked, this, &ConnectFragment::onConnectButtonSlot );
    connect( ui->propertyPushButton, &QPushButton::clicked, this, &ConnectFragment::onPropertyButtonSlot );
    connect( ui->discoverPushButton, &QPushButton::clicked, this, &ConnectFragment::onDiscoverButtonSlot );
    connect( ui->deviceComboBox, static_cast< void ( QComboBox::* )( int ) >( &QComboBox::currentIndexChanged ), this,
             &ConnectFragment::onCurrentIndexChangedSlot );
    connect( remoteSPX42.get(), &SPX42RemotBtDevice::onStateChangedSig, this, &ConnectFragment::onOnlineStatusChangedSlot );
    connect( remoteSPX42.get(), &SPX42RemotBtDevice::onSocketErrorSig, this, &ConnectFragment::onSocketErrorSlot );
    // setzte den Connectionsstatus
    setGuiConnected( remoteSPX42->getConnectionStatus() == SPX42RemotBtDevice::SPX42_CONNECTED );
  }

  ConnectFragment::~ConnectFragment()
  {
    lg->debug( "ConnectFragment::~ConnectFragment..." );
    deactivateTab();
  }

  void ConnectFragment::deactivateTab( void )
  {
    //
    // deaktiviere signale für reciver
    //
    disconnect( spxConfig.get(), nullptr, this, nullptr );
    disconnect( remoteSPX42.get(), nullptr, this, nullptr );
  }

  void ConnectFragment::onOnlineStatusChangedSlot( bool )
  {
    setGuiConnected( ( remoteSPX42->getConnectionStatus() == SPX42RemotBtDevice::SPX42_CONNECTED ) ||
                     remoteSPX42->getConnectionStatus() == SPX42RemotBtDevice::SPX42_CONNECTING );
    // TODO: evtl mehr machen
  }

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
    lg->debug( QString( "ConnectFragment::onSocketErrorSlot -> send error message <%1> to main window..." ).arg( sendMsg ) );
    emit onErrorgMessageSig( sendMsg, true );
  }

  void ConnectFragment::onConfLicChangedSlot()
  {
    // TODO was machen
  }

  void ConnectFragment::onCloseDatabaseSlot()
  {
    // TODO: implementieren
  }

  void ConnectFragment::onConnectButtonSlot()
  {
    bool isConnected = remoteSPX42->getConnectionStatus() == SPX42RemotBtDevice::SPX42_CONNECTED;
    //
    lg->debug( "ConnectFragment::onConnectButtonSlot -> connect/disconnect button clicked." );
    //
    // je nachdem ob das gerät verbunden oder getrennt ist
    //
    if ( isConnected )
    {
      lg->debug( "ConnectFragment::onConnectButtonSlot -> disconnect current connection" );
      remoteSPX42->endConnection();
    }
    else
    {
      //
      // stelle sicher, dass ein Eintrag ausgewählt ist
      //
      if ( ui->deviceComboBox->currentIndex() < 0 )
      {
        lg->warn( "ConnectFragment::onConnectButtonSlot -> not an device delected!" );
        emit onWarningMessageSig( tr( "NOT SELECTED A DEVICE TO CONNECT" ), true );
      }
      QString remoteMac = ui->deviceComboBox->itemData( ui->deviceComboBox->currentIndex() ).toString();
      lg->debug( QString( "ConnectFragment::onConnectButtonSlot -> connect to device <%1>" ).arg( remoteMac ) );
      //
      // blockiere mal die GUI solange
      // setGuiConnected( true );
      //
      // jetzt das remote objekt arbeiten lassen
      //
      remoteSPX42->startConnection( remoteMac );
    }
  }

  void ConnectFragment::onPropertyButtonSlot()
  {
    lg->debug( "ConnectFragment::onPropertyButtonSlot -> property button clicked." );
  }

  void ConnectFragment::onDiscoverButtonSlot()
  {
    lg->debug( "ConnectFragment::onDiscoverButtonSlot -> discover button clicked." );
    //
    // den Dialog erzeugen
    //
    BtDiscoverDialog discoverDialog( lg, database );
    //
    // Dialog modal starten
    //
    if ( QDialog::Accepted == discoverDialog.exec() )
    {
      //
      // Ergebnis übernehmen
      //
      lg->debug( "ConnectFragment::onDiscoverButtonSlot -> discovering dialog returned: accept result...." );
      SPXDeviceList devList( discoverDialog.getSPX42Devices() );
      lg->debug(
          QString( "ConnectFragment::onDiscoverButtonSlot -> scanned %1 devices" ).arg( devList.count(), 2, 10, QChar( '0' ) ) );
      QBluetoothDeviceInfo devInfo( discoverDialog.getActivatedDevice() );
      //
      // ein Gerät gültig markiert?
      //
      if ( devInfo.isValid() )
      {
        lg->debug( QString( "ConnectFragment::onDiscoverButtonSlot -> selected device <%1>" ).arg( devInfo.address().toString() ) );
      }
      else
      {
        lg->debug( "ConnectFragment::onDiscoverButtonSlot -> not an device selected..." );
        return;
      }
      //
      // die Geräteliste neu befüllen
      //
      devices = database->getDeviceAliasHash();
      fillDevicesList();
      // mac, <name,alias>
      QPair< QString, QString > nPair( devInfo.address().toString(), devInfo.name() );
      QString title = QString( "%1(%2)" ).arg( nPair.second ).arg( nPair.first );
      lg->debug( QString( "ConnectFragment::onDiscoverButtonSlot -> title: <%1> name: <%2> addr: <%3>" )
                     .arg( title )
                     .arg( nPair.second )
                     .arg( nPair.first ) );
      ui->deviceComboBox->addItem( title, devInfo.address().toString() );
      if ( ui->deviceComboBox->count() > 0 )
      {
        //
        // und wenn machbar die Auswahl gleich auf dieses Gerät setzten
        //
        ui->deviceComboBox->setCurrentIndex( ui->deviceComboBox->count() - 1 );
      }
    }
  }

  void ConnectFragment::onCurrentIndexChangedSlot( int index )
  {
    lg->debug( QString( "ConnectFragment::onCurrentIndexChangedSlot -> index changed to <%1>. addr: <%2>" )
                   .arg( index, 2, 10, QChar( '0' ) )
                   .arg( ui->deviceComboBox->itemData( index ).toString() ) );
  }

  // ##########################################################################
  // PRIVATE Funktionen
  // ##########################################################################

  void ConnectFragment::fillDevicesList()
  {
    //
    // Die Gerätebox leeren
    //
    ui->deviceComboBox->clear();
    //
    // Liste der Schlüssel (die MAC)
    //
    QStringList dKeys = devices.keys();
    //
    // die Liste durch gehen
    //
    // for ( QStringList::iterator keyMacIter = dKeys.begin(); keyMacIter != dKeys.end(); keyMacIter++ )
    for ( auto &keyMac : dKeys )
    {
      //
      // die Elemente zu Titel zusammenfügen
      //
      QPair< QString, QString > nPair( devices.take( keyMac ) );
      QString title = QString( "%1(%2)" ).arg( nPair.second ).arg( nPair.first );
      lg->debug( QString( "ConnectFragment::fillDevicesList -> title: <%1> name: <%2> addr: <%3>" )
                     .arg( title )
                     .arg( nPair.second )
                     .arg( nPair.first ) );
      ui->deviceComboBox->addItem( title, keyMac );
    }
  }

  void ConnectFragment::setGuiConnected( bool isConnected )
  {
    //
    // setzte die GUI Elemente entsprechend des Online Status
    //
    isConnected ? ui->connectButton->setText( tr( "DISCONNECT DEVICE" ) ) : ui->connectButton->setText( tr( "CONNECT DEVICE" ) );
    ui->deviceComboBox->setEnabled( !isConnected );
    ui->propertyPushButton->setEnabled( !isConnected );
    ui->discoverPushButton->setEnabled( !isConnected );
  }
}  // namespace spx
