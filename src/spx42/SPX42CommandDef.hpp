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
    constexpr char SPX_LICENSE_STATE{0x45};              //! Lizenz Status zurückgeben!
    constexpr char SPX_GET_LOG_NUMBER_DETAIL{0x47};      //! Logdatei senden
    constexpr char SPX_GET_LOG_NUMBER_DETAIL_OK{0x48};   //! Logdatei senden OK/ENDE
    constexpr char SPX_SHUTDOWN{0x66};                   //! Schaltet den SPX nach UPLOAD aus *upload*
    constexpr char SPX_START_YMODEM{0x67};               //! starte YMODEM *upload*
    constexpr char SPX_START_FW_PROGRAMM{0x68};          //! programmieren starten *upload*
    constexpr char SPX_START_EXEC_ADDR{0x69};            //! startet ab angegebener Addr *upload*
    constexpr char SPX_SET_UPDATE_ADDR{0x6a};            //! setzt die Addr, ab wlcher das Update geschrieben wird *upload*
    constexpr char SPX_CHECK_UPLOADED_FILE{0x6b};        //! gibt die übertragene Datei als Hexdump aus *upload*
  }

  namespace SPXCmdParam
  {
    constexpr int ALIVE_POWER{1};                //! Parameter Nummer 1
    constexpr int FIRMWARE_VERSION{1};           //! Parameter 1 Firmwareversion
    constexpr int SERIAL_NUMBER{1};              //! Parameter 1 Seriennummer
    constexpr int LICENSE_STATE{1};              //! Lizenzstatus
    constexpr int LICENSE_INDIVIDUAL{2};         //! Individual
    constexpr int DECO_GF_LOW{1};                //! Gradient für LOW Deco
    constexpr int DECO_GF_HIGH{2};               //! Gradient für HIGH Deco
    constexpr int DECO_DEEPSTOPS{3};             //! Deepstops ja/nein
    constexpr int DECO_DYNGRADIENTS{4};          //! dynamische gradienten
    constexpr int DECO_LASTSTOP{5};              //! letzter Decostop 0 => 3 Meter, 1 => 6 Meter
    constexpr int SETPOINT_AUTO{1};              //! Autosetpoint bei Tiefe...
    constexpr int SETPOINT_VALUE{2};             //! Setpoint in PPO 0..4
    constexpr int DISPLAY_BRIGHTNESS{1};         //! Display Helligkeit
    constexpr int DISPLAY_ORIENT{2};             //! Diaplay Ausrichtung
    constexpr int UNITS_TEMPERATURE{1};          //! Temperatur (Celsius
    constexpr int UNITS_METRIC_OR_IMPERIAL{2};   //! metrisch oder imperial
    constexpr int UNITS_SALT_OR_FRESHWATER{3};   //! Salz- oder Süsswasser
    constexpr int INDIVIDUAL_SENSORSENABLED{1};  //! Sensoren an/aus?
    constexpr int INDIVIDUAL_PASSIVEMODE{2};     //! PSCR Mode
    constexpr int INDIVIDUAL_SENSORCOUNT{3};     //! Anzahl der Sensoren
    constexpr int INDIVIDUAL_SOUND_ONOFF{4};     //! Akustische Warnungen an/aus
    constexpr int INDIVIDUAL_LOGINTERVAL{5};     //! Loginterval 10, 20 oder 30 Sekunden
    constexpr int INDIVIDUAL_TEMPSTICK{6};       //! Tempstickversion
    constexpr int GASLIST_GAS_NUMBER{1};         //! Nummer des Gases
    constexpr int GASLIST_N2{2};                 //! Stickstoffanteil
    constexpr int GASLIST_HE{3};                 //! Heliumanteil
    constexpr int GASLIST_BAILOUT{4};            //! ist das Gas Bailout
    constexpr int GASLIST_DILUENT{5};            //! ist es DIL01, DIL02 oder Nix davon
    constexpr int GASLIST_IS_CURRENT{6};         //! ist es das aktuelle Gas
    constexpr int LOGDIR_CURR_NUMBER{1};         //! Nummer des Log verzeichnis eintrages
    constexpr int LOGDIR_FILENAME{2};            //! Dateiname auf dem Gerät und Angabe der Zeit und des Datums
    constexpr int LOGDIR_MAXNUMBER{3};           //! Höchste Nummer...
  }
}

#endif  // SPXCOMMANDDEF_HPP
