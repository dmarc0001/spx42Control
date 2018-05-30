#ifndef BTDEVICE_HPP
#define BTDEVICE_HPP

#include <QObject>
#include <QByteArray>
#include <QString>
#include "../config/ProjectConst.hpp"
#include "../logging/Logger.hpp"
#include "BtDeviceDescriber.hpp"


namespace spx
{
  class ABTDevice : public QObject
  {
    private:
      Q_OBJECT
      //###########################################################################
      //#### Member Variable                                                   ####
      //###########################################################################
    protected:
      QByteArray deviceAddr;
      QByteArray pin;
      IndicatorStati isInitOk;
      Logger *lg;

    public:
      ABTDevice( Logger *log, const char *dAddr );                 //! Logger, MAC-Adressec_string oder NULL
      ABTDevice( Logger *log, const QByteArray& dAddr );           //! Logger, MAC-Adressec_string oder NULL
      ABTDevice( const char *dAddr = nullptr );                    //! MAC-Adressec_string oder NULL
      ABTDevice( const QByteArray& dAddr );                        //! MAC-Adressec_string oder NULL
      virtual int setLogger( Logger * log );                      //! den Systemlogger übergeben
      virtual void discoverDevices(void) = 0;                     //! Asyncron, finde BT-Geräte, sende Signal bei Finden
      virtual int connectBT( void ) = 0;                          //! verbinden, wenn Gerätename vorhanden
      virtual int connectBT( const char *dAddr, const char *pin = nullptr ) = 0;                   //! verbinden mit Adresse/Name und PIN
      virtual int connectBT( const QByteArray& dAddr, const QByteArray& pin = nullptr ) = 0;       //! verbinden mit Adresse/Name und PIN
      virtual int disconnectBT( void ) = 0;                       //! trenne die Verbindung zum Gerät
      virtual const QByteArray& getConnectedDeviceAddr(void);     //! gib die Adresse des verbundenen Gerätes zurück
      virtual int write( const char *str, int len ) = 0;          //! schreibe zum Gerät (wenn verbunden)
      virtual int write( const QByteArray& cmd ) = 0;             //! schreibe zum Gerät (wenn verbunden)
      virtual int read( char *dest, int maxlen ) = 0;             //! lese vom Gerät (wenn verbunden)
      virtual int avail( void ) = 0;                              //! vom Gerät bereits übertragene, verfügbare Daten
      virtual bool spxPDUAvail(void) = 0;                         //! ist eine SPX42 PDU empfangen
      virtual QByteArray getSpxPDU(void) = 0;                     //! gib eine SPX-PDU zurück
      virtual bool removeCrLf(void) = 0;                          //! entferne CRLF am Anfang
      virtual bool isConnected() = 0;                             //! Info, ob das Gerät verbunden ist
      virtual QString getErrorText( int errnr, qint64 langId ) const = 0;  //! Liefere Erklärung für Fehler, in sprache langId
      //
    signals:
      void btDiscoveringSig(void);                                //! Signal, wenn discovering startet
      void btDeviceDiscoveredSig(BluetoothDeviceDescriber *device ); //! Signal, wenn ein SPX42 gefunden wurde
      void btDiscoverEndSig(void);                                //! Signal, wenn discovering beendet ist
      void btConnectingSig( void );                               //! Signal, wenn eine Verbindung aufgebaut wird
      void btConnectedSig( const QByteArray& dAddr );             //! Signal, wenn eine Verbindung zustande gekommen ist
      void btDisconnectSig( void );                               //! Signal, wenn eine Verbindung beendet/unterbrochen wurde
      void btConnectErrorSig( int errnr );                        //! Signal, wenn es Fehler beim Verbinden gab
      void btDataRecivedSig();                                    //! Signal, wenn Daten kamen
      void btPairingPinRequestSig(void);                          //! Signal, wenn Pairing gefordert wird

  };
}

#endif // BTDEVICE_HPP
