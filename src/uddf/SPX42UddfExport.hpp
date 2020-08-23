#ifndef SPX42UDDFEXPORT_HPP
#define SPX42UDDFEXPORT_HPP

#include <QObject>
#include <QString>
#include <QVector>
#include <QXmlStreamWriter>
#include <QtXml>
#include <algorithm>
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
    std::shared_ptr< Logger > lg;               //! der Logger
    std::shared_ptr< SPX42Database > database;  //! Datenbankzugriff
    QString XMLFileNameTemplate;                //! BASIS-Name der zu exportierenden Datei
    QString device_mac;                         //! MAC des Gerätes/Tauchcomputers
    QVector< int > diveNums;                    //! Liste mit Nummern der Tauchgänge
    QVector< UddfGasDefinition > gasDefs;       //! Liste der genutzten Gase

    public:
    //! Konstruktor
    explicit SPX42UDDFExport( std::shared_ptr< Logger > logger,
                              std::shared_ptr< SPX42Database > _database,
                              QObject *parent = nullptr );
    ~SPX42UDDFExport();                                                        //! Destruktor
    void setXmlFileBaseName( const QString &_fileName );                       //! Basisname der zu exportierenden XML Datei
    void setExportDives( const QString &mac, const QVector< int > &numbers );  //! Angaben zu den zu exportierenden Tauchgängen
    bool createExportXml( void );                                              //! Erzeuge den Export

    private:
    bool writeGenerator( QXmlStreamWriter &st );                          //! Schreibe in den Stream den "Generator" block
    bool writeDiverDefinitions( QXmlStreamWriter &st );                   //! Schreibe Taucher Referenzen für Rebreathre Sensoren
    bool writeOwnerDefinitions( QXmlStreamWriter &st );                   //! Eigentüber Referenzen schreiben
    bool writeOwnerEquipment( QXmlStreamWriter &st );                     //! die Ausrüstung des Eigentümers schreiben
    bool writeRebreatherDefinitions( QXmlStreamWriter &st );              //! Rebreather als Ausrüstung beschreiben
    bool writeO2Sensor( int number, QXmlStreamWriter &st );               //! Sauerstoffsensoren innerhalb Rebreather
    bool writeDiveSamples( DiveDataSetsPtr data, QXmlStreamWriter &st );  //! Schreibe die Sample-Daten
    QString getGasRef( int n2, int he ) const;                            //! suche die richtge Gasreferenz heraus
    int getLowestTemp( DiveDataSetsPtr data );                            //! suche den kleinsten Temperaturwert
    double getGreatestDepth( DiveDataSetsPtr data );                      //! suche grösste Tiefe
    bool writeGasDefinitions( QXmlStreamWriter &st );                     //! Schreibe Gasdefinitionen
    bool writeProfileData( QXmlStreamWriter &st );           //! Schreibe Profiledata Sektion (alle datennzu jedem Tauchgang)
    bool writeAllDives( QXmlStreamWriter &st );              //! Schreibe alle Tauchgänge
    bool writeOneDive( int diveNum, QXmlStreamWriter &st );  //! Schreibe einen Tauchgang in die Datei

    signals:
    void onExportStartSig( int diveNum );
    void onExportEndSingleDiveSig( int diveNum );
    void onExportEndSig( bool wasOk, const QString &fileName );

    public slots:
  };
}  // namespace spx
#endif  // SPX42UDDFEXPORT_HPP
