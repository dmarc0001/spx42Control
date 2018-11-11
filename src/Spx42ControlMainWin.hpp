#ifndef SPX42CONTROLMAINWIN_HPP
#define SPX42CONTROLMAINWIN_HPP

#include <memory>

#include <QCloseEvent>
#include <QDebug>
#include <QFont>
#include <QFontDatabase>
#include <QMainWindow>
#include <QMessageBox>
#include <QStringList>
#include <QTabWidget>
#include <QTimer>

#include "bluetooth/SPX42RemotBtDevice.hpp"
#include "config/AppConfigClass.hpp"
#include "config/ProjectConst.hpp"
#include "database/SPX42Database.hpp"
#include "guiFragments/ChartsFragment.hpp"
#include "guiFragments/ConnectFragment.hpp"
#include "guiFragments/DeviceConfigFragment.hpp"
#include "guiFragments/GasFragment.hpp"
#include "guiFragments/LogFragment.hpp"
#include "logging/Logger.hpp"
#include "utils/AboutDialog.hpp"
#include "utils/SPX42Config.hpp"

namespace Ui
{
  //
  // Vorwärtsdeklaration, include in der CPP
  //
  class SPX42ControlMainWin;
}

namespace spx
{
  class SPX42ControlMainWin : public QMainWindow
  {
    private:
    Q_OBJECT
    std::unique_ptr< Ui::SPX42ControlMainWin > ui;
    std::shared_ptr< Logger > lg;                       //! Loggerobjekt für Logs
    const std::unique_ptr< QTimer > watchdog;           //! Wachhund für Timeouts
    const std::shared_ptr< SPX42Config > spx42Config;   //! Konfiguration des verbundenen SPX42
    std::shared_ptr< SPX42RemotBtDevice > remoteSPX42;  //! Objekt des entfernten SPX42 (verbunden oder nicht verbunden)
    ApplicationStat currentStatus;                      //! welchen Status hat die App?
    std::shared_ptr< SPX42Database > spx42Database;     //! Datenbankobjekt zur Speicherung der SPX Daten/Einstellungen
    int watchdogTimer;                                  //! Zeitspanne zum Timeout
    AppConfigClass cf;                                  //! Konfiguration aus Datei
    ApplicationTab currentTab;                          //! welcher Tab ist aktiv?
    QStringList tabTitle;                               //! Tab Titel (nicht statisch, das Objekt gibts eh nur einmal)

    public:
    explicit SPX42ControlMainWin( QWidget *parent = nullptr );
    ~SPX42ControlMainWin();
    void closeEvent( QCloseEvent *event );  //! Das Beenden-Ereignis

    private:
    bool createLogger();                               //! Erzeuge den Logger
    void fillTabTitleArray( void );                    //! Fülle das Titelarray lokalisiert
    bool setActionStati( void );                       //! setze Actions entsprchend des Status
    bool connectActions( void );                       //! Verbinde Actions mit Slots
    void createApplicationTabs( void );                //! Erzeuge die (noch leeren) Tabs
    void clearApplicationTabs( void );                 //! Leere die Tabs
    void simulateLicenseChanged( LicenseType lType );  //! Simuliere lizenzwechsel
    ApplicationTab getApplicationTab( void );          //! Welcher Tab war noch aktiv?

    private slots:
    void onTabCurrentChangedSlot( int idx );                                //! TAB Index gewechselt
    void onLicenseChangedSlot( void );                                      //! Lizenztyp getriggert
    void onWarningMessageSlot( const QString &msg, bool asPopup = false );  //! eine Warnmeldung soll das Main darstellen
    void onErrorgMessageSlot( const QString &msg, bool asPopup = false );   //! eine Warnmeldung soll das Main darstellen
  };
}  // namespace spx42

#endif  // SPX42CONTROLMAINWIN_HPP
