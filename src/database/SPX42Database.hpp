#ifndef SPX42DATABASE_HPP
#define SPX42DATABASE_HPP

#include <QDir>
#include <QFileInfo>
#include <QHash>
#include <QObject>
#include <QPair>
#include <QtSql>
#include <logging/Logger.hpp>

namespace spx
{
  //
  class SPX42DeviceAlias;  // forward deklaration
  using DeviceAliasHash = QHash< QString, SPX42DeviceAlias >;

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
    std::shared_ptr< Logger > lg;         //! Zeiger auf ein Loggerobjekt
    QSqlDatabase db;                      //! die lokale Datenbankinstanz
    QString dbName;                       //! Name der Datenbank (sqlite dateiname)
    QString currentConnectionName;        //! name der aktuellen Verbindung
    static const QString sqlDriver;       //! name des SQL Treibers für SQLITE3
    static const qint16 databaseVersion;  //! aktuelle Version der Datenbank (für spätere Versionen wichtig)

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

    private:
    bool existTable( const QString &tableName );  //! gibt es folgende Tabelle?
    bool createAllTables( void );                 //! neue Datenbank, alleTabellen neu anlegen
    qint16 getDatabaseVersion( void );            //! erfrage die Datenbankversion
    bool createVersionTable( void );              //! erzeuge die Versionstabelle mit Versiuonsinhalt
    bool checkOrCreateTables( void );             //! teste ob alle Tabellen vorhanden sind, wenn ncht erzeuge diese
    bool createAliasTable( void );                //! erzeuge die ALIAS Tabelle

    // GETTER und SETTER
    bool isDbOpen() const;
    QString getDbName() const;

    private:
  };
}
#endif  // SPX42DATABASE_HPP
