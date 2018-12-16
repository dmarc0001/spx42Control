#ifndef SPX42CONTROLMAINWIN_HPP
#define SPX42CONTROLMAINWIN_HPP

#include <QCloseEvent>
#include <QDebug>
#include <QFont>
#include <QFontDatabase>
#include <QIcon>
#include <QMainWindow>
#include <QMessageBox>
#include <QPalette>
#include <QStringList>
#include <QTabWidget>
#include <QTimer>
#include <memory>
#include "bluetooth/SPX42RemotBtDevice.hpp"
#include "config/AppConfigClass.hpp"
#include "config/ProjectConst.hpp"
#include "database/SPX42Database.hpp"
#include "guiFragments/ChartsFragment.hpp"
#include "guiFragments/ConnectFragment.hpp"
#include "guiFragments/DeviceConfigFragment.hpp"
#include "guiFragments/DeviceInfoFragment.hpp"
#include "guiFragments/GasFragment.hpp"
#include "guiFragments/LogFragment.hpp"
#include "logging/Logger.hpp"
#include "spx42/SPX42Config.hpp"
#include "utils/AboutDialog.hpp"
#include "utils/HelpDialog.hpp"

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
    static const CmdMarker marker;                      //! zur initialisierung von Einträgen
    static const QByteArray ar;                         //! zur initialisierung von Einträgen
    std::unique_ptr< Ui::SPX42ControlMainWin > ui;      //! das GUI Objekt
    std::shared_ptr< Logger > lg;                       //! Loggerobjekt für Logs
    const std::shared_ptr< SPX42Config > spx42Config;   //! Konfiguration des verbundenen SPX42
    std::shared_ptr< SPX42RemotBtDevice > remoteSPX42;  //! Objekt des entfernten SPX42 (verbunden oder nicht verbunden)
    std::shared_ptr< SPX42Database > spx42Database;     //! Datenbankobjekt zur Speicherung der SPX Daten/Einstellungen
    std::unique_ptr< QLabel > onlineLabel;              //! Label signalisiert online oder offline
    std::unique_ptr< QLabel > akkuLabel;                //! Label signalisiert online oder offline
    std::unique_ptr< QLabel > waitForWriteLabel;        //! Label zeigt an, wenn noch Daten zum Schreiben offen sind
    QStringList tabTitle;                               //! Tab Titel (nicht statisch, das Objekt gibts eh nur einmal)
    AppConfigClass cf;                                  //! Konfiguration aus Datei
    QTimer watchdog;                                    //! Wachhund für Timeouts
    QTimer configWriteTimer;        //! Nach Änderungen der Konfiguration zurück zum SPX schreiben (etwas verzögert)
    QStringList spx42GasHashes;     //! Hashwerte der Gaseinstellungen (acht Stück)
    ApplicationStat currentStatus;  //! welchen Status hat die App?
    qint16 watchdogCounter;         //! Zeitspanne zum Timeout
    qint16 zyclusCounter;           //! zählt die Timerzyklen
    ApplicationTab currentTab;      //! welcher Tab ist aktiv?
    QPalette offlinePalette;        //! Pallette für offline Schrift
    QPalette onlinePalette;         //! Pallette für offline Schrift
    QPalette busyPalette;           //! Pallette für "offline Schrift"ist beschäftigt"
    QPalette connectingPalette;     //! Pallette für verbinden
    QPalette errorPalette;          //! Pallette für den Fehlerfall

    public:
    explicit SPX42ControlMainWin( QWidget *parent = nullptr );
    ~SPX42ControlMainWin();
    void closeEvent( QCloseEvent *event );  //! Das Beenden-Ereignis

    private:
    bool createLogger();                                     //! Erzeuge den Logger
    void fillTabTitleArray( void );                          //! Fülle das Titelarray lokalisiert
    bool setActionStati( void );                             //! setze Actions entsprchend des Status
    bool connectActions( void );                             //! Verbinde Actions mit Slots
    bool disconnectActions( void );                          //! alle slots freigeben
    void createApplicationTabs( void );                      //! Erzeuge die (noch leeren) Tabs
    void clearApplicationTabs( void );                       //! Leere die Tabs
    void simulateLicenseChanged( LicenseType lType );        //! Simuliere lizenzwechsel
    ApplicationTab getApplicationTab( void );                //! Welcher Tab war noch aktiv?
    void setOnlineStatusMessage( const QString &msg = "" );  //! setze eine Meldung in den Fenstertitel
    void makeOnlineStatus( void );                           //! Mache einen Online Status

    private slots:
    void onWatchdogTimerSlot( void );                                       //! timer für zyklische Sachen, watchdog...
    void onTabCurrentChangedSlot( int idx );                                //! TAB Index gewechselt
    void onLicenseChangedSlot( void );                                      //! Lizenztyp getriggert
    void onAkkuValueChangedSlot( double akkuValue = 0.0 );                  //! Akkuwert setzen
    void onOnlineStatusChangedSlot( bool isOnline );                        //! Wenn sich der Onlinestatus des SPX42 ändert
    void onWarningMessageSlot( const QString &msg, bool asPopup = false );  //! eine Warnmeldung soll das Main darstellen
    void onErrorgMessageSlot( const QString &msg, bool asPopup = false );   //! eine Warnmeldung soll das Main darstellen
    void onConfigWasChangedSlot( void );                                    //! Signal empfangen, dass config geschrieben wurde
    void onConfigWriteBackSlot( void );                                     //! timer wenn configs zurück geschrieben werden müssen
    void onGetHelpForUser( void );                                          //! Hilfe anzeigen
  };
}  // namespace spx42

#endif  // SPX42CONTROLMAINWIN_HPP
