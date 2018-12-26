﻿#include "LogFragment.hpp"

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
      , model( new QStringListModel() )
      , chart( new QtCharts::QChart() )
      , dummyChart( new QtCharts::QChart() )
      , chartView( new QtCharts::QChartView( dummyChart ) )
      , axisY( new QCategoryAxis() )
      , logWriter( this, logger, spx42Database )
  {
    lg->debug( "LogFragment::LogFragment..." );
    ui->setupUi( this );
    logWriter.reset();
    ui->transferProgressBar->setVisible( false );
    ui->transferProgressBar->setRange( 0, 0 );
    ui->logentryListView->setModel( model.get() );
    ui->logentryListView->setEditTriggers( QAbstractItemView::NoEditTriggers );
    // ui->logentryListView->setSelectionMode( QAbstractItemView::MultiSelection );
    ui->logentryListView->setSelectionMode( QAbstractItemView::ExtendedSelection );
    ui->logentryListView->setSpacing( 2 );
    fragmentTitlePattern = tr( "LOGFILES SPX42 Serial [%1] LIC: %2" );
    diveNumberStr = tr( "DIVE NUMBER: %1" );
    diveDateStr = tr( "DIVE DATE: %1" );
    diveDepthStr = tr( "DIVE DEPTH: %1" );
    ui->diveNumberLabel->setText( diveNumberStr.arg( "-" ) );
    ui->diveDateLabel->setText( diveDateStr.arg( "-" ) );
    ui->diveDepthLabel->setText( diveDepthStr.arg( "-" ) );
    prepareMiniChart();
    // tausche den Platzhalter aus und entsorge den gleich
    delete ui->detailsGroupBox->layout()->replaceWidget( ui->diveProfileGraphicsView, chartView );
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
    }
    onConfLicChangedSlot();
    connect( spxConfig.get(), &SPX42Config::licenseChangedSig, this, &LogFragment::onConfLicChangedSlot );
    connect( ui->readLogdirPushButton, &QPushButton::clicked, this, &LogFragment::onReadLogDirectorySlot );
    connect( ui->readLogContentPushButton, &QPushButton::clicked, this, &LogFragment::onReadLogContentSlot );
    connect( ui->logentryListView, &QAbstractItemView::clicked, this, &LogFragment::onLogListViewClickedSlot );
    connect( remoteSPX42.get(), &SPX42RemotBtDevice::onStateChangedSig, this, &LogFragment::onOnlineStatusChangedSlot );
    connect( remoteSPX42.get(), &SPX42RemotBtDevice::onSocketErrorSig, this, &LogFragment::onSocketErrorSlot );
    connect( remoteSPX42.get(), &SPX42RemotBtDevice::onCommandRecivedSig, this, &LogFragment::onCommandRecivedSlot );
    connect( &transferTimeout, &QTimer::timeout, this, &LogFragment::onTransferTimeout );
    connect( &logWriter, &LogDetailWriter::onWriteDoneSig, this, &LogFragment::onWriterDoneSlot );
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
    ui->logentryListView->setModel( Q_NULLPTR );
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
    ui->transferProgressBar->setVisible( false );
    lg->warn( "LogFragment::onTransferTimeout -> transfer timeout!!!" );
    // logWriter.reset();
    transferTimeout.stop();
    // TODO: Warn oder Fehlermeldung ausgeben
  }

  void LogFragment::onWriterDoneSlot( int )
  {
    lg->debug( "LogFragment::onWriterDoneSlot..." );
    // Writer ist fertig, es könnte weiter gehen
    if ( dbWriterFuture.isFinished() )
    {
      logWriter.reset();
      lg->debug( "LogFragment::onWriterDoneSlot -> writer finished!" );
    }
    //
    // wenn in der Queue was drin ist UND der Writer bereits fertig ist.
    // ISt er nicht fertig, wird er dieses beim Beenden selber noch einmal aufrufen
    //
    if ( !logDetailRead.isEmpty() && dbWriterFuture.isFinished() )
    {
      int logDetailNum = logDetailRead.dequeue();
      SendListEntry sendCommand_d = remoteSPX42->askForLogDetailFor( logDetailNum );
      remoteSPX42->sendCommand( sendCommand_d );
      lg->debug( "LogFragment::onWriterDoneSlot -> start writer thread..." );
      dbWriterFuture = QtConcurrent::run( &this->logWriter, &LogDetailWriter::writeLogDataToDatabase,
                                          remoteSPX42->getRemoteConnected(), logDetailNum );
    }
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
      model->setStringList( QStringList{} );
      // preview löschen
      ui->logentryListView->reset();
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
    QModelIndexList indexList = ui->logentryListView->selectionModel()->selectedIndexes();
    if ( model->rowCount() == 0 )
    {
      lg->warn( "LogFragment::onReadLogContentSlot: no log entrys!" );
      return;
    }
    //
    // gibt es was zu tun?
    //
    if ( indexList.isEmpty() )
    {
      lg->warn( "LogFragment::onReadLogContentSlot: nothing selected, read all?" );
      // TODO: Messagebox aufpoppen und Nutzer fragen
      return;
    }
    lg->debug( QString( "LogFragment::onReadLogContentSlot: read %1 logs from spx42..." ).arg( indexList.count() ) );
    //
    // so, fülle mal die Queue mit den zu lesenden Nummern
    //
    logDetailRead.clear();
    for ( auto idxEntry : indexList )
    {
      QStringList el = idxEntry.data().toString().split( ':' );
      logDetailRead.enqueue( el.at( 0 ).toInt() );
    }
    //
    // und nu simuliere ich den "FERTIG" Ruf des Writers...
    //
    onWriterDoneSlot( 0 );
    /*
    logWriter.reset();
    SendListEntry sendCommand_d = remoteSPX42->askForLogDetailFor( 3 );
    remoteSPX42->sendCommand( sendCommand_d );
    lg->debug( "LogFragment::onReadLogContentSlot -> start writer thread..." );
    dbWriterFuture =
        QtConcurrent::run( &this->logWriter, &LogDetailWriter::writeLogDataToDatabase, remoteSPX42->getRemoteConnected(), 3 );
    // TODO: mehrfach starten verboten!
    transferTimeout.start( TIMEOUTVAL );
    */
  }

  /**
   * @brief Slot für das Signal wenn auf einen Log Directory Eintrag geklickt wird
   * @param index welcher Index war es?
   */
  void LogFragment::onLogListViewClickedSlot( const QModelIndex &index )
  {
    QString entry = index.data().toString();
    QString number = "-";
    QString date = "-";
    QString depth = "-";
    lg->debug( QString( "LogFragment::onLogListViewClickedSlot: data: %1..." ).arg( entry ) );
    QStringList pieces = index.data().toString().split( ':' );
    number = pieces.at( 0 );
    int start = entry.indexOf( '[' ) + 1;
    int end = entry.indexOf( ']' );
    date = entry.mid( start, end - start );

    /*
    [08.07.2012 12:13:46]
    oder
    [2012/07/08 12:13:46]
    */
    // TODO: aus dem Eintrag die Nummer lesen
    // TODO: TG in der Database -> Parameter auslesen und in die Labels eintragen
    ui->diveNumberLabel->setText( diveNumberStr.arg( number ) );
    ui->diveDateLabel->setText( diveDateStr.arg( date ) );
    ui->diveDepthLabel->setText( diveDepthStr.arg( depth ) );
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
    // älteste zuerst...
    //
    // int row = model->rowCount();
    // model->insertRows( row, 1 );
    // QModelIndex index = model->index( row );
    //
    // neueste zuerst
    //
    model->insertRows( 0, 1 );
    QModelIndex index = model->index( 0 );
    model->setData( index, entry );
  }

  /**
   * @brief Slot für ankommende Logdaten (für eine Logdatei)
   * @param line die Logzeile
   */
  void LogFragment::onAddLogLineSlot( const QString &line )
  {
    lg->debug( QString( "LogFragment::onAddLogLineSlot: logline: <" ).append( line ).append( ">" ) );
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
          // Das Ergebnis in die Liste der Config
          newEntry = SPX42LogDirectoryEntry( static_cast< int >( recCommand->getValueFromHexAt( SPXCmdParam::LOGDIR_CURR_NUMBER ) ),
                                             static_cast< int >( recCommand->getValueFromHexAt( SPXCmdParam::LOGDIR_MAXNUMBER ) ),
                                             recCommand->getParamAt( SPXCmdParam::LOGDIR_FILENAME ) );
          spxConfig->addDirectoryEntry( newEntry );
          newListEntry = QString( "%1:[%2]" ).arg( newEntry.number, 2, 10, QChar( '0' ) ).arg( newEntry.getDateTimeStr() );
          onAddLogdirEntrySlot( newListEntry );
          //
          // war das der letzte Eintrag oder sollte noch mehr kommen
          //
          if ( newEntry.number == newEntry.maxNumber )
          {
            // TODO: fertig Markieren
            ui->transferProgressBar->setVisible( false );
            transferTimeout.stop();
          }
          else
          {
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
            // START der Details...
            logWriter.clear();
            // Timer verlängern
            transferTimeout.start( TIMEOUTVAL );
          }
          else
          {
            // ENDE der Details
            logWriter.addDetail( recCommand );
            // Timer ist auch zu stoppen!
            transferTimeout.stop();
          }
          break;
        case SPX42CommandDef::SPX_GET_LOG_DETAIL:
          // Datensatz in die Wartschlange
          logWriter.addDetail( recCommand );
          lg->debug( QString( "LogFragment::onDatagramRecivedSlot -> log detail %1 for dive number %2..." )
                         .arg( logWriter.getGlobal() )
                         .arg( recCommand->getTag() ) );
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
      model->setStringList( QStringList{} );
      // preview löschen
      ui->logentryListView->reset();
      chart->removeAllSeries();
    }
  }

}  // namespace spx
