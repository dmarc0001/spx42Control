﻿#ifndef SPX42DATABASE_HPP
#define SPX42DATABASE_HPP

#include <QDir>
#include <QFileInfo>
#include <QHash>
#include <QMutex>
#include <QMutexLocker>
#include <QObject>
#include <QPair>
#include <QVector>
#include <QtSql>
#include <logging/Logger.hpp>
#include "spx42/SPX42CommandDef.hpp"
#include "spx42/SPX42LogDirectoryEntry.hpp"
#include "spx42/SPX42SingleCommand.hpp"
#include "spx42databaseconstants.hpp"

namespace spx
{
  // forward deklarationen
  class SPX42DeviceAlias;
  class DiveDataset;
  // Lesbar machen
  using DeviceAliasHash = QHash< QString, SPX42DeviceAlias >;
  using DiveDataSetsVector = QVector< DiveDataset >;
  using DiveDataSetsPtr = std::shared_ptr< DiveDataSetsVector >;
  using UsedGasList = QVector< QPair< int, int > >;  // Liste aus Paaren <N2,HE>

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
    //! Id aius Tabelle detaildir
    int detailId;
    //! logeintrag nummer des Tauchganges
    int lfdNr;
    //! Umgebungsdruck == Tiefe
    int pressure;
    //! Tiefe
    int depth;
    //! Temperatur Wasser
    int temperature;
    //! Akkuspannung
    double acku;
    //! Sauerstoffpartioaldruck reslultierend
    double ppo2;
    //! PPO2 Sensor 1
    double ppo2_1;
    //! PPO2 Sensor 2
    double ppo2_2;
    //! PPO2 Sensor 3
    double ppo2_3;
    //! Setpoint PPO2
    int setpoint;
    //! Stickstoff in prozent
    int n2;
    //! Helium in prozent
    int he;
    //! Nullzeit zu diesem Zeitpunkt
    int zeroTime;
    //! nächster Logeintrag in zeitlichem Abstand
    int nextStep;
    // Konstruktoren
    DiveLogEntry( void );
    DiveLogEntry( int, int, int, int, int, double, double, double, double, double, int, int, int, int, int );
    DiveLogEntry( const DiveLogEntry &en );
  };

  class DiveDataset
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
    double setpoint;
    int n2;
    int he;
    int z_time;
    double abs_depth;
    void clear( void );
  };

  /**
   * @brief The SPX42Database class
   */
  class SPX42Database : public QObject
  {
    Q_OBJECT

    private:
    //! Zeiger auf ein Loggerobjekt
    std::shared_ptr< Logger > lg;
    //! die lokale Datenbankinstanz
    QSqlDatabase db;
    //! Name der Datenbank (sqlite dateiname)
    QString dbName;
    //! name der aktuellen Verbindung
    QString currentConnectionName;
    //! name des SQL Treibers für SQLITE3
    static const QString sqlDriver;
    //! aktuelle Version der Datenbank (für spätere Versionen wichtig)
    static const qint16 databaseVersion;
    //! das template für einen INSERT einer Logzeile
    static const QString loglineInsertTemplate;
    //! Threadsaves schreiben auf die DB
    QMutex dbMutex;

    public:
    //! Der Konstruktor, explizit der Standartkonstruktor
    explicit SPX42Database( std::shared_ptr< Logger > logger, const QString &databaseName, QObject *parent = nullptr );
    //! Der Destruktor
    ~SPX42Database();
    //! öffne die Datenbank, checke auf Vollständigkeit
    QSqlError openDatabase( bool createPath = false );
    //! öffne die Datenbank, checke auf Vollständigkeit
    QSqlError openDatabase( const QString &databaseName, bool createPath = false );
    //! schliesse Datenbank
    void closeDatabase( void );
    //! Zeiger auf aktuelle DB zurück
    QSqlDatabase *getDatabase( void );
    //! gib einen hash mit einem hash (ALIAS <-> DEVICENAME) zurück
    DeviceAliasHash getDeviceAliasHash( void );
    //! erzeuge einen Alias Eintrag
    bool addAlias( const QString &mac, const QString &name, const QString &alias, bool lastConnected = false );
    //! erzeuge einen Alias Eintrag
    bool addAlias( const SPX42DeviceAlias &devAlias );
    //! setzte einen Aliasnamen für MAC
    bool setAliasForMac( const QString &mac, const QString &alias );
    //! setzte einen Aliasnamen für MAC
    bool setAliasForName( const QString &name, const QString &alias );
    //! gib den Alias für eine MAC Adresse zurück
    QString getAliasForMac( const QString &mac );
    //! gib den Alias für einen Namen zurück
    QString getAliasForName( const QString &name );
    //! setzte das Gerät auf "last connected"
    bool setLastConnected( const QString &mac );
    //! wer war der letzte?
    QString getLastConnected( void );
    //! gib die Geräteid zurück, wenn Gerät in der Liste ist
    int getDevicveId( const QString &mac );
    //! Gib für einen Tauchgang für ein Gerät die Detsail-id zurück
    int getDetailId( const QString &mac, int diveNum );
    //! fügt einen Eintrag in das Logverzeichnis ein
    int insertDiveLogInBase( const QString &mac, int diveNum, qint64 timestamp );
    //! existiert ein log mit der Nummer
    bool existDiveLogInBase( const QString &mac, int diveNum );
    //! einen Tauchgang entfernen
    bool delDiveLogFromBase( const QString &mac, int diveNum );
    //! einen Logeintrag zufügen
    bool insertLogentry( const DiveLogEntry &entr );
    //! Logeintrag einfügen
    bool insertLogentry( int detail_id, spSingleCommand );
    //! maximale Tiefe für Tauchgang
    int getMaxDepthFor( const QString &mac, int diveNum );
    //! gib daten für CHART für einen Tauchgang
    DiveDataSetsPtr getDiveDataSets( const QString &mac, int diveNum );
    //! gib eine Liste der gespeicherten Tauchgänge für ein Gerät zurück
    SPX42LogDirectoryEntryListPtr getLogentrysForDevice( const QString &mac );
    //! erzeuge statistik daten in der verzeichnistabelle
    bool computeStatistic( int detail_id );
    //! erzeuge eine Liste mit Gasen, welche bei den gewählten TG genutzt wurde
    UsedGasList getGasList( const QString &mac, const QVector< int > *diveNums );
    //! gib den Zeitpunkt eines Tauchganges zurück
    qint64 getTimestampForDive( const QString &mac, int diveNum );
    //! Schreibe eine Bemerkung in die datenbank (max 120 Zeichen)
    bool writeNotesForDive( const QString &mac, int diveNum, const QString &notes );
    //! lese Bemerkung aus der DB, wenn vorhanden
    QString getNotesForDive( const QString &mac, int diveNum );

    private:
    //! gibt es folgende Tabelle?
    bool existTable( const QString &tableName );
    //! erzeuge die ALIAS Tabelle
    bool createAliasTable( void );
    //! tabelle zu version 2 heben
    bool alterAliasTableFrom1To2( void );
    //! erzeuge /lösche und erzeuge eine Tablelle für DEtails der Logdaten
    bool createLogDataTable( void );
    //! erzeuge Verzeichnistabelle für dive logs
    bool createLogDirTable( void );
    //! neue Datenbank, alleTabellen neu anlegen
    bool createAllTables( void );
    //! erfrage die Datenbankversion
    qint16 getDatabaseVersion( void );
    //! erzeuge die Versionstabelle mit Versiuonsinhalt
    bool createVersionTable( void );
    //! teste ob alle Tabellen vorhanden sind, wenn ncht erzeuge diese
    bool checkOrCreateTables( void );
    //! Update Tabelle detaildir, füge feld "notes" hinzu
    bool updateDetailDirTableFromFour( void );
    //! teste ob next step plausibel ist (es gab fehlerhafte Einträge)
    int validateNextStep( int val, int oldval );

    // GETTER und SETTER
    bool isDbOpen() const;
    QString getDbName() const;

    private:
  };
}  // namespace spx
#endif  // SPX42DATABASE_HPP
