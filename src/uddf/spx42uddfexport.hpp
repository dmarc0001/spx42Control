#ifndef SPX42UDDFEXPORT_HPP
#define SPX42UDDFEXPORT_HPP

#include <QObject>
#include <QString>
#include <QXmlStreamWriter>
#include <QtXml>
#include <memory>
#include "config/ProjectConst.hpp"
#include "database/SPX42Database.hpp"
#include "logging/Logger.hpp"

namespace spx
{
  class SPX42UDDFExport : public QObject
  {
    Q_OBJECT
    private:
    std::shared_ptr< Logger > lg;               // der Logger
    std::shared_ptr< SPX42Database > database;  // Datenbankzugriff
    QString XMLFileNameTemplate;                // BASIS-Name der zu exportierenden Datei
    QString device_mac;                         // MAC des Gerätes/Tauchcomputers
    QVector< int > diveNums;                    // Liste mit Nummern der Tauchgänge

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
    bool writeGenerator( QXmlStreamWriter &stream );
    signals:

    public slots:
  };
}  // namespace spx
#endif  // SPX42UDDFEXPORT_HPP
