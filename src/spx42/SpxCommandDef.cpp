#include "SPX42CommandDef.hpp"

//
namespace spx
{
  const char SPX42CommandDef::STX{0x02};  //! Start Char
  const char SPX42CommandDef::ETX{0x03};  //! Ende Char
  const char SPX42CommandDef::CR{0x0d};
  const char SPX42CommandDef::LF{0x0a};
  //
  // Kommandonummern
  //
  const char SPX42CommandDef::SPX_MANUFACTURERS{0x01};              //! Gerätekennung *uploader*
  const char SPX42CommandDef::SPX_FACTORY_NUMBER{0x02};             //! ?
  const char SPX42CommandDef::SPX_ALIVE{0x03};                      //! Akkuspannung und Alive *uploader*
  const char SPX42CommandDef::SPX_APPLICATION_ID{0x04};             //! Firmwareversion des SPX *uploader*
  const char SPX42CommandDef::SPX_DEV_IDENTIFIER{0x05};             //! ?
  const char SPX42CommandDef::SPX_DEVSOFTVERSION{0x06};             //! ?
  const char SPX42CommandDef::SPX_SERIAL_NUMBER{0x07};              //! Seriennummer des SPX *uploader*
  const char SPX42CommandDef::SPX_SER1_FROM_SER0{0x08};             //! ?
  const char SPX42CommandDef::SPX_DATETIME{0x20};                   //! Datum und Zeit setzen...
  const char SPX42CommandDef::SPX_DATE_OSOLETE{0x21};               //! erledigt
  const char SPX42CommandDef::SPX_TEMPSTICK{0x22};                  //! ?
  const char SPX42CommandDef::SPX_HUD{0x23};                        //! HUD Status senden
  const char SPX42CommandDef::SPX_UBAT{0x24};                       //! UBAT anfordern auswerten
  const char SPX42CommandDef::SPX_IO_STATUS{0x25};                  //! ?
  const char SPX42CommandDef::SPX_CAL_CO2{0x25};                    //! CO2 Kalibrierung
  const char SPX42CommandDef::SPX_CAL_CO2_IS_CALIBRATED{0x27};      //! CO2 Flag ob kalibriert wurde
  const char SPX42CommandDef::SPX_DEBUG_DEPTH{0x28};                //! ?
  const char SPX42CommandDef::SPX_SET_SETUP_DEKO{0x29};             //! Bluetoothkommunikation, setzen der Dekodaten
  const char SPX42CommandDef::SPX_SET_SETUP_SETPOINT{0x30};         //! Einstellung des Setpoints
  const char SPX42CommandDef::SPX_SET_SETUP_DISPLAYSETTINGS{0x31};  //! Displayeinstellungen setzen
  const char SPX42CommandDef::SPX_SET_SETUP_UNITS{0x32};            //! Einheiten setzen
  const char SPX42CommandDef::SPX_SET_SETUP_INDIVIDUAL{0x33};       //! Individualsettings
  const char SPX42CommandDef::SPX_GET_SETUP_DEKO{0x34};             //! Dekodaten senden
  const char SPX42CommandDef::SPX_GET_SETUP_SETPOINT{0x35};         //! Setpointdaten senden
  const char SPX42CommandDef::SPX_GET_SETUP_DISPLAYSETTINGS{0x36};  //! Displayeinstellungen senden
  const char SPX42CommandDef::SPX_GET_SETUP_UNITS{0x37};            //! Einheiten senden
  const char SPX42CommandDef::SPX_GET_SETUP_INDIVIDUAL{0x38};       //! Individualeinstellungen senden
  const char SPX42CommandDef::SPX_GET_SETUP_GASLIST{0x39};          //! Gasliste senden
  const char SPX42CommandDef::SPX_SET_SETUP_GASLIST{0x40};          //! Gasliste setzen
  const char SPX42CommandDef::SPX_GET_LOG_INDEX{0x41};              //! Logbuch index senden
  const char SPX42CommandDef::SPX_GET_LOG_NUMBER{0x42};             //! Logbuch senden
  const char SPX42CommandDef::SPX_GET_LOG_NUMBER_SE{0x43};          //! Logbuch senden START/ENDE
  const char SPX42CommandDef::SPX_GET_DEVICE_OFF{0x44};             //! Flag ob Device aus den Syncmode gegangen ist
  const char SPX42CommandDef::SPX_SEND_FILE{0x45};                  //! Sende ein File
  const char SPX42CommandDef::SPX_LICENSE_STATE{0x45};              //! Lizenz Status zurückgeben!
  const char SPX42CommandDef::SPX_GET_LIC_STATUS{0x46};             //! Lizenzstatus senden
  const char SPX42CommandDef::SPX_GET_LOG_NUMBER_DETAIL{0x47};      //! Logdatei senden
  const char SPX42CommandDef::SPX_GET_LOG_NUMBER_DETAIL_OK{0x48};   //! Logdatei senden OK/ENDE
  const char SPX42CommandDef::SPX_SHUTDOWN{0x66};                   //! Schaltet den SPX nach UPLOAD aus *upload*
  const char SPX42CommandDef::SPX_START_YMODEM{0x67};               //! starte YMODEM *upload*
  const char SPX42CommandDef::SPX_START_FW_PROGRAMM{0x68};          //! programmieren starten *upload*
  const char SPX42CommandDef::SPX_START_EXEC_ADDR{0x69};            //! startet ab angegebener Addr *upload*
  const char SPX42CommandDef::SPX_SET_UPDATE_ADDR{0x6a};            //! setzt die Addr, ab wlcher das Update geschrieben wird *upload*
  const char SPX42CommandDef::SPX_CHECK_UPLOADED_FILE{0x6b};        //! gibt die übertragene Datei als Hexdump aus *upload*
}
