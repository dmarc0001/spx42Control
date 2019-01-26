#include "spx42uddfexport.hpp"

namespace spx
{
  SPX42UDDFExport::SPX42UDDFExport( std::shared_ptr< Logger > logger, std::shared_ptr< SPX42Database > _database, QObject *parent )
      : QObject( parent ), lg( logger ), database( _database )
  {
    lg->debug( "SPX42UDDFExport::SPX42UDDFExport(...)" );
  }

  SPX42UDDFExport::~SPX42UDDFExport()
  {
    lg->debug( "SPX42UDDFExport::~SPX42UDDFExport" );
  }

  void SPX42UDDFExport::setXmlFileBaseName( const QString &_fileName )
  {
    XMLFileNameTemplate = _fileName + "%1.xml";
    lg->debug( QString( "SPX42UDDFExport::setXmlFileBaseName -> file base name template: %1" ).arg( XMLFileNameTemplate ) );
  }

  void SPX42UDDFExport::setExportDives( const QString &mac, const QVector< int > &numbers )
  {
    lg->debug( QString( "SPX42UDDFExport::setExportDives -> device: %1" ).arg( mac ) );
    device_mac = mac;
    diveNums.clear();
    diveNums.append( numbers );
  }

}  // namespace spx
