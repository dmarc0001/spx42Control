#ifndef SPX42UDDFEXPORT_HPP
#define SPX42UDDFEXPORT_HPP

#include <QObject>
#include <QString>
#include <QVector>
#include <QXmlStreamWriter>
#include <QtXml>
#include <memory>
#include "config/ProjectConst.hpp"
#include "database/SPX42Database.hpp"
#include "logging/Logger.hpp"

namespace spx
{
  class UddfGasDefinition
  {
    public:
    QString gName;
    double o2;
    double n2;
    double he;
    double ar;
    double h2;
    UddfGasDefinition();
    explicit UddfGasDefinition( int _n2, int _he );

    private:
    UddfGasDefinition( double n2, double he );
    UddfGasDefinition( double o2, double n2, double he, double ar = 0.0, double h2 = 0.0 );

    public:
    QString getName( void ) const;
    QString getO2( void ) const;
    QString getN2( void ) const;
    QString getHe( void ) const;
    QString getAr( void ) const;
    QString getH2( void ) const;

    private:
    QString getFloatStr( double val ) const;
  };

  class SPX42UDDFExport : public QObject
  {
    Q_OBJECT
    private:
    //! der Logger
    std::shared_ptr< Logger > lg;
    //! Datenbankzugriff
    std::shared_ptr< SPX42Database > database;
    //! BASIS-Name der zu exportierenden Datei
    QString XMLFileNameTemplate;
    //! MAC des Gerätes/Tauchcomputers
    QString device_mac;
    //! Liste mit Nummern der Tauchgänge
    QVector< int > diveNums;
    //! Liste der genutzten Gase
    QVector< UddfGasDefinition > gasDefs;

    public:
    //! Konstruktor
    explicit SPX42UDDFExport( std::shared_ptr< Logger > logger,
                              std::shared_ptr< SPX42Database > _database,
                              QObject *parent = nullptr );
    // Destruktor
    ~SPX42UDDFExport();
    //! Basisname der zu exportierenden XML Datei
    void setXmlFileBaseName( const QString &_fileName );
    //! Angaben zu den zu exportierenden Tauchgängen
    void setExportDives( const QString &mac, const QVector< int > &numbers );
    //! Erzeuge den Export
    int createExportXml( void );

    private:
    //! Schreibe in den Stream den "Generator" block
    bool writeGenerator( QXmlStreamWriter &st );
    //! Schreibe Gasdefinitionen
    bool writeGasDefinitions( QXmlStreamWriter &st );

    signals:

    public slots:
  };
}  // namespace spx
#endif  // SPX42UDDFEXPORT_HPP
