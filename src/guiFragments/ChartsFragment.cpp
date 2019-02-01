#include "ChartsFragment.hpp"
#include "ui_ChartsFragment.h"

namespace spx
{
  //
  // TODO: zoomfunktion (Beispiel in den QT Beispielen)
  //
  ChartsFragment::ChartsFragment( QWidget *parent,
                                  std::shared_ptr< Logger > logger,
                                  std::shared_ptr< SPX42Database > spx42Database,
                                  std::shared_ptr< SPX42Config > spxCfg,
                                  std::shared_ptr< SPX42RemotBtDevice > remSPX42 )
      : QWidget( parent )
      , IFragmentInterface( logger, spx42Database, spxCfg, remSPX42 )
      , ui( new Ui::ChartsFragment )
      , diveChart( nullptr )
      , dummyChart( new QtCharts::QChart() )
      , chartView( std::unique_ptr< QtCharts::QChartView >( new QtCharts::QChartView( dummyChart ) ) )
      , chartWorker( std::unique_ptr< ChartDataWorker >( new ChartDataWorker( logger, database, this ) ) )
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
    chartView->setRenderHint( QPainter::Antialiasing );
    initDeviceSelection();
    if ( ui->diveSelectComboBox->currentIndex() > -1 )
      onDiveComboChangedSlot( ui->diveSelectComboBox->currentIndex() );
    connect( ui->deviceSelectComboBox, static_cast< void ( QComboBox::* )( int ) >( &QComboBox::currentIndexChanged ), this,
             &ChartsFragment::onDeviceComboChangedSlot );
    connect( ui->diveSelectComboBox, static_cast< void ( QComboBox::* )( int ) >( &QComboBox::currentIndexChanged ), this,
             &ChartsFragment::onDiveComboChangedSlot );
    connect( chartWorker.get(), &ChartDataWorker::onChartReadySig, this, &ChartsFragment::onChartReadySlot );
  }

  ChartsFragment::~ChartsFragment()
  {
    lg->debug( "ChartsFragment::~ChartsFragment..." );
    //
    // das Ding einfügen und vom destruktor entsorgen lassen
    //
    chartView->setChart( dummyChart );
    delete diveChart;
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
    if ( spxDevicesAliasHash.keys().count() < 3 )
    {
      //
      // einen Dummy Eintrag einfügen, damit Einträge hzu sehen sind,
      // scheint im Framework nicht gut zu passen
      //
      ui->deviceSelectComboBox->addItem( " ", "" );
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
    chartView->setChart( dummyChart );
    if ( deviceAddr.isEmpty() )
    {
      return;
    }
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
    //
    // zuerst das Chart entfernen
    //
    chartView->setChart( dummyChart );
    diveChart->deleteLater();
    //
    // nichts markiert => dann bin ich schon fertig
    //
    if ( index == -1 || deviceAddr.isEmpty() )
      return;
    int diveNum = ui->diveSelectComboBox->itemData( index ).toInt();
    lg->debug( QString( "ChartsFragment::onDiveComboChangedSlot -> change to dive #%1..." ).arg( diveNum, 3, 10, QChar( '0' ) ) );
    // chartView->setChart( diveChart.get() );
    if ( dbgetDataFuture.isFinished() )
    {
      // QThreadPool::globalInstance()
      diveChart = new QtCharts::QChart();
      chartWorker->prepareDiveChart( diveChart );
      chartView->setChart( diveChart );
      dbgetDataFuture = QtConcurrent::run( chartWorker.get(), &ChartDataWorker::makeDiveChart, diveChart, deviceAddr, diveNum );
    }
    else
    {
      // später nochmal...
      lg->debug( "ChartsFragment::onDiveComboChangedSlot -> last chart is under construction, tzry later (automatic) again..." );
      QTimer::singleShot( 100, this, [=]() { this->onDiveComboChangedSlot( index ); } );
    }
  }

  void ChartsFragment::onChartReadySlot()
  {
    // QTimer::singleShot( 20, this, [=]() { ChartsFragment::onDiveComboChangedSlot( int index ); } );
    lg->debug( "ChartsFragment::onChartReadySlot" );
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
