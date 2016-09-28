#ifndef LOGGERCLASS_HPP
#define LOGGERCLASS_HPP

#include <QString>
#include <QSettings>
#include <QVector>

#include "currBuildDef.hpp"

namespace spx42
{
  class AppConfigClass
  {
    private:
      static const QString constBuildDate;                      //! Builddatum
      static const QString constBuildNumStr;                    //! Buildnummer
      static const QString constLogGroupName;                   //! Gruppenname Logeinstellungen
      static const QString constLogFileKey;                     //! Einstellung für Logdatei
      static const QString constNoData;                         //! Kennzeichner für keine Daten
      static const QString constAppGroupName;                   //! Gruppenname für App Einstellungen
      static const QString constAppTimeoutKey;                  //! Einstellung für Watchdog
      static const int defaultWatchdogTimerVal;                 //! Default für Watchdog
      static const QString constAppThresholdKey;                //! Einstellung für Logebene
      static const qint8 defaultAppThreshold;                   //! defaultwert für Loggingebene
      // ab hier die Konfiguration lagern
      QString configFile;                                       //! wie nennt sich die Konfigurationsdatei
      QString logfileName;                                      //! Wie heisst das Logfile
      int watchdogTimer;                                        //! welchen Wert hat der Timer
      qint8 logThreshold;                                       //! welche Loggerstufe hat die App

    public:
      AppConfigClass( void );                                   //! Konstruktor
      virtual ~AppConfigClass();                                //! Destruktor
      bool loadSettings(void );                                 //! lade Einstellungen aus default Konfigdatei
      bool loadSettings( QString& configFile );                 //! lade Einstellungen aus benannter Konfigdatei
      bool saveSettings(void );                                 //! sichere Einstellungen
      QString getConfigFile(void) const;                        //! Name der Konfigdatei ausgeben
      QString getLogfileName(void) const;                       //! Name der Logdatei ausgeben
      void setLogfileName(const QString& value);                //! Name der Logdatei setzten
      int getWatchdogTime(void);                                //! Wert des Watchdog holen
      void setLogThreshold( qint8 th );                         //! setzte Loggingstufe in Config
      qint8 getLogTreshold(void);                               //! hole Loggingstufe aus config
      static QString getBuildDate();                            //! hole das Builddatum als String
      static QString getBuildNumStr();                          //! hole die Buildnummer als String

    private:
      // Logeinstellungen
      bool loadLogSettings(QSettings& settings);
      void makeDefaultLogSettings( QSettings& settings );
      bool saveLogSettings(QSettings& settings);
      // allg. Programmeinstellungen
      bool loadAppSettings( QSettings& settings);
      void makeAppDefaultSettings( QSettings& settings );
      bool saveAppSettings(QSettings& settings);
  };
}
#endif // LOGGERCLASS_HPP
