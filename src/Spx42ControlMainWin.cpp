#include "Spx42ControlMainWin.hpp"
#include "ui_Spx42ControlMainWin.h"

namespace spx
{
  /**
   * @brief Der Konstruktor des Hauptfensters
   * @param parent Elternteil...
   */
  SPX42ControlMainWin::SPX42ControlMainWin( QWidget *parent )
      : QMainWindow( parent )
      , ui( new Ui::SPX42ControlMainWin() )
      , watchdog( new QTimer( this ) )
      , spx42Config( new SPX42Config() )
      , currentStatus( ApplicationStat::STAT_OFFLINE )
      , watchdogTimer( 0 )
      , currentTab()
  {
    //
    // Hilfebutton ausblenden
    //
    this->setWindowFlags( this->windowFlags() & ~Qt::WindowContextHelpButtonHint );
    //
    // lade die Einstellungen des Programmes
    //
    cf.loadSettings();
    //
    // Programmlogger initialisieren
    //
    if ( !createLogger() )
    {
      exit( -1 );
    }
    lg->debug( QString( "SPX42ControlMainWin::SPX42ControlMainWin:" ).append( "Program start..." ) );
    lg->debug( QString( "SPX42ControlMainWin::SPX42ControlMainWin:" ).append( "Program build date:   " ).append( cf.getBuildDate() ) );
    lg->debug(
        QString( "SPX42ControlMainWin::SPX42ControlMainWin:" ).append( "Program build number: " ).append( cf.getBuildNumStr() ) );
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
    if ( id1 < 0 || id2 < 0 )
    {
      QMessageBox::critical( this, tr( "CRITICAL" ), tr( "internal font can't load!" ) );
    }
    else
    {
#ifdef UNIX
      setFont( QFont( "DejaVu Sans Mono", 11 ) );
#else
      setFont( QFont( "DejaVu Sans Mono", 12 ) );
#endif
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
    setTitleMessage();
  }

  /**
   * @brief Der Destruktor, Aufräumarbeiten
   */
  SPX42ControlMainWin::~SPX42ControlMainWin()
  {
    lg->debug( "SPX42ControlMainWin::~SPX42ControlMainWin..." );
    try
    {
      if ( watchdog->isActive() )
      {
        watchdog->stop();
      }
    }
    catch ( std::exception &ex )
    {
      lg->crit( QString( "SPX42ControlMainWin::~SPX42ControlMainWin -> watchdog stopping failed (%1)" ).arg( ex.what() ) );
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
    QMessageBox msgBox;
    msgBox.setText( tr( "Really EXIT?" ) );
    msgBox.setDetailedText( tr( "Will you really quit this programm and close all files, databases, connections?" ) );
    msgBox.setIcon( QMessageBox::Question );
    msgBox.setStandardButtons( QMessageBox::No | QMessageBox::Yes );
    lg->debug( "SPX42ControlMainWin::closeEvent -> show dialogbox..." );
    if ( QMessageBox::Yes != msgBox.exec() )
    {
      event->ignore();
      lg->debug( "SPX42ControlMainWin::closeEvent -> ignore close application..." );
      return;
    }
    lg->debug( "SPX42ControlMainWin::closeEvent -> close application..." );
    event->accept();
    disconnectActions();
    QMainWindow::closeEvent( event );
  }

  /**
   * @brief Erzeuge den Programm Logger
   * @return erfolgreich?
   */
  bool SPX42ControlMainWin::createLogger()
  {
    // erzeuge einen Logger mit
    lg = std::shared_ptr< Logger >( new Logger() );
    if ( lg )
    {
      lg->startLogging( static_cast< LgThreshold >( cf.getLogTreshold() ), cf.getLogfileName() );
      return ( true );
    }

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
    tabTitle << tr( "Connection" );    // CONNECT_TAB
    tabTitle << tr( "SPX42 Config" );  // CONFIG_TAB
    tabTitle << tr( "Gas Lists" );     // GAS_TAB
    tabTitle << tr( "Diving Log" );    // Logger-Tab
    tabTitle << tr( "Log Charts" );    // Loggiing CHARTS
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
        ui->actionActionPrint->setEnabled( false );
        ui->actionConnectBluetooth->setEnabled( true );
        ui->actionOpenDB->setEnabled( true );
        ui->actionSettings->setEnabled( true );
        break;
      case ApplicationStat::STAT_ONLINE:
        ui->actionAbout->setEnabled( true );
        ui->actionActionPrint->setEnabled( false );
        ui->actionConnectBluetooth->setEnabled( false );
        ui->actionOpenDB->setEnabled( true );
        ui->actionSettings->setEnabled( true );
        break;
      case ApplicationStat::STAT_ERROR:
        ui->actionAbout->setEnabled( true );
        ui->actionActionPrint->setEnabled( false );
        ui->actionConnectBluetooth->setEnabled( true );
        ui->actionOpenDB->setEnabled( false );
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
      // About angeklickt
      //
      connect( ui->actionAbout, &QAction::triggered, [=]( bool checked ) {
        lg->debug( QString( "SPX42ControlMainWin::aboutActionSlot <%1>" ).arg( checked ) );
        AboutDialog dlg( this, cf, lg );
        dlg.exec();
      } );
      //
      // CLOSE
      //
      connect( ui->actionQUIT, &QAction::triggered, [=]( bool checked ) {
        lg->debug( QString( "SPX42ControlMainWin::quitActionSlot <%1>" ).arg( checked ) );
        close();
      } );
      //
      // TAB wurde gewechselt
      //
      connect( ui->areaTabWidget, &QTabWidget::currentChanged, this, &SPX42ControlMainWin::onTabCurrentChangedSlot );
      //
      // Lizenz wurde geändert
      //
      connect( spx42Config.get(), &SPX42Config::licenseChangedSig, this, &SPX42ControlMainWin::onLicenseChangedSlot );
      //
      // onölinestatus wurde geändert
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
        lg->debug( "SPX42ControlMainWin::simulateIndividualLicenseChanged ..." );
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
      lg->crit( QString( "exception while connecting signals to slots (" ).append( ex.what() ).append( ")" ) );
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
      lg->crit( QString( "exception while disconnecting signals from slots (" ).append( ex.what() ).append( ")" ) );
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
    wg = new QWidget();
    wg->setObjectName( "DUMMY" );
    ui->areaTabWidget->addTab( wg, tabTitle.at( 0 ) );
    //
    // der CONFIG-Platzhalter
    //
    wg = new QWidget();
    wg->setObjectName( "DUMMY" );
    ui->areaTabWidget->addTab( wg, tabTitle.at( 1 ) );
    //
    // der Gaslisten-Platzhalter
    //
    wg = new QWidget();
    wg->setObjectName( "DUMMY" );
    ui->areaTabWidget->addTab( wg, tabTitle.at( 2 ) );
    //
    // der Logging-Platzhalter
    //
    wg = new QWidget();
    wg->setObjectName( "DUMMY" );
    ui->areaTabWidget->addTab( wg, tabTitle.at( 3 ) );
    //
    // der Chart Platzhalter
    //
    wg = new QWidget();
    wg->setObjectName( "DUMMY" );
    ui->areaTabWidget->addTab( wg, tabTitle.at( 4 ) );
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
        lg->debug( QString( "SPX42ControlMainWin::clearApplicationTabs -> <%1> should delete" ).arg( currObj->objectName() ) );
#ifdef DEBUG
        QString title = ui->areaTabWidget->tabText( i ).append( "-D" );
#endif
        IFragmentInterface *oldFragment = reinterpret_cast< IFragmentInterface * >( currObj );
        if ( nullptr != oldFragment )
        {
          lg->debug( "SPX42ControlMainWin::clearApplicationTabs -> deactivate..." );
          // wenn das ein Fragment ist deaktiviere signale
          oldFragment->deactivateTab();
          // und als QWidget die löschung verfügen, sobald das möglich ist
          lg->debug( "SPX42ControlMainWin::clearApplicationTabs -> deactivate...OK" );
        }
        else
        {
          lg->debug( "SPX42ControlMainWin::clearApplicationTabs -> not deactivated..." );
        }
        ui->areaTabWidget->removeTab( i );
        delete currObj;
        currObj = new QWidget();
        currObj->setObjectName( "DUMMY" );
        ui->areaTabWidget->insertTab( i, currObj, title );
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

  void SPX42ControlMainWin::setTitleMessage( const QString &msg )
  {
    QString online;
    // ist die Kiste verbunden?
    if ( remoteSPX42->getConnectionStatus() == SPX42RemotBtDevice::SPX42_CONNECTED )
    {
      online = tr( "online" );
    }
    else
    {
      online = tr( "offline" );
    }
    // und, ist eine Meldung vorhanden?
    if ( !msg.isEmpty() )
    {
      setWindowTitle( QString( "%1 - %2 - " ).arg( ProjectConst::MAIN_TITLE ).arg( online ).arg( msg ) );
    }
    else
    {
      setWindowTitle( QString( "%1 - %2 -" ).arg( ProjectConst::MAIN_TITLE ).arg( online ) );
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
    lg->debug( QString( "SPX42ControlMainWin::tabCurrentChanged -> new index <%1>" ).arg( idx, 2, 10, QChar( '0' ) ) );
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
        lg->debug( "SPX42ControlMainWin::setApplicationTab -> CONNECT TAB..." );
        currObj = new ConnectFragment( Q_NULLPTR, lg, spx42Database, spx42Config, remoteSPX42 );
        currObj->setObjectName( "spx42Connect" );
        ui->areaTabWidget->insertTab( idx, currObj, tabTitle.at( static_cast< int >( ApplicationTab::CONNECT_TAB ) ) );
        currentTab = ApplicationTab::CONNECT_TAB;
        connect( static_cast< ConnectFragment * >( currObj ), &ConnectFragment::onWarningMessageSig, this,
                 &SPX42ControlMainWin::onWarningMessageSlot );
        break;

      case static_cast< int >( ApplicationTab::CONFIG_TAB ):
        lg->debug( "SPX42ControlMainWin::setApplicationTab -> CONFIG TAB..." );
        currObj = new DeviceConfigFragment( Q_NULLPTR, lg, spx42Database, spx42Config, remoteSPX42 );
        currObj->setObjectName( "spx42Config" );
        ui->areaTabWidget->insertTab( idx, currObj, tabTitle.at( static_cast< int >( ApplicationTab::CONFIG_TAB ) ) );
        currentTab = ApplicationTab::CONFIG_TAB;
        break;

      case static_cast< int >( ApplicationTab::GAS_TAB ):
        lg->debug( "SPX42ControlMainWin::setApplicationTab -> GAS TAB..." );
        currObj = new GasFragment( Q_NULLPTR, lg, spx42Database, spx42Config, remoteSPX42 );
        currObj->setObjectName( "spx42Gas" );
        ui->areaTabWidget->insertTab( idx, currObj, tabTitle.at( static_cast< int >( ApplicationTab::GAS_TAB ) ) );
        currentTab = ApplicationTab::GAS_TAB;
        break;

      case static_cast< int >( ApplicationTab::LOG_TAB ):
        lg->debug( "SPX42ControlMainWin::setApplicationTab -> LOG TAB..." );
        currObj = new LogFragment( Q_NULLPTR, lg, spx42Database, spx42Config, remoteSPX42 );
        currObj->setObjectName( "spx42log" );
        ui->areaTabWidget->insertTab( idx, currObj, tabTitle.at( static_cast< int >( ApplicationTab::LOG_TAB ) ) );
        currentTab = ApplicationTab::LOG_TAB;
        break;

      case static_cast< int >( ApplicationTab::CHART_TAB ):
        lg->debug( "SPX42ControlMainWin::setApplicationTab -> CHART TAB..." );
        currObj = new ChartsFragment( Q_NULLPTR, lg, spx42Database, spx42Config, remoteSPX42 );
        currObj->setObjectName( "spx42charts" );
        ui->areaTabWidget->insertTab( idx, currObj, tabTitle.at( static_cast< int >( ApplicationTab::CHART_TAB ) ) );
        currentTab = ApplicationTab::CHART_TAB;
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
    lg->debug( QString( "SPX42ControlMainWin::onLicenseChangedSlot -> set: %1" ).arg( spx42Config->getLicName() ) );
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
    lg->debug( "SPX42ControlMainWin::onOnlineStatusChangedSlot" );
    setTitleMessage();
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
      lg->warn( QString( "SPX42ControlMainWin::onWarningMessageSlot -> as POPUP: <%1>" ).arg( msg ) );
    }
    else
    {
      // TODO: statusline
      lg->warn( QString( "SPX42ControlMainWin::onWarningMessageSlot -> as msgline: <%1>" ).arg( msg ) );
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
      lg->warn( QString( "SPX42ControlMainWin::onErrorMessageSlot -> as POPUP: <%1>" ).arg( msg ) );
    }
    else
    {
      // TODO: statusline
      lg->warn( QString( "SPX42ControlMainWin::onErrorMessageSlot -> as msgline: <%1>" ).arg( msg ) );
    }
  }
}
