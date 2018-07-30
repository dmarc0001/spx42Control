#ifndef SPX42DATABASE_HPP
#define SPX42DATABASE_HPP

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
    private:
    std::shared_ptr< Logger > lg;    //! Zeiger auf ein Loggerobjekt
    QSqlDatabase db;                 //! die lokale Datenbankinstanz
    QString dbName;                  //! Name der Datenbank (sqlite dateiname)
    QString currentConnectionName;   //! name der aktuellen Verbindung
    static const QString sqlDriver;  //! name des SQL Treibers für SQLITE3

    public:
    explicit SPX42Database( std::shared_ptr< Logger > logger, const QString databaseName, QObject *parent = nullptr );
    ~SPX42Database();
    QSqlError openDatabase( void );
    QSqlError openDatabase( const QString databaseName );
    void closeDatabase( void );
    DeviceAliasHash getDeviceAliasHash( void );

    // GETTER und SETTER
    bool isDbOpen() const;
    QString getDbName() const;

    private:
  };
}
#endif  // SPX42DATABASE_HPP
