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
    //
    // Actions mit den richtigen Slots verbinden
    //
    connectActions();
    //
    // setze Action Stati
    //
    setActionStati();
    //
    // TAB initialisieren
    //
    myTab = ui->areaTabWidget;
    createApplicationTabs();
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
    QWidget *wg;

    myTab->clear();
    //
    // der Connect Tab Platzhalter
    //
    wg = new QWidget();
    wg->setObjectName(tr("Connect Tab"));
    myTab->addTab(wg, QString(tr("Connect")));
    //
    // der Gaslisten-Platzhalter
    //
    wg = new QWidget();
    wg->setObjectName(tr("Gas Tab"));
    myTab->addTab(wg, QString(tr("Gases")));
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
    QWidget *wg;
    //
    // Alle Childs entfernen
    //
    lg->debug("SPX42ControlMainWin::tabCurrentChanged -> delete all widget children");
    for( int i = 0; i < myTab->count(); i++ )
    {
      QWidget *parentWidget = myTab->widget( i );
      UpdatesEnabledHelper helper(parentWidget);
      qDeleteAll(parentWidget->findChildren<QWidget*>("", Qt::FindDirectChildrenOnly));
      delete parentWidget->layout();
    }
    //
    // Neuen Inhalt des Tabs aufbauen
    //
    lg->debug(QString("SPX42ControlMainWin::tabCurrentChanged -> new index <%1>").arg(idx, 2, 10, QChar('0')));
    switch( idx )
    {
      //default:
      case static_cast<int>(ApplicationTab::CONNECT_TAB):
        lg->debug("SPX42ControlMainWin::setApplicationTab -> CONNECT TAB...");
        wg = new ConnectForm( myTab->widget(idx), lg);
        wg->setObjectName(tr("Connect Tab"));
        currentTab = ApplicationTab::CONNECT_TAB;
        break;

      case static_cast<int>(ApplicationTab::GAS_TAB):
        lg->debug("SPX42ControlMainWin::setApplicationTab -> GAS TAB...");
        /*
        wg = new GasForm(myTab, lg);
        wg->setObjectName(tr("Gas Tab"));
        myTab->addTab(wg, QString(tr("Gas")));
        */
        currentTab = ApplicationTab::GAS_TAB;
        break;
    }

  }
}
