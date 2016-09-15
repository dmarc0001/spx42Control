#ifndef SPX42GAS_HPP
#define SPX42GAS_HPP

#include <QtGlobal>
#include <QObject>
#include <QString>

#include "SPX42Defs.hpp"

namespace spx42
{
  class SPX42Gas
  {
    private:
      int O2;                                         //! Sauerstoffanteil des Gases
      int He;                                         //! Heliumanteil des Gases
      DiluentType dType;                              //! Kein DIL, Dil1 oder Dil2
      bool isBaulout;                                 //! Ist es ein Bauloutgas?

    public:
      SPX42Gas(void);                                 //! Standartkonstruktor
      SPX42Gas(int o2, int he,
               DiluentType dilType = DiluentType::DIL_NONE,
               bool isBaulout = false  );             //! Konstruktor mit Parametern
      ~SPX42Gas(void);                                //! der Destruktor
      int getO2(void) const;                          //! Sauerstoff zurück geben
      int setO2(int value,
                LicenseType licType
                = LicenseType::LIC_NITROX );          //! Sauerstoff setzten
      int getHe(void) const;                          //! Heliumanteil zurück geben
      int setHe(int value,
                LicenseType licType
                = LicenseType::LIC_NITROX );          //! Heliumanteil setzten
      int getN2(void) const;                          //! Stickstoffanteil zurückgeben
      DiluentType setDiluentType( DiluentType dType );//! setzte den Typ des Diluents
      DiluentType getDiluentType( void );             //! erfrage den Diluenttyp
      bool setBailout(bool isBailout );               //! setzte das Gas als Bailout
      bool getBailout( void );                        //! Ist das Gas Bailout?
      QString getGasName(void) const;                 //! Klarnamen des Gases
      int getMOD(void);                               //! Maximal Oxigen Depth
      int getEAD(void);                               //! Equivalent Air Depth
      void reset(void );                              //! Alles auf Voreinstellung
  };
}
#endif // SPX42GAS_HPP
