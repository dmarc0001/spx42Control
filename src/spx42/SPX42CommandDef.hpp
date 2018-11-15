#ifndef SPXCOMMANDDEF_HPP
#define SPXCOMMANDDEF_HPP

namespace spx
{
  namespace SPX42CommandDef
  {
    constexpr char STX{0x02};                            //! Start Char
    constexpr char ETX{0x03};                            //! Ende Char
    constexpr char CR{0x0d};                             //! Cardridge return
    constexpr char LF{0x0a};                             //! Line Feed
    constexpr char SPX_MANUFACTURERS{0x01};              //! Gerätekennung *uploader*
    constexpr char SPX_FACTORY_NUMBER{0x02};             //! ?
    constexpr char SPX_ALIVE{0x03};                      //! Akkuspannung und Alive *uploader*
    constexpr char SPX_APPLICATION_ID{0x04};             //! Firmwareversion des SPX *uploader*
    constexpr char SPX_DEV_IDENTIFIER{0x05};             //! ?
    constexpr char SPX_DEVSOFTVERSION{0x06};             //! ?
    constexpr char SPX_SERIAL_NUMBER{0x07};              //! Seriennummer des SPX *uploader*
    constexpr char SPX_SER1_FROM_SER0{0x08};             //! ?
    constexpr char SPX_DATETIME{0x20};                   //! Datum und Zeit setzen...
    constexpr char SPX_DATE_OSOLETE{0x21};               //! erledigt
    constexpr char SPX_TEMPSTICK{0x22};                  //! ?
    constexpr char SPX_HUD{0x23};                        //! HUD Status senden
    constexpr char SPX_UBAT{0x24};                       //! UBAT anfordern auswerten
    constexpr char SPX_IO_STATUS{0x25};                  //! ?
    constexpr char SPX_CAL_CO2{0x25};                    //! CO2 Kalibrierung
    constexpr char SPX_CAL_CO2_IS_CALIBRATED{0x27};      //! CO2 Flag ob kalibriert wurde
    constexpr char SPX_DEBUG_DEPTH{0x28};                //! ?
    constexpr char SPX_SET_SETUP_DEKO{0x29};             //! Bluetoothkommunikation, setzen der Dekodaten
    constexpr char SPX_SET_SETUP_SETPOINT{0x30};         //! Einstellung des Setpoints
    constexpr char SPX_SET_SETUP_DISPLAYSETTINGS{0x31};  //! Displayeinstellungen setzen
    constexpr char SPX_SET_SETUP_UNITS{0x32};            //! Einheiten setzen
    constexpr char SPX_SET_SETUP_INDIVIDUAL{0x33};       //! Individualsettings
    constexpr char SPX_GET_SETUP_DEKO{0x34};             //! Dekodaten senden
    constexpr char SPX_GET_SETUP_SETPOINT{0x35};         //! Setpointdaten senden
    constexpr char SPX_GET_SETUP_DISPLAYSETTINGS{0x36};  //! Displayeinstellungen senden
    constexpr char SPX_GET_SETUP_UNITS{0x37};            //! Einheiten senden
    constexpr char SPX_GET_SETUP_INDIVIDUAL{0x38};       //! Individualeinstellungen senden
    constexpr char SPX_GET_SETUP_GASLIST{0x39};          //! Gasliste senden
    constexpr char SPX_SET_SETUP_GASLIST{0x40};          //! Gasliste setzen
    constexpr char SPX_GET_LOG_INDEX{0x41};              //! Logbuch index senden
    constexpr char SPX_GET_LOG_NUMBER{0x42};             //! Logbuch senden
    constexpr char SPX_GET_LOG_NUMBER_SE{0x43};          //! Logbuch senden START/ENDE
    constexpr char SPX_GET_DEVICE_OFF{0x44};             //! Flag ob Device aus den Syncmode gegangen ist
    constexpr char SPX_SEND_FILE{0x45};                  //! Sende ein File
    constexpr char SPX_LICENSE_STATE{0x45};              //! Lizenz Status zurückgeben!
    constexpr char SPX_GET_LIC_STATUS{0x46};             //! Lizenzstatus senden
    constexpr char SPX_GET_LOG_NUMBER_DETAIL{0x47};      //! Logdatei senden
    constexpr char SPX_GET_LOG_NUMBER_DETAIL_OK{0x48};   //! Logdatei senden OK/ENDE
    constexpr char SPX_SHUTDOWN{0x66};                   //! Schaltet den SPX nach UPLOAD aus *upload*
    constexpr char SPX_START_YMODEM{0x67};               //! starte YMODEM *upload*
    constexpr char SPX_START_FW_PROGRAMM{0x68};          //! programmieren starten *upload*
    constexpr char SPX_START_EXEC_ADDR{0x69};            //! startet ab angegebener Addr *upload*
    constexpr char SPX_SET_UPDATE_ADDR{0x6a};            //! setzt die Addr, ab wlcher das Update geschrieben wird *upload*
    constexpr char SPX_CHECK_UPLOADED_FILE{0x6b};        //! gibt die übertragene Datei als Hexdump aus *upload*
  }
}

#endif  // SPXCOMMANDDEF_HPP
