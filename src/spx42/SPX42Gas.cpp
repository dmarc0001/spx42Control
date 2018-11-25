#include "SPX42Gas.hpp"

namespace spx
{
  SPX42Gas::SPX42Gas() : O2( 21 ), He( 0 ), dType( DiluentType::DIL_NONE ), isBaulout( false )
  {
  }

  SPX42Gas::SPX42Gas( quint8 o2, quint8 he, DiluentType dilType, bool isBaulout )
      : O2( o2 ), He( he ), dType( dilType ), isBaulout( isBaulout )
  {
  }

  quint8 SPX42Gas::getO2()
  {
    return O2;
  }

  quint8 SPX42Gas::setO2( quint8 value, LicenseType licType )
  {
    if ( value > 100 )
    {
      value = 100;
    }
    O2 = value;
    //
    switch ( static_cast< qint8 >( licType ) )
    {
      case static_cast< qint8 >( LicenseType::LIC_NITROX ):
        He = 0;         // Bei der Lizenz ist HE immer 0
        if ( O2 < 21 )  // Hier darf O2 nicht kleiner als 21%
        {
          O2 = 21;
        }
        break;

      case static_cast< qint8 >( LicenseType::LIC_NORMOXIX ):
        if ( O2 < 21 )  // Hier darf O2 nicht kleiner als 21%
        {
          O2 = 21;
        }
        // He = 100 - O2;
      // weiter bei höheren Lizenzen, da darf O2 kleiner 21 und He darf auch
      case static_cast< qint8 >( LicenseType::LIC_FULLTMX ):
      case static_cast< qint8 >( LicenseType::LIC_MIL ):
        // Priorität hat O2, helium runter, wenn es nicht passt
        int n2 = ( 100 - O2 - He );
        if ( n2 < 0 )
        {
          He = 100 - O2;
        }
    }
    return ( O2 );
  }

  quint8 SPX42Gas::getHe()
  {
    return He;
  }

  quint8 SPX42Gas::setHe( quint8 value, LicenseType licType )
  {
    if ( value > 99 )
    {
      value = 100;
    }
    He = value;
    //
    switch ( static_cast< qint8 >( licType ) )
    {
      case static_cast< qint8 >( LicenseType::LIC_NITROX ):
        He = 0;  // Bei der Lizenz ist HE immer 0
      // weiter bei normoxic, dabei auch O2 auf Zulässigkeit testen
      case static_cast< qint8 >( LicenseType::LIC_NORMOXIX ):
        if ( O2 < 21 )  // Hier darf O2 nicht kleiner als 21%
        {
          O2 = 21;
        }
      // weiter bei höheren Lizenzen, da darf O2 kleiner 21 und He darf auch
      case static_cast< qint8 >( LicenseType::LIC_FULLTMX ):
      case static_cast< qint8 >( LicenseType::LIC_MIL ):
        // Priorität hat O2, helium runter, wenn es nicht passt
        int n2 = ( 100 - O2 - He );
        if ( n2 < 0 )
        {
          He = 100 - O2;
        }
    }
    return ( He );
  }

  quint8 SPX42Gas::getN2()
  {
    return ( 100 - O2 - He );
  }

  DiluentType SPX42Gas::setDiluentType( DiluentType dType )
  {
    this->dType = dType;
    return ( this->dType );
  }

  DiluentType SPX42Gas::getDiluentType()
  {
    return ( this->dType );
  }

  bool SPX42Gas::setBailout( bool isBailout )
  {
    this->isBaulout = isBailout;
    return ( this->isBaulout );
  }

  bool SPX42Gas::getBailout()
  {
    return ( this->isBaulout );
  }

  QString SPX42Gas::getGasName() const
  {
    if ( O2 == 100 )
    {
      // Sauerstoff
      return ( QObject::tr( "Oxygen" ) );
    }
    if ( He == 0 )
    {
      // Luft oder Nitrox
      if ( O2 == 21 )
      {
        return ( QObject::tr( "Air" ) );
      }
      return ( QString( QObject::tr( "NX%1" ) ).arg( O2, 2, 10, QChar( '0' ) ) );
    }
    // Hier bleibt noch TX und HX
    int n2 = 100 - O2 - He;
    if ( n2 == 0 )
    {
      // Heliox
      return ( QString( QObject::tr( "HX%1/%2" ) ).arg( O2, 2, 10, QChar( '0' ) ).arg( He, 2, 10, QChar( '0' ) ) );
    }
    return ( QString( QObject::tr( "TX%1/%2" ) ).arg( O2, 2, 10, QChar( '0' ) ).arg( He, 2, 10, QChar( '0' ) ) );
  }

  qint16 SPX42Gas::getMOD()
  {
    // FIXME: Berechne MOD
    return ( 0 );
  }

  qint16 SPX42Gas::getEAD()
  {
    // FIXME: Berechne EAD
    return ( 0 );
  }

  void SPX42Gas::reset()
  {
    O2 = 21;
    He = 0;
    dType = DiluentType::DIL_NONE;
    isBaulout = false;
  }

  QByteArray SPX42Gas::serialize()
  {
    QByteArray serialized;
    serialized.append( QString( "o2:%1 he:%2 type:%3 bailout:%4" )
                           .arg( static_cast< int >( O2 ) )
                           .arg( static_cast< int >( He ) )
                           .arg( static_cast< int >( dType ) )
                           .arg( static_cast< bool >( isBaulout ) ) );
    return ( serialized );
  }
