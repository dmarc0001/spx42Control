#include "SPX42Gas.hpp"

namespace spx42
{
  SPX42Gas::SPX42Gas(void) :
    O2(21),
    He(0),
    dType( DiluentType::DIL_NONE ),
    isBaulout( false )
  {

  }

  SPX42Gas::SPX42Gas(int o2, int he, DiluentType dilType, bool isBaulout ) :
    O2(o2),
    He(he),
    dType( dilType ),
    isBaulout( isBaulout )
  {

  }

  SPX42Gas::~SPX42Gas(void)
  {

  }

  int SPX42Gas::getO2(void) const
  {
    return O2;
  }

  int SPX42Gas::setO2(int value, LicenseType licType)
  {
    if( value > 100 ) value = 100;
    O2 = value;
    //
    switch( static_cast<qint8>(licType) )
    {
      case static_cast<qint8>(LicenseType::LIC_NITROX ):
        He = 0;                                                                // Bei der Lizenz ist HE immer 0
      // weiter bei normoxic, dabei auch O2 auf Zulässigkeit testen
      case static_cast<qint8>(LicenseType::LIC_NORMOXIX ):
        if( O2 < 21 )                                                       // Hier darf O2 nicht kleiner als 21%
        {
          O2 = 21;
        }
        He = 100 - O2;
        // weiter bei höheren Lizenzen, da darf O2 kleiner 21 und He darf auch
      case static_cast<qint8>(LicenseType::LIC_FULLTMX ):
      case static_cast<qint8>(LicenseType::LIC_MIL ):
        // Priorität hat O2, helium runter, wenn es nicht passt
        int n2 = (100 - O2 - He);
        if( n2 < 0 )
        {
          He = 100 - O2;
        }
    }
    return( O2 );
  }

  int SPX42Gas::getHe(void) const
  {
    return He;
  }

  int SPX42Gas::setHe(int value, LicenseType licType)
  {
    if( value > 99 ) value = 100;
    He = value;
    //
    switch( static_cast<qint8>(licType) )
    {
      case static_cast<qint8>(LicenseType::LIC_NITROX ):
        He = 0;                                                                // Bei der Lizenz ist HE immer 0
      // weiter bei normoxic, dabei auch O2 auf Zulässigkeit testen
      case static_cast<qint8>(LicenseType::LIC_NORMOXIX ):
        if( O2 < 21 )                                                       // Hier darf O2 nicht kleiner als 21%
        {
          O2 = 21;
        }
        if( He > 78 )                                                       // und damit darf HE nicht mehr als 78
        {
          He = 78;
        }
        // weiter bei höheren Lizenzen, da darf O2 kleiner 21 und He darf auch
      case static_cast<qint8>(LicenseType::LIC_FULLTMX ):
      case static_cast<qint8>(LicenseType::LIC_MIL ):
        // Priorität hat O2, helium runter, wenn es nicht passt
        int n2 = (100 - O2 - He);
        if( n2 < 0 )
        {
          He = 100 - O2;
        }
    }
    return( He );
  }

  int SPX42Gas::getN2(void) const
  {
    return(100 - O2 - He);
  }

  DiluentType SPX42Gas::setDiluentType( DiluentType dType )
  {
    this->dType = dType;
    return( this->dType );
  }

  DiluentType SPX42Gas::getDiluentType( void )
  {
    return( this->dType );
  }

  bool SPX42Gas::setBailout(bool isBailout )
  {
    this->isBaulout = isBailout;
    return( this->isBaulout );
  }

  bool SPX42Gas::getBailout( void )
  {
    return( this->isBaulout );
  }

  QString SPX42Gas::getGasName(void) const
  {
    if( O2 == 100 )
    {
      // Sauerstoff
      return( QObject::tr("Oxygen") );
    }
    if( He == 0 )
    {
      // Luft oder Nitrox
      if( O2 == 21 )
      {
        return( QObject::tr("Air"));
      }
      return( QString(QObject::tr("NX%1")).arg(O2, 1, 10));
    }
    // Hier bleibt noch TX und HX
    int n2 = 100 - O2 - He;
    if( n2 == 0 )
    {
      // Heliox
      return( QString(QObject::tr("HX%1/%2")).arg(O2, 1, 10).arg(He, 1, 10));
    }
    return( QString(QObject::tr("TX%1/%2")).arg(O2, 1, 10).arg(He, 1, 10));
  }

  int SPX42Gas::getMOD(void)
  {
    // FIXME: Berechne MOD
    return(0);
  }

  int SPX42Gas::getEAD(void)
  {
    // FIXME: Berechne EAD
    return(0);
  }

  void SPX42Gas::reset( void )
  {
    O2 = 21;
    He = 0;
    dType = DiluentType::DIL_NONE;
    isBaulout = false;
  }

}
