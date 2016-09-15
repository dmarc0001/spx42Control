#ifndef SPX42CONTROLMAINWIN_HPP
#define SPX42CONTROLMAINWIN_HPP

#include <memory>

#include <QMainWindow>
#include <QFontDatabase>
#include <QMessageBox>
#include <QFont>
#include <QDebug>
#include <QTimer>
#include <QTabWidget>
#include <QCloseEvent>
#include <QStringList>

#include "config/ProjectConst.hpp"
#include "logging/Logger.hpp"
#include "config/AppConfigClass.hpp"
#include "utils/aboutDialog.hpp"
#include "guiFragments/connectFragment.hpp"
#include "guiFragments/gasFragment.hpp"
#include "utils/SPX42Config.hpp"

namespace Ui
{
  //
  // Vorwärtsdeklaration, include in der CPP
  //
  class SPX42ControlMainWin;
}

namespace spx42
{
  //! Welcher Tab ist aktiv
  enum class ApplicationTab : int { CONNECT_TAB, GAS_TAB, COUNT_OF_TABS };
  //! Welcher Status ist aktiv
  enum class ApplicationStat : int { STAT_OFFLINE, STAT_ONLINE, STAT_ERROR };

  class SPX42ControlMainWin : public QMainWindow
  {
    private:
      Q_OBJECT
      Ui::SPX42ControlMainWin *ui;
      Logger *lg;                                               //! Loggerobjekt für Logs
      QTimer *watchdog;                                         //! Wachhund für Timeouts
      ApplicationStat currentStatus;                            //! welchen Status hat die App?
      int watchdogTimer;                                        //! Zeitspanne zum Timeout
      AppConfigClass cf;                                        //! Konfiguration aus Datei
      ApplicationTab currentTab;                                //! welcher Tab ist aktiv?
      QStringList tabTitle;                                     //! Tab Titel (nicht statisch, das Objekt gibts eh nur einmal)
      SPX42Config spx42Config;                                  //! Konfiguration des verbundenen SPX42

    public:
      explicit SPX42ControlMainWin(QWidget *parent = 0);
      ~SPX42ControlMainWin();
      void closeEvent(QCloseEvent *event);                      //! Das Beenden-Ereignis

    private:
      bool createLogger( AppConfigClass *cf );                  //! Erzeuge den Logger
      void fillTabTitleArray( void );                           //! Fülle das Titelarray lokalisiert
      bool setActionStati( void );                              //! setze Actions entsprchend des Status
      bool connectActions( void );                              //! Verbinde Actions mit Slots
      void createApplicationTabs( void );                       //! Erzeuge die (noch leeren) Tabs
      void clearApplicationTabs( void );                        //! Leere die eventuell vorhandenen Tab-Objekte
      ApplicationTab getApplicationTab( void );                 //! Welcher Tab war noch aktiv?

    private slots:
      void aboutActionSlot( bool checked );                     //! ABOUT wurde gefordert
      void quitActionSlot( bool checked );                      //! ENDE wurde gefordert
      void tabCurrentChanged( int idx );                        //! TAB Index gewechselt
  };

} // namespace spx42

#endif // SPX42CONTROLMAINWIN_HPP
