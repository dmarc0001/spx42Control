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
  // hash<MAC<NAME,ALIAS>>
  using DeviceAliasHash = QHash< QString, QPair< QString, QString > >;

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
    bool addAlias( const QString &mac, const QString &name, const QString &alias );  //! erzeuge einen Alias Eintrag
    bool setAliasForMac( const QString &mac, const QString &alias );                 //! setzte einen Aliasnamen für MAC
    bool setAliasForName( const QString &name, const QString &alias );               //! setzte einen Aliasnamen für MAC

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
