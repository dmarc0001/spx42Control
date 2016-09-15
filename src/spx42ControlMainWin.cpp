#include "spx42ControlMainWin.hpp"
#include "ui_spx42controlmainwin.h"

namespace spx42
{
  SPX42ControlMainWin::SPX42ControlMainWin(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::SPX42ControlMainWin),
    lg(Q_NULLPTR),
    watchdog( new QTimer(this) ),
    currentStatus( ApplicationStat::STAT_OFFLINE )
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
    if( !createLogger( &cf ) )
    {
      exit( -1 );
    }
    lg->debug( "Program start..." );
    lg->debug( QString( tr("Program build date:   ") ).append(cf.getBuildDate()) );
    lg->debug( QString( tr("Program build number: ") ).append(cf.getBuildNumStr()) );
    //
    // GUI initialisieren
    //
    ui->setupUi(this);
    //
    // Font laden
    //
    int id1 = QFontDatabase::addApplicationFont(":/fonts/Hack-Regular.ttf");
    int id2 = QFontDatabase::addApplicationFont(":/fonts/DejaVuSansMono.ttf");
    if( id1 < 0 || id2 < 0 )
    {
      QMessageBox::critical(this,tr("CRITICAL"), tr("internal font can't load!"));
    }
    else
    {
      #ifdef UNIX
      setFont(QFont("Hack Regular", 11));
      #else
      setFont(QFont("Hack Regular", 12));
      #endif
    }
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
    ui->areaTabWidget->setCurrentIndex(0);
    tabCurrentChanged( 0 );
    connectActions();
  }

  SPX42ControlMainWin::~SPX42ControlMainWin()
  {
    try
    {
      if( watchdog->isActive() )
      {
        watchdog->stop();
      }
    }
    catch( std::exception ex )
    {
      lg->crit("SPX42ControlMainWin::~SPX42ControlMainWin -> watchdog stopping failed");
    }
    lg->shutdown();
    delete ui;
  }

  void SPX42ControlMainWin::closeEvent(QCloseEvent *event)
  {
    QMessageBox msgBox;
    msgBox.setText(tr("Really EXIT?"));
    msgBox.setDetailedText( tr("Will you really quit this programm and close all files, databases, connections?"));
    msgBox.setIcon(QMessageBox::Question);
    msgBox.setStandardButtons(QMessageBox::No|QMessageBox::Yes);
    lg->debug("SPX42ControlMainWin::closeEvent -> show dialogbox...");
    if( QMessageBox::Yes != msgBox.exec() )
    {
      event->ignore();
      lg->debug("SPX42ControlMainWin::closeEvent -> ignore close application...");
      return;
    }
    event->accept();
    lg->debug("SPX42ControlMainWin::closeEvent -> close application...");
    //TODO: Aufräumen, Datenbanke schliessen, Connection beenden
    QMainWindow::closeEvent(event);
  }

  bool SPX42ControlMainWin::createLogger( AppConfigClass *cf )
  {
    // erzeuge einen Logger mit
    lg = new Logger( cf );
    if( lg )
    {
      lg->startLogging(static_cast<LgThreshold>(cf->getLogTreshold()));
      return (true);
    }
    QMessageBox msgBox;
    msgBox.setText(tr("Logging start FAIL!"));
    msgBox.setDetailedText( tr("Check write rights for program directory or reinstall software."));
    msgBox.setIcon(QMessageBox::Critical);
    msgBox.exec();
    return (false);
  }

  void SPX42ControlMainWin::fillTabTitleArray( void )
  {
    tabTitle.clear();
    tabTitle << tr("Connection");   // CONNECT_TAB
    tabTitle << tr("Gas Lists");    // GAS_TAB
  }


  bool SPX42ControlMainWin::setActionStati( void )
  {
    switch( currentStatus )
    {
      case ApplicationStat::STAT_OFFLINE:
        ui->actionAbout->setEnabled(true);
        ui->actionActionPrint->setEnabled(false);
        ui->actionConnectBluetooth->setEnabled(true);
        ui->actionOpenDB->setEnabled(true);
        ui->actionSettings->setEnabled(true);
        break;
      case ApplicationStat::STAT_ONLINE:
        ui->actionAbout->setEnabled(true);
        ui->actionActionPrint->setEnabled(false);
        ui->actionConnectBluetooth->setEnabled(false);
        ui->actionOpenDB->setEnabled(true);
        ui->actionSettings->setEnabled(true);
        break;
      case ApplicationStat::STAT_ERROR:
        ui->actionAbout->setEnabled(true);
        ui->actionActionPrint->setEnabled(false);
        ui->actionConnectBluetooth->setEnabled(true);
        ui->actionOpenDB->setEnabled(false);
        ui->actionSettings->setEnabled(true);
        break;
    }
    return( true );
  }

  bool SPX42ControlMainWin::connectActions( void )
  {
    try
    {
    connect( ui->actionAbout, &QAction::triggered, this, &SPX42ControlMainWin::aboutActionSlot );
    connect( ui->actionQUIT, &QAction::triggered, this, &SPX42ControlMainWin::quitActionSlot );
    connect( ui->areaTabWidget, &QTabWidget::currentChanged, this, &SPX42ControlMainWin::tabCurrentChanged );
    }
    catch(std::exception ex )
    {
      lg->crit( QString("exception while connecting signals to slots (").append(ex.what()).append( ")"));
      return( false );
    }
    return( true );
  }

  void SPX42ControlMainWin::createApplicationTabs( void )
  {
    //
    // lasse die TAB-Inhalte verschwinden
    //
    while (ui->areaTabWidget->count() )
    {
      QWidget *canDel = ui->areaTabWidget->widget(0);
      ui->areaTabWidget->removeTab(0);
      delete canDel;
    }
    //
    // der Connect Tab Platzhalter
    //
    QWidget *wg1 = new QWidget();
    wg1->setObjectName("DUMMY");
    ui->areaTabWidget->addTab(wg1, tabTitle.at(0));
    //
    // der Gaslisten-Platzhalter
    //
    QWidget *wg2 = new QWidget();
    wg2->setObjectName("DUMMY");
    ui->areaTabWidget->addTab(wg2, tabTitle.at(1));
    // FIXME wieder löschen
    clearApplicationTabs();
  }

  void SPX42ControlMainWin::clearApplicationTabs( void )
  {
    for( int i = 0; i < ui->areaTabWidget->count(); i++ )
    {
      QWidget *currObj = ui->areaTabWidget->widget( i );
      if( ! QString(currObj->metaObject()->className()).startsWith( "QWidget" ) )
      {
        lg->debug( QString("SPX42ControlMainWin::clearApplicationTabs -> <%1> should delete").arg(currObj->objectName()) );
        QString title = ui->areaTabWidget->tabText( i ).append("-D");
        ui->areaTabWidget->removeTab(i);
        delete currObj;
        currObj = new QWidget();
        currObj->setObjectName("DUMMY");
        ui->areaTabWidget->insertTab( i, currObj, title );
        //ui->areaTabWidget->insertTab( i, currObj, "deleted" );
      }
    }
  }

  ApplicationTab SPX42ControlMainWin::getApplicationTab( void )
  {
    return( currentTab );
  }

  void SPX42ControlMainWin::aboutActionSlot( bool checked )
  {
    lg->debug(QString("SPX42ControlMainWin::aboutActionSlot <%1>").arg(checked));
    AboutDialog dlg(this, &cf, lg );
    dlg.exec();
  }

  void SPX42ControlMainWin::quitActionSlot( bool checked )
  {
    lg->debug(QString("SPX42ControlMainWin::quitActionSlot <%1>").arg(checked));
    close();
  }

  void SPX42ControlMainWin::tabCurrentChanged( int idx )
  {
    static bool ignore = false;
    QWidget *currObj;

    if( ignore ) return;
    ignore = true;
    //
    // Neuen Inhalt des Tabs aufbauen
    //
    lg->debug(QString("SPX42ControlMainWin::tabCurrentChanged -> new index <%1>").arg(idx, 2, 10, QChar('0')));
    clearApplicationTabs();

    QWidget *oldTab = ui->areaTabWidget->widget(idx);
    ui->areaTabWidget->removeTab(idx);
    delete oldTab;

    switch( idx )
    {
      //default:
      case static_cast<int>(ApplicationTab::CONNECT_TAB):
        lg->debug("SPX42ControlMainWin::setApplicationTab -> CONNECT TAB...");
        currObj = new ConnectFragment( Q_NULLPTR, lg, &spx42Config);
        currObj->setObjectName(tr("Connect Tab"));
        ui->areaTabWidget->insertTab( idx, currObj, tabTitle.at(static_cast<int>(ApplicationTab::CONNECT_TAB)) );
        currentTab = ApplicationTab::CONNECT_TAB;
        break;

      case static_cast<int>(ApplicationTab::GAS_TAB):
        lg->debug("SPX42ControlMainWin::setApplicationTab -> GAS TAB...");
        //gasForm = std::shared_ptr<GasFragment>(new GasFragment( ui->areaTabWidget->widget(idx), lg));
        currObj = new GasFragment( Q_NULLPTR, lg, &spx42Config);
        currObj->setObjectName(tr("Gas Tab"));
        ui->areaTabWidget->insertTab( idx, currObj, tabTitle.at(static_cast<int>(ApplicationTab::GAS_TAB)) );
        currentTab = ApplicationTab::GAS_TAB;
        break;
    }
    ui->areaTabWidget->setCurrentIndex(idx);
    ignore = false;
  }
}
