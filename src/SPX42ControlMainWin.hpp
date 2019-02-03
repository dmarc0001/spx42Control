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
#include "utils/OptionsDialog.hpp"

namespace Ui
{
  //
  // Vorwärtsdeklaration, include in der CPP
  //
  class SPX42ControlMainWin;
}  // namespace Ui

namespace spx
{
  constexpr int MAIN_WATCHDOG_TIMERCOUNT = 1000;
  constexpr int MAIN_ALIVE_TIMEVALUE = MAIN_WATCHDOG_TIMERCOUNT * 10;
  class SPX42ControlMainWin : public QMainWindow
  {
    private:
    Q_OBJECT
    //! zur initialisierung von Einträgen
    static const CmdMarker marker;
    //! zur initialisierung von Einträgen
    static const QByteArray ar;
    //! das GUI Objekt
    std::unique_ptr< Ui::SPX42ControlMainWin > ui;
    //! Loggerobjekt für Logs
    std::shared_ptr< Logger > lg;
    //! Konfiguration des verbundenen SPX42
    const std::shared_ptr< SPX42Config > spx42Config;
    //! Objekt des entfernten SPX42 (verbunden oder nicht verbunden)
    std::shared_ptr< SPX42RemotBtDevice > remoteSPX42;
    //! Datenbankobjekt zur Speicherung der SPX Daten/Einstellungen
    std::shared_ptr< SPX42Database > spx42Database;
    //! Label signalisiert online oder offline
    std::unique_ptr< QLabel > onlineLabel;
    //! Label signalisiert online oder offline
    std::unique_ptr< QLabel > akkuLabel;
    //! Label zeigt an, wenn noch Daten zum Schreiben offen sind
    std::unique_ptr< QLabel > waitForWriteLabel;
    //! Dialog zum verändern/anzeigen der Programmoptionen
    std::unique_ptr< OptionsDialog > optionsDlg;
    //! Tab Titel (nicht statisch, das Objekt gibts eh nur einmal)
    QStringList tabTitle;
    //! Konfiguration aus Datei
    AppConfigClass cf;
    //! Wachhund für Timeouts
    QTimer watchdog;
    //! Nach Änderungen der Konfiguration zurück zum SPX schreiben (etwas verzögert)
    QTimer configWriteTimer;
    //! Hashwerte der Gaseinstellungen (acht Stück)
    QStringList spx42GasHashes;
    //! welchen Status hat die App?
    ApplicationStat currentStatus;
    //! Zeitspanne zum Timeout
    qint16 watchdogCounter;
    //! zählt die Timerzyklen
    qint16 zyclusCounter;
    //! welcher Tab ist aktiv?
    ApplicationTab currentTab;
    //! Pallette für offline Schrift
    QPalette offlinePalette;
    //! Pallette für offline Schrift
    QPalette onlinePalette;
    //! Pallette für "offline Schrift"ist beschäftigt"
    QPalette busyPalette;
    //! Pallette für verbinden
    QPalette connectingPalette;
    //! Pallette für den Fehlerfall
    QPalette errorPalette;

    public:
    explicit SPX42ControlMainWin( QWidget *parent = nullptr );
    ~SPX42ControlMainWin();
    //! Das Beenden-Ereignis
    void closeEvent( QCloseEvent *event );
    //! gib dem shared zeiger auf den logger zurück
    std::shared_ptr< Logger > getLogger( void );

    private:
    //! Erzeuge den Logger
    bool createLogger();
    //! Fülle das Titelarray lokalisiert
    void fillTabTitleArray( void );
    //! setze Actions entsprchend des Status
    bool setActionStati( void );
    //! Verbinde Actions mit Slots
    bool connectActions( void );
    //! alle slots freigeben
    bool disconnectActions( void );
    //! Erzeuge die (noch leeren) Tabs
    void createApplicationTabs( void );
    //! Leere die Tabs
    void clearApplicationTabs( void );
    //! Simuliere lizenzwechsel
    void simulateLicenseChanged( LicenseType lType );
    //! Welcher Tab war noch aktiv?
    ApplicationTab getApplicationTab( void );
    //! setze eine Meldung in den Fenstertitel
    void setOnlineStatusMessage( const QString &msg = "" );
    //! Mache einen Online Status
    void makeOnlineStatus( void );

    signals:
    //! statusänderungeen im Puffer oder rledigt
    void onSendBufferStateChangedSig( bool isBusy );

    private slots:
    //! timer für zyklische Sachen, watchdog...
    void onWatchdogTimerSlot( void );
    //! TAB Index gewechselt
    void onTabCurrentChangedSlot( int idx );
    //! Lizenztyp getriggert
    void onLicenseChangedSlot( void );
    //! Akkuwert setzen
    void onAkkuValueChangedSlot( double akkuValue = 0.0 );
    //! Wenn sich der Onlinestatus des SPX42 ändert
    void onOnlineStatusChangedSlot( bool isOnline );
    //! eine Warnmeldung soll das Main darstellen
    void onWarningMessageSlot( const QString &msg, bool asPopup = false );
    //! eine Warnmeldung soll das Main darstellen
    void onErrorgMessageSlot( const QString &msg, bool asPopup = false );
    //! Signal empfangen, dass config geschrieben wurde
    void onConfigWasChangedSlot( void );
    //! timer wenn configs zurück geschrieben werden müssen
    void onConfigWriteBackSlot( void );
    //! Hilfe anzeigen
    void onGetHelpForUser( void );
    //! Wenn der User nach Optionen bettelt
    void onOptionsActionSlot( void );
  };
}  // namespace spx

#endif  // SPX42CONTROLMAINWIN_HPP
