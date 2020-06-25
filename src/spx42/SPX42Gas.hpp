#ifndef SPX42GAS_HPP
#define SPX42GAS_HPP

#include <QObject>
#include <QString>
#include <QtGlobal>
#include <cmath>
#include "config/ProjectConst.hpp"
#include "spx42/SPX42Defs.hpp"

namespace spx
{
  class SPX42Gas
  {
    private:
    //! Sauerstoffanteil des Gases
    quint8 O2;
    //! Heliumanteil des Gases
    quint8 He;
    //! Kein DIL, Dil1 oder Dil2
    DiluentType dType;
    //! Ist es ein Bauloutgas?
    bool isBaulout;

    public:
    //! Standartkonstruktor
    SPX42Gas( void );
    //! Konstruktor mit Parametern
    SPX42Gas( quint8 o2, quint8 he, DiluentType dilType = DiluentType::DIL_NONE, bool isBaulout = false );
    //! der Destruktor
    ~SPX42Gas( void ) = default;
    //! Sauerstoff zurück geben
    quint8 getO2( void );
    //! Sauerstoff setzten
    quint8 setO2( quint8 value, LicenseType licType = LicenseType::LIC_NITROX );
    //! Heliumanteil zurück geben
    quint8 getHe( void );
    //! Heliumanteil setzten
    quint8 setHe( quint8 value, LicenseType licType = LicenseType::LIC_NITROX );
    //! Stickstoffanteil zurückgeben
    quint8 getN2( void );
    //! setzte den Typ des Diluents
    DiluentType setDiluentType( DiluentType dType );
    //! erfrage den Diluenttyp
    DiluentType getDiluentType( void );
    //! setzte das Gas als Bailout
    bool setBailout( bool isBailout );
    //! Ist das Gas Bailout?
    bool getBailout( void );
    //! Klarnamen des Gases
    QString getGasName( void ) const;
    //! Maximal Oxigen Depth
    qint16 getMOD( DeviceWaterType wType );
    //! Equivalent Air Depth
    qint16 getEAD( qint16 depth, DeviceWaterType wType = DeviceWaterType::FRESHWATER );
    //! Alles auf Voreinstellung
    void reset( void );
    //! serialize
    QByteArray serialize( void );
  };
}  // namespace spx
#endif  //   SPX42GAS_HPP
