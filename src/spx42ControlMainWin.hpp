#ifndef SPX42CONTROLMAINWIN_HPP
#define SPX42CONTROLMAINWIN_HPP

#include <QMainWindow>
#include <QFontDatabase>
#include <QMessageBox>
#include <QFont>
#include <QDebug>
#include <QTimer>
#include <QTabWidget>
#include <QCloseEvent>

#include "config/ProjectConst.hpp"
#include "logging/Logger.hpp"
#include "config/AppConfigClass.hpp"
#include "utils/aboutDialog.hpp"
#include "guiFragments/connectForm.hpp"
#include "guiFragments/gasForm.hpp"

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
  enum class ApplicationTab : int { CONNECT_TAB, GAS_TAB };
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
      QTabWidget *myTab;                                        //! Zeiger auf das TabWidget
      ApplicationTab currentTab;                                //! welcher Tab ist aktiv?

    public:
      explicit SPX42ControlMainWin(QWidget *parent = 0);
      ~SPX42ControlMainWin();
      void closeEvent(QCloseEvent *event);                      //! Das Beenden-Ereignis

    private:
      bool createLogger( AppConfigClass *cf );                  //! Erzeuge den Logger
      bool setActionStati( void );                              //! setze Actions entsprchend des Status
      bool connectActions( void );                              //! Verbinde Actions mit Slots
      void createApplicationTabs( void );                       //! Erzeuge die (noch leeren) Tabs
      ApplicationTab getApplicationTab( void );                 //! Welcher Tab war noch aktiv?

    private slots:
      void aboutActionSlot( bool checked );                     //! ABOUT wurde gefordert
      void quitActionSlot( bool checked );                      //! ENDE wurde gefordert
      void tabCurrentChanged( int idx );                        //! TAB Index gewechselt
  };

  class UpdatesEnabledHelper
  {
      QWidget* m_parentWidget;
  public:
      UpdatesEnabledHelper(QWidget* parentWidget) : m_parentWidget(parentWidget) { parentWidget->setUpdatesEnabled(false); }
      ~UpdatesEnabledHelper() { m_parentWidget->setUpdatesEnabled(true); }
  };
} // namespace spx42

#endif // SPX42CONTROLMAINWIN_HPP
