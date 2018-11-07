#include "ConnectFragment.hpp"
#include "ui_ConnectFragment.h"

namespace spx
{
  ConnectFragment::ConnectFragment( QWidget *parent,
                                    std::shared_ptr< Logger > logger,
                                    std::shared_ptr< SPX42Database > spx42Database,
                                    std::shared_ptr< SPX42Config > spxCfg )
      : QWidget( parent ), IFragmentInterface( logger, spx42Database, spxCfg ), ui( new Ui::connectForm )
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
  }

  ConnectFragment::~ConnectFragment()
  {
    lg->debug( "ConnectFragment::~ConnectFragment..." );
    // delete ui;
  }

  void ConnectFragment::onOnlineStatusChangedSlot( bool )
  {
    // TODO: was machen
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
    lg->debug( "ConnectFragment::onConnectButtonSlot -> connect button clicked." );
    // TODO: BT Verbinden!
    // TODO: Button auf disconnect setzten
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
      // TODO: was mache ich mit dem Gerät nun?
      //

      //
      // dioe Geräteliste neu befüllen
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

}  // namespace spx
