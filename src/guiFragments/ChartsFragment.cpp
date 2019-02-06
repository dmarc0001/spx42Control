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
      , bigDiveChart( nullptr )
      , ppo2DiveChart( nullptr )
      , bigDummyChart( new QtCharts::QChart() )
      , ppo2DummyChart( new QtCharts::QChart() )
      , bigChartView( std::unique_ptr< QtCharts::QChartView >( new QtCharts::QChartView( bigDummyChart ) ) )
      , ppo2ChartView( std::unique_ptr< QtCharts::QChartView >( new QtCharts::QChartView( ppo2DummyChart ) ) )
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
    bigChartView->setMinimumSize( ui->placeHolderWidget->minimumSize() );
    bigChartView->setSizePolicy( ui->placeHolderWidget->sizePolicy() );
    delete ui->chartFrame->layout()->replaceWidget( ui->placeHolderWidget, bigChartView.get() );
    bigChartView->setChart( bigDummyChart );
    bigChartView->setRenderHint( QPainter::Antialiasing );
    bigChartView->setRubberBand( QChartView::HorizontalRubberBand );
    //
    ppo2ChartView->setMinimumSize( ui->placeHolderWidget2->minimumSize() );
    ppo2ChartView->setSizePolicy( ui->placeHolderWidget2->sizePolicy() );
    ppo2ChartView->setMaximumSize( ui->placeHolderWidget2->maximumSize() );
    ppo2ChartView->setMinimumSize( ui->placeHolderWidget2->minimumSize() );
    delete ui->chartFrame->layout()->replaceWidget( ui->placeHolderWidget2, ppo2ChartView.get() );
    //
    ppo2ChartView->setChart( ppo2DummyChart );
    ppo2ChartView->setRenderHint( QPainter::Antialiasing );
    ppo2ChartView->setRubberBand( QChartView::HorizontalRubberBand );
    //
    ui->notesLineEdit->setClearButtonEnabled( true );
    initDeviceSelection();
    if ( ui->diveSelectComboBox->currentIndex() > -1 )
      onDiveComboChangedSlot( ui->diveSelectComboBox->currentIndex() );
    connect( ui->deviceSelectComboBox, static_cast< void ( QComboBox::* )( int ) >( &QComboBox::currentIndexChanged ), this,
             &ChartsFragment::onDeviceComboChangedSlot );
    connect( ui->diveSelectComboBox, static_cast< void ( QComboBox::* )( int ) >( &QComboBox::currentIndexChanged ), this,
             &ChartsFragment::onDiveComboChangedSlot );
    connect( chartWorker.get(), &ChartDataWorker::onChartReadySig, this, &ChartsFragment::onChartReadySlot );
    connect( ui->notesLineEdit, &QLineEdit::editingFinished, this, &ChartsFragment::onNotesLineEditFinishedSlot );
  }

  ChartsFragment::~ChartsFragment()
  {
    lg->debug( "ChartsFragment::~ChartsFragment..." );
    //
    // das Ding einfügen und vom destruktor entsorgen lassen
    //
    bigChartView->setChart( bigDummyChart );
    ppo2ChartView->setChart( ppo2DummyChart );
    delete bigDiveChart;
    delete ppo2DiveChart;
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
    bigChartView->setChart( bigDummyChart );
    ppo2ChartView->setChart( ppo2DummyChart );
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
    bigChartView->setChart( bigDummyChart );
    ppo2ChartView->setChart( ppo2DummyChart );
    bigDiveChart->deleteLater();
    ppo2DiveChart->deleteLater();
    //
    // nichts markiert => dann bin ich schon fertig
    //
    if ( index == -1 || deviceAddr.isEmpty() )
      return;
    diveNum = ui->diveSelectComboBox->itemData( index ).toInt();
    if ( diveNum == -1 )
      return;
    lg->debug( QString( "ChartsFragment::onDiveComboChangedSlot -> change to dive #%1..." ).arg( diveNum, 3, 10, QChar( '0' ) ) );
    // chartView->setChart( diveChart.get() );
    if ( dbgetDataFuture.isFinished() )
    {
      // QThreadPool::globalInstance()
      bigDiveChart = new QtCharts::QChart( nullptr );
      ppo2DiveChart = new QtCharts::QChart( nullptr );
      chartWorker->prepareDiveCharts( bigDiveChart, ppo2DiveChart );
      bigChartView->setChart( bigDiveChart );
      ppo2ChartView->setChart( ppo2DiveChart );
      ui->notesLineEdit->setText( database->getNotesForDive( deviceAddr, diveNum ) );
      dbgetDataFuture =
          QtConcurrent::run( chartWorker.get(), &ChartDataWorker::makeDiveChart, bigDiveChart, ppo2DiveChart, deviceAddr, diveNum );
    }
    else
    {
      // später nochmal...
      lg->debug( "ChartsFragment::onDiveComboChangedSlot -> last chart is under construction, try later (automatic) again..." );
      QTimer::singleShot( 100, this, [=]() { this->onDiveComboChangedSlot( index ); } );
    }
  }

  void ChartsFragment::onChartReadySlot()
  {
    // QTimer::singleShot( 20, this, [=]() { ChartsFragment::onDiveComboChangedSlot( int index ); } );
    lg->debug( "ChartsFragment::onChartReadySlot" );
  }

  void ChartsFragment::onNotesLineEditFinishedSlot()
  {
    lg->debug( "ChartsFragment::onNotesLineEditFinishedSlot -> edit finished..." );
    if ( database->writeNotesForDive( deviceAddr, diveNum, ui->notesLineEdit->text() ) )
    {
      lg->debug( "ChartsFragment::onNotesLineEditFinishedSlot -> notes saved...OK" );
    }
    else
    {
      lg->warn( "ChartsFragment::onNotesLineEditFinishedSlot -> notes NOT saved!" );
    }
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

  //  void ChartsFragment::keyPressEvent( QKeyEvent *event )
  //  {
  //    if ( ppo2DiveChart && bigDiveChart )
  //    {
  //      switch ( event->key() )
  //      {
  //        case Qt::Key_Plus:
  //          ppo2DiveChart->zoomIn();
  //          bigDiveChart->zoomIn();
  //          break;
  //        case Qt::Key_Minus:
  //          ppo2DiveChart->zoomOut();
  //          bigDiveChart->zoomOut();
  //          break;
  //          //![1]
  //        case Qt::Key_Left:
  //          ppo2DiveChart->scroll( -10, 0 );
  //          bigDiveChart->scroll( -10, 0 );
  //          break;
  //        case Qt::Key_Right:
  //          ppo2DiveChart->scroll( 10, 0 );
  //          bigDiveChart->scroll( 10, 0 );
  //          break;
  //        default:
  //          ChartsFragment::keyPressEvent( event );
  //          break;
  //      }
  //    }
  //    else
  //    {
  //      ChartsFragment::keyPressEvent( event );
  //    }
  //  }
}  // namespace spx

/*
void wheelEvent(QWheelEvent *event)
{
  if(chart() && mDirectionZoom != NotZoom){
    const qreal factor = 1.001;
    QRectF r = chart()->plotArea();
    QPointF c = r.center();
    qreal val = std::pow(factor, event->delta());
    if(mDirectionZoom & VerticalZoom)
      r.setHeight(r.height()*val);
    if (mDirectionZoom & HorizontalZoom) {
      r.setWidth(r.width()*val);
    }
    r.moveCenter(c);
    chart()->zoomIn(r);
  }
  QChartView::wheelEvent(event);
}
*/
