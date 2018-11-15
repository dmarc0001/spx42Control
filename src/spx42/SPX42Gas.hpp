#ifndef SPX42GAS_HPP
#define SPX42GAS_HPP

#include <QObject>
#include <QString>
#include <QtGlobal>

#include "spx42/SPX42Defs.hpp"

namespace spx
{
  class SPX42Gas
  {
    private:
    quint8 O2;          //! Sauerstoffanteil des Gases
    quint8 He;          //! Heliumanteil des Gases
    DiluentType dType;  //! Kein DIL, Dil1 oder Dil2
    bool isBaulout;     //! Ist es ein Bauloutgas?

    public:
    SPX42Gas( void );  //! Standartkonstruktor
    SPX42Gas( quint8 o2,
              quint8 he,
              DiluentType dilType = DiluentType::DIL_NONE,
              bool isBaulout = false );  //! Konstruktor mit Parametern
    ~SPX42Gas( void ) = default;         //! der Destruktor
    quint8 getO2( void );                //! Sauerstoff zurück geben
    quint8 setO2( quint8 value,
                  LicenseType licType = LicenseType::LIC_NITROX );  //! Sauerstoff setzten
    quint8 getHe( void );                                           //! Heliumanteil zurück geben
    quint8 setHe( quint8 value,
                  LicenseType licType = LicenseType::LIC_NITROX );  //! Heliumanteil setzten
    quint8 getN2( void );                                           //! Stickstoffanteil zurückgeben
    DiluentType setDiluentType( DiluentType dType );                //! setzte den Typ des Diluents
    DiluentType getDiluentType( void );                             //! erfrage den Diluenttyp
    bool setBailout( bool isBailout );                              //! setzte das Gas als Bailout
    bool getBailout( void );                                        //! Ist das Gas Bailout?
    QString getGasName( void ) const;                               //! Klarnamen des Gases
    qint16 getMOD( void );                                          //! Maximal Oxigen Depth
    qint16 getEAD( void );                                          //! Equivalent Air Depth
    void reset( void );                                             //! Alles auf Voreinstellung
  };
}
#endif  //   SPX42GAS_HPP
