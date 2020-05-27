﻿#include "SPX42ControlMainWin.hpp"
#include "ui_SPX42ControlMainWin.h"

namespace spx
{
  const CmdMarker SPX42ControlMainWin::marker{'\0'};
  const QByteArray SPX42ControlMainWin::ar{};

  /**
   * @brief Der Konstruktor des Hauptfensters
   * @param parent Elternteil...
   */
  SPX42ControlMainWin::SPX42ControlMainWin( QWidget *parent )
      : QMainWindow( parent )
      , ui( new Ui::SPX42ControlMainWin() )
      , spx42Config( new SPX42Config() )
      , onlineLabel( std::unique_ptr< QLabel >( new QLabel( tr( "SPX42 OFFLINE" ) ) ) )
      , akkuLabel( std::unique_ptr< QLabel >( new QLabel( "---" ) ) )
      , waitForWriteLabel( std::unique_ptr< QLabel >( new QLabel( tr( "CLEAR" ) ) ) )
      , currentStatus( ApplicationStat::STAT_OFFLINE )
      , watchdogCounter( 0 )
      , zyclusCounter( 0 )
      , currentTab()
      , offlinePalette( onlineLabel->palette() )
      , onlinePalette( onlineLabel->palette() )
      , busyPalette( onlineLabel->palette() )
      , connectingPalette( onlineLabel->palette() )
      , errorPalette( onlineLabel->palette() )
  {
    //
    // Hilfebutton ausblenden...
    //
    this->setWindowFlags( this->windowFlags() & ~Qt::WindowContextHelpButtonHint );
    cf.loadSettings();
    //
    // Programmlogger initialisieren
    //
    if ( !createLogger() )
    {
      exit( -1 );
    }
    *lg << LDEBUG << "SPX42ControlMainWin::SPX42ControlMainWin: Program start..." << Qt::endl;
    *lg << LDEBUG << "SPX42ControlMainWin::SPX42ControlMainWin: Program build date:   " << AppConfigClass::getBuildDate() << Qt::endl;
    *lg << LDEBUG << "SPX42ControlMainWin::SPX42ControlMainWin: Program build number: " << AppConfigClass::getBuildNumStr()
        << Qt::endl;
    //
    // erzeuge ein Objekt für den entfernten SPX42
    //
    remoteSPX42 = std::shared_ptr< SPX42RemotBtDevice >( new SPX42RemotBtDevice( lg, this ) );
    //
    // Datenbank öffnen oder initialisieren
    //
    spx42Database = std::shared_ptr< SPX42Database >( new SPX42Database( lg, cf.getFullDatabaseLocation(), this ) );
    QSqlError err = spx42Database->openDatabase( true );
    if ( err.isValid() && err.type() != QSqlError::NoError )
    {
      QMessageBox::critical( this, tr( "CRITICAL" ), tr( "database can't open!" ) );
      this->close();
    }
    //
    // GUI initialisieren
    //
    ui->setupUi( this );
    //
    // Font laden
    //
    int id1 = QFontDatabase::addApplicationFont( ":/fonts/Hack-Regular.ttf" );
    int id2 = QFontDatabase::addApplicationFont( ":/fonts/DejaVuSansMono.ttf" );
    int id3 = QFontDatabase::addApplicationFont( ":/fonts/Hack-Bold.ttf" );
    int id4 = QFontDatabase::addApplicationFont( ":/fonts/Hack-Italic.ttf" );
    int id5 = QFontDatabase::addApplicationFont( ":/fonts/Hack-BoldItalic.ttf" );
    int id6 = QFontDatabase::addApplicationFont( ":/fonts/bahnschrift.ttf" );
    if ( id1 < 0 || id2 < 0 || id3 < 0 || id4 < 0 || id5 < 0 || id6 < 0 )
    {
      QMessageBox::critical( this, tr( "CRITICAL" ), tr( "internal font(s) can't load!" ) );
    }
    else
    {
      // setFont( QFont( "DejaVu Sans Mono" ) );
      // setFont( QFont( "Hack" ) );
      setFont( QFont( "Bahnschrift" ) );
    }
    //
    // das folgende wird nur kompiliert, wenn DEBUG NICHT konfiguriert ist
    //
#ifndef DEBUG
    ui->menuDEBUGGING->clear();
    ui->menuDEBUGGING->setParent( Q_NULLPTR );
    delete ( ui->menuDEBUGGING );
#endif
    //
    // einen onlineindikator in die Statusleiste bauen
    //
    makeOnlineStatus();
    //
    fillTabTitleArray();
    //
    // setze Action Stati
    //
    setActionStati();
    //
    // TAB initialisieren
    //
    createApplicationTabs();
    //
    // Actions mit den richtigen Slots verbinden
    //
    ui->areaTabWidget->setCurrentIndex( 0 );
    onTabCurrentChangedSlot( 0 );
    connectActions();
    QString mainTitleString;
#ifdef TESTVERSION
#ifdef DEBUG
    setWindowTitle( QString( "%1 - %2 - %3" ).arg( ProjectConst::MAIN_TITLE ).arg( "TESTVERSION" ).arg( "DEBUG" ) );
#else
    setWindowTitle( QString( "%1 - %2" ).arg( ProjectConst::MAIN_TITLE ).arg( "TESTVERSION" ) );
#endif
#else
#ifdef DEBUG
    setWindowTitle( QString( "%1 - %2" ).arg( ProjectConst::MAIN_TITLE ).arg( "DEBUG" ) );
#else
    setWindowTitle( ProjectConst::MAIN_TITLE );
#endif
#endif
    setOnlineStatusMessage();
    connect( &watchdog, &QTimer::timeout, this, &SPX42ControlMainWin::onWatchdogTimerSlot );
    watchdog.start( MAIN_WATCHDOG_TIMERCOUNT );
    connect( &configWriteTimer, &QTimer::timeout, this, &SPX42ControlMainWin::onConfigWriteBackSlot );
    configWriteTimer.stop();
  }

  /**
   * @brief Der Destruktor, Aufräumarbeiten
   */
  SPX42ControlMainWin::~SPX42ControlMainWin()
  {
    *lg << LDEBUG << "SPX42ControlMainWin::~SPX42ControlMainWin..." << Qt::endl;
    try
    {
      if ( watchdog.isActive() )
      {
        watchdog.stop();
      }
    }
    catch ( std::exception &ex )
    {
      *lg << LCRIT << "SPX42ControlMainWin::~SPX42ControlMainWin -> watchdog stopping failed " << ex.what() << Qt::endl;
    }
    clearApplicationTabs();
    spx42Database->closeDatabase();
  }

  /**
   * @brief Ereignis bei der Anfrage nach Beenden der App
   * @param event Das Ereignis
   */
  void SPX42ControlMainWin::closeEvent( QCloseEvent *event )
  {
    QMessageBox::StandardButton result;
    result = QMessageBox::question( this, tr( "Really EXIT?" ),
                                    tr( "Will you really quit this programm and close all files, databases, connections?" ),
                                    QMessageBox::No | QMessageBox::Yes, QMessageBox::No );
    if ( QMessageBox::Yes != result )
    {
      event->ignore();
      *lg << LDEBUG << "SPX42ControlMainWin::closeEvent -> ignore close application..." << Qt::endl;
      return;
    }
    *lg << LDEBUG << "SPX42ControlMainWin::closeEvent -> close application..." << Qt::endl;
    event->accept();
    disconnectActions();
    QMainWindow::closeEvent( event );
  }

  /**
   * @brief SPX42ControlMainWin::getLogger
   * @return
   */
  std::shared_ptr< Logger > SPX42ControlMainWin::getLogger( void )
  {
    return ( lg );
  }

  /**
   * @brief SPX42ControlMainWin::getConfig
   * @return
   */
  AppConfigClass &SPX42ControlMainWin::getConfig()
  {
    return ( cf );
  }

  /**
   * @brief SPX42ControlMainWin::makeOnlineStatus
   */
  void SPX42ControlMainWin::makeOnlineStatus()
  {
    onlinePalette.setColor( onlineLabel->foregroundRole(), ProjectConst::COLOR_ONLINE );
    busyPalette.setColor( onlineLabel->foregroundRole(), ProjectConst::COLOR_BUSY );
    offlinePalette.setColor( onlineLabel->foregroundRole(), ProjectConst::COLOR_OFFLINE );
    connectingPalette.setColor( onlineLabel->foregroundRole(), ProjectConst::COLOR_CONNECTING );
    errorPalette.setColor( onlineLabel->foregroundRole(), ProjectConst::COLOR_ERROR );
    onlineLabel->setIndent( 25 );
    this->statusBar()->addWidget( onlineLabel.get(), 250 );
    this->statusBar()->addWidget( waitForWriteLabel.get(), 40 );
    this->statusBar()->addWidget( akkuLabel.get(), 40 );
    onlineLabel->setPalette( offlinePalette );
    akkuLabel->setPalette( offlinePalette );
    waitForWriteLabel->setPalette( offlinePalette );
  }

  /**
   * @brief Erzeuge den Programm Logger
   * @return erfolgreich?
   */
  bool SPX42ControlMainWin::createLogger()
  {
    //
    // erzeuge einen Logger, untersuche zunächst ob es das Verzeichnis gibt
    //
    QString logDirStr = cf.getLogfilePath();
    QDir logDir( logDirStr );
    // Logger erzeugen
    lg = std::shared_ptr< Logger >( new Logger() );
    if ( lg )
    {
      //
      // gibt es das Verzeichnis
      //
      if ( !logDir.exists() )
      {
        if ( !QDir().mkpath( logDirStr ) )
        {
          //
          // Das ging schief
          //
          qDebug() << "SPX42ControlMainWin::createLogger -> path NOT created!";
          QMessageBox msgBox;
          msgBox.setText( tr( "Log dirctory create FAIL!" ) );
          msgBox.setDetailedText( tr( "Check write rights for log directory or reinstall software.\n(%1)" ).arg( logDirStr ) );
          msgBox.setIcon( QMessageBox::Critical );
          msgBox.exec();
          return ( false );
        }
      }
      //
      // das wird wohl klappen
      //
      lg->startLogging( static_cast< LgThreshold >( cf.getLogTreshold() ), cf.getLogfileName(), cf.getConsoleLog() );
      return ( true );
    }
    //
    // Fehler, melde das dem User
    //
    QMessageBox msgBox;
    msgBox.setText( tr( "Logging start FAIL!" ) );
    msgBox.setDetailedText( tr( "Check write rights for program directory or reinstall software." ) );
    msgBox.setIcon( QMessageBox::Critical );
    msgBox.exec();
    return ( false );
  }

  /**
   * @brief Erzeuge die Titel für die Programmtabs
   */
  void SPX42ControlMainWin::fillTabTitleArray()
  {
    tabTitle.clear();
    tabTitle << tr( "CONNECTION" );  // CONNECT_TAB
    tabTitle << tr( "INFO" );        // DEVICE_INFO_TAB
    tabTitle << tr( "CONFIG" );      // CONFIG_TAB
    tabTitle << tr( "GAS" );         // GAS_TAB
    tabTitle << tr( "LOG" );         // Logger-Tab
    tabTitle << tr( "CHARTS" );      // Loggiing CHARTS
  }

  /**
   * @brief Setze die GUI entsprechend des aktuellen Status
   * @return erfolgreich?
   */
  bool SPX42ControlMainWin::setActionStati()
  {
    switch ( currentStatus )
    {
      case ApplicationStat::STAT_OFFLINE:
        ui->actionAbout->setEnabled( true );
        ui->actionSettings->setEnabled( true );
        break;
      case ApplicationStat::STAT_ONLINE:
        ui->actionAbout->setEnabled( true );
        ui->actionSettings->setEnabled( true );
        break;
      case ApplicationStat::STAT_ERROR:
        ui->actionAbout->setEnabled( true );
        ui->actionSettings->setEnabled( true );
        break;
    }
    return ( true );
  }

  /**
   * @brief Verbinde die Signale mit ihren Slots
   * @return  erfolgreich?
   */
  bool SPX42ControlMainWin::connectActions()
  {
    try
    {
      //
      // Optionen action
      //
      connect( ui->actionSettings, &QAction::triggered, this, &SPX42ControlMainWin::onOptionsActionSlot );
      //
      // About angeklickt
      //
      connect( ui->actionAbout, &QAction::triggered, [=]( bool checked ) {
        *lg << LDEBUG << "SPX42ControlMainWin::aboutActionSlot <" << ( checked ? "True" : "False" ) << ">" << Qt::endl;
        AboutDialog dlg( this, cf, lg );
        dlg.exec();
      } );
      //
      // CLOSE
      //
      connect( ui->actionQUIT, &QAction::triggered, [=]( bool checked ) {
        *lg << LDEBUG << "SPX42ControlMainWin::quitActionSlot <" << ( checked ? "True" : "False" ) << ">" << Qt::endl;
        close();
      } );
      //
      // der SPX42 Onlinestatusbutton
      //
      connect( ui->actionSPX_STATE, &QAction::triggered, [this] {
        if ( this->remoteSPX42->getConnectionStatus() == SPX42RemotBtDevice::SPX42_CONNECTED )
        {
          this->remoteSPX42->endConnection();
        }
      } );
      connect( ui->actionGetHelp, &QAction::triggered, this, &SPX42ControlMainWin::onGetHelpForUser );
      //
      // TAB wurde gewechselt
      //
      connect( ui->areaTabWidget, &QTabWidget::currentChanged, this, &SPX42ControlMainWin::onTabCurrentChangedSlot );
      //
      // Lizenz wurde geändert
      //
      connect( spx42Config.get(), &SPX42Config::licenseChangedSig, this, &SPX42ControlMainWin::onLicenseChangedSlot );
      //
      // onlinestatus wurde geändert
      //
      connect( remoteSPX42.get(), &SPX42RemotBtDevice::onStateChangedSig, this, &SPX42ControlMainWin::onOnlineStatusChangedSlot );
#ifdef DEBUG
      //
      // simuliert zu Nitrox lizenz geändert
      //
      connect( ui->actionNitrox, &QAction::triggered, this, [=]( bool tr ) {
        if ( tr )
          simulateLicenseChanged( LicenseType::LIC_NITROX );
      } );
      //
      // simuliert zu TX Normoxic geändert
      //
      connect( ui->actionNormoxic_TMX, &QAction::triggered, this, [=]( bool tr ) {
        if ( tr )
          simulateLicenseChanged( LicenseType::LIC_NORMOXIX );
      } );
      //
      // simuliert zu full Trimix geändert
      //
      connect( ui->actionFull_TMX, &QAction::triggered, this, [=]( bool tr ) {
        if ( tr )
          simulateLicenseChanged( LicenseType::LIC_FULLTMX );
      } );
      //
      // simuliert zu militära Lizenz geändert
      //
      connect( ui->actionMilitary, &QAction::triggered, this, [=]( bool tr ) {
        if ( tr )
          simulateLicenseChanged( LicenseType::LIC_MIL );
      } );
      //
      // simuliert Individuell Lizenz geändert
      //
      connect( ui->actionINDIVIDUAL_LIC, &QAction::triggered, [=]() {
        *lg << LDEBUG << "SPX42ControlMainWin::simulateIndividualLicenseChanged ..." << Qt::endl;
        if ( ui->actionINDIVIDUAL_LIC->isChecked() )
        {
          spx42Config->setLicense( IndividualLicense::LIC_INDIVIDUAL );
        }
        else
        {
          spx42Config->setLicense( IndividualLicense::LIC_NONE );
        }
      } );
#endif
    }
    catch ( std::exception &ex )
    {
      *lg << LCRIT << "exception while connecting signals to slots (" << ex.what() << ")" << Qt::endl;
      return ( false );
    }
    return ( true );
  }

  /**
   * @brief SPX42ControlMainWin::disconnectActions
   * @return
   */
  bool SPX42ControlMainWin::disconnectActions()
  {
    //
    // vor dem Beenden die sig von den slot trennen, sonst werden tabs
    // beim beenden erzeugt, was zum absturz führt
    //
    try
    {
      disconnect( ui->actionAbout, nullptr, nullptr, nullptr );
      disconnect( ui->areaTabWidget, nullptr, nullptr, nullptr );
      disconnect( spx42Config.get(), nullptr, this, nullptr );
      disconnect( ui->actionNitrox, nullptr, nullptr, nullptr );
      disconnect( ui->actionNormoxic_TMX, nullptr, nullptr, nullptr );
      disconnect( ui->actionFull_TMX, nullptr, nullptr, nullptr );
      disconnect( ui->actionMilitary, nullptr, nullptr, nullptr );
      disconnect( ui->actionINDIVIDUAL_LIC, nullptr, nullptr, nullptr );
    }
    catch ( std::exception &ex )
    {
      *lg << LCRIT << "exception while disconnecting signals from slots (" << ex.what() << ")" << Qt::endl;
      return ( false );
    }
    return ( true );
  }

  /**
   * @brief Simuliere Lizenzwechsel (debugging)
   * @param lType Neuer Lizenztyp
   */
  void SPX42ControlMainWin::simulateLicenseChanged( LicenseType lType )
  {
    //
    // fürs debuggen simuliere einen wechsel der Lizenz
    //
    spx42Config->setLicense( lType );
  }

  /**
   * @brief Erzeuge die Programm-Tabs
   */
  void SPX42ControlMainWin::createApplicationTabs()
  {
    QWidget *wg;
    //
    // lasse die TABs und Inhalte verschwinden
    //
    while ( ui->areaTabWidget->count() )
    {
      QWidget *canDel = ui->areaTabWidget->widget( 0 );
      ui->areaTabWidget->removeTab( 0 );
      delete canDel;
    }

    //
    // der Connect Tab Platzhalter
    // ACHTUNG: tabTitle hat eine Grösse, bein einfügen tabTitle erweitern
    //
    //
    wg = new QWidget();
    wg->setObjectName( "DUMMY" );
    ui->areaTabWidget->addTab( wg, tabTitle.at( static_cast< int >( ApplicationTab::CONNECT_TAB ) ) );
    //
    // der DEVINFO-Platzhalter
    //
    wg = new QWidget();
    wg->setObjectName( "DUMMY" );
    ui->areaTabWidget->addTab( wg, tabTitle.at( static_cast< int >( ApplicationTab::DEVICE_INFO_TAB ) ) );
    //
    // der CONFIG-Platzhalter
    //
    wg = new QWidget();
    wg->setObjectName( "DUMMY" );
    ui->areaTabWidget->addTab( wg, tabTitle.at( static_cast< int >( ApplicationTab::CONFIG_TAB ) ) );
    //
    // der Gaslisten-Platzhalter
    //
    wg = new QWidget();
    wg->setObjectName( "DUMMY" );
    ui->areaTabWidget->addTab( wg, tabTitle.at( static_cast< int >( ApplicationTab::GAS_TAB ) ) );
    //
    // der Logging-Platzhalter
    //
    wg = new QWidget();
    wg->setObjectName( "DUMMY" );
    ui->areaTabWidget->addTab( wg, tabTitle.at( static_cast< int >( ApplicationTab::LOG_TAB ) ) );
    //
    // der Chart Platzhalter
    //
    wg = new QWidget();
    wg->setObjectName( "DUMMY" );
    ui->areaTabWidget->addTab( wg, tabTitle.at( static_cast< int >( ApplicationTab::CHART_TAB ) ) );
  }

  /**
   * @brief Leere die Application Tabs
   */
  void SPX42ControlMainWin::clearApplicationTabs()
  {
    for ( int i = 0; i < ui->areaTabWidget->count(); i++ )
    {
      QWidget *currObj = ui->areaTabWidget->widget( i );
      if ( !QString( currObj->metaObject()->className() ).startsWith( "QWidget" ) )
      {
        ui->areaTabWidget->removeTab( i );
        *lg << LDEBUG << "SPX42ControlMainWin::clearApplicationTabs -> <" << currObj->objectName() << "> should delete" << Qt::endl;
        delete currObj;
        currObj = new QWidget();
        currObj->setObjectName( "DUMMY" );
#ifdef DEBUG
        ui->areaTabWidget->insertTab( i, currObj, QString( tabTitle.at( i ) ).append( "-D" ) );
#else
        ui->areaTabWidget->insertTab( i, currObj, tabTitle.at( i ) );
#endif
      }
    }
  }

  /**
   * @brief Erfrage den aktuellen aktiven Tab
   * @return aktiver Tab
   */
  ApplicationTab SPX42ControlMainWin::getApplicationTab()
  {
    return ( currentTab );
  }

  void SPX42ControlMainWin::setOnlineStatusMessage( const QString &msg )
  {
    QString online;
    // ist die Kiste verbunden?
    QFont font = onlineLabel->font();
    //
    // welcher Fall?
    //
    switch ( remoteSPX42->getConnectionStatus() )
    {
      case SPX42RemotBtDevice::SPX42_DISCONNECTED:
      case SPX42RemotBtDevice::SPX42_DISCONNECTING:
        //
        // nicht verbunden darstellen
        //
        online = tr( "SPX42 OFFLINE" );
        onlineLabel->setPalette( offlinePalette );
        waitForWriteLabel->setPalette( offlinePalette );
        ui->actionSPX_STATE->setIcon( QIcon( ":/icons/ic_spx_offline" ) );
        ui->actionSPX_STATE->setStatusTip( tr( "spx42 is offline..." ) );
        break;
      case SPX42RemotBtDevice::SPX42_CONNECTED:
        //
        // verbunden darstellen
        //
        online = tr( "SPX42 ONLINE" );
        onlineLabel->setPalette( onlinePalette );
        waitForWriteLabel->setPalette( onlinePalette );
        ui->actionSPX_STATE->setEnabled( true );
        ui->actionSPX_STATE->setIcon( QIcon( ":/icons/ic_spx_online" ) );
        ui->actionSPX_STATE->setStatusTip( tr( "spx42 is online, click for disconnect..." ) );
        break;
      case SPX42RemotBtDevice::SPX42_CONNECTING:
        //
        // verbinden... darstellen
        //
        online = tr( "SPX42 CONNECTING" );
        onlineLabel->setPalette( connectingPalette );
        break;
      case SPX42RemotBtDevice::SPX42_ERROR:
        //
        // FEHLER darstellen
        //
        online = tr( "SPX42 OFFLINE/ERROR" );
        onlineLabel->setPalette( errorPalette );
        ui->actionSPX_STATE->setIcon( QIcon( ":/icons/ic_spx_error" ) );
        ui->actionSPX_STATE->setStatusTip( tr( "spx42 had connection error..." ) );
        break;
    }
    onlineLabel->setFont( font );
    //
    // und, ist eine Meldung vorhanden?
    //
    if ( !msg.isEmpty() )
    {
      onlineLabel->setText( QString( "%1 - %2" ).arg( online ).arg( msg ) );
    }
    else
    {
      onlineLabel->setText( QString( "%1" ).arg( online ) );
    }
  }

  /**
   * @brief SPX42ControlMainWin::onWatchdogTimerSlot
   */
  void SPX42ControlMainWin::onWatchdogTimerSlot()
  {
    //
    // Falls der Wachhund aktiv ist
    //
    if ( watchdogCounter > 0 )
    {
      if ( --watchdogCounter == 0 )
      {
        *lg << LWARN << "SPX42ControlMainWin::onWatchdogTimerSlot -> watchdog alert!" << Qt::endl;
      }
    }
    //
    // wenn der SPX Online ist...
    //
    if ( remoteSPX42->getConnectionStatus() == SPX42RemotBtDevice::SPX42_CONNECTED )
    {
      if ( --zyclusCounter < 0 )
      {
        //
        // nächsten Zeitpunkt vorbereiten
        //
        zyclusCounter = MAIN_ALIVE_TIMEVALUE;
#ifdef DEBUG
        *lg << LDEBUG << "SPX42ControlMainWin::onWatchdogTimerSlot -> send cmd alive..." << Qt::endl;
#endif
        if ( remoteSPX42->getIsNormalCommandMode() )
        {
          //
          // während der übertragung der logdetails NICHT nachd em Acku fragen
          //
          SendListEntry sendCommand = remoteSPX42->aksForAliveSignal();
          remoteSPX42->sendCommand( sendCommand );
        }
      }
    }
  }

  /**
   * @brief Slot, welcher mit dem Tab-Wechsel-dich Signal verbunden wird
   * @param idx
   */
  void SPX42ControlMainWin::onTabCurrentChangedSlot( int idx )
  {
    static bool ignore = false;
    QWidget *currObj = nullptr;

    if ( ignore )
    {
      return;
    }
    ignore = true;
    *lg << LDEBUG << QString( "SPX42ControlMainWin::tabCurrentChanged -> new index <%1>" ).arg( idx, 2, 10, QChar( '0' ) ) << Qt::endl;
    clearApplicationTabs();
    // altes widget aus dem Tab entfernen
    currObj = ui->areaTabWidget->widget( idx );
    ui->areaTabWidget->removeTab( idx );
    delete currObj;
    currObj = nullptr;
    //
    // Neuen Inhalt des Tabs aufbauen
    //
    switch ( idx )
    {
      // default:
      case static_cast< int >( ApplicationTab::CONNECT_TAB ):
        *lg << LDEBUG << "SPX42ControlMainWin::setApplicationTab -> CONNECT TAB..." << Qt::endl;
        currObj = new ConnectFragment( this, lg, spx42Database, spx42Config, remoteSPX42, &cf );
        currObj->setObjectName( "spx42Connect" );
        ui->areaTabWidget->insertTab( idx, currObj, tabTitle.at( static_cast< int >( ApplicationTab::CONNECT_TAB ) ) );
        currentTab = ApplicationTab::CONNECT_TAB;
        connect( dynamic_cast< ConnectFragment * >( currObj ), &ConnectFragment::onWarningMessageSig, this,
                 &SPX42ControlMainWin::onWarningMessageSlot );
        connect( dynamic_cast< ConnectFragment * >( currObj ), &ConnectFragment::onAkkuValueChangedSig, this,
                 &SPX42ControlMainWin::onAkkuValueChangedSlot );
        break;

      case static_cast< int >( ApplicationTab::DEVICE_INFO_TAB ):
        *lg << LDEBUG << "SPX42ControlMainWin::setApplicationTab -> Device INFO TAB..." << Qt::endl;
        currObj = new DeviceInfoFragment( this, lg, spx42Database, spx42Config, remoteSPX42, &cf );
        currObj->setObjectName( "spx42DeviceInfo" );
        ui->areaTabWidget->insertTab( idx, currObj, tabTitle.at( static_cast< int >( ApplicationTab::DEVICE_INFO_TAB ) ) );
        currentTab = ApplicationTab::DEVICE_INFO_TAB;
        connect( dynamic_cast< DeviceInfoFragment * >( currObj ), &DeviceInfoFragment::onAkkuValueChangedSig, this,
                 &SPX42ControlMainWin::onAkkuValueChangedSlot );
        break;

      case static_cast< int >( ApplicationTab::CONFIG_TAB ):
        *lg << LDEBUG << "SPX42ControlMainWin::setApplicationTab -> CONFIG TAB..." << Qt::endl;
        currObj = new DeviceConfigFragment( this, lg, spx42Database, spx42Config, remoteSPX42, &cf );
        currObj->setObjectName( "spx42Config" );
        ui->areaTabWidget->insertTab( idx, currObj, tabTitle.at( static_cast< int >( ApplicationTab::CONFIG_TAB ) ) );
        currentTab = ApplicationTab::CONFIG_TAB;
        connect( dynamic_cast< DeviceConfigFragment * >( currObj ), &DeviceConfigFragment::onConfigWasChangedSig, this,
                 &SPX42ControlMainWin::onConfigWasChangedSlot );
        connect( dynamic_cast< DeviceConfigFragment * >( currObj ), &DeviceConfigFragment::onAkkuValueChangedSig, this,
                 &SPX42ControlMainWin::onAkkuValueChangedSlot );
        connect( this, &SPX42ControlMainWin::onSendBufferStateChangedSig, dynamic_cast< DeviceConfigFragment * >( currObj ),
                 &DeviceConfigFragment::onSendBufferStateChangedSlot );
        break;

      case static_cast< int >( ApplicationTab::GAS_TAB ):
        *lg << LDEBUG << "SPX42ControlMainWin::setApplicationTab -> GAS TAB..." << Qt::endl;
        currObj = new GasFragment( this, lg, spx42Database, spx42Config, remoteSPX42, &cf );
        currObj->setObjectName( "spx42Gas" );
        ui->areaTabWidget->insertTab( idx, currObj, tabTitle.at( static_cast< int >( ApplicationTab::GAS_TAB ) ) );
        currentTab = ApplicationTab::GAS_TAB;
        connect( dynamic_cast< GasFragment * >( currObj ), &GasFragment::onConfigWasChangedSig, this,
                 &SPX42ControlMainWin::onConfigWasChangedSlot );
        connect( dynamic_cast< GasFragment * >( currObj ), &GasFragment::onAkkuValueChangedSig, this,
                 &SPX42ControlMainWin::onAkkuValueChangedSlot );
        connect( this, &SPX42ControlMainWin::onSendBufferStateChangedSig, dynamic_cast< GasFragment * >( currObj ),
                 &GasFragment::onSendBufferStateChangedSlot );
        break;

      case static_cast< int >( ApplicationTab::LOG_TAB ):
        *lg << LDEBUG << "SPX42ControlMainWin::setApplicationTab -> LOG TAB..." << Qt::endl;
        currObj = new LogFragment( this, lg, spx42Database, spx42Config, remoteSPX42, &cf );
        currObj->setObjectName( "spx42log" );
        static_cast< LogFragment * >( currObj )->setExportPath( cf.getExportPath() );
        ui->areaTabWidget->insertTab( idx, currObj, tabTitle.at( static_cast< int >( ApplicationTab::LOG_TAB ) ) );
        currentTab = ApplicationTab::LOG_TAB;
        connect( dynamic_cast< LogFragment * >( currObj ), &LogFragment::onAkkuValueChangedSig, this,
                 &SPX42ControlMainWin::onAkkuValueChangedSlot );
        connect( this, &SPX42ControlMainWin::onSendBufferStateChangedSig, dynamic_cast< LogFragment * >( currObj ),
                 &LogFragment::onSendBufferStateChangedSlot );
        break;

      case static_cast< int >( ApplicationTab::CHART_TAB ):
        *lg << LDEBUG << "SPX42ControlMainWin::setApplicationTab -> CHART TAB..." << Qt::endl;
        currObj = new ChartsFragment( this, lg, spx42Database, spx42Config, remoteSPX42, &cf );
        currObj->setObjectName( "spx42charts" );
        ui->areaTabWidget->insertTab( idx, currObj, tabTitle.at( static_cast< int >( ApplicationTab::CHART_TAB ) ) );
        currentTab = ApplicationTab::CHART_TAB;
        connect( dynamic_cast< ChartsFragment * >( currObj ), &ChartsFragment::onAkkuValueChangedSig, this,
                 &SPX42ControlMainWin::onAkkuValueChangedSlot );
        break;
    }
    ui->areaTabWidget->setCurrentIndex( idx );
    ignore = false;
  }

  /**
   * @brief Reaktion auf Lizenzänderung (Signal Lizenz Änderung)
   */
  void SPX42ControlMainWin::onLicenseChangedSlot()
  {
    *lg << LDEBUG << "SPX42ControlMainWin::onLicenseChangedSlot -> set: " << spx42Config->getLicName() << Qt::endl;
//
// das folgende wird nur kompiliert, wenn DEBUG konfiguriert ist
//
#ifdef DEBUG
    switch ( static_cast< int >( spx42Config->getLicense().getLicType() ) )
    {
      case static_cast< int >( LicenseType::LIC_NITROX ):
        ui->actionNormoxic_TMX->setChecked( false );
        ui->actionFull_TMX->setChecked( false );
        ui->actionMilitary->setChecked( false );
        break;
      case static_cast< int >( LicenseType::LIC_NORMOXIX ):
        ui->actionNitrox->setChecked( false );
        ui->actionFull_TMX->setChecked( false );
        ui->actionMilitary->setChecked( false );
        break;
      case static_cast< int >( LicenseType::LIC_FULLTMX ):
        ui->actionNitrox->setChecked( false );
        ui->actionNormoxic_TMX->setChecked( false );
        ui->actionMilitary->setChecked( false );
        break;
      case static_cast< int >( LicenseType::LIC_MIL ):
        ui->actionNitrox->setChecked( false );
        ui->actionNormoxic_TMX->setChecked( false );
        ui->actionFull_TMX->setChecked( false );
        break;
    }
#endif
  }

  /**
   * @brief SPX42ControlMainWin::onOnlineStatusChangedSlot
   */
  void SPX42ControlMainWin::onOnlineStatusChangedSlot( bool )
  {
    *lg << LDEBUG << "SPX42ControlMainWin::onOnlineStatusChangedSlot" << Qt::endl;
    setOnlineStatusMessage();
    //
    // was soll immer passieren
    //
    switch ( remoteSPX42->getConnectionStatus() )
    {
      case SPX42RemotBtDevice::SPX42_DISCONNECTED:
      case SPX42RemotBtDevice::SPX42_DISCONNECTING:
      case SPX42RemotBtDevice::SPX42_ERROR:
        //
        // config leeren
        //
        spx42Config->resetConfig();
        onAkkuValueChangedSlot();
        break;
      case SPX42RemotBtDevice::SPX42_CONNECTING:
        break;
      case SPX42RemotBtDevice::SPX42_CONNECTED:
        //
        // so, wenn der SPX online ist immer als erstes ein paar Kommandos
        // zum SPX42 schicken um ihn zu identifizieren
        //
        //
        // Frage nach dem Hersteller
        // gleich danach Frage nach der Seriennummer
        // und dann noch Frage nach der Firmwareversion
        // und lizenz nicht vergessen
        // und alive
        //
        SendListEntry sendCommand = remoteSPX42->askWhileStartup();
        *lg << LDEBUG << "SPX42ControlMainWin::onOnlineStatusChangedSlot -> ask params while startup..." << Qt::endl;
        remoteSPX42->sendCommand( sendCommand );
        *lg << LDEBUG << "SPX42ControlMainWin::onOnlineStatusChangedSlot -> ask params while startup...OK" << Qt::endl;
        break;
    }
  }

  /**
   * @brief SPX42ControlMainWin::onAkkuValueChangedSlot
   * @param akkuValue
   */
  void SPX42ControlMainWin::onAkkuValueChangedSlot( double akkuValue )
  {
    //
    // versuche den Akkustand darzustellen
    //
    if ( akkuValue == 0.0 )
    {
      //
      // kein Wert
      //
      akkuLabel->setText( "---" );
    }
    else
    {
      //
      // konvertiere in String
      // TODO: Abhängig von der Spannung färben
      //
      QString labelText = tr( "AKKU" ).append( QString::asprintf( " %2.2fV", akkuValue ) );
      akkuLabel->setText( labelText );
    }
  }

  /**
   * @brief SPX42ControlMainWin::onWarningMessageSlot
   * @param msg
   * @param asPopup
   */
  void SPX42ControlMainWin::onWarningMessageSlot( const QString &msg, bool asPopup )
  {
    if ( !asPopup )
    {
      // TODO: Messagebox !
      *lg << LWARN << "SPX42ControlMainWin::onWarningMessageSlot -> as POPUP: <" << msg << ">" << Qt::endl;
    }
    else
    {
      // TODO: statusline
      *lg << LWARN << "SPX42ControlMainWin::onWarningMessageSlot -> as msgline: <" << msg << ">" << Qt::endl;
    }
  }

  /**
   * @brief SPX42ControlMainWin::onErrorgMessageSlot
   * @param msg
   * @param asPopup
   */
  void SPX42ControlMainWin::onErrorgMessageSlot( const QString &msg, bool asPopup )
  {
    if ( !asPopup )
    {
      // TODO: Messagebox !
      *lg << LWARN << "SPX42ControlMainWin::onErrorMessageSlot -> as POPUP: <" << msg << ">" << Qt::endl;
    }
    else
    {
      // TODO: statusline
      *lg << LWARN << "SPX42ControlMainWin::onErrorMessageSlot -> as msgline: <" << msg << ">" << Qt::endl;
    }
  }

  /**
   * @brief SPX42ControlMainWin::onConfigWriteBackSlot
   */
  void SPX42ControlMainWin::onConfigWriteBackSlot()
  {
    SendListEntry sendCommand( SPX42ControlMainWin::marker, SPX42ControlMainWin::ar );
    //
    // erledigt, Timer stoppen
    //
    configWriteTimer.stop();
    //
    // finde heraus, was sich verändert hat
    //
    quint8 changed = spx42Config->getChangedConfig();
    if ( changed != 0 )
    {
      *lg << LDEBUG
          << QString( "SPX42ControlMainWin::onConfigWriteBackSlot -> write back config, changed value: <0x%1> (bitwhise)" )
                 .arg( static_cast< int >( changed & 0xff ), 2, 16, QChar( '0' ) )
          << Qt::endl;
      if ( changed & SPX42ConfigClass::CF_CLASS_DECO )
      {
        //
        // sende neue DECO Einstellungen
        //
        sendCommand = remoteSPX42->sendDecoParams( *spx42Config );
        *lg << LDEBUG << "SPX42ControlMainWin::onConfigWriteBackSlot -> deco write <" << sendCommand.second
            << "> old order: " << ( spx42Config->getIsOldParamSorting() ? "true" : "false" ) << Qt::endl;
        remoteSPX42->sendCommand( sendCommand );
      }
      if ( changed & SPX42ConfigClass::CF_CLASS_DISPLAY )
      {
        //
        // sende neue Display einstellungen
        //
        sendCommand = remoteSPX42->sendDisplayParams( *spx42Config );
        *lg << LDEBUG << "SPX42ControlMainWin::onConfigWriteBackSlot -> display write <" << sendCommand.second << ">" << Qt::endl;
        remoteSPX42->sendCommand( sendCommand );
      }
      if ( changed & SPX42ConfigClass::CF_CLASS_SETPOINT )
      {
        //
        // setpoint Einstellungen senden
        //
        sendCommand = remoteSPX42->sendSetpointParams( *spx42Config );
        *lg << LDEBUG << "SPX42ControlMainWin::onConfigWriteBackSlot -> setpoint write <" << sendCommand.second
            << "> old order: " << ( spx42Config->getIsOldParamSorting() ? "true" : "false" ) << Qt::endl;
        remoteSPX42->sendCommand( sendCommand );
      }
      if ( changed & SPX42ConfigClass::CF_CLASS_UNITS )
      {
        //
        // einheiten senden
        //
        sendCommand = remoteSPX42->sendUnitsParams( *spx42Config );
        *lg << LDEBUG << "SPX42ControlMainWin::onConfigWriteBackSlot -> units write <" << sendCommand.second << ">" << Qt::endl;
        remoteSPX42->sendCommand( sendCommand );
      }
      if ( changed & SPX42ConfigClass::CF_CLASS_INDIVIDUAL )
      {
        //
        // individual einstellungen senden
        //
        sendCommand = remoteSPX42->sendCustomParams( *spx42Config );
        *lg << LDEBUG << "SPX42ControlMainWin::onConfigWriteBackSlot -> custom write <" << sendCommand.second << ">" << Qt::endl;
        remoteSPX42->sendCommand( sendCommand );
      }
      if ( changed & SPX42ConfigClass::CF_CLASS_GASES )
      {
        //
        // gase senden (nur die, welche verändert sind)
        // TODO: in schleife programmieren
        //
        quint8 changedGases = spx42Config->getChangedGases();
        if ( changedGases & SPX42ConfigClass::CF_GAS01 )
        {
          sendCommand = remoteSPX42->sendGas( 0, *spx42Config );
          *lg << LDEBUG << "SPX42ControlMainWin::onConfigWriteBackSlot -> gas 01 write <" << sendCommand.second << ">" << Qt::endl;
          remoteSPX42->sendCommand( sendCommand );
        }
        if ( changedGases & SPX42ConfigClass::CF_GAS02 )
        {
          sendCommand = remoteSPX42->sendGas( 1, *spx42Config );
          *lg << LDEBUG << "SPX42ControlMainWin::onConfigWriteBackSlot -> gas 02 write <" << sendCommand.second << ">" << Qt::endl;
          remoteSPX42->sendCommand( sendCommand );
        }
        if ( changedGases & SPX42ConfigClass::CF_GAS03 )
        {
          sendCommand = remoteSPX42->sendGas( 2, *spx42Config );
          *lg << LDEBUG << "SPX42ControlMainWin::onConfigWriteBackSlot -> gas 03 write <" << sendCommand.second << ">" << Qt::endl;
          remoteSPX42->sendCommand( sendCommand );
        }
        if ( changedGases & SPX42ConfigClass::CF_GAS04 )
        {
          sendCommand = remoteSPX42->sendGas( 3, *spx42Config );
          *lg << LDEBUG << "SPX42ControlMainWin::onConfigWriteBackSlot -> gas 04 write <" << sendCommand.second << ">" << Qt::endl;
          remoteSPX42->sendCommand( sendCommand );
        }
        if ( changedGases & SPX42ConfigClass::CF_GAS05 )
        {
          sendCommand = remoteSPX42->sendGas( 4, *spx42Config );
          *lg << LDEBUG << "SPX42ControlMainWin::onConfigWriteBackSlot -> gas 05 write <" << sendCommand.second << ">" << Qt::endl;
          remoteSPX42->sendCommand( sendCommand );
        }
        if ( changedGases & SPX42ConfigClass::CF_GAS06 )
        {
          sendCommand = remoteSPX42->sendGas( 5, *spx42Config );
          *lg << LDEBUG << "SPX42ControlMainWin::onConfigWriteBackSlot -> gas 06 write <" << sendCommand.second << ">" << Qt::endl;
          remoteSPX42->sendCommand( sendCommand );
        }
        if ( changedGases & SPX42ConfigClass::CF_GAS07 )
        {
          sendCommand = remoteSPX42->sendGas( 6, *spx42Config );
          *lg << LDEBUG << "SPX42ControlMainWin::onConfigWriteBackSlot -> gas 07 write <" << sendCommand.second << ">" << Qt::endl;
          remoteSPX42->sendCommand( sendCommand );
        }
        if ( changedGases & SPX42ConfigClass::CF_GAS08 )
        {
          sendCommand = remoteSPX42->sendGas( 7, *spx42Config );
          *lg << LDEBUG << "SPX42ControlMainWin::onConfigWriteBackSlot -> gas 08 write <" << sendCommand.second << ">" << Qt::endl;
          remoteSPX42->sendCommand( sendCommand );
        }
      }
    }
    else
    {
      *lg << LDEBUG << "SPX42ControlMainWin::onConfigWriteBackSlot -> no changes, go away..." << Qt::endl;
    }
    //
    // danach CONFIG als syncron setzen
    //
    spx42Config->freezeConfigs();
    //
    // Label rücksetzten
    // TODO: nach der Quittung erst
    //
    QFont font = onlineLabel->font();
    font.setBold( false );
    waitForWriteLabel->setFont( font );
    waitForWriteLabel->setPalette( onlinePalette );
    waitForWriteLabel->setText( tr( "CLEAR" ) );
    ui->actionSPX_STATE->setIcon( QIcon( ":/icons/ic_spx_online" ) );
    ui->actionSPX_STATE->setStatusTip( tr( "spx42 is online, click for disconnect..." ) );
    emit onSendBufferStateChangedSig( false );
  }

  /**
   * @brief SPX42ControlMainWin::onConfigWasChanged
   */
  void SPX42ControlMainWin::onConfigWasChangedSlot()
  {
    //
    // Timer starten, bei jeden Aufruf neu
    // so dass nicht sofort geschrieben wird.
    //
    configWriteTimer.start( ProjectConst::CONFIG_WRITE_DELAY );
    *lg << LDEBUG << "SPX42ControlMainWin::onConfigWasChangedSlot -> start wait timer again..." << Qt::endl;
    //
    // LABEL für Schreibpuffer...
    //
    QFont font = onlineLabel->font();
    font.setBold( true );
    waitForWriteLabel->setFont( font );
    waitForWriteLabel->setPalette( busyPalette );
    waitForWriteLabel->setText( tr( "BUSY" ) );
    ui->actionSPX_STATE->setIcon( QIcon( ":/icons/ic_spx_buffering" ) );
    ui->actionSPX_STATE->setStatusTip( tr( "spx42 is online and wait for write config data..." ) );
    emit onSendBufferStateChangedSig( true );
  }

  /**
   * @brief SPX42ControlMainWin::onGetHelpForUser
   */
  void SPX42ControlMainWin::onGetHelpForUser()
  {
    *lg << LDEBUG << "SPX42ControlMainWin::onGetHelpForUser..." << Qt::endl;
    HelpDialog helpDial( currentTab, this, lg );
    helpDial.exec();
  }

  /**
   * @brief SPX42ControlMainWin::onOptionsActionSlot
   */
  void SPX42ControlMainWin::onOptionsActionSlot()
  {
    *lg << LDEBUG << "SPX42ControlMainWin::onOptionsActionSlot..." << Qt::endl;
    if ( !optionsDlg )
    {
      optionsDlg = std::unique_ptr< OptionsDialog >( new OptionsDialog( this, cf, lg ) );
    }
    optionsDlg->init();
    if ( QDialog::Accepted == optionsDlg->exec() )
    {
      *lg << LINFO << "SPX42ControlMainWin::onOptionsActionSlot -> dialog accepted" << Qt::endl;
      QMessageBox::information( this, tr( "CAUTION" ), tr( "TO APPLY CHANGES RESTART APPLICATION" ), QMessageBox::Ok,
                                QMessageBox::Ok );
    }
  }
}  // namespace spx
