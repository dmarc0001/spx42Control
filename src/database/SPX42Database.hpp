#ifndef SPX42DATABASE_HPP
#define SPX42DATABASE_HPP

#include <QDir>
#include <QFileInfo>
#include <QHash>
#include <QObject>
#include <QPair>
#include <QtSql>
#include <logging/Logger.hpp>
#include "spx42/SPX42CommandDef.hpp"
#include "spx42/SPX42SingleCommand.hpp"

namespace spx
{
  //
  class SPX42DeviceAlias;  // forward deklaration
  class DiveChartDataset;
  using DeviceAliasHash = QHash< QString, SPX42DeviceAlias >;
  using DiveChartSet = QVector< DiveChartDataset >;
  using DiveChartSetPtr = std::shared_ptr< DiveChartSet >;

  /**
   * @brief The SPX42DeviceAlias class definiert parameter für gerät
   */
  class SPX42DeviceAlias
  {
    public:
    SPX42DeviceAlias();
    explicit SPX42DeviceAlias( const QString &_mac, const QString &_name, const QString &_alias, bool _lastConnect = false );
    SPX42DeviceAlias( const SPX42DeviceAlias &di );
    QString mac;
    QString name;
    QString alias;
    bool lastConnected;
  };

  class DiveLogEntry
  {
    public:
    int diveNum;      // Tauchgangsnummer laufend
    int lfdNr;        // logeintrag nummer des Tauchganges
    int pressure;     // Umgebungsdruck == Tiefe
    int depth;        // Tiefe
    int temperature;  // Temperatur Wasser
    double acku;      // Akkuspannung
    double ppo2;      // Sauerstoffpartioaldruck reslultierend
    double ppo2_1;    // PPO2 Sensor 1
    double ppo2_2;    // PPO2 Sensor 2
    double ppo2_3;    // PPO2 Sensor 3
    int setpoint;     // Setpoint PPO2
    int n2;           // Stickstoff in prozent
    int he;           // Helium in prozent
    int zeroTime;     // Nullzeit zu diesem Zeitpunkt
    int nextStep;     // nächster Logeintrag in zeitlichem Abstand
    DiveLogEntry( void );
    DiveLogEntry( int, int, int, int, int, double, double, double, double, double, int, int, int, int, int );
    DiveLogEntry( const DiveLogEntry &en );
  };

  class DiveChartDataset
  {
    public:
    int lfdnr;
    double depth;
    int temperature;
    double ppo2;
    double ppo2_1;
    double ppo2_2;
    double ppo2_3;
    int nextStep;
    void clear( void );
  };

  /**
   * @brief The SPX42Database class
   */
  class SPX42Database : public QObject
  {
    Q_OBJECT
    public:
    static const QString versionTableName;  //! Name der Versionstabelle
    static const QString deviceTableName;   //! Name der Aliase für Geräte

    private:
    std::shared_ptr< Logger > lg;                //! Zeiger auf ein Loggerobjekt
    QSqlDatabase db;                             //! die lokale Datenbankinstanz
    QString dbName;                              //! Name der Datenbank (sqlite dateiname)
    QString currentConnectionName;               //! name der aktuellen Verbindung
    static const QString sqlDriver;              //! name des SQL Treibers für SQLITE3
    static const qint16 databaseVersion;         //! aktuelle Version der Datenbank (für spätere Versionen wichtig)
    static const QString loglineInsertTemplate;  //! das template für einen INSERT einer Logzeile

    public:
    explicit SPX42Database( std::shared_ptr< Logger > logger, const QString &databaseName, QObject *parent = nullptr );
    ~SPX42Database();
    QSqlError openDatabase( bool createPath = false );  //! öffne die Datenbank, checke auf Vollständigkeit
    QSqlError openDatabase( const QString &databaseName,
                            bool createPath = false );  //! öffne die Datenbank, checke auf Vollständigkeit
    void closeDatabase( void );                         //! schliesse Datenbank
    DeviceAliasHash getDeviceAliasHash( void );         //! gib einen hash mit einem hash (ALIAS <-> DEVICENAME) zurück
    bool addAlias( const QString &mac,
                   const QString &name,
                   const QString &alias,
                   bool lastConnected = false );                        //! erzeuge einen Alias Eintrag
    bool addAlias( const SPX42DeviceAlias &devAlias );                  //! erzeuge einen Alias Eintrag
    bool setAliasForMac( const QString &mac, const QString &alias );    //! setzte einen Aliasnamen für MAC
    bool setAliasForName( const QString &name, const QString &alias );  //! setzte einen Aliasnamen für MAC
    bool setLastConnected( const QString &mac );                        //! setzte das Gerät auf "last connected"
    QString getLastConnected( void );                                   //! wer war der letzte?
    QString getLogTableName( const QString &mac );  //! gib die Tabelle für das Gerät zurück, wenn Gerät in der Liste ist
    bool existDiveLogInBase( const QString &tableName, int diveNum );           //! existiert ein log mit der Nummer
    bool delDiveLogFromBase( const QString &tableName, int diveNum );           //! einen Tauchgang entfernen
    bool insertLogentry( const QString &tableName, const DiveLogEntry &entr );  //! einen Logeintrag zufügen
    bool insertLogentry( const QString &tableName, spSingleCommand );           //! Logeintrag einfügen
    int getMaxDepthFor( const QString &tableName, int diveNum );                //! maximale Tiefe für Tauchgang
    DiveChartSetPtr getChartSet( const QString &tableName, int diveNum );       //! gib daten für CHART für einen Tauchgang

    private:
    bool existTable( const QString &tableName );     //! gibt es folgende Tabelle?
    bool createAliasTable( void );                   //! erzeuge die ALIAS Tabelle
    bool alterAliasTableFrom1To2( void );            //! tabelle zu version 2 heben
    bool createDeviceLogTable( QString deviceMac );  //! erzeuge /lösche und erzeuge eine Tablelle für Logdaten für ein Gerät
    bool createAllTables( void );                    //! neue Datenbank, alleTabellen neu anlegen
    qint16 getDatabaseVersion( void );               //! erfrage die Datenbankversion
    bool createVersionTable( void );                 //! erzeuge die Versionstabelle mit Versiuonsinhalt
    bool checkOrCreateTables( void );                //! teste ob alle Tabellen vorhanden sind, wenn ncht erzeuge diese

    // GETTER und SETTER
    bool isDbOpen() const;
    QString getDbName() const;

    private:
  };
}
#endif  // SPX42DATABASE_HPP
