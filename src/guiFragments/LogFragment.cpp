﻿#include "LogFragment.hpp"
#include <QMessageBox>

using namespace QtCharts;
namespace spx
{
  /**
   * @brief Konstruktor für das Logfragment
   * @param parent Elternteil
   * @param logger der Logger
   * @param spxCfg Konfig des SPX
   */
  LogFragment::LogFragment( QWidget *parent,
                            std::shared_ptr< Logger > logger,
                            std::shared_ptr< SPX42Database > spx42Database,
                            std::shared_ptr< SPX42Config > spxCfg,
                            std::shared_ptr< SPX42RemotBtDevice > remSPX42,
                            AppConfigClass *appCfg )
      : QWidget( parent )
      , IFragmentInterface( logger, spx42Database, spxCfg, remSPX42, appCfg )
      , ui( new Ui::LogFragment() )
      , miniChart( new QtCharts::QChart() )
      , dummyChart( new QtCharts::QChart() )
      , chartView( std::unique_ptr< QtCharts::QChartView >( new QtCharts::QChartView( dummyChart ) ) )
      , chartWorker( std::unique_ptr< ChartDataWorker >( new ChartDataWorker( logger, database, this ) ) )
      , logWriter( std::unique_ptr< LogDetailWalker >(
            new LogDetailWalker( this, logger, spx42Database, spxCfg, remSPX42->getRemoteConnected() ) ) )
      , xmlExport( logger, spx42Database, this )
      , savedIcon( ( appCfg->getGuiThemeName().compare( AppConfigClass::lightStr ) == 0 ) ? ":/icons/saved_black"
                                                                                          : ":/icons/saved_white" )
      , nullIcon()
      , offlineDeviceAddr()
  {
    *lg << LDEBUG << "LogFragment::LogFragment..." << Qt::endl;
    ui->setupUi( this );
    exportPath = QStandardPaths::writableLocation( QStandardPaths::DownloadLocation );
    ui->transferProgressBar->setVisible( false );
    ui->transferProgressBar->setRange( 0, 0 );
    // tableview zurecht machen
    ui->logentryTableWidget->setEditTriggers( QAbstractItemView::NoEditTriggers );
    ui->logentryTableWidget->setSelectionMode( QAbstractItemView::ExtendedSelection );
    ui->logentryTableWidget->setGridStyle( Qt::PenStyle::DotLine );
    ui->logentryTableWidget->showGrid();
    ui->logentryTableWidget->setColumnCount( 2 );
    ui->logentryTableWidget->horizontalHeader()->setSectionResizeMode( 0, QHeaderView::Stretch );
    ui->logentryTableWidget->setColumnWidth( 1, 25 );
    ui->logentryTableWidget->setSelectionBehavior( QAbstractItemView::SelectRows );
    //
    ui->dbWriteNumLabel->setVisible( false );
    ui->diveLogReadLabel->setVisible( false );
    fragmentTitlePattern = tr( "LOGFILES SPX42 Serial [%1] LIC: %2" );
    fragmentTitleOfflinePattern = tr( "LOGFILES SPX42 [%1] in database" );
    diveNumberStr = tr( "DIVE NUMBER: %1" );
    diveDateStr = tr( "DIVE DATE: %1" );
    diveDepthStr = tr( "DIVE DEPTH: %1m, LEN: %2 min" );
    diveDepthShortStr = tr( "DIVE DEPTH: %1m" );
    dbWriteNumTemplate = tr( "WRITE DIVE #%1 TO DB..." );
    dbWriteNumIDLE = tr( "WAIT FOR START..." );
    readetLogTemplate = tr( "READ FROM DEVICE #%1" );
    dbDeleteNumTemplate = tr( "DELETE DIVE %1 DONE." );
    exportDiveStartTemplate = tr( "EXPORT DIVE #%1..." );
    exportDiveEndTemplate = tr( "EXPORT DIVE #%1 DONE." );
    exportDiveErrorTemplate = tr( "EXPORT FAILED!" );
    ui->diveNumberLabel->setText( diveNumberStr.arg( "-" ) );
    ui->diveDateLabel->setText( diveDateStr.arg( "-" ) );
    ui->diveDepthLabel->setText( diveDepthShortStr.arg( "-" ) );
    ui->dbWriteNumLabel->setText( dbWriteNumIDLE );
    ui->diveLogReadLabel->clear();
    ui->deleteContentPushButton->setEnabled( false );
    ui->exportContentPushButton->setEnabled( false );
    //
    // Dummy Chart Thema
    //
    if ( appConfig->getGuiThemeName().compare( AppConfigClass::lightStr ) == 0 )
    {
      miniChart->setTheme( QChart::ChartTheme::ChartThemeLight );
      dummyChart->setTheme( QChart::ChartTheme::ChartThemeLight );
    }
    else
    {
      miniChart->setTheme( QChart::ChartTheme::ChartThemeDark );
      dummyChart->setTheme( QChart::ChartTheme::ChartThemeDark );
    }
    chartView->setMinimumHeight( 180 );
    chartView->setMaximumHeight( 250 );
    ui->logDetailsGroupBox->layout()->addWidget( chartView.get() );
    // GUI dem Onlinestatus entsprechend vorbereiten
    setGuiConnected( remoteSPX42->getConnectionStatus() == SPX42RemotBtDevice::SPX42_CONNECTED );
    // ist der online gleich noch die Lizenz setzten
    if ( remoteSPX42->getConnectionStatus() == SPX42RemotBtDevice::SPX42_CONNECTED )
      this->onConfLicChangedSlot();
    logWriter->start();
    //
    // signale verbinden
    //
    connect( spxConfig.get(), &SPX42Config::licenseChangedSig, this, &LogFragment::onConfLicChangedSlot );
    connect( ui->deviceSelectComboBox, static_cast< void ( QComboBox::* )( int ) >( &QComboBox::currentIndexChanged ), this,
             &LogFragment::onDeviceComboChangedSlot );
    connect( ui->readLogdirPushButton, &QPushButton::clicked, this, &LogFragment::onReadLogDirectoryClickSlot );
    connect( ui->readLogContentPushButton, &QPushButton::clicked, this, &LogFragment::onReadLogContentClickSlot );
    connect( ui->logentryTableWidget, &QAbstractItemView::clicked, this, &LogFragment::onLogListClickeSlot );
    connect( ui->logentryTableWidget, &QTableWidget::itemSelectionChanged, this, &LogFragment::itemSelectionChangedSlot );
    connect( ui->deleteContentPushButton, &QPushButton::clicked, this, &LogFragment::onDeleteLogDetailClickSlot );
    connect( ui->exportContentPushButton, &QPushButton::clicked, this, &LogFragment::onExportLogDetailClickSlot );
    connect( remoteSPX42.get(), &SPX42RemotBtDevice::onStateChangedSig, this, &LogFragment::onOnlineStatusChangedSlot );
    connect( remoteSPX42.get(), &SPX42RemotBtDevice::onSocketErrorSig, this, &LogFragment::onSocketErrorSlot );
    connect( remoteSPX42.get(), &SPX42RemotBtDevice::onCommandRecivedSig, this, &LogFragment::onCommandRecivedSlot );
    connect( &transferTimeout, &QTimer::timeout, this, &LogFragment::onTransferTimeoutSlot );
    connect( logWriter.get(), &LogDetailWalker::onWriteDiveStartSig, this, &LogFragment::onWriteDiveStartSlot );
    connect( logWriter.get(), &LogDetailWalker::onWriteDiveDoneSig, this, &LogFragment::onWriteDiveDoneSlot );
    connect( logWriter.get(), &LogDetailWalker::onWriteFinishedSig, this, &LogFragment::onWriteFinishedSlot );
    connect( logWriter.get(), &LogDetailWalker::onWriteCritSig, this, &LogFragment::onWriterCritSlot );
    connect( &xmlExport, &SPX42UDDFExport::onExportStartSig, this, &LogFragment::onExportSingleDiveStartSlot );
    connect( &xmlExport, &SPX42UDDFExport::onExportEndSingleDiveSig, this, &LogFragment::onExportSingleDiveEndSlot );
    connect( &xmlExport, &SPX42UDDFExport::onExportEndSig, this, &LogFragment::onExportEndSlot );
  }

  // UNFRTIG IMPLEMENTIERT, PRIVATE
  LogFragment::LogFragment( const LogFragment &lf )
      : IFragmentInterface( lf.lg, lf.database, lf.spxConfig, lf.remoteSPX42, lf.appConfig )
      , miniChart( new QtCharts::QChart() )
      , dummyChart( new QtCharts::QChart() )
      , chartView( std::unique_ptr< QtCharts::QChartView >( new QtCharts::QChartView( dummyChart ) ) )
      , chartWorker( std::unique_ptr< ChartDataWorker >( new ChartDataWorker( lg, database, this ) ) )
      , logWriter( std::unique_ptr< LogDetailWalker >(
            new LogDetailWalker( this, lf.lg, lf.database, lf.spxConfig, lf.remoteSPX42->getRemoteConnected() ) ) )
      , xmlExport( lg, database, this )
  {
  }

  LogFragment &LogFragment::operator=( const LogFragment & )
  {
    miniChart = new QtCharts::QChart();
    dummyChart = new QtCharts::QChart();
    *lg << LCRIT << "Operator \"=\" -> not implemented!" << Qt::endl;
    return *this;
  }

  /**
   * @brief Der Destruktor, Aufräumarbeiten
   */
  LogFragment::~LogFragment()
  {
    *lg << LDEBUG << "LogFragment::~LogFragment..." << Qt::endl;
    // setze wieder den Dummy ein und lasse den
    // die Objekte im ChartView entsorgen
    chartView->setChart( dummyChart );
    spxConfig->disconnect( this );
    logWriter->disconnect();
    logWriter->setThreadEnd( true );
    xmlExport.disconnect();
    remoteSPX42->disconnect( this );
    if ( detailDeleterThread )
    {
      detailDeleterThread->disconnect();
      if ( !detailDeleterThread->isFinished() )
      {
        detailDeleterThread->quit();
        // detailDeleterThread->deleteLater();
      }
    }
    logWriter->quit();
    logWriter->wait();
    *lg << LDEBUG << "LogFragment::~LogFragment...OK" << Qt::endl;
  }

  void LogFragment::setExportPath( const QString &_export )
  {
    exportPath = _export;
  }

  /**
   * @brief LogFragment::onSendBufferStateChangedSlot
   * @param isBusy
   */
  void LogFragment::onSendBufferStateChangedSlot( bool isBusy )
  {
    ui->transferProgressBar->setVisible( isBusy );
  }

  /**
   * @brief Systemänderungen
   * @param e event
   */
  void LogFragment::changeEvent( QEvent *e )
  {
    QWidget::changeEvent( e );
    switch ( e->type() )
    {
      case QEvent::LanguageChange:
        ui->retranslateUi( this );
        break;
      default:
        break;
    }
  }

  /**
   * @brief LogFragment::onTransferTimeout
   */
  void LogFragment::onTransferTimeoutSlot()
  {
    //
    // wenn der Writer noch läuft, dann noch nicht den Balken ausblenden
    //
    *lg << LWARN << "LogFragment::onTransferTimeout -> transfer timeout!!!" << Qt::endl;
    if ( logWriter->isThreadSleeping() )
    {
      ui->transferProgressBar->setVisible( false );
      ui->dbWriteNumLabel->setText( dbWriteNumIDLE );
      ui->dbWriteNumLabel->setVisible( false );
      ui->diveLogReadLabel->setVisible( false );
      transferTimeout.stop();
      testForSavedDetails();
      // TODO: Warn oder Fehlermeldung ausgeben
    }
  }

  /**
   * @brief LogFragment::onWriterDoneSlot
   */
  void LogFragment::onWriteFinishedSlot( int /*count*/ )
  {
    *lg << LDEBUG << "LogFragment::onWriteFinishedSlot..." << Qt::endl;
    if ( logDetailRead.isEmpty() )
    {
      ui->transferProgressBar->setVisible( false );
      ui->dbWriteNumLabel->setText( dbWriteNumIDLE );
      ui->dbWriteNumLabel->setVisible( false );
      ui->diveLogReadLabel->setVisible( false );
      transferTimeout.stop();
    }
    testForSavedDetails();
    //
    // Writer ist fertig mit der gesamten Queue, er geht dann in den sleep mode
    //
  }

  /**
   * @brief LogFragment::onWriterCritSlot
   * @param err
   */
  void LogFragment::onWriterCritSlot( LOGWRITEERR err )
  {
    //
    // es trat ein kritischer Fehler beim Sichern auf
    // TODO: Messagebox
    //
    *lg << LCRIT << "LogFragment::onWriterCritSlot -> critical error nr <" << static_cast< int >( err ) << ">..." << Qt::endl;
    ui->transferProgressBar->setVisible( false );
    ui->dbWriteNumLabel->setText( dbWriteNumIDLE );
    ui->dbWriteNumLabel->setVisible( false );
    ui->diveLogReadLabel->setVisible( false );
  }

  /**
   * @brief LogFragment::onNewDiveStartSlot
   * @param newDiveNum
   */
  void LogFragment::onWriteDiveStartSlot( int newDiveNum )
  {
    //
    // ein neueer Tauchgang wird gesichert
    //
    ui->dbWriteNumLabel->setText( dbWriteNumTemplate.arg( newDiveNum, 3, 10, QChar( '0' ) ) );
    *lg << LDEBUG
        << QString( "LogFragment::onWriteDiveStartSlot -> write dive number #%1 to database..." )
               .arg( newDiveNum, 3, 10, QChar( '0' ) )
        << Qt::endl;
  }

  /**
   * @brief LogFragment::onNewDiveDoneSlot
   * @param diveNum
   */
  void LogFragment::onWriteDiveDoneSlot( int diveNum )
  {
    //
    // ein Tauchgang ist komplett gesichert
    // den aktuellen Eintrag korrigieren
    //
    QList< QTableWidgetItem * > items =
        ui->logentryTableWidget->findItems( QString( "%1:" ).arg( diveNum, 3, 10, QChar( '0' ) ), Qt::MatchStartsWith );
    if ( items.count() > 0 )
    {
      ui->logentryTableWidget->item( items.at( 0 )->row(), 1 )->setIcon( savedIcon );
      ui->logentryTableWidget->viewport()->update();
    }
  }

  /**
   * @brief Slot für das Signal von button zum Directory lesen
   */
  void LogFragment::onReadLogDirectoryClickSlot()
  {
    *lg << LDEBUG << "LogFragment::onReadLogDirectorySlot: ..." << Qt::endl;
    if ( remoteSPX42->getConnectionStatus() == SPX42RemotBtDevice::SPX42_CONNECTED )
    {
      //
      // Liste löschen
      //
      spxConfig->resetConfig( SPX42ConfigClass::CF_CLASS_LOG );
      // GUI löschen
      ui->logentryTableWidget->setRowCount( 0 );
      // chart dummy setzten
      chartView->setChart( dummyChart );
      // Timeout starten
      ui->transferProgressBar->setVisible( true );
      transferTimeout.start( TIMEOUTVAL );
      //
      // rufe die Liste der Einträge vom Computer ab
      //
      SendListEntry sendCommand = remoteSPX42->askForLogDir();
      remoteSPX42->sendCommand( sendCommand );
      *lg << LDEBUG << "LogFragment::onReadLogDirectorySlot -> send cmd get log directory..." << Qt::endl;
    }
  }

  /**
   * @brief Slot für das Signal vom button zum Inhalt des Logs lesen
   */
  void LogFragment::onReadLogContentClickSlot()
  {
    *lg << LDEBUG << "LogFragment::onReadLogContentClickSlot: ..." << Qt::endl;
    QModelIndexList indexList = ui->logentryTableWidget->selectionModel()->selectedIndexes();
    if ( ui->logentryTableWidget->rowCount() == 0 )
    {
      *lg << LWARN << "LogFragment::onReadLogContentClickSlot -> no log entrys!" << Qt::endl;
      return;
    }
    //
    // gibt es was zu tun?
    //
    if ( indexList.isEmpty() )
    {
      *lg << LWARN << "LogFragment::onReadLogContentClickSlot -> nothing selected, read all?" << Qt::endl;
      // TODO: Messagebox aufpoppen und Nutzer fragen
      return;
    }
    //
    // so, fülle mal die Queue mit den zu lesenden Nummern
    //
    logDetailRead.clear();
    int countDetails = 0;
    for ( auto idxEntry : indexList )
    {
      //
      // die spalte 0 finden, da steht die Lognummer
      //
      int row = idxEntry.row();
      int col = idxEntry.column();
      if ( col == 0 )
      {
        QString entry = ui->logentryTableWidget->item( row, 0 )->text();
        *lg << LDEBUG << "LogFragment::onReadLogContentClickSlot -> entry: " << entry << "..." << Qt::endl;
        QStringList el = entry.split( ':' );
        // in die Liste kommt die Nummer!
        if ( !el.isEmpty() && el.count() > 1 )
        {
          ++countDetails;
          *lg << LDEBUG << "LogFragment::onReadLogContentClickSlot -> entry: " << entry << "...OK" << Qt::endl;
          logDetailRead.enqueue( el.at( 0 ).toInt() );
        }
      }
    }
    *lg << LDEBUG << "LogFragment::onReadLogContentClickSlot -> to read " << countDetails << " logs from spx42..." << Qt::endl;

    //
    // und nun starte ich die Ereigniskette
    //
    if ( !logDetailRead.isEmpty() )
    {
      //
      // GUI anzeigen...
      //
      ui->transferProgressBar->setVisible( true );
      //
      // den ersten Detaileintrag abrufen
      //
      int logDetailNum = logDetailRead.dequeue();
      SendListEntry sendCommand = remoteSPX42->askForLogDetailFor( logDetailNum );
      remoteSPX42->sendCommand( sendCommand );
      //
      // das kann etwas dauern...
      //
      transferTimeout.start( TIMEOUTVAL * 8 );
      *lg << LDEBUG << "LogFragment::onReadLogContentClickSlot -> request log nr " << logDetailNum << " from spx42..." << Qt::endl;
    }
  }

  /**
   * @brief Slot für das Signal wenn auf einen Log Directory Eintrag geklickt wird
   * @param index welcher Index war es?
   */
  void LogFragment::onLogListClickeSlot( const QModelIndex &index )
  {
    QString depthStr = " ? ";
    if ( index.isValid() )
    {
      int row = index.row();
      // items finden
      QTableWidgetItem *clicked1stItem = ui->logentryTableWidget->item( row, 0 );
      QTableWidgetItem *clicked2ndItem = ui->logentryTableWidget->item( row, 1 );
      if ( clicked1stItem && clicked2ndItem )
      {
        int diveNum = 0;
        QString deviceAddr;
        // Aus dem Text die Nummer extraieren
        QString entry = clicked1stItem->text();
        *lg << LDEBUG << "LogFragment::onLogListClickeSlot: data: " << entry << "..." << Qt::endl;
        // die Nummer des dives extraieren
        // und den Datumsstring für die Anzeige extraieren
        //
        // [08.07.2012 12:13:46]
        // oder
        // [2012/07/08 12:13:46]
        //
        QStringList pieces = entry.split( ':' );
        diveNum = pieces.at( 0 ).toInt();
        int start = entry.indexOf( '[' ) + 1;
        int end = entry.indexOf( ']' );
        ui->diveNumberLabel->setText( diveNumberStr.arg( diveNum, 3, 10, QChar( '0' ) ) );
        ui->diveDateLabel->setText( diveDateStr.arg( entry.mid( start, end - start ) ) );
        //
        // ist ein icon da == gibt es eine Sicherung?
        //
        if ( clicked2ndItem->icon().isNull() )
        {
          ui->diveDepthLabel->setText( diveDepthShortStr.arg( depthStr ) );
          chartView->setChart( dummyChart );
        }
        else
        {
          //
          // die Datenbank fragen, ob und wie tief
          // unterscheide ob ich online oder offline arbeite
          //
          if ( remoteSPX42->getConnectionStatus() == SPX42RemotBtDevice::SPX42_CONNECTED )
          {
            //
            // mit wem bin ich verbunden ==> dessen Daten lösche ich
            //
            deviceAddr = remoteSPX42->getRemoteConnected();
          }
          else
          {
            //
            // habe ich ein Gerät ausgewählt, wenn ja welches
            //
            deviceAddr = offlineDeviceAddr;
          }
          //
          // zuerst dummychart setzten
          //
          chartView->setChart( dummyChart );
          if ( !deviceAddr.isEmpty() )
          {
            double depth = ( database->getMaxDepthFor( deviceAddr, diveNum ) / 10.0 );
            int diveLen = database->getDiveLenFor( deviceAddr, diveNum );
            QString diveLenStr = QString( "%1:%2" )
                                     .arg( static_cast< int >( diveLen / 60 ), 2, 10, QChar( '0' ) )
                                     .arg( diveLen % 60, 2, 10, QChar( '0' ) );
            if ( depth > 0 )
            {
              //
              // tiefe eintragen
              //
              ui->diveDepthLabel->setText( diveDepthStr.arg( depth, 2, 'f', 1 ).arg( diveLenStr ) );
              //
              // das Chart anzeigen, wenn Daten vorhanden sind
              //
              if ( dbgetDataFuture.isFinished() )
              {
                //
                // starte die Datenabfrage als future...
                //
                dbgetDataFuture = QtConcurrent::run( chartWorker.get(), &ChartDataWorker::getFutureDiveDataSet, deviceAddr, diveNum );
                //
                // das timerevent prüft ob die Daten verfügbar sind
                //
                QTimer::singleShot( 100, this, [=]() { this->onDiveDataWaitFor( diveNum ); } );
              }
              else
              {
                // später nochmal...
                *lg << LDEBUG << "LogFragment::onLogListClickeSlot -> last chart is under construction, try later (automatic) again..."
                    << Qt::endl;
                QTimer::singleShot( 100, this, [=]() { this->onLogListClickeSlot( index ); } );
              }
            }
            else
            {
              ui->diveDepthLabel->setText( diveDepthShortStr.arg( depthStr ).arg( diveLenStr ) );
            }
          }
        }
      }
      else  // if ( clicked1stItem && clicked2ndItem )
      {
        //
        // nicht valider Click -> dummy chart setzen
        //
        *lg << LWARN << "LogFragment::onLogListClickeSlot -> no item entry on position found. Ignore Click." << Qt::endl;
        ui->diveDepthLabel->setText( diveDepthShortStr.arg( depthStr ) );
        chartView->setChart( dummyChart );
      }
    }  // if is valid
    else
    {
      //
      // nicht valider Click -> dummy chart setzen
      //
      *lg << LWARN << "LogFragment::onLogListClickeSlot -> no valid entry in list found. Ignore Click." << Qt::endl;
      ui->diveDepthLabel->setText( diveDepthShortStr.arg( depthStr ) );
      chartView->setChart( dummyChart );
    }
  }

  void LogFragment::onDiveDataWaitFor( int diveNum )
  {
    if ( dbgetDataFuture.isFinished() )
    {
      //
      // das future zur Datenabfrage ist fertig
      // dann sollte ein Dataset vorhanden sein
      // zuerst die alten charts entfernen und neue erzeugen
      // TODO: diveNum könnte validieren ob daten noch passen
      //
      *lg << LDEBUG << "LogFragment::onDiveDataWaitFor -> create new Mini Chart..." << Qt::endl;
      miniChart->deleteLater();
      miniChart = new QtCharts::QChart();
      chartWorker->prepareMiniChart( miniChart, appConfig->getGuiThemeName().compare( AppConfigClass::lightStr ) == 0 );
      chartView->setChart( miniChart );
      chartWorker->makeChartDataMini( miniChart, dbgetDataFuture.result() );
      *lg << LDEBUG << "ChartsFragment::onDiveDataWaitFor -> OK." << Qt::endl;
    }
    else
    {
      QTimer::singleShot( 100, this, [=]() { this->onDiveDataWaitFor( diveNum ); } );
      *lg << LDEBUG << "LogFragment::onDiveDataWaitFor -> future is always running, wait..." << Qt::endl;
    }
  }

  /**
   * @brief LogFragment::getSelectedInDb
   * @return
   */
  std::shared_ptr< QVector< int > > LogFragment::getSelectedInDb()
  {
    //
    // erzeuge mal den Zeiger auf einen Vector
    //
    auto deleteList( std::shared_ptr< QVector< int > >( new QVector< int >() ) );
    auto indexList = ui->logentryTableWidget->selectionModel()->selectedIndexes();
    if ( !indexList.isEmpty() )
    {
      //
      // es gibt was zu tun
      for ( auto idxEntry : indexList )
      {
        //
        // die spalte 1 finden, da steht ein icon oder auch nicht
        // deshalb, weil das Model 2 Spalten hat, aber
        // die Einstellung für das Widget (bei der Initialisierung)
        // immer die ganze Zeile selektiert
        // ui->logentryTableWidget->setSelectionBehavior( QAbstractItemView::SelectRows );
        //
        int row = idxEntry.row();
        int col = idxEntry.column();
        if ( col == 1 )
        {
          if ( !ui->logentryTableWidget->item( row, 1 )->icon().isNull() )
          {
            QString entry = ui->logentryTableWidget->item( row, 0 )->text();
            QStringList el = entry.split( ':' );
            // in die Liste kommt die Nummer!
            deleteList->append( el.at( 0 ).toInt() );
          }
        }
      }
    }
    return ( deleteList );
  }

  /**
   * @brief LogFragment::onLogDetailDeleteClickSlot
   */
  void LogFragment::onDeleteLogDetailClickSlot()
  {
    *lg << LDEBUG << "LogFragment::onLogDetailDeleteClickSlot..." << Qt::endl;
    //
    // gib mir eine Liste mit diveNum
    //
    std::shared_ptr< QVector< int > > deleteList = getSelectedInDb();
    if ( deleteList->count() == 0 )
      return;
#ifdef DEBUG
    for ( int i : *deleteList.get() )
    {
      *lg << LDEBUG << "LogFragment::onLogDetailDeleteClickSlot -> delete <" << i << ">..." << Qt::endl;
    }
#endif
    QString deviceAddr;
    //
    // unterscheide ob ich online oder offline arbeite
    //
    if ( remoteSPX42->getConnectionStatus() == SPX42RemotBtDevice::SPX42_CONNECTED )
    {
      //
      // mit wem bin ich verbunden ==> dessen Daten lösche ich
      //
      deviceAddr = remoteSPX42->getRemoteConnected();
    }
    else
    {
      //
      // habe ich ein Gerät ausgewählt, wenn ja welches
      //
      deviceAddr = offlineDeviceAddr;
    }
    //
    // ist ein Zielgerät benannt, kann ich versuchen zu löschen
    //
    if ( !deviceAddr.isNull() && !deviceAddr.isEmpty() )
    {
      *lg << LDEBUG << "LogFragment::onLogDetailDeleteClickSlot -> start delete thread..." << Qt::endl;
      //
      // erzeuge den Deleterthread und starte diesen
      //
      detailDeleterThread = new LogDetailDeleter( this, lg, database, spxConfig, deviceAddr, deleteList );
      connect( detailDeleterThread, &LogDetailDeleter::onDeleteDoneSig, this, &LogFragment::onDeleteDoneSlot );
      ui->transferProgressBar->setVisible( true );
      detailDeleterThread->start();
    }
  }

  /**
   * @brief LogFragment::onLogDetailExportClickSlot
   */
  void LogFragment::onExportLogDetailClickSlot()
  {
    QString device_mac;
    QString _exportPath;
    //
    *lg << LDEBUG << "LogFragment::onLogDetailExportClickSlot..." << Qt::endl;
    std::shared_ptr< QVector< int > > exportList = getSelectedInDb();
    if ( exportList->count() == 0 )
      return;
#ifdef DEBUG
    for ( int i : *exportList.get() )
    {
      *lg << LDEBUG << "LogFragment::onLogDetailExportClickSlot -> export <" << i << ">..." << Qt::endl;
    }
#endif
    //
    // welches Verzeichnis?
    //
    QFileDialog fileDial( this, tr( "SELECT EXPORT DIR" ), exportPath, nullptr );
    fileDial.setFileMode( QFileDialog::Directory );
    fileDial.setViewMode( QFileDialog::Detail );
    if ( QDialog::Accepted == fileDial.exec() )
    {
      _exportPath = fileDial.selectedFiles().first();
      QDir fileDir( _exportPath );
      if ( fileDir.exists() )
      {
        *lg << LINFO << "LogFragment::onExportLogDetailClickSlott -> " << exportPath << "..." << Qt::endl;
        exportPath = _exportPath;
      }
      else
      {
        //
        // default lassen, user bescheid geben
        //
        QMessageBox::critical( this, tr( "EXPORT DIRECTORY ERROR" ), tr( "The selected Directory don't exist!" ), QMessageBox::Close,
                               QMessageBox::Close );
        return;
      }
    }
    else
    {
      //
      // Abgebrochen!
      //
      return;
    }
    //
    // kann los gehen
    //
    *lg << LDEBUG << "LogFragment::onLogDetailExportClickSlot -> set parameters for export..." << Qt::endl;
    if ( remoteSPX42->getConnectionStatus() == SPX42RemotBtDevice::SPX42_CONNECTED )
    {
      //
      // mit wem bin ich verbunden ==> dessen Daten exportiere ich
      //
      device_mac = remoteSPX42->getRemoteConnected();
    }
    else
    {
      //
      // habe ich ein Gerät ausgewählt, wenn ja welches
      //
      device_mac = offlineDeviceAddr;
    }
    if ( exportFuture.isFinished() )
    {
      xmlExport.setExportDives( device_mac, *( exportList.get() ) );
      QString uddfFile = exportPath + "/" + database->getAliasForMac( device_mac ) + "_";
      xmlExport.setXmlFileBaseName( uddfFile );
      //
      // starte einen Thread zum exportieren
      //
      exportFuture = QtConcurrent::run( &this->xmlExport, &SPX42UDDFExport::createExportXml );
    }
    else
    {
      // FEHLER - läuft noch
      QMessageBox::warning( this, tr( "EXPORT WARNING" ), tr( "An other export is current running..." ), QMessageBox::Close,
                            QMessageBox::Close );
    }
  }

  /**
   * @brief Wird ein Log Verzeichniseintrag angeliefert....
   * @param entry Der Eintrag
   */
  void LogFragment::onAddLogdirEntrySlot( const QString &entry, bool inDatabase )
  {
    QTableWidgetItem *itLoadet;
    //
    // neueste zuerst
    //
    auto *itName = new QTableWidgetItem( entry );
    if ( inDatabase )
      itLoadet = new QTableWidgetItem( savedIcon, "" );
    else
      itLoadet = new QTableWidgetItem( "" );
    ui->logentryTableWidget->insertRow( 0 );
    ui->logentryTableWidget->setItem( 0, 0, itName );
    ui->logentryTableWidget->setItem( 0, 1, itLoadet );
  }

  /**
   * @brief LogFragment::onOnlineStatusChangedSlot
   */
  void LogFragment::onOnlineStatusChangedSlot( bool )
  {
    setGuiConnected( remoteSPX42->getConnectionStatus() == SPX42RemotBtDevice::SPX42_CONNECTED );

    if ( remoteSPX42->getConnectionStatus() == SPX42RemotBtDevice::SPX42_CONNECTED )
    {
      logWriter->setDeviceName( remoteSPX42->getRemoteConnected() );
      if ( !logWriter->isRunning() )
      {
        logWriter->start();
      }
    }
    else
    {
      logWriter->setDeviceName( "" );
      if ( !logWriter->isRunning() )
      {
        // thread beenden
        logWriter->setThreadEnd( true );
      }
    }
  }

  /**
   * @brief LogFragment::onSocketErrorSlot
   */
  void LogFragment::onSocketErrorSlot( QBluetoothSocket::SocketError )
  {
    // TODO: implementieren
  }

  /**
   * @brief LogFragment::onConfLicChangedSlot
   */
  void LogFragment::onConfLicChangedSlot()
  {
    *lg << LDEBUG << "LogFragment::onOnlineStatusChangedSlot -> set: " << static_cast< int >( spxConfig->getLicense().getLicType() )
        << Qt::endl;
    ui->tabHeaderLabel->setText( QString( fragmentTitlePattern.arg( spxConfig->getSerialNumber() ).arg( spxConfig->getLicName() ) ) );
  }

  /**
   * @brief LogFragment::onCloseDatabaseSlot
   */
  void LogFragment::onCloseDatabaseSlot()
  {
    // TODO: implementieren
  }

  /**
   * @brief LogFragment::onCommandRecivedSlot
   */
  void LogFragment::onCommandRecivedSlot()
  {
    spSingleCommand recCommand;
    QDateTime nowDateTime;
    QString newListEntry;
    SPX42LogDirectoryEntry newEntry;
    //
    *lg << LDEBUG << "LogFragment::onCommandRecivedSlot..." << Qt::endl;
    //
    // alle abholen...
    //
    while ( ( recCommand = remoteSPX42->getNextRecCommand() ) )
    {
      int logNumber;
      uint startStop = 0;
      // ja, es gab ein Datagram zum abholen
      char kdo = recCommand->getCommand();
      switch ( kdo )
      {
        case SPX42CommandDef::SPX_ALIVE:
          // Kommando ALIVE liefert zurück:
          // ~03:PW
          // PX => Angabe HEX in Milivolt vom Akku
          *lg << LDEBUG << "LogFragment::onCommandRecivedSlot -> alive/acku..." << Qt::endl;
          ackuVal = ( recCommand->getValueFromHexAt( SPXCmdParam::ALIVE_POWER ) / 100.0 );
          emit onAkkuValueChangedSig( ackuVal );
          break;
        case SPX42CommandDef::SPX_APPLICATION_ID:
          // Kommando APPLICATION_ID (VERSION)
          // ~04:NR -> VERSION als String
          *lg << LDEBUG << "LogFragment::onCommandRecivedSlot -> firmwareversion..." << Qt::endl;
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
          *lg << LDEBUG << "LogFragment::onCommandRecivedSlot -> serialnumber..." << Qt::endl;
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
          *lg << LDEBUG << "LogFragment::onCommandRecivedSlot -> license state..." << Qt::endl;
          spxConfig->setLicense( recCommand->getParamAt( SPXCmdParam::LICENSE_STATE ),
                                 recCommand->getParamAt( SPXCmdParam::LICENSE_INDIVIDUAL ) );
          spxConfig->freezeConfigs( SPX42ConfigClass::CF_CLASS_SPX );
          setGuiConnected( remoteSPX42->getConnectionStatus() == SPX42RemotBtDevice::SPX42_CONNECTED );
          break;
        case SPX42CommandDef::SPX_GET_LOG_INDEX:
          // Kommando SPX_GET_LOG_INDEX
          // <~41:NR:TT_MM_YY_hh_mm_ss.txt:MAX>
          // NR: Nummer des Eintrages
          // TT Tag des Tauchganges
          // MM Monat des TG
          // YY JAhr...
          // hh Stunde...
          // mm Minute
          // Sekunde
          // MAX höchste Nummer der Einträge ( NR == MAX ==> letzter Eintrag )
          *lg << LDEBUG << "LogFragment::onCommandRecivedSlot -> log direntry..." << Qt::endl;
          newEntry = SPX42LogDirectoryEntry( static_cast< int >( recCommand->getValueFromHexAt( SPXCmdParam::LOGDIR_CURR_NUMBER ) ),
                                             static_cast< int >( recCommand->getValueFromHexAt( SPXCmdParam::LOGDIR_MAXNUMBER ) ),
                                             recCommand->getParamAt( SPXCmdParam::LOGDIR_FILENAME ) );
          //
          // war das der letzte Eintrag oder sollte noch mehr kommen
          //
          if ( newEntry.number == newEntry.maxNumber )
          {
            // TODO: fertig Markieren
            ui->transferProgressBar->setVisible( false );
            transferTimeout.stop();
            // noch hinterher testen ob da was gesichert war
            // TODO: nebenläufig...
            testForSavedDetails();
          }
          else
          {
            // Das Ergebnis in die Liste der Config
            spxConfig->addDirectoryEntry( newEntry );
            newListEntry = QString( "%1:[%2]" ).arg( newEntry.number, 3, 10, QChar( '0' ) ).arg( newEntry.getDateTimeStr() );
            onAddLogdirEntrySlot( newListEntry );
            // Timer verlängern
            transferTimeout.start( TIMEOUTVAL );
          }
          break;
        case SPX42CommandDef::SPX_GET_LOG_NUMBER_SE:
          // Start oder Ende Logdetails...
          // welches startet?
          logNumber = static_cast< int >( recCommand->getValueFromHexAt( SPXCmdParam::LOGDETAIL_NUMBER ) );
          startStop = recCommand->getValueFromHexAt( SPXCmdParam::LOGDETAIL_START_END );
          //
          // Start oder Ende Signal?
          //
          if ( recCommand->getValueFromHexAt( SPXCmdParam::LOGDETAIL_START_END ) == 1 )
          {
            //
            // START der Details...
            //
            *lg << LDEBUG << "LogFragment::onCommandRecivedSlot -> log detail " << logNumber << " START..." << Qt::endl;
            logDetailQueue.clear();
            ui->transferProgressBar->setVisible( true );
            ui->dbWriteNumLabel->setVisible( true );
            ui->diveLogReadLabel->setVisible( true );
            // Timer verlängern
            transferTimeout.start( TIMEOUTVAL );
          }
          else
          {
            //
            // ENDE der Details
            //
            *lg << LDEBUG << "LogFragment::onCommandRecivedSlot -> log detail " << logNumber << " STOP..." << Qt::endl;
            ui->diveLogReadLabel->setVisible( false );
            ui->diveLogReadLabel->clear();
            logWriter->addLogQueue( logDetailQueue );
            if ( !logDetailRead.isEmpty() )
            {
              //
              // da ist noch was anzufordern
              // nutze die next-transfer-Timerroutine
              //
              int logDetailNum = logDetailRead.dequeue();
              //
              // senden lohnt nur, wenn ich das auch verarbeiten kann
              //
              SendListEntry sendCommand = remoteSPX42->askForLogDetailFor( logDetailNum );
              remoteSPX42->sendCommand( sendCommand );
              //
              // das kann etwas dauern...
              //
              transferTimeout.start( TIMEOUTVAL * 8 );
              ui->dbWriteNumLabel->setVisible( true );
            }
          }
          break;
        case SPX42CommandDef::SPX_GET_LOG_DETAIL:
          // Datensatz empfangen, ab in die Wartschlange
          logDetailQueue.enqueue( recCommand );
          ui->diveLogReadLabel->setText( readetLogTemplate.arg( recCommand->getSequence() ) );
          *lg << LDEBUG << "LogFragment::onCommandRecivedSlot -> log detail " << recCommand->getSequence() << " for dive number "
              << recCommand->getDiveNum() << "..." << Qt::endl;
          // Timer verlängern
          transferTimeout.start( TIMEOUTVAL );
          break;
      }
      //
      // falls es mehr gibt, lass dem Rest der App auch eine Chance
      //
      QCoreApplication::processEvents();
    }
  }

  /**
   * @brief LogFragment::setGuiConnected
   * @param isConnected
   */
  void LogFragment::setGuiConnected( bool isConnected )
  {
    //
    // setzte die GUI Elemente entsprechend des Online Status
    //
    ui->readLogdirPushButton->setVisible( isConnected );
    ui->readLogContentPushButton->setVisible( isConnected );
    ui->readLogdirPushButton->setEnabled( isConnected );
    ui->readLogContentPushButton->setEnabled( isConnected );
    ui->deviceSelectLabel->setVisible( !isConnected );
    ui->deviceSelectComboBox->setVisible( !isConnected );
    //
    // Liste löschen
    //
    ui->logentryTableWidget->setRowCount( 0 );
    //
    if ( isConnected )
    {
      //
      // wenn ein SPX42 verbunden ist
      //
      offlineDeviceAddr.clear();
      ui->tabHeaderLabel->setText( fragmentTitlePattern.arg( spxConfig->getSerialNumber() ).arg( spxConfig->getLicName() ) );
      ui->logentryTableWidget->setRowCount( 0 );
      //
      // ist das Verzeichnis im cache?
      //
      SPX42LogDirectoryEntryListPtr dirList;
      if ( !spxConfig->getLogDirectory()->isEmpty() )
      {
        // Daten im Cache
        // Speicherstatus sollte auch drin stehen
        *lg << LDEBUG << "LogFragment::LogFragment -> fill log directory list from cache..." << Qt::endl;
        dirList = spxConfig->getLogDirectory();
      }
      else
      {
        // Daten in der DB
        *lg << LDEBUG << "LogFragment::LogFragment -> fill log directory list from database..." << Qt::endl;
        dirList = database->getLogentrysForDevice( remoteSPX42->getRemoteConnected() );
      }
      //
      // Alle Einträge sortiert in die Liste
      //
      auto sortKeys = dirList.get()->keys();
      std::sort( sortKeys.begin(), sortKeys.end() );
      for ( auto entr : sortKeys )
      {
        SPX42LogDirectoryEntry dEntry = dirList->value( entr );
        onAddLogdirEntrySlot( QString( "%1:[%2]" ).arg( dEntry.number, 3, 10, QChar( '0' ) ).arg( dEntry.getDateTimeStr() ),
                              dEntry.inDatabase );
      }
    }
    else
    {
      //
      // wenn kein SPX42 verbunden ist
      //
      // preview löschen
      chartView->setChart( dummyChart );
      ui->tabHeaderLabel->setText( fragmentTitleOfflinePattern.arg( tr( "unknown" ) ) );
      ui->deviceSelectComboBox->clear();
      //
      // Tabelle füllen
      //
      spxDevicesAliasHash = database->getDeviceAliasHash();
      if ( spxDevicesAliasHash.isEmpty() )
      {
        //
        // nix in der Datenbank, dummy zeigen
        //
        ui->deviceSelectComboBox->addItem( tr( "database empty" ), 0 );
      }
      else
      {
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
          *lg << LDEBUG << "LogFragment::setGuiConnected -> last connected was: " << mac << Qt::endl;
          //
          // verbunden oder nicht, versuche etwas zu selektiern
          //
          // suche nach diesem Eintrag...
          //
          int index = ui->deviceSelectComboBox->findData( mac );
          if ( index != ui->deviceSelectComboBox->currentIndex() && index != -1 )
          {
            *lg << LDEBUG << "LogFragment::setGuiConnected -> found at idx " << index << ", set to idx" << Qt::endl;
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
              // slot manuell starten mit akteuellen index
              onDeviceComboChangedSlot( index );
          }
        }
      }
    }
  }

  /**
   * @brief LogFragment::testForSavedDetails
   */
  void LogFragment::testForSavedDetails()
  {
    QString deviceAddr;
    //
    *lg << LDEBUG << "LogFragment::testForSavedDetails..." << Qt::endl;
    //
    // die Liste der gespeicherten Tauchgänge aus der Datenbank holen
    // unterscheide ob ich online oder offline arbeite
    //
    if ( remoteSPX42->getConnectionStatus() == SPX42RemotBtDevice::SPX42_CONNECTED )
    {
      //
      // mit wem bin ich verbunden ==> dessen Daten lösche ich
      //
      deviceAddr = remoteSPX42->getRemoteConnected();
    }
    else
    {
      //
      // habe ich ein Gerät ausgewählt, wenn ja welches
      //
      deviceAddr = offlineDeviceAddr;
    }
    if ( !deviceAddr.isEmpty() )
    {
      SPX42LogDirectoryEntryListPtr directoryList = database->getLogentrysForDevice( deviceAddr );
      //
      // und die gecachte Liste holen
      //
      SPX42LogDirectoryEntryListPtr cachedList = spxConfig->getLogDirectory();
      for ( int i = 0; i < ui->logentryTableWidget->rowCount(); i++ )
      {
        // den Eintrag aus der widget liste holen
        QTableWidgetItem *it = ui->logentryTableWidget->item( i, 0 );
        // jetzt daten extraieren, dive nummer des Eintrages finden
        QStringList entryPieces = it->text().split( ':' );
        if ( !entryPieces.isEmpty() && entryPieces.count() > 1 )
        {
          //
          // ich hab die Tauchgangsnummer gefunden
          //
          int diveNum = entryPieces.at( 0 ).toInt();
          // in der Datenbank nachschauen
          SPX42LogDirectoryEntry dbEntry( directoryList->value( diveNum ) );
          if ( dbEntry.number == diveNum )
          {
            //
            // Eintrag in der DB gefunden == Gesicherter Eintrag == Markieren
            //
            SPX42LogDirectoryEntry cacheEntry( directoryList->value( diveNum ) );
            if ( cacheEntry.number == diveNum )
            {
              // im Cache gefunden, eintragen
              cacheEntry.inDatabase = true;
              cachedList->insert( diveNum, cacheEntry );
            }
            else
            {
              // war noch nicht im Cache...
              // dann den Eintrag su der datenbank dazu tun
              cachedList->insert( diveNum, dbEntry );
            }
            *lg << LDEBUG << QString( "LogFragment::testForSavedDetails -> saved in dive %1" ).arg( diveNum, 3, 10, QChar( '0' ) )
                << Qt::endl;
            QTableWidgetItem *itLoadet = new QTableWidgetItem( savedIcon, "" );
            ui->logentryTableWidget->setItem( i, 1, itLoadet );
          }
          else
          {
            *lg << LDEBUG << QString( "LogFragment::testForSavedDetails -> NOT saved in dive %1" ).arg( diveNum, 3, 10, QChar( '0' ) )
                << Qt::endl;
            QTableWidgetItem *itLoadet = new QTableWidgetItem( "" );
            ui->logentryTableWidget->setItem( i, 1, itLoadet );
          }
        }
      }
    }
    *lg << LDEBUG << "LogFragment::testForSavedDetails...OK" << Qt::endl;
  }

  /**
   * @brief LogFragment::onDeleteDoneSlot
   * @param diveNum
   */
  void LogFragment::onDeleteDoneSlot( int diveNum )
  {
    if ( diveNum < 0 )
    {
      //
      // zum Ende noch mal überarbeiten
      //
      ui->dbWriteNumLabel->setVisible( false );
      ui->transferProgressBar->setVisible( false );
      testForSavedDetails();
      if ( detailDeleterThread )
      {
        detailDeleterThread->disconnect();
        detailDeleterThread->deleteLater();  // möglich dass das nicht nötig ist, macht der Thread selber
        detailDeleterThread = nullptr;
      }
      return;
    }
    //
    // Sichtbarkeit
    //
    if ( !ui->dbWriteNumLabel->isVisible() )
      ui->dbWriteNumLabel->setVisible( true );
    //
    // das Label schreiben
    //
    ui->dbWriteNumLabel->setText( dbDeleteNumTemplate.arg( diveNum ) );
    //
    // den aktuellen Eintrag korrigieren
    //
    QList< QTableWidgetItem * > items =
        ui->logentryTableWidget->findItems( QString( "%1:" ).arg( diveNum, 3, 10, QChar( '0' ) ), Qt::MatchStartsWith );
    if ( items.count() > 0 )
    {
      if ( remoteSPX42->getConnectionStatus() == SPX42RemotBtDevice::SPX42_CONNECTED )
      {
        ui->logentryTableWidget->item( items.at( 0 )->row(), 1 )->setIcon( nullIcon );
      }
      else
      {
        ui->logentryTableWidget->removeRow( items.at( 0 )->row() );
      }
    }
  }

  /**
   * @brief LogFragment::itemSelectionChangedSlot
   */
  void LogFragment::itemSelectionChangedSlot()
  {
    bool dataInDatabaseSelected = false;
    QModelIndexList indexList = ui->logentryTableWidget->selectionModel()->selectedIndexes();
    if ( !indexList.isEmpty() )
    {
      //
      // es gibt was zu tun
      for ( auto idxEntry : indexList )
      {
        //
        // die spalte 1 finden, da steht ein icon oder auch nicht
        //
        int row = idxEntry.row();
        if ( !ui->logentryTableWidget->item( row, 1 )->icon().isNull() )
        {
          //
          // mindestens einen Eintrag gefunden
          //
          dataInDatabaseSelected = true;
          break;
        }
      }
    }
    //
    // Buttons erlauben oder eben nicht
    //
    ui->deleteContentPushButton->setEnabled( dataInDatabaseSelected );
    ui->exportContentPushButton->setEnabled( dataInDatabaseSelected );
  }

  /**
   * @brief LogFragment::onDeviceComboChangedSlot
   * @param index
   */
  void LogFragment::onDeviceComboChangedSlot( int index )
  {
    //
    // Liste leeren
    //
    ui->logentryTableWidget->setRowCount( 0 );
    if ( index == -1 )
    {
      offlineDeviceAddr.clear();
      return;
    }
    offlineDeviceAddr = ui->deviceSelectComboBox->itemData( index ).toString();
    *lg << LDEBUG
        << QString( "LogFragment::onDeviceComboChangedSlot -> index changed to <%1>. addr: <%2>" )
               .arg( index, 2, 10, QChar( '0' ) )
               .arg( offlineDeviceAddr )
        << Qt::endl;
    ui->tabHeaderLabel->setText( fragmentTitleOfflinePattern.arg( database->getAliasForMac( offlineDeviceAddr ) ) );
    // Daten in der DB
    *lg << LDEBUG << "LogFragment::LogFragment -> fill log directory list from database..." << Qt::endl;
    SPX42LogDirectoryEntryListPtr dirList = database->getLogentrysForDevice( offlineDeviceAddr );
    //
    // Alle Einträge sortiert in die Liste
    //
    auto sortKeys = dirList.get()->keys();
    std::sort( sortKeys.begin(), sortKeys.end() );
    for ( auto entr : sortKeys )
    {
      SPX42LogDirectoryEntry dEntry = dirList->value( entr );
      onAddLogdirEntrySlot( QString( "%1:[%2]" ).arg( dEntry.number, 3, 10, QChar( '0' ) ).arg( dEntry.getDateTimeStr() ),
                            dEntry.inDatabase );
    }
  }

  /**
   * @brief LogFragment::onExportSignleDiveStartSlot
   * @param diveNum
   */
  void LogFragment::onExportSingleDiveStartSlot( int diveNum )
  {
    if ( !ui->transferProgressBar->isVisible() )
      ui->transferProgressBar->setVisible( true );
    if ( !ui->dbWriteNumLabel - isVisible() )
      ui->dbWriteNumLabel->setVisible( true );
    ui->dbWriteNumLabel->setText( exportDiveStartTemplate.arg( diveNum, 3, 10, QChar( '0' ) ) );
  }

  /**
   * @brief LogFragment::onExportSingleDiveEndSlot
   * @param diveNum
   */
  void LogFragment::onExportSingleDiveEndSlot( int diveNum )
  {
    ui->dbWriteNumLabel->setText( exportDiveEndTemplate.arg( diveNum, 3, 10, QChar( '0' ) ) );
  }

  /**
   * @brief LogFragment::onExportEndSlot
   * @param wasOk
   */
  void LogFragment::onExportEndSlot( bool wasOk, const QString &fileName )
  {
    ui->transferProgressBar->setVisible( false );
    if ( wasOk )
    {
      ui->dbWriteNumLabel->setVisible( false );
      ui->dbWriteNumLabel->setText( dbWriteNumIDLE );
      if ( exportFuture.isFinished() )
      {
        *lg << LDEBUG << "LogFragment::onExportEndSlot -> export finished!" << Qt::endl;
        QMessageBox::information( this, tr( "EXPORT SUCCESS" ), tr( "Export was successful to file: \n<%1>" ).arg( fileName ),
                                  QMessageBox::Close, QMessageBox::Close );
      }
    }
    else
    {
      ui->dbWriteNumLabel->setText( exportDiveErrorTemplate );
      QMessageBox::critical( this, tr( "EXPORT ERROR" ), tr( "Can't export dives to UDDF file" ), QMessageBox::Close,
                             QMessageBox::Close );
    }
  }

}  // namespace spx
