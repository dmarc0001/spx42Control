#include "ChartsFragment.hpp"
#include "ui_ChartsFragment.h"

namespace spx
{
  ChartsFragment::ChartsFragment( QWidget *parent,
                                  std::shared_ptr< Logger > logger,
                                  std::shared_ptr< SPX42Database > spx42Database,
                                  std::shared_ptr< SPX42Config > spxCfg,
                                  std::shared_ptr< SPX42RemotBtDevice > remSPX42 )
      : QWidget( parent )
      , IFragmentInterface( logger, spx42Database, spxCfg, remSPX42 )
      , ui( new Ui::ChartsFragment )
      , diveChart( std::unique_ptr< DiveChart >( new DiveChart( logger, spx42Database ) ) )
      , dummyChart( new QtCharts::QChart() )
      , chartView( std::unique_ptr< QtCharts::QChartView >( new QtCharts::QChartView( dummyChart ) ) )
  {
    lg->debug( "ChartsFragment::ChartsFragment..." );
    deviceAddr.clear();
    ui->setupUi( this );
    onConfLicChangedSlot();
    fragmentTitleOfflinePattern = tr( "LOGFILES SPX42 [%1] in database" );
    connect( spxConfig.get(), &SPX42Config::licenseChangedSig, this, &ChartsFragment::onConfLicChangedSlot );
    // tausche den Platzhalter aus und entsorge den gleich
    // kopiere die policys und größe
    chartView->setMinimumSize( ui->placeHolderWidget->minimumSize() );
    chartView->setSizePolicy( ui->placeHolderWidget->sizePolicy() );
    delete ui->chartFrame->layout()->replaceWidget( ui->placeHolderWidget, chartView.get() );
    chartView->setChart( dummyChart );
    initDeviceSelection();
    connect( ui->deviceSelectComboBox, static_cast< void ( QComboBox::* )( int ) >( &QComboBox::currentIndexChanged ), this,
             &ChartsFragment::onDeviceComboChangedSlot );
    connect( ui->diveSelectComboBox, static_cast< void ( QComboBox::* )( int ) >( &QComboBox::currentIndexChanged ), this,
             &ChartsFragment::onDiveComboChangedSlot );
  }

  ChartsFragment::~ChartsFragment()
  {
    lg->debug( "ChartsFragment::~ChartsFragment..." );
    // setze wieder den Dummy ein und lasse den
    // die Objekte im ChartView entsorgen
    chartView->setChart( dummyChart );
  }

  void ChartsFragment::initDeviceSelection( void )
  {
    ui->deviceSelectComboBox->clear();
    ui->diveSelectComboBox->clear();
    ui->tabHeaderLabel->setText( fragmentTitleOfflinePattern.arg( tr( "unknown" ) ) );
    //
    // Geräte auswahlbox box füllen
    //
    spxDevicesAliasHash = database->getDeviceAliasHash();
    if ( spxDevicesAliasHash.isEmpty() )
    {
      //
      // nix in der Datenbank, dummy zeigen
      //
      ui->deviceSelectComboBox->addItem( tr( "database empty" ), 0 );
      deviceAddr.clear();
      return;
    }
    //
    // Daten gefunden, fülle die combobox
    //
    for ( auto deviceKey : spxDevicesAliasHash.keys() )
    {
      //
      // alles in die Combobox schreiben, TAG ist MAC Addr
      //
      SPX42DeviceAlias devAlias = spxDevicesAliasHash.value( deviceKey );
      QString title = QString( "%1 (%2)" ).arg( devAlias.alias ).arg( devAlias.name );
      ui->deviceSelectComboBox->addItem( title, devAlias.mac );
    }
    QString mac = database->getLastConnected();
    if ( !mac.isEmpty() )
    {
      lg->debug( QString( "LogFragment::setGuiConnected -> last connected was: " ).append( mac ) );
      //
      // verbunden oder nicht, versuche etwas zu selektiern
      //
      // suche nach diesem Eintrag...
      //
      int index = ui->deviceSelectComboBox->findData( mac );
      if ( index != ui->deviceSelectComboBox->currentIndex() && index != -1 )
      {
        lg->debug( QString( "LogFragment::setGuiConnected -> found at idx %1, set to idx" ).arg( index ) );
        //
        // -1 for not found
        // und der index ist nicht schon auf diesen Wert gesetzt
        // also, wenn gefunden, selektiere diesen Eintrag
        //
        ui->deviceSelectComboBox->setCurrentIndex( index );
        onDeviceComboChangedSlot( index );
      }
      else
      {
        //
        // der index ist entweder schon gesetzt oder keiner
        //
        if ( index == -1 )
          // setzte den ersten Eintrag ==> löst slot aus
          ui->deviceSelectComboBox->setCurrentIndex( index );
        else
        {
          // slot manuell starten mit akteuellen index
          onDeviceComboChangedSlot( index );
        }
      }
    }
  }

  void ChartsFragment::onDeviceComboChangedSlot( int index )
  {
    ui->diveSelectComboBox->clear();
    deviceAddr = ui->deviceSelectComboBox->itemData( index ).toString();
    lg->debug( QString( "ChartsFragment::onDeviceComboChangedSlot -> index changed to <%1>. addr: <%2>" )
                   .arg( index, 2, 10, QChar( '0' ) )
                   .arg( deviceAddr ) );
    ui->tabHeaderLabel->setText( fragmentTitleOfflinePattern.arg( database->getAliasForMac( deviceAddr ) ) );
    // Daten in der DB
    lg->debug( "ChartsFragment::onDeviceComboChangedSlot -> fill log directory list from database..." );
    SPX42LogDirectoryEntryListPtr dirList = database->getLogentrysForDevice( deviceAddr );
    //
    // Alle Einträge sortiert in die Liste
    //
    auto sortKeys = dirList.get()->keys();
    std::sort( sortKeys.begin(), sortKeys.end() );
    for ( auto entr : sortKeys )
    {
      SPX42LogDirectoryEntry dEntry = dirList->value( entr );
      // eintrag in die Liste, Tauchgangsnummer als TAG
      ui->diveSelectComboBox->addItem( QString( "%1:[%2]" ).arg( dEntry.number, 3, 10, QChar( '0' ) ).arg( dEntry.getDateTimeStr() ),
                                       dEntry.number );
    }
  }

  void ChartsFragment::onDiveComboChangedSlot( int index )
  {
    int diveNum = ui->diveSelectComboBox->itemData( index ).toInt();
    lg->debug( QString( "ChartsFragment::onDiveComboChangedSlot -> change to dive #%1..." ).arg( diveNum, 3, 10, QChar( '0' ) ) );
    // lg->debug( QString( "ChartsFragment::onDiveComboChangedSlot -> change to dive #%1..." ).arg( diveNum ) );
  }

  void ChartsFragment::onOnlineStatusChangedSlot( bool )
  {
    // IGNORIEREN
  }

  void ChartsFragment::onSocketErrorSlot( QBluetoothSocket::SocketError )
  {
    // IGNORIEREN
  }

  void ChartsFragment::onConfLicChangedSlot()
  {
    lg->debug( QString( "ChartsFragment::onConfLicChangedSlot -> set: %1" )
                   .arg( static_cast< int >( spxConfig->getLicense().getLicType() ) ) );
    ui->tabHeaderLabel->setText(
        QString( tr( "LOGCHARTS SPX42 Serial [%1] Lic: %2" ).arg( spxConfig->getSerialNumber() ).arg( spxConfig->getLicName() ) ) );
  }

  void ChartsFragment::onCloseDatabaseSlot()
  {
    // TODO: implementieren
  }

  void ChartsFragment::onCommandRecivedSlot()
  {
    // IGNORIEREN
  }
}  // namespace spx
