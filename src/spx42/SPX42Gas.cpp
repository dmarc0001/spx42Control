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
        if ( O2 < ProjectConst::SPX_MINIMUM_O2 )
        {
          // Minimum...
          O2 = ProjectConst::SPX_MINIMUM_O2;
        }
        // Priorität hat O2, helium runter, wenn es nicht passt
        int N2 = ( 100 - O2 - He );
        if ( N2 < 0 )
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
        if ( O2 < ProjectConst::SPX_MINIMUM_O2 )
        {
          // Minimum...
          O2 = ProjectConst::SPX_MINIMUM_O2;
        }
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

  qint16 SPX42Gas::getMOD( DeviceWaterType wType )
  {
    //
    // Berechne MOD für 1.6 Bar PPO2
    //
    // Salzwasser: 9,7 Meter per Bar
    // Süsswasser: 10,0 Meter per Bar
    //
    if ( wType == DeviceWaterType::SALTWATER )
    {
      //
      // Errechnen der Maximalen Einsatztiefe bei 1.6 Bar PPO2
      //
      double mod_d = 9.7 * ( ( SPX42ConfigClass::MAX_PPO2 / ( O2 / 100.0 ) ) - 1 );
      //
      // umrechnen in qint16, d.h. nur ganze Meter
      //
      return ( static_cast< qint16 >( std::floor( mod_d ) ) );
    }
    else
    {
      //
      // Errechnen der Maximalen Einsatztiefe bei 1.6 Bar PPO2
      //
      double mod_d = 10.0 * ( ( SPX42ConfigClass::MAX_PPO2 / ( O2 / 100.0 ) ) - 1 );
      //
      // umrechnen in qint16, d.h. nur ganze Meter
      //
      return ( static_cast< qint16 >( std::floor( mod_d ) ) );
    }
  }

  qint16 SPX42Gas::getEAD( qint16 depth, DeviceWaterType wType )
  {
    // Berechne EAD
    //
    // Salzwasser: 9,7 Meter per Bar
    // Süsswasser: 10,0 Meter per Bar
    //
    double pDepth;
    if ( wType == DeviceWaterType::SALTWATER )
    {
      pDepth = ( 9.7 * depth ) + 1;
      //
      // wirkicher N2 Partialdruck in der Tiefe
      //
      double p_n2 = pDepth * ( getN2() / 100.0 );
      //
      // in welcher Tiefe wäre der Partialdruck mit Luft genauso?
      //
      double p_ead = p_n2 / 0.79;
      //
      // und welche Tiefe wäre das nun?
      //
      return ( static_cast< qint16 >( std::round( p_ead - 1.0 ) / 9.7 ) );
    }
    else
    {
      pDepth = ( 10.0 * depth ) + 1;
      //
      // wirkicher N2 Partialdruck in der Tiefe
      //
      double p_n2 = pDepth * ( getN2() / 100.0 );
      //
      // in welcher Tiefe wäre der Partialdruck mit Luft genauso?
      //
      double p_ead = p_n2 / 0.79;
      //
      // und welche Tiefe wäre das nun?
      //
      return ( static_cast< qint16 >( std::round( p_ead - 1.0 ) / 10.0 ) );
    }
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
}  // namespace spx
