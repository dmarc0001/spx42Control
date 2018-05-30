#include <QString>
#include <QVector>

#ifndef BLUETHOOTHDEVICE_HPP
#define BLUETHOOTHDEVICE_HPP

namespace spx
{
  // TODO: QBluetoothUuid QBluetoothDeviceInfo::deviceUuid() const bei OSX, address bei OSX nicht nutzbar!
  class BluetoothDeviceDescriber
  {
      //###########################################################################
      //#### Member Variablen
      //###########################################################################
    public:
    private:
      QString devName;                                     //! der Gerätename, vom BT Gerät gemeldet
      QString addr;                                        //! die Adresse (MAC-Addr)
      QString alias;                                       //! ein Aliasname zum merken
      QString pin;                                         //! PIN der Verbindung, wenn vorhanden
      QVector<QString> serviceNames;                       //! Liste von angebotenen Diensten

      //###########################################################################
      //#### Member Funktionen
      //###########################################################################
    public:
      BluetoothDeviceDescriber( void );                    //! Standartkonstruktor
      BluetoothDeviceDescriber( const QString& addr );   //! Konstruktor mit Addr
      BluetoothDeviceDescriber( const BluetoothDeviceDescriber& btd ); //! Kopierkonsttruktor
      virtual ~BluetoothDeviceDescriber( void );           //! der Zerstörer
      void clear(void);                                    //! Leere alle Felder, mache ungültig
      bool istDefined( void );                             //! ist die Definition vollständig?
      const QString& getDevName( void );                   //! wie ist der Gerätename?
      void setDevName( const QString& devName );           //! Setze den Gerätenamen
      const QString& getAddr(void);                        //! Gib die Geräteadresse zurück
      void setAddr( const QString& addr );                 //! Setze die Gerätreadresse
      const QString& getAlias( void );                     //! Alias des Gerätes
      void setAlias( const QString& alias );               //! setze den Aliasnamen
      const QVector<QString>& getServiceNames( void );     //! Gib die Liste der Services zurück
      void addServiceName( const QString& sName );         //! Füge einen Servicenamen zu
      void clearServices( void );                          //! Leere Serviceliste
      const QString& getPin(void);                         //! gib die PIN zuurück
      void setPin(const QString& value);                   //! setze PIN
  };
}
#endif
