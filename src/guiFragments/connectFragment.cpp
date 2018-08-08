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
    // FIXME: zur Ansicht einfach Elemente einfügen
    //
    ui->deviceComboBox->addItem( "EINS", 1 );
    ui->deviceComboBox->addItem( "ZWEI", 2 );
    ui->deviceComboBox->addItem( "DREI", 3 );
    ui->deviceComboBox->addItem( "VIER", 4 );
    onConfLicChangedSlot();
    connect( spxConfig.get(), &SPX42Config::licenseChangedSig, this, &ConnectFragment::onConfLicChangedSlot );
    connect( ui->connectButton, &QPushButton::clicked, this, &ConnectFragment::connectButtonSlot );
    connect( ui->propertyPushButton, &QPushButton::clicked, this, &ConnectFragment::propertyButtonSlot );
    connect( ui->discoverPushButton, &QPushButton::clicked, this, &ConnectFragment::discoverButtonSlot );
    connect( ui->deviceComboBox, static_cast< void ( QComboBox::* )( int ) >( &QComboBox::currentIndexChanged ), this,
             &ConnectFragment::currentIndexChangedSlot );
  }

  ConnectFragment::~ConnectFragment()
  {
    lg->debug( "ConnectFragment::~ConnectForm..." );
    // delete ui;
  }

  void ConnectFragment::onOnlineStatusChangedSlot( bool isOnline )
  {
    // TODO: was machen
  }

  void ConnectFragment::onConfLicChangedSlot( void )
  {
    // TODO was machen
  }

  void ConnectFragment::onCloseDatabaseSlot( void )
  {
    // TODO: implementieren
  }

  void ConnectFragment::connectButtonSlot( void )
  {
    lg->debug( "ConnectFragment::connectButtonSlot -> connect button clicked." );
  }

  void ConnectFragment::propertyButtonSlot( void )
  {
    lg->debug( "ConnectFragment::propertyButtonSlot -> property button clicked." );
  }

  void ConnectFragment::discoverButtonSlot( void )
  {
    lg->debug( "ConnectFragment::discoverButtonSlot -> discover button clicked." );
    //
    // den Dialog erzeugen
    //
    BtDiscoverDialog discoverDialog( lg, database );
    if ( QDialog::Accepted == discoverDialog.exec() )
    {
      //
      // Ergebnis übernehmen
      //
      lg->debug( "ConnectFragment::discoverButtonSlot -> discovering dialog returned: accept result...." );
      SPXDeviceList devList( discoverDialog.getSPX42Devices() );
      lg->debug( QString( "ConnectFragment::discoverButtonSlot -> scanned %1 devices" ).arg( devList.count(), 2, 10, QChar( '0' ) ) );
      QBluetoothDeviceInfo devInfo( discoverDialog.getActivatedDevice() );
      //
      // ein Gerät gültig markiert?
      //
      if ( devInfo.isValid() )
      {
        lg->debug( QString( "ConnectFragment::discoverButtonSlot -> selected device <%1>" ).arg( devInfo.address().toString() ) );
      }
      else
      {
        lg->debug( "ConnectFragment::discoverButtonSlot -> not an device selected..." );
      }
    }
  }

  void ConnectFragment::currentIndexChangedSlot( int index )
  {
    lg->debug( QString( "ConnectFragment::currentIndexChangedSlot -> index changed to <%1>." ).arg( index, 2, 10, QChar( '0' ) ) );
  }
}
