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
      , chart( new QtCharts::QChart() )
      , dummyChart( new QtCharts::QChart() )
      , chartView( new QtCharts::QChartView( dummyChart ) )
      , axisY( new QCategoryAxis() )
      , logWriter( this, logger, spx42Database )
      , savedIcon( ":/icons/saved_black" )
      , nullIcon()
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
    diveNumberStr = tr( "DIVE NUMBER: %1" );
    diveDateStr = tr( "DIVE DATE: %1" );
    diveDepthStr = tr( "DIVE DEPTH: %1m" );
    dbWriteNumTemplate = tr( "WRITE DIVE #%1..." );
    dbWriteNumIDLE = tr( "WAIT FOR START..." );
    ui->diveNumberLabel->setText( diveNumberStr.arg( "-" ) );
    ui->diveDateLabel->setText( diveDateStr.arg( "-" ) );
    ui->diveDepthLabel->setText( diveDepthStr.arg( "-" ) );
    ui->dbWriteNumLabel->setText( dbWriteNumIDLE );
    prepareMiniChart();
    // tausche den Platzhalter aus und entsorge den gleich
    delete ui->logDetailsGroupBox->layout()->replaceWidget( ui->diveProfileGraphicsView, chartView );
    // GUI dem Onlinestatus entsprechend vorbereiten
    setGuiConnected( remoteSPX42->getConnectionStatus() == SPX42RemotBtDevice::SPX42_CONNECTED );
    //
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
    connect( ui->readLogdirPushButton, &QPushButton::clicked, this, &LogFragment::onReadLogDirectorySlot );
    connect( ui->readLogContentPushButton, &QPushButton::clicked, this, &LogFragment::onReadLogContentSlot );
    connect( ui->logentryTableWidget, &QAbstractItemView::clicked, this, &LogFragment::onLogListClickedSlot );
    connect( remoteSPX42.get(), &SPX42RemotBtDevice::onStateChangedSig, this, &LogFragment::onOnlineStatusChangedSlot );
    connect( remoteSPX42.get(), &SPX42RemotBtDevice::onSocketErrorSig, this, &LogFragment::onSocketErrorSlot );
    connect( remoteSPX42.get(), &SPX42RemotBtDevice::onCommandRecivedSig, this, &LogFragment::onCommandRecivedSlot );
    connect( &transferTimeout, &QTimer::timeout, this, &LogFragment::onTransferTimeout );
    connect( &logWriter, &LogDetailWriter::onWriteDoneSig, this, &LogFragment::onWriterDoneSlot );
    connect( &logWriter, &LogDetailWriter::onNewDiveStartSig, this, &LogFragment::onNewDiveStartSlot );
  }

  /**
   * @brief Der Destruktor, Aufräumarbeiten
   */
  LogFragment::~LogFragment()
  {
    lg->debug( "LogFragment::~LogFragment..." );
    // setze wieder den Dummy ein und lasse den
    // uniqe_ptr die Objekte im ChartView entsorgen
    chartView->setChart( dummyChart );
    // ui->logentryTableWidget->setModel( Q_NULLPTR );
    deactivateTab();
  }

  void LogFragment::deactivateTab()
  {
    disconnect( spxConfig.get(), nullptr, this, nullptr );
    disconnect( remoteSPX42.get(), nullptr, this, nullptr );
  }

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

  void LogFragment::onTransferTimeout()
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
      // int result = dbWriterFuture.result();
    }
    //
    // wenn in der Queue was drin ist UND der Writer bereits fertig ist.
    // Ist er nicht fertig, wird er dieses beim Beenden selber noch einmal aufrufen
    //
    if ( !logDetailRead.isEmpty() && dbWriterFuture.isFinished() )
    {
      lg->debug( "LogFragment::onWriterDoneSlot -> start writer thread again..." );
      ui->dbWriteNumLabel->setVisible( true );
      dbWriterFuture =
          QtConcurrent::run( &this->logWriter, &LogDetailWriter::writeLogDataToDatabase, remoteSPX42->getRemoteConnected() );
    }
  }

  void LogFragment::onNewDiveStartSlot( int newDiveNum )
  {
    ui->dbWriteNumLabel->setText( dbWriteNumTemplate.arg( newDiveNum, 3, 10, QChar( '0' ) ) );
    lg->debug(
        QString( "LogFragment::onNewDiveStartSlot -> write dive number #%1 to database..." ).arg( newDiveNum, 3, 10, QChar( '0' ) ) );
  }

  /**
   * @brief Slot für das Signal von button zum Directory lesen
   */
  void LogFragment::onReadLogDirectorySlot()
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
      // preview löschen
      // Chart löschen
      chart->removeAllSeries();
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
  void LogFragment::onReadLogContentSlot()
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
      ui->dbWriteNumLabel->setVisible( true );
      dbWriterFuture =
          QtConcurrent::run( &this->logWriter, &LogDetailWriter::writeLogDataToDatabase, remoteSPX42->getRemoteConnected() );
    }
  }

  /**
   * @brief Slot für das Signal wenn auf einen Log Directory Eintrag geklickt wird
   * @param index welcher Index war es?
   */
  void LogFragment::onLogListClickedSlot( const QModelIndex &index )
  {
    QString depthStr = "-";
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
    }
    else
    {
      //
      // die Datenbank fragen, ob und wie tief
      //
      depth = ( database->getMaxDepthFor( remoteSPX42->getRemoteConnected().replace( ':', '-' ), diveNum ) / 10.0 );
      if ( depth > 0 )
      {
        //
        // tiefe eintragen
        //
        ui->diveDepthLabel->setText( diveDepthStr.arg( depth, 2, 'f', 1 ) );
      }
      else
      {
        ui->diveDepthLabel->setText( diveDepthStr.arg( depthStr ) );
      }
    }

    //
    // Daten anzeigen, oder auch nicht
    // FIXME: zum testen nur gerade anzahl
    //
    if ( ( index.row() % 2 ) > 0 )
    {
      // FIXME: natürlich noch die richtigen Daten einbauen
      showDiveDataForGraph( 1, 2 );
    }
    else
    {
      chartView->setChart( dummyChart );
    }
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

  void LogFragment::prepareMiniChart()
  {
    chart->legend()->hide();  // Keine Legende in der Minivorschau
    // Chart Titel aufhübschen
    QFont font;
    font.setPixelSize( 10 );
    chart->setTitleFont( font );
    chart->setTitleBrush( QBrush( Qt::darkBlue ) );
    chart->setTitle( tr( "PREVIEW" ) );
    // Hintergrund aufhübschen
    QBrush backgroundBrush( Qt::NoBrush );
    chart->setBackgroundBrush( backgroundBrush );
    // Malhintergrund auch noch
    QLinearGradient plotAreaGradient;
    plotAreaGradient.setStart( QPointF( 0, 1 ) );
    plotAreaGradient.setFinalStop( QPointF( 1, 0 ) );
    plotAreaGradient.setColorAt( 0.0, QRgb( 0x202040 ) );
    plotAreaGradient.setColorAt( 1.0, QRgb( 0x2020f0 ) );
    plotAreaGradient.setCoordinateMode( QGradient::ObjectBoundingMode );
    chart->setPlotAreaBackgroundBrush( plotAreaGradient );
    chart->setPlotAreaBackgroundVisible( true );
    //
    // Achse machen
    //
    // Y-Achse
    QPen axisPen( QRgb( 0xd18952 ) );
    axisPen.setWidth( 1 );
    axisY->setLinePen( axisPen );
    QBrush axisBrush( Qt::white );
    axisY->setLabelsBrush( axisBrush );
    // achsen grid lines and shades
    axisY->setGridLineVisible( false );
    axisY->setShadesPen( Qt::NoPen );
    axisY->setShadesBrush( QBrush( QColor( 0x99, 0xcc, 0xcc, 0x55 ) ) );
    axisY->setShadesVisible( true );
    // Achsen Werte und Bereiche setzten
    axisY->setRange( 0, 30 );
    auto *se = new QLineSeries();
    chart->setAxisY( axisY, se );
    chart->setMargins( QMargins( 0, 0, 0, 0 ) );
    // Hübsch malen
    chartView->setRenderHint( QPainter::Antialiasing );
  }

  /**
   * @brief hole/erzeuge daten für einen Tauchgang wenn vorhanden
   * @param deviceId Geräteid in der Datenbank
   * @param diveNum Nummer des TG für das Gerät
   */
  void LogFragment::showDiveDataForGraph( int deviceId, int diveNum )
  {
    // Polimorphes Objekt hier mit DEBUG belegt
    IDataSeriesGenerator *gen = new DebugDataSeriesGenerator( lg, spxConfig );
    // Device-Id für Datenbank hinterlegen
    gen->setDeviceId( deviceId );
    // erzeuge Datenserie(n)
    QLineSeries *series = gen->makeDepthSerie( diveNum );
    // die Serie aufhübschen
    QPen pen( QRgb( 0xfdb157 ) );
    pen.setWidth( 2 );
    series->setPen( pen );
    // Chartobjekt etwas leeren
    chart->removeAllSeries();
    chart->removeAxis( axisY );
    chart->addSeries( series );  // Serie zufügen
    // Y-Achse
    // Achsen Werte und Bereiche setzten
    // vorher Skalierung testen
    float min = getMinYValue( series );
    min += ( min / 8.0f );  // 8% zugeben
    axisY->setRange( static_cast< qreal >( min ), 0.50 );
    // in chart setzten
    chart->setAxisY( axisY, series );
    chartView->setChart( chart.get() );
  }

  /**
   * @brief Y-Minimum einer Serie finden
   * @param series Zeiger auf die serie (const)
   * @return Minimum
   */
  float LogFragment::getMinYValue( const QLineSeries *series )
  {
    QVector< QPointF > points = series->pointsVector();
    QVector< QPointF >::iterator it = points.begin();
    float min = FLT_MAX;
    while ( it != points.end() )
    {
      if ( it->ry() < static_cast< qreal >( min ) )
        min = static_cast< float >( it->ry() );
      it++;
    }
    return ( min );
  }

  /**
   * @brief Y-Maximum einer Serie finden
   * @param series Zeiger auf die serie (const)
   * @return Maximum
   */
  float LogFragment::getMaxYValue( const QLineSeries *series )
  {
    QVector< QPointF > points = series->pointsVector();
    QVector< QPointF >::iterator it = points.begin();
    float max = FLT_MIN;
    while ( it != points.end() )
    {
      if ( it->ry() > static_cast< qreal >( max ) )
        max = static_cast< float >( it->ry() );
      it++;
    }
    return ( max );
  }

  void LogFragment::onOnlineStatusChangedSlot( bool )
  {
    setGuiConnected( remoteSPX42->getConnectionStatus() == SPX42RemotBtDevice::SPX42_CONNECTED );

    if ( remoteSPX42->getConnectionStatus() != SPX42RemotBtDevice::SPX42_CONNECTED )
      logWriter.reset();
  }

  void LogFragment::onSocketErrorSlot( QBluetoothSocket::SocketError )
  {
    // TODO: implementieren
  }

  void LogFragment::onConfLicChangedSlot()
  {
    lg->debug( QString( "LogFragment::onOnlineStatusChangedSlot -> set: %1" )
                   .arg( static_cast< int >( spxConfig->getLicense().getLicType() ) ) );
    ui->tabHeaderLabel->setText(
        QString( tr( "LOGFILES SPX42 Serial [%1] Lic: %2" ).arg( spxConfig->getSerialNumber() ).arg( spxConfig->getLicName() ) ) );
  }

  void LogFragment::onCloseDatabaseSlot()
  {
    // TODO: implementieren
  }

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
            newListEntry = QString( "%1:[%2]" ).arg( newEntry.number, 2, 10, QChar( '0' ) ).arg( newEntry.getDateTimeStr() );
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
              dbWriterFuture =
                  QtConcurrent::run( &this->logWriter, &LogDetailWriter::writeLogDataToDatabase, remoteSPX42->getRemoteConnected() );
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
                dbWriterFuture =
                    QtConcurrent::run( &this->logWriter, &LogDetailWriter::writeLogDataToDatabase, remoteSPX42->getRemoteConnected() );
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

  void LogFragment::setGuiConnected( bool isConnected )
  {
    //
    // setzte die GUI Elemente entsprechend des Online Status
    //
    ui->readLogdirPushButton->setEnabled( isConnected );
    ui->readLogContentPushButton->setEnabled( isConnected );
    ui->tabHeaderLabel->setText( fragmentTitlePattern.arg( spxConfig->getSerialNumber() ).arg( spxConfig->getLicName() ) );
    chart->setVisible( isConnected );
    if ( !isConnected )
    {
      ui->logentryTableWidget->setRowCount( 0 );
      // preview löschen
      chart->removeAllSeries();
    }
  }

  void LogFragment::testForSavedDetails()
  {
    lg->debug( "LogFragment::testForSavedDetails..." );
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
        if ( database->existDiveLogInBase( remoteSPX42->getRemoteConnected().replace( ':', '-' ), diveNum ) )
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

}  // namespace spx
