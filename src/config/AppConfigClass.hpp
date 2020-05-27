#ifndef LOGGERCLASS_HPP
#define LOGGERCLASS_HPP

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QSettings>
#include <QStandardPaths>
#include <QString>
#include <QVector>

namespace spx
{
  class AppConfigClass
  {
    private:
    //! Builddatum
    static const QString constBuildDate;
    //! Buildnummer
    static const QString constBuildNumStr;
    //! BuildType DEBUG/RELEASE
    static const QString constBuildTypeStr;
    //! Gruppenname Logeinstellungen
    static const QString constLogGroupName;
    //! Einstellung für Logdatei
    static const QString constLogFileKey;
    //! Einstellung für den Pfad zur Logdatei
    static const QString constLogPathKey;
    //! Einstellung für log auf Konsole
    static const QString constLogToConsoleKey;
    //! Kennzeichner für keine Daten
    static const QString constNoData;
    //! Gruppenname für App Einstellungen
    static const QString constAppGroupName;
    //! Einstellung für Watchdog
    static const QString constAppTimeoutKey;
    //! Einstellungen für Datenbankdatei
    static const QString constAppDatabaseNameKey;
    //! An welchem Pfad fidne ich die Datenbank
    static const QString constAppDatabasePathKey;
    //! Wohin exportiere ich XML/UDDF Dateien
    static const QString constExportPathKey;
    //! Default für Watchdog
    static const int defaultWatchdogTimerVal;
    //! Einstellung für Thema
    static const QString constThemeKey;
    //! Default Thema
    static const QString constThemeDefaultName;
    //! Einstellung für Logebene
    static const QString constAppThresholdKey;
    //! defaultwert für Loggingebene
    static const qint8 defaultAppThreshold;
    //! defaultwert für den Namen der Datenbank
    static const QString defaultDatabaseName;
    //! der Standartpfad für die Database (OS abhängig)
    static const QString defaultDatabasePath;
    //! defaultwert für export (download path, OS abhängig)
    static const QString defaultExportPath;

    public:
    //! mögliche Werte für Themen
    static const QString lightStr;
    static const QString darktStr;
    static const QString customtStr;

    private:
    //
    // ab hier die Konfiguration lagern
    //
    //! wie nennt sich die Konfigurationsdatei
    QString configFile;
    //! Wie heisst das Logfile
    QString logfileName;
    //! Wo ist die Logdatei gespeichert
    QString logfilePath;
    //! Auf der Konsole loggen?
    bool consoleLog;
    //! welchen Wert hat der Timer
    int watchdogTimer;
    //! welche Loggerstufe hat die App
    qint8 logThreshold;
    //! wie ist der name der Datenbank
    QString databaseName;
    //! an welchem Pfad finde ich die Datenbank
    QString databasePath;
    //! auf welchem Pfad wird der Export gespeichert
    QString exportPath;
    //! Wie ist der Name des Tjhemas der GUI
    QString guiThemeName;

    public:
    //! Konstruktor
    AppConfigClass( void );
    //! Destruktor
    virtual ~AppConfigClass();
    //! lade Einstellungen aus default Konfigdatei
    bool loadSettings( void );
    //! lade Einstellungen aus benannter Konfigdatei
    bool loadSettings( QString &configFile );
    //! sichere Einstellungen
    bool saveSettings( void );
    //! Name der Konfigdatei ausgeben
    QString getConfigFile( void ) const;
    //! Name der Logdatei ausgeben
    QString getLogfileName( void ) const;
    // Pfad zur Logdatei zurück geben
    QString getLogfilePath( void ) const;
    //! Name der Logdatei setzten
    void setLogfileName( const QString &value );
    //! Name des Pfades zur Logdatei setzten
    void setLogFilePath( const QString &value );
    //! den vollen Pfas/Namen der logdatei...
    QString getFullLogFilePath( void );
    //! Wert des Watchdog holen
    int getWatchdogTime( void );
    //! setzte Loggingstufe in Config
    void setLogThreshold( qint8 th );
    //! hole Loggingstufe aus config
    qint8 getLogTreshold( void );
    //! hole das Builddatum als String
    static QString getBuildDate( void );
    //! hole die Buildnummer als String
    static QString getBuildNumStr( void );
    //! hole den Buildtyp als String
    static QString getBuldType( void );
    //! datenbankdateiname
    QString getDatabaseName( void ) const;
    //! datenbankdatei Name
    void setDatabaseName( const QString &value );
    //! auf welchem Datenpfad finde ich die Datenbank
    QString getDatabasePath( void ) const;
    //! auf welchem Datenpfad finde ich die Datenbank
    void setDatabasePath( const QString &value );
    //! volle Beschreibung des Ortes + Name der Datenbank
    QString getFullDatabaseLocation( void ) const;
    //! exportpfad zurück geben
    QString getExportPath( void ) const;
    //! Name des Themas zurück geben
    QString getGuiThemeName() const;
    //! Name des Themas setzen
    void setGuiThemeName( const QString &value );
    //! Console logging
    bool getConsoleLog() const;
    void setConsoleLog( bool value );

    private:
    // Logeinstellungen
    bool loadLogSettings( QSettings &settings );
    void makeDefaultLogSettings( QSettings &settings );
    bool saveLogSettings( QSettings &settings );
    // allg. Programmeinstellungen
    bool loadAppSettings( QSettings &settings );
    void makeAppDefaultSettings( QSettings &settings );
    bool saveAppSettings( QSettings &settings );
  };
}  // namespace spx
#endif  // LOGGERCLASS_HPP
