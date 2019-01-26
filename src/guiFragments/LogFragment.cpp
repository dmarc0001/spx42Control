#include "LogFragment.hpp"

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
                            std::shared_ptr< SPX42RemotBtDevice > remSPX42 )
      : QWidget( parent )
      , IFragmentInterface( logger, spx42Database, spxCfg, remSPX42 )
      , ui( new Ui::LogFragment() )
      , miniChart( std::unique_ptr< DiveMiniChart >( new DiveMiniChart( logger, spx42Database ) ) )
      , dummyChart( new QtCharts::QChart() )
      , chartView( std::unique_ptr< QtCharts::QChartView >( new QtCharts::QChartView( dummyChart ) ) )
<<<<<<< HEAD
      , logWriter( this, logger, spx42Database )
      , logWriterTableExist( false )
      , savedIcon( ":/icons/saved_black" )
      , nullIcon()
=======
      , logWriter( this, logger, spx42Database, spxCfg )
      , savedIcon( ":/icons/saved_black" )
      , nullIcon()
      , offlineDeviceAddr()
>>>>>>> dev/stabilizing
  {
    lg->debug( "LogFragment::LogFragment..." );
    ui->setupUi( this );
    logWriter.reset();
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
    fragmentTitlePattern = tr( "LOGFILES SPX42 Serial [%1] LIC: %2" );
<<<<<<< HEAD
=======
    fragmentTitleOfflinePattern = tr( "LOGFILES SPX42 [%1] in database" );
>>>>>>> dev/stabilizing
    diveNumberStr = tr( "DIVE NUMBER: %1" );
    diveDateStr = tr( "DIVE DATE: %1" );
    diveDepthStr = tr( "DIVE DEPTH: %1m" );
    dbWriteNumTemplate = tr( "WRITE DIVE #%1 TO DB..." );
    dbWriteNumIDLE = tr( "WAIT FOR START..." );
    dbDeleteNumTemplate = tr( "DELETE DIVE %1 DONE." );
    ui->diveNumberLabel->setText( diveNumberStr.arg( "-" ) );
    ui->diveDateLabel->setText( diveDateStr.arg( "-" ) );
    ui->diveDepthLabel->setText( diveDepthStr.arg( "-" ) );
    ui->dbWriteNumLabel->setText( dbWriteNumIDLE );
    ui->deleteContentPushButton->setEnabled( false );
    ui->exportContentPushButton->setEnabled( false );
    // tausche den Platzhalter aus und entsorge den gleich
    // kopiere die policys und größe
    chartView->setMinimumSize( ui->diveProfileGraphicsView->minimumSize() );
    chartView->setSizePolicy( ui->diveProfileGraphicsView->sizePolicy() );
    delete ui->logDetailsGroupBox->layout()->replaceWidget( ui->diveProfileGraphicsView, chartView.get() );
    // GUI dem Onlinestatus entsprechend vorbereiten
    setGuiConnected( remoteSPX42->getConnectionStatus() == SPX42RemotBtDevice::SPX42_CONNECTED );
    //
<<<<<<< HEAD
    // ist das Verzeichnis im cache?
    //
    if ( remoteSPX42->getConnectionStatus() == SPX42RemotBtDevice::SPX42_CONNECTED )
    {
      //
      // wenn Einträge vorhanden sind
      //
      if ( spxConfig->getLogDirectory().size() > 0 )
      {
        const QVector< SPX42LogDirectoryEntry > &dirList = spxConfig->getLogDirectory();
        lg->debug( QString( "LogFragment::LogFragment -> fill log directory list from cache..." ) );
        //
        // Alle Einträge in die Liste
        //
        for ( auto entr : dirList )
        {
          onAddLogdirEntrySlot( QString( "%1:[%2]" ).arg( entr.number, 2, 10, QChar( '0' ) ).arg( entr.getDateTimeStr() ) );
        }
      }
      testForSavedDetails();
    }
    onConfLicChangedSlot();
    connect( spxConfig.get(), &SPX42Config::licenseChangedSig, this, &LogFragment::onConfLicChangedSlot );
=======
    // ist der SPX online?
    //
    if ( remoteSPX42->getConnectionStatus() == SPX42RemotBtDevice::SPX42_CONNECTED )
    {
      //
      // ist das Verzeichnis im cache?
      //
      SPX42LogDirectoryEntryListPtr dirList;
      if ( !spxConfig->getLogDirectory()->isEmpty() )
      {
        // Daten im Cache
        // Speicherstatus sollte auch drin stehen
        lg->debug( QString( "LogFragment::LogFragment -> fill log directory list from cache..." ) );
        dirList = spxConfig->getLogDirectory();
      }
      else
      {
        // Daten in der DB
        lg->debug( QString( "LogFragment::LogFragment -> fill log directory list from database..." ) );
        dirList = database->getLogentrysForDevice( remoteSPX42->getRemoteConnected() );
      }
      //
      // Alle Einträge sortiert in die Liste
      //
      auto sortKeys = dirList.get()->keys();
      qSort( sortKeys.begin(), sortKeys.end() );
      for ( auto entr : sortKeys )
      {
        SPX42LogDirectoryEntry dEntry = dirList->value( entr );
        onAddLogdirEntrySlot( QString( "%1:[%2]" ).arg( dEntry.number, 2, 10, QChar( '0' ) ).arg( dEntry.getDateTimeStr() ),
                              dEntry.inDatabase );
      }
      // ist der online gleich noch die Lizenz setzten
      onConfLicChangedSlot();
    }
    //
    // signale verbinden
    //
    connect( spxConfig.get(), &SPX42Config::licenseChangedSig, this, &LogFragment::onConfLicChangedSlot );
    connect( ui->deviceSelectComboBox, static_cast< void ( QComboBox::* )( int ) >( &QComboBox::currentIndexChanged ), this,
             &LogFragment::onDeviceComboChangedSlot );
>>>>>>> dev/stabilizing
    connect( ui->readLogdirPushButton, &QPushButton::clicked, this, &LogFragment::onReadLogDirectoryClickSlot );
    connect( ui->readLogContentPushButton, &QPushButton::clicked, this, &LogFragment::onReadLogContentClickSlot );
    connect( ui->logentryTableWidget, &QAbstractItemView::clicked, this, &LogFragment::onLogListClickeSlot );
    connect( ui->logentryTableWidget, &QTableWidget::itemSelectionChanged, this, &LogFragment::itemSelectionChangedSlot );
    connect( ui->deleteContentPushButton, &QPushButton::clicked, this, &LogFragment::onLogDetailDeleteClickSlot );
    connect( ui->exportContentPushButton, &QPushButton::clicked, this, &LogFragment::onLogDetailExportClickSlot );
    connect( remoteSPX42.get(), &SPX42RemotBtDevice::onStateChangedSig, this, &LogFragment::onOnlineStatusChangedSlot );
    connect( remoteSPX42.get(), &SPX42RemotBtDevice::onSocketErrorSig, this, &LogFragment::onSocketErrorSlot );
    connect( remoteSPX42.get(), &SPX42RemotBtDevice::onCommandRecivedSig, this, &LogFragment::onCommandRecivedSlot );
<<<<<<< HEAD
    connect( &transferTimeout, &QTimer::timeout, this, &LogFragment::onTransferTimeout );
=======
    connect( &transferTimeout, &QTimer::timeout, this, &LogFragment::onTransferTimeoutSlot );
>>>>>>> dev/stabilizing
    connect( &logWriter, &LogDetailWalker::onWriteDoneSig, this, &LogFragment::onWriterDoneSlot );
    connect( &logWriter, &LogDetailWalker::onNewDiveStartSig, this, &LogFragment::onNewDiveStartSlot );
    connect( &logWriter, &LogDetailWalker::onDeleteDoneSig, this, &LogFragment::onDeleteDoneSlot );
    connect( &logWriter, &LogDetailWalker::onNewDiveDoneSig, this, &LogFragment::onNewDiveDoneSlot );
  }

  /**
   * @brief Der Destruktor, Aufräumarbeiten
   */
  LogFragment::~LogFragment()
  {
    lg->debug( "LogFragment::~LogFragment..." );
    // setze wieder den Dummy ein und lasse den
    // die Objekte im ChartView entsorgen
    chartView->setChart( dummyChart );
    // ui->logentryTableWidget->setModel( Q_NULLPTR );
    deactivateTab();
  }

  /**
   * @brief LogFragment::deactivateTab
   */
  void LogFragment::deactivateTab()
  {
    disconnect( spxConfig.get(), nullptr, this, nullptr );
    disconnect( remoteSPX42.get(), nullptr, this, nullptr );
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
<<<<<<< HEAD
  void LogFragment::onTransferTimeout()
=======
  void LogFragment::onTransferTimeoutSlot()
>>>>>>> dev/stabilizing
  {
    //
    // wenn der Writer noch läuft, dann noch nicht den Balken ausblenden
    //
    if ( dbWriterFuture.isFinished() )
    {
      ui->transferProgressBar->setVisible( false );
      ui->dbWriteNumLabel->setText( dbWriteNumIDLE );
      transferTimeout.stop();
      lg->warn( "LogFragment::onTransferTimeout -> transfer timeout!!!" );
      // TODO: Warn oder Fehlermeldung ausgeben
    }
  }

  /**
   * @brief LogFragment::onWriterDoneSlot
   */
  void LogFragment::onWriterDoneSlot( int )
  {
    lg->debug( "LogFragment::onWriterDoneSlot..." );
    // Writer ist fertig, es könnte weiter gehen
    if ( dbWriterFuture.isFinished() )
    {
      logWriter.reset();
      lg->debug( "LogFragment::onWriterDoneSlot -> writer finished!" );
      ui->transferProgressBar->setVisible( false );
      ui->dbWriteNumLabel->setVisible( false );
      ui->dbWriteNumLabel->setText( dbWriteNumIDLE );
      testForSavedDetails();
      // TODO: Auswerten der Ergebnisse
      int result = dbWriterFuture.result();
      if ( result < 0 )
      {
<<<<<<< HEAD
        logWriterTableExist = false;
=======
        // TODO: was machen
>>>>>>> dev/stabilizing
      }
    }
    //
    // wenn in der Queue was drin ist UND der Writer bereits fertig ist.
    // Ist er nicht fertig, wird er dieses beim Beenden selber noch einmal aufrufen
    //
    tryStartLogWriterThread();
  }

  /**
   * @brief LogFragment::tryStartLogWriterThread
   */
  void LogFragment::tryStartLogWriterThread()
  {
<<<<<<< HEAD
    if ( !logWriterTableExist )
    {
      lg->crit( "LogFragment::tryStartLogWriterThread -> can't start writer thread while database error! Try restart programm." );
      ui->dbWriteNumLabel->setVisible( false );
      return;
    }
=======
>>>>>>> dev/stabilizing
    if ( !logDetailRead.isEmpty() && dbWriterFuture.isFinished() )
    {
      lg->debug( "LogFragment::onWriterDoneSlot -> start writer thread again..." );
      ui->dbWriteNumLabel->setVisible( true );
      dbWriterFuture =
          QtConcurrent::run( &this->logWriter, &LogDetailWalker::writeLogDataToDatabase, remoteSPX42->getRemoteConnected() );
    }
  }

  /**
   * @brief LogFragment::onNewDiveStartSlot
   * @param newDiveNum
   */
  void LogFragment::onNewDiveStartSlot( int newDiveNum )
  {
    ui->dbWriteNumLabel->setText( dbWriteNumTemplate.arg( newDiveNum, 3, 10, QChar( '0' ) ) );
    lg->debug(
        QString( "LogFragment::onNewDiveStartSlot -> write dive number #%1 to database..." ).arg( newDiveNum, 3, 10, QChar( '0' ) ) );
  }

  /**
   * @brief Slot für das Signal von button zum Directory lesen
   */
  void LogFragment::onReadLogDirectoryClickSlot()
  {
    lg->debug( "LogFragment::onReadLogDirectorySlot: ..." );
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
      lg->debug( "LogFragment::onReadLogDirectorySlot -> send cmd get log directory..." );
    }
  }

  /**
   * @brief Slot für das Signal vom button zum Inhalt des Logs lesen
   */
  void LogFragment::onReadLogContentClickSlot()
  {
    lg->debug( "LogFragment::onReadLogContentSlot: ..." );
    QModelIndexList indexList = ui->logentryTableWidget->selectionModel()->selectedIndexes();
    if ( ui->logentryTableWidget->rowCount() == 0 )
    {
      lg->warn( "LogFragment::onReadLogContentSlot -> no log entrys!" );
      return;
    }
    //
    // gibt es was zu tun?
    //
    if ( indexList.isEmpty() )
    {
      lg->warn( "LogFragment::onReadLogContentSlot -> nothing selected, read all?" );
      // TODO: Messagebox aufpoppen und Nutzer fragen
      return;
    }
    lg->debug( QString( "LogFragment::onReadLogContentSlot -> read %1 logs from spx42..." ).arg( indexList.count() ) );
    //
    // so, fülle mal die Queue mit den zu lesenden Nummern
    //
    logDetailRead.clear();
    for ( auto idxEntry : indexList )
    {
      //
      // die spalte 0 finden, da steht die Lognummer
      //
      int row = idxEntry.row();
      QString entry = ui->logentryTableWidget->item( row, 0 )->text();
      QStringList el = entry.split( ':' );
      // in die Liste kommt die Nummer!
      logDetailRead.enqueue( el.at( 0 ).toInt() );
    }
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
      logWriter.reset();
      lg->debug( QString( "LogFragment::onReadLogContentSlot -> request  %1 logs from spx42..." ).arg( logDetailNum ) );
      tryStartLogWriterThread();
    }
  }

  /**
   * @brief Slot für das Signal wenn auf einen Log Directory Eintrag geklickt wird
   * @param index welcher Index war es?
   */
  void LogFragment::onLogListClickeSlot( const QModelIndex &index )
  {
<<<<<<< HEAD
=======
    QString deviceAddr;
>>>>>>> dev/stabilizing
    QString depthStr = " ? ";
    int diveNum = 0;
    double depth = 0;
    int row = index.row();
    // items finden
    QTableWidgetItem *clicked1stItem = ui->logentryTableWidget->item( row, 0 );
    QTableWidgetItem *clicked2ndItem = ui->logentryTableWidget->item( row, 1 );
    // Aus dem Text die Nummer extraieren
    QString entry = clicked1stItem->text();
    lg->debug( QString( "LogFragment::onLogListViewClickedSlot: data: %1..." ).arg( entry ) );
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
      ui->diveDepthLabel->setText( diveDepthStr.arg( depthStr ) );
      chartView->setChart( dummyChart );
    }
    else
    {
      //
      // die Datenbank fragen, ob und wie tief
<<<<<<< HEAD
      //
      depth = ( database->getMaxDepthFor( database->getLogTableName( remoteSPX42->getRemoteConnected() ), diveNum ) / 10.0 );
      if ( depth > 0 )
      {
        //
        // tiefe eintragen
        //
        ui->diveDepthLabel->setText( diveDepthStr.arg( depth, 2, 'f', 1 ) );
        //
        // das Chart anzeigen, wenn Daten vorhanden sind
        //
        miniChart->showDiveDataInMiniGraph( remoteSPX42->getRemoteConnected(), diveNum );
        //
        chartView->setChart( miniChart.get() );
      }
      else
      {
        ui->diveDepthLabel->setText( diveDepthStr.arg( depthStr ) );
        chartView->setChart( dummyChart );
      }
=======
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
        depth = ( database->getMaxDepthFor( deviceAddr, diveNum ) / 10.0 );
        if ( depth > 0 )
        {
          //
          // tiefe eintragen
          //
          ui->diveDepthLabel->setText( diveDepthStr.arg( depth, 2, 'f', 1 ) );
          //
          // das Chart anzeigen, wenn Daten vorhanden sind
          //
          miniChart->showDiveDataInMiniGraph( deviceAddr, diveNum );
          //
          chartView->setChart( miniChart.get() );
        }
        else
        {
          ui->diveDepthLabel->setText( diveDepthStr.arg( depthStr ) );
          chartView->setChart( dummyChart );
        }
      }
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
  void LogFragment::onLogDetailDeleteClickSlot()
  {
    lg->debug( "LogFragment::onLogDetailDeleteClickSlot..." );
    //
    // gib mir eine Liste mit diveNum
    //
    std::shared_ptr< QVector< int > > deleteList = getSelectedInDb();
    if ( deleteList->count() == 0 )
      return;
#ifdef DEBUG
    for ( int i : *deleteList.get() )
    {
      lg->debug( QString( "LogFragment::onLogDetailDeleteClickSlot -> delete <%1>..." ).arg( i ) );
    }
#endif
    if ( dbDeleteFuture.isFinished() )
    {
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
        lg->debug( "LogFragment::onLogDetailDeleteClickSlot -> start delete thread..." );
        dbDeleteFuture = QtConcurrent::run( &this->logWriter, &LogDetailWalker::deleteLogDataFromDatabase, deviceAddr, deleteList );
      }
    }
  }

  /**
   * @brief LogFragment::onLogDetailExportClickSlot
   */
  void LogFragment::onLogDetailExportClickSlot()
  {
    lg->debug( "LogFragment::onLogDetailExportClickSlot..." );
    std::shared_ptr< QVector< int > > deleteList = getSelectedInDb();
    if ( deleteList->count() == 0 )
      return;
    for ( int i : *deleteList.get() )
    {
      lg->debug( QString( "LogFragment::onLogDetailExportClickSlot -> export <%1>..." ).arg( i ) );
>>>>>>> dev/stabilizing
    }
    // DEBUG: erst mal nurt Nachricht
    QMessageBox msgBox;
    msgBox.setText( tr( "FUNCTION NOT IMPLEMENTED YET" ) );
    msgBox.setInformativeText( "Keep in mind: it is an test version..." );
    msgBox.setIcon( QMessageBox::Information );
    msgBox.exec();
  }

  /**
<<<<<<< HEAD
   * @brief LogFragment::getSelectedInDb
   * @return
   */
  std::shared_ptr< QVector< int > > LogFragment::getSelectedInDb( void )
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
  void LogFragment::onLogDetailDeleteClickSlot()
  {
    lg->debug( "LogFragment::onLogDetailDeleteClickSlot..." );
    std::shared_ptr< QVector< int > > deleteList = getSelectedInDb();
    if ( deleteList->count() == 0 )
      return;
#ifdef DEBUG
    for ( int i : *deleteList.get() )
    {
      lg->debug( QString( "LogFragment::onLogDetailDeleteClickSlot -> delete <%1>..." ).arg( i ) );
    }
#endif
    if ( dbDeleteFuture.isFinished() )
    {
      lg->debug( "LogFragment::onLogDetailDeleteClickSlot -> start delete thread..." );
      dbDeleteFuture = QtConcurrent::run( &this->logWriter, &LogDetailWalker::deleteLogDataFromDatabase,
                                          remoteSPX42->getRemoteConnected(), deleteList );
    }
  }

  /**
   * @brief LogFragment::onLogDetailExportClickSlot
   */
  void LogFragment::onLogDetailExportClickSlot()
  {
    lg->debug( "LogFragment::onLogDetailExportClickSlot..." );
    std::shared_ptr< QVector< int > > deleteList = getSelectedInDb();
    if ( deleteList->count() == 0 )
      return;
    for ( int i : *deleteList.get() )
    {
      lg->debug( QString( "LogFragment::onLogDetailExportClickSlot -> export <%1>..." ).arg( i ) );
    }
    // DEBUG: erst mal nurt Nachricht
    QMessageBox msgBox;
    msgBox.setText( tr( "FUNCTION NOT IMPLEMENTED YET" ) );
    msgBox.setInformativeText( "Keep in mind: it is an test version..." );
    msgBox.setIcon( QMessageBox::Information );
    msgBox.exec();
  }

  /**
   * @brief Wird ein Log Verzeichniseintrag angeliefert....
   * @param entry Der Eintrag
   */
  void LogFragment::onAddLogdirEntrySlot( const QString &entry )
  {
    //
    // neueste zuerst
    //
    QTableWidgetItem *itName = new QTableWidgetItem( entry );
    QTableWidgetItem *itLoadet = new QTableWidgetItem( "" );
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

    if ( remoteSPX42->getConnectionStatus() != SPX42RemotBtDevice::SPX42_CONNECTED )
      logWriter.reset();
  }

  /**
   * @brief LogFragment::onSocketErrorSlot
   */
  void LogFragment::onSocketErrorSlot( QBluetoothSocket::SocketError )
=======
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

    if ( remoteSPX42->getConnectionStatus() != SPX42RemotBtDevice::SPX42_CONNECTED )
      logWriter.reset();
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
    lg->debug( QString( "LogFragment::onOnlineStatusChangedSlot -> set: %1" )
                   .arg( static_cast< int >( spxConfig->getLicense().getLicType() ) ) );
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
    char kdo;
    int logNumber;
    //
    lg->debug( "ConnectFragment::onDatagramRecivedSlot..." );
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
          lg->debug( "LogFragment::onDatagramRecivedSlot -> alive/acku..." );
          ackuVal = ( recCommand->getValueFromHexAt( SPXCmdParam::ALIVE_POWER ) / 100.0 );
          emit onAkkuValueChangedSig( ackuVal );
          break;
        case SPX42CommandDef::SPX_APPLICATION_ID:
          // Kommando APPLICATION_ID (VERSION)
          // ~04:NR -> VERSION als String
          lg->debug( "LogFragment::onDatagramRecivedSlot -> firmwareversion..." );
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
          lg->debug( "LogFragment::onDatagramRecivedSlot -> serialnumber..." );
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
          lg->debug( "LogFragment::onDatagramRecivedSlot -> license state..." );
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
          lg->debug( "LogFragment::onDatagramRecivedSlot -> log direntry..." );
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
          lg->debug( QString( "LogFragment::onDatagramRecivedSlot -> log detail %1 for dive number %2..." )
                         .arg( recCommand->getValueFromHexAt( SPXCmdParam::LOGDETAIL_NUMBER ) )
                         .arg( recCommand->getValueFromHexAt( SPXCmdParam::LOGDETAIL_START_END ) == 0 ? "STOP" : "START" ) );
          //
          // Start oder Ende Signal?
          //
          if ( recCommand->getValueFromHexAt( SPXCmdParam::LOGDETAIL_START_END ) == 1 )
          {
            //
            // START der Details...
            //
            if ( dbWriterFuture.isFinished() )
            {
              lg->debug( "LogFragment::onDatagramRecivedSlot -> start writer thread again..." );
              tryStartLogWriterThread();
              ui->dbWriteNumLabel->setVisible( true );
            }
            // Timer verlängern
            transferTimeout.start( TIMEOUTVAL );
          }
          else
          {
            // ENDE der Details
            if ( !logDetailRead.isEmpty() )
            {
              //
              // da ist noch was anzufordern
              // nutze die next-transfer-Timerroutine
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
              if ( dbWriterFuture.isFinished() )
              {
                // Thread neu starten
                lg->debug( QString( "LogFragment::onReadLogContentSlot -> request  %1 logs from spx42..." ).arg( logDetailNum ) );
                tryStartLogWriterThread();
                ui->dbWriteNumLabel->setVisible( true );
              }
            }
            else
            {
              // Timer ist zu stoppen!
              logWriter.nowait( true );
              transferTimeout.stop();
            }
          }
          break;
        case SPX42CommandDef::SPX_GET_LOG_DETAIL:
          // Datensatz empfangen, ab in die Wartschlange
          logWriter.addDetail( recCommand );
          lg->debug( QString( "LogFragment::onDatagramRecivedSlot -> log detail %1 for dive number %2..." )
                         .arg( logWriter.getGlobal() )
                         .arg( recCommand->getDiveNum() ) );
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
    if ( isConnected )
    {
      //
      // wenn ein SPX42 verbunden ist
      //
      offlineDeviceAddr.clear();
      ui->tabHeaderLabel->setText( fragmentTitlePattern.arg( spxConfig->getSerialNumber() ).arg( spxConfig->getLicName() ) );
    }
    else
    {
      //
      // wenn kein SPX42 verbunden ist
      //
      ui->logentryTableWidget->setRowCount( 0 );
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
    lg->debug( "LogFragment::testForSavedDetails..." );
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
            lg->debug( QString( "LogFragment::testForSavedDetails -> saved in dive %1" ).arg( diveNum, 3, 10, QChar( '0' ) ) );
            QTableWidgetItem *itLoadet = new QTableWidgetItem( savedIcon, "" );
            ui->logentryTableWidget->setItem( i, 1, itLoadet );
          }
          else
          {
            lg->debug( QString( "LogFragment::testForSavedDetails -> NOT saved in dive %1" ).arg( diveNum, 3, 10, QChar( '0' ) ) );
            QTableWidgetItem *itLoadet = new QTableWidgetItem( "" );
            ui->logentryTableWidget->setItem( i, 1, itLoadet );
          }
        }
      }
    }
    lg->debug( "LogFragment::testForSavedDetails...OK" );
  }

  /**
   * @brief LogFragment::onDeleteDoneSlot
   * @param diveNum
   */
  void LogFragment::onDeleteDoneSlot( int diveNum )
>>>>>>> dev/stabilizing
  {
    if ( diveNum < 0 )
    {
      //
      // zum Ende noch mal überarbeiten
      //
      ui->dbWriteNumLabel->setVisible( false );
      testForSavedDetails();
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
        ui->logentryTableWidget->findItems( QString( "%1:" ).arg( diveNum, 2, 10, QChar( '0' ) ), Qt::MatchStartsWith );
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
<<<<<<< HEAD
   * @brief LogFragment::onConfLicChangedSlot
   */
  void LogFragment::onConfLicChangedSlot()
=======
   * @brief LogFragment::onNewDiveDoneSlot
   * @param diveNum
   */
  void LogFragment::onNewDiveDoneSlot( int diveNum )
>>>>>>> dev/stabilizing
  {
    //
    // den aktuellen Eintrag korrigieren
    //
    QList< QTableWidgetItem * > items =
        ui->logentryTableWidget->findItems( QString( "%1:" ).arg( diveNum, 2, 10, QChar( '0' ) ), Qt::MatchStartsWith );
    if ( items.count() > 0 )
    {
      ui->logentryTableWidget->item( items.at( 0 )->row(), 1 )->setIcon( savedIcon );
      ui->logentryTableWidget->viewport()->update();
    }
  }

  /**
<<<<<<< HEAD
   * @brief LogFragment::onCloseDatabaseSlot
   */
  void LogFragment::onCloseDatabaseSlot()
=======
   * @brief LogFragment::itemSelectionChangedSlot
   */
  void LogFragment::itemSelectionChangedSlot()
>>>>>>> dev/stabilizing
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
<<<<<<< HEAD
   * @brief LogFragment::onCommandRecivedSlot
   */
  void LogFragment::onCommandRecivedSlot()
  {
    spSingleCommand recCommand;
    QDateTime nowDateTime;
    QByteArray value;
    QString newListEntry;
    SPX42LogDirectoryEntry newEntry;
    char kdo;
    int logNumber;
    //
    lg->debug( "ConnectFragment::onDatagramRecivedSlot..." );
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
          lg->debug( "LogFragment::onDatagramRecivedSlot -> alive/acku..." );
          ackuVal = ( recCommand->getValueFromHexAt( SPXCmdParam::ALIVE_POWER ) / 100.0 );
          emit onAkkuValueChangedSig( ackuVal );
          break;
        case SPX42CommandDef::SPX_APPLICATION_ID:
          // Kommando APPLICATION_ID (VERSION)
          // ~04:NR -> VERSION als String
          lg->debug( "LogFragment::onDatagramRecivedSlot -> firmwareversion..." );
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
          lg->debug( "LogFragment::onDatagramRecivedSlot -> serialnumber..." );
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
          lg->debug( "LogFragment::onDatagramRecivedSlot -> license state..." );
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
          lg->debug( "LogFragment::onDatagramRecivedSlot -> log direntry..." );
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
          lg->debug( QString( "LogFragment::onDatagramRecivedSlot -> log detail %1 for dive number %2..." )
                         .arg( recCommand->getValueFromHexAt( SPXCmdParam::LOGDETAIL_NUMBER ) )
                         .arg( recCommand->getValueFromHexAt( SPXCmdParam::LOGDETAIL_START_END ) == 0 ? "STOP" : "START" ) );
          //
          // Start oder Ende Signal?
          //
          if ( recCommand->getValueFromHexAt( SPXCmdParam::LOGDETAIL_START_END ) == 1 )
          {
            //
            // START der Details...
            //
            if ( dbWriterFuture.isFinished() )
            {
              lg->debug( "LogFragment::onDatagramRecivedSlot -> start writer thread again..." );
              tryStartLogWriterThread();
              ui->dbWriteNumLabel->setVisible( true );
            }
            // Timer verlängern
            transferTimeout.start( TIMEOUTVAL );
          }
          else
          {
            // ENDE der Details
            if ( !logDetailRead.isEmpty() )
            {
              //
              // da ist noch was anzufordern
              //
              int logDetailNum = logDetailRead.dequeue();
              if ( logWriterTableExist )
              {
                //
                // senden lohnt nur, wenn ich das auch verarbeiten kann
                //
                SendListEntry sendCommand = remoteSPX42->askForLogDetailFor( logDetailNum );
                remoteSPX42->sendCommand( sendCommand );
                //
                // das kann etwas dauern...
                //
                transferTimeout.start( TIMEOUTVAL * 8 );
                if ( dbWriterFuture.isFinished() )
                {
                  // Thread neu starten
                  lg->debug( QString( "LogFragment::onReadLogContentSlot -> request  %1 logs from spx42..." ).arg( logDetailNum ) );
                  tryStartLogWriterThread();
                  ui->dbWriteNumLabel->setVisible( true );
                }
              }
              else
              {
                lg->warn( "LogFragment::onDatagramRecivedSlot -> cancel request details while database write error!" );
              }
            }
            else
            {
              // Timer ist zu stoppen!
              logWriter.nowait( true );
              transferTimeout.stop();
            }
          }
          break;
        case SPX42CommandDef::SPX_GET_LOG_DETAIL:
          // Datensatz empfangen, ab in die Wartschlange
          if ( !logWriterTableExist )
          {
            lg->warn( "LogFragment::onDatagramRecivedSlot -> cancel processing details while database write error!" );
          }
          else
          {
            logWriter.addDetail( recCommand );
            lg->debug( QString( "LogFragment::onDatagramRecivedSlot -> log detail %1 for dive number %2..." )
                           .arg( logWriter.getGlobal() )
                           .arg( recCommand->getDiveNum() ) );
            // Timer verlängern
            transferTimeout.start( TIMEOUTVAL );
          }
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
    ui->readLogdirPushButton->setEnabled( isConnected );
    ui->readLogContentPushButton->setEnabled( isConnected );
    ui->tabHeaderLabel->setText( fragmentTitlePattern.arg( spxConfig->getSerialNumber() ).arg( spxConfig->getLicName() ) );

    if ( isConnected )
    {
      QString tableName = database->getLogTableName( remoteSPX42->getRemoteConnected() );
      if ( tableName.isNull() || tableName.isEmpty() )
      {
        logWriterTableExist = false;
      }
      else
      {
        logWriterTableExist = true;
      }
    }
    else
    {
      logWriterTableExist = false;
    }
    // TODO: chart->setVisible( isConnected );
    if ( !isConnected )
    {
      ui->logentryTableWidget->setRowCount( 0 );
      // preview löschen
      chartView->setChart( dummyChart );
    }
  }

  /**
   * @brief LogFragment::testForSavedDetails
   */
  void LogFragment::testForSavedDetails()
  {
    lg->debug( "LogFragment::testForSavedDetails..." );
    QString tableName = database->getLogTableName( remoteSPX42->getRemoteConnected() );
    for ( int i = 0; i < ui->logentryTableWidget->rowCount(); i++ )
    {
      // den Eintrag holen
      QTableWidgetItem *it = ui->logentryTableWidget->item( i, 0 );
      // jetzt daten extraieren, nummer des Eintrages finden
      QStringList pieces = it->text().split( ':' );
      if ( !pieces.isEmpty() && pieces.count() > 1 )
      {
        //
        // ich hab was gefunden
        //
        int diveNum = pieces.at( 0 ).toInt();
        if ( database->existDiveLogInBase( tableName, diveNum ) )
        {
          lg->debug( QString( "LogFragment::testForSavedDetails -> saved in dive %1" ).arg( diveNum, 3, 10, QChar( '0' ) ) );
          QTableWidgetItem *itLoadet = new QTableWidgetItem( savedIcon, "" );
          // itLoadet->setStatusTip( tr( "NEW STATUS TIP --SAVED --" ) );
          ui->logentryTableWidget->setItem( i, 1, itLoadet );
        }
        else
        {
          lg->debug( QString( "LogFragment::testForSavedDetails -> NOT saved in dive %1" ).arg( diveNum, 3, 10, QChar( '0' ) ) );
          QTableWidgetItem *itLoadet = new QTableWidgetItem( "" );
          // itLoadet->setStatusTip( tr( "NEW STATUS TIP --UNSAVED --" ) );
          ui->logentryTableWidget->setItem( i, 1, itLoadet );
        }
      }
    }
    lg->debug( "LogFragment::testForSavedDetails...OK" );
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
      testForSavedDetails();
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
        ui->logentryTableWidget->findItems( QString( "%1:" ).arg( diveNum, 2, 10, QChar( '0' ) ), Qt::MatchStartsWith );
    if ( items.count() > 0 )
    {
      ui->logentryTableWidget->item( items.at( 0 )->row(), 1 )->setIcon( nullIcon );
    }
  }

  /**
   * @brief LogFragment::onNewDiveDoneSlot
   * @param diveNum
   */
  void LogFragment::onNewDiveDoneSlot( int diveNum )
  {
    //
    // den aktuellen Eintrag korrigieren
    //
    QList< QTableWidgetItem * > items =
        ui->logentryTableWidget->findItems( QString( "%1:" ).arg( diveNum, 2, 10, QChar( '0' ) ), Qt::MatchStartsWith );
    if ( items.count() > 0 )
    {
      this->ui->logentryTableWidget->item( items.at( 0 )->row(), 1 )->setIcon( savedIcon );
    }
  }

  /**
   * @brief LogFragment::itemSelectionChangedSlot
   */
  void LogFragment::itemSelectionChangedSlot( void )
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
=======
   * @brief LogFragment::onDeviceComboChangedSlot
   * @param index
   */
  void LogFragment::onDeviceComboChangedSlot( int index )
  {
    offlineDeviceAddr = ui->deviceSelectComboBox->itemData( index ).toString();
    lg->debug( QString( "LogFragment::onDeviceComboChangedSlot -> index changed to <%1>. addr: <%2>" )
                   .arg( index, 2, 10, QChar( '0' ) )
                   .arg( offlineDeviceAddr ) );
    ui->tabHeaderLabel->setText( fragmentTitleOfflinePattern.arg( database->getAliasForMac( offlineDeviceAddr ) ) );
    //
    // Liste leeren
    //
    ui->logentryTableWidget->clear();
    // Daten in der DB
    lg->debug( QString( "LogFragment::LogFragment -> fill log directory list from database..." ) );
    SPX42LogDirectoryEntryListPtr dirList = database->getLogentrysForDevice( offlineDeviceAddr );
    //
    // Alle Einträge sortiert in die Liste
    //
    auto sortKeys = dirList.get()->keys();
    qSort( sortKeys.begin(), sortKeys.end() );
    for ( auto entr : sortKeys )
    {
      SPX42LogDirectoryEntry dEntry = dirList->value( entr );
      onAddLogdirEntrySlot( QString( "%1:[%2]" ).arg( dEntry.number, 2, 10, QChar( '0' ) ).arg( dEntry.getDateTimeStr() ),
                            dEntry.inDatabase );
    }
  }
>>>>>>> dev/stabilizing
}  // namespace spx
