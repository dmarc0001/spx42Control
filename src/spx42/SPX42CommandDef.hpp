#ifndef SPXCOMMANDDEF_HPP
#define SPXCOMMANDDEF_HPP

#include <QByteArray>

namespace spx
{
  namespace SPX42CommandDef
  {
    //! Start Char Datagramm
    constexpr char STX{0x02};
    //! Ende Char Datagramm
    constexpr char ETX{0x03};
    //! Cardridge return
    constexpr char CR{0x0d};
    //! Line Feed
    constexpr char LF{0x0a};
    //! Gerätekennung *uploader*
    constexpr char SPX_MANUFACTURERS{0x01};
    //! ?
    constexpr char SPX_FACTORY_NUMBER{0x02};
    //! Akkuspannung und Alive *uploader*
    constexpr char SPX_ALIVE{0x03};
    //! Firmwareversion des SPX *uploader*
    constexpr char SPX_APPLICATION_ID{0x04};
    //! ?
    constexpr char SPX_DEV_IDENTIFIER{0x05};
    //! ?
    constexpr char SPX_DEVSOFTVERSION{0x06};
    //! Seriennummer des SPX *uploader*
    constexpr char SPX_SERIAL_NUMBER{0x07};
    //! ?
    constexpr char SPX_SER1_FROM_SER0{0x08};
    //! Datum und Zeit setzen...
    constexpr char SPX_DATETIME{0x20};
    //! erledigt
    constexpr char SPX_DATE_OSOLETE{0x21};
    //! ?
    constexpr char SPX_TEMPSTICK{0x22};
    //! HUD Status senden
    constexpr char SPX_HUD{0x23};
    //! UBAT anfordern auswerten
    constexpr char SPX_UBAT{0x24};
    //! ?
    constexpr char SPX_IO_STATUS{0x25};
    //! CO2 Kalibrierung
    constexpr char SPX_CAL_CO2{0x25};
    //! CO2 Flag ob kalibriert wurde
    constexpr char SPX_CAL_CO2_IS_CALIBRATED{0x27};
    //! ?
    constexpr char SPX_DEBUG_DEPTH{0x28};
    //! Bluetoothkommunikation, setzen der Dekodaten
    constexpr char SPX_SET_SETUP_DEKO{0x29};
    //! Einstellung des Setpoints
    constexpr char SPX_SET_SETUP_SETPOINT{0x30};
    //! Displayeinstellungen setzen
    constexpr char SPX_SET_SETUP_DISPLAYSETTINGS{0x31};
    //! Einheiten setzen metrisch/imperial
    constexpr char SPX_SET_SETUP_UNITS{0x32};
    //! Individualsettings
    constexpr char SPX_SET_SETUP_INDIVIDUAL{0x33};
    //! Dekodaten senden
    constexpr char SPX_GET_SETUP_DEKO{0x34};
    //! Setpointdaten senden
    constexpr char SPX_GET_SETUP_SETPOINT{0x35};
    //! Displayeinstellungen senden
    constexpr char SPX_GET_SETUP_DISPLAYSETTINGS{0x36};
    //! Einheiten senden
    constexpr char SPX_GET_SETUP_UNITS{0x37};
    //! Individualeinstellungen senden
    constexpr char SPX_GET_SETUP_INDIVIDUAL{0x38};
    //! Gasliste senden
    constexpr char SPX_GET_SETUP_GASLIST{0x39};
    //! Gasliste setzen
    constexpr char SPX_SET_SETUP_GASLIST{0x40};
    //! Logbuch index senden
    constexpr char SPX_GET_LOG_INDEX{0x41};
    //! Logbuch senden
    constexpr char SPX_GET_LOG_NUMBER{0x42};
    //! Logbuch senden START/ENDE
    constexpr char SPX_GET_LOG_NUMBER_SE{0x43};
    //! Flag ob Device aus den Syncmode gegangen ist
    constexpr char SPX_GET_DEVICE_OFF{0x44};
    //! Lizenz Status zurückgeben!
    constexpr char SPX_LICENSE_STATE{0x45};
    //! Logdatei senden
    constexpr char SPX_GET_LOG_NUMBER_DETAIL{0x47};
    //! Logdatei senden OK/ENDE
    constexpr char SPX_GET_LOG_NUMBER_DETAIL_OK{0x48};
    //! Schaltet den SPX nach UPLOAD aus *upload*
    constexpr char SPX_SHUTDOWN{0x66};
    //! starte YMODEM *upload*
    constexpr char SPX_START_YMODEM{0x67};
    //! programmieren starten *upload*
    constexpr char SPX_START_FW_PROGRAMM{0x68};
    //! startet ab angegebener Addr *upload*
    constexpr char SPX_START_EXEC_ADDR{0x69};
    //! setzt die Addr, ab wlcher das Update geschrieben wird *upload*
    constexpr char SPX_SET_UPDATE_ADDR{0x6a};
    //! gibt die übertragene Datei als Hexdump aus *upload*
    constexpr char SPX_CHECK_UPLOADED_FILE{0x6b};
    //! Logbuch detail senden
    constexpr char SPX_GET_LOG_DETAIL{0x70};
  }  // namespace SPX42CommandDef

  namespace SPXCmdParam
  {
    //! Parameter Nummer 1
    constexpr int ALIVE_POWER{1};
    //! Parameter 1 Firmwareversion
    constexpr int FIRMWARE_VERSION{1};
    //! Parameter 1 Seriennummer
    constexpr int SERIAL_NUMBER{1};
    //! Lizenzstatus
    constexpr int LICENSE_STATE{1};
    //! Individual
    constexpr int LICENSE_INDIVIDUAL{2};
    //! Gradient für LOW Deco
    constexpr int DECO_GF_LOW{1};
    //! Gradient für HIGH Deco
    constexpr int DECO_GF_HIGH{2};
    //! Deepstops ja/nein
    constexpr int DECO_DEEPSTOPS{3};
    //! dynamische gradienten
    constexpr int DECO_DYNGRADIENTS{4};
    //! letzter Decostop 0 => 3 Meter, 1 => 6 Meter
    constexpr int DECO_LASTSTOP{5};
    //! Autosetpoint bei Tiefe...
    constexpr int SETPOINT_AUTO{1};
    //! Setpoint in PPO 0..4
    constexpr int SETPOINT_VALUE{2};
    //! Display Helligkeit
    constexpr int DISPLAY_BRIGHTNESS{1};
    //! Diaplay Ausrichtung
    constexpr int DISPLAY_ORIENT{2};
    //! Temperatur (Celsius
    constexpr int UNITS_TEMPERATURE{1};
    //! metrisch oder imperial
    constexpr int UNITS_METRIC_OR_IMPERIAL{2};
    //! Salz- oder Süsswasser
    constexpr int UNITS_SALT_OR_FRESHWATER{3};
    //! Sensoren an/aus?
    constexpr int INDIVIDUAL_SENSORSENABLED{1};
    //! PSCR Mode
    constexpr int INDIVIDUAL_PASSIVEMODE{2};
    //! Anzahl der Sensoren
    constexpr int INDIVIDUAL_SENSORCOUNT{3};
    //! Akustische Warnungen an/aus
    constexpr int INDIVIDUAL_SOUND_ONOFF{4};
    //! Loginterval 10, 20 oder 30 Sekunden
    constexpr int INDIVIDUAL_LOGINTERVAL{5};
    //! Tempstickversion
    constexpr int INDIVIDUAL_TEMPSTICK{6};
    //! Nummer des Gases
    constexpr int GASLIST_GAS_NUMBER{1};
    //! Stickstoffanteil
    constexpr int GASLIST_N2{2};
    //! Heliumanteil
    constexpr int GASLIST_HE{3};
    //! ist das Gas Bailout
    constexpr int GASLIST_BAILOUT{4};
    //! ist es DIL01, DIL02 oder Nix davon
    constexpr int GASLIST_DILUENT{5};
    //! ist es das aktuelle Gas
    constexpr int GASLIST_IS_CURRENT{6};
    //! Nummer des Log verzeichnis eintrages
    constexpr int LOGDIR_CURR_NUMBER{1};
    //! Dateiname auf dem Gerät und Angabe der Zeit und des Datums
    constexpr int LOGDIR_FILENAME{2};
    //! Höchste Nummer...
    constexpr int LOGDIR_MAXNUMBER{3};
    //! Start oder Ende der Detailübertragung
    constexpr int LOGDETAIL_START_END{1};
    //! Nummer des Datensatzes deren Details gleich kommen
    constexpr int LOGDETAIL_NUMBER{2};
    //! Logdetail eintrag Druck
    constexpr int LOGDETAIL_PRESSURE{0};
    //! Logdetail tauchtiefe
    constexpr int LOGDETAIL_DEPTH{1};
    //! Logdetail TEmperatur
    constexpr int LOGDETAIL_TEMP{2};
    //! Logdetail Ackuspannung
    constexpr int LOGDETAIL_ACKU{3};
    //! Logdetail Sauerstoff Partialdruck
    constexpr int LOGDETAIL_PPO2{5};
    //! Logdetail Sauerstoff Partioaldruck Sensor 1
    constexpr int LOGDETAIL_PPO2_1{13};
    //! Logdetail PPO2 Sensor 2
    constexpr int LOGDETAIL_PPO2_2{14};
    //! Logdetail PPO2 Sensor 3
    constexpr int LOGDETAIL_PPO2_3{15};
    //! Logdetail O2 Setpoint )(*10)
    constexpr int LOGDETAIL_SETPOINT{6};
    //! Logdetail Stickstoff Prozent
    constexpr int LOGDETAIL_N2{16};
    //! Logdetail Helium Prozent
    constexpr int LOGDETAIL_HE{17};
    //! Logdetail Nullzeit bis zur Dekopflicht
    constexpr int LOGDETAIL_ZEROTIME{20};
    //! Logdetail Näschster Zeitschritt
    constexpr int LOGDETAIL_NEXT_STEP{24};
  }  // namespace SPXCmdParam
}  // namespace spx

#endif  // SPXCOMMANDDEF_HPP
