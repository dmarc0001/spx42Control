#ifndef SPXCOMMANDDEF_HPP
#define SPXCOMMANDDEF_HPP

namespace spx
{
  struct SPX42CommandDef
  {
    //
    // Kommunikation mit SPX42
    // Frame Kapselung
    //
    static const char STX;  //! Beginn einer PDU
    static const char ETX;  //! Ende einer PDU
    static const char CR;   //! CR -> 0d
    static const char LF;   //! CR -> 0a
    //
    // Kommandonummern für SPX-Kommandos
    //
    static const char SPX_MANUFACTURERS;              //! Gerätekennung *uploader*
    static const char SPX_FACTORY_NUMBER;             //! ??
    static const char SPX_ALIVE;                      //! Akkuspannung und Alive *uploader*
    static const char SPX_APPLICATION_ID;             //! Firmwareversion des SPX *uploader*
    static const char SPX_DEV_IDENTIFIER;             //!?
    static const char SPX_DEVSOFTVERSION;             //! ???
    static const char SPX_SERIAL_NUMBER;              //! Seriennummer des SPX *uploader*
    static const char SPX_SER1_FROM_SER0;             //! ???
    static const char SPX_DATETIME;                   //! Datum und Zeit setzen...
    static const char SPX_DATE_OSOLETE;               //! erledigt
    static const char SPX_TEMPSTICK;                  //! TEMPSTICK Version
    static const char SPX_HUD;                        //! HUD Status senden
    static const char SPX_UBAT;                       //! UBAT anfordern auswerten
    static const char SPX_IO_STATUS;                  //! ???
    static const char SPX_CAL_CO2;                    //! CO2 Kalibrierung
    static const char SPX_CAL_CO2_IS_CALIBRATED;      //! CO2 Flag ob kalibriert wurde
    static const char SPX_DEBUG_DEPTH;                //! ??
    static const char SPX_SET_SETUP_DEKO;             //! setzen der Dekodaten
    static const char SPX_SET_SETUP_SETPOINT;         //! Einstellung des Setpoints
    static const char SPX_SET_SETUP_DISPLAYSETTINGS;  //! Displayeinstellungen setzen
    static const char SPX_SET_SETUP_UNITS;            //! Einheiten setzen
    static const char SPX_SET_SETUP_INDIVIDUAL;       //! Individualsettings
    static const char SPX_GET_SETUP_DEKO;             //! Dekodaten senden
    static const char SPX_GET_SETUP_SETPOINT;         //! Setpointdaten senden
    static const char SPX_GET_SETUP_DISPLAYSETTINGS;  //! Displayeinstellungen senden
    static const char SPX_GET_SETUP_UNITS;            //! Einheiten senden
    static const char SPX_GET_SETUP_INDIVIDUAL;       //! Individualeinstellungen senden
    static const char SPX_GET_SETUP_GASLIST;          //! Gasliste senden
    static const char SPX_SET_SETUP_GASLIST;          //! Gasliste setzen
    static const char SPX_GET_LOG_INDEX;              //! Logbuch index senden
    static const char SPX_GET_LOG_NUMBER;             //! Logbuch senden
    static const char SPX_GET_LOG_NUMBER_SE;          //! Logbuch senden START/ENDE
    static const char SPX_GET_DEVICE_OFF;             //! Flag ob Device aus den Syncmode gegangen ist
    static const char SPX_SEND_FILE;                  //! Sende ein File
    static const char SPX_LICENSE_STATE;              //! Lizenz Status zurückgeben!
    static const char SPX_GET_LIC_STATUS;             //! Lizenzstatus senden
    static const char SPX_GET_LOG_NUMBER_DETAIL;      //! Logdatei senden
    static const char SPX_GET_LOG_NUMBER_DETAIL_OK;   //! Logdatei senden OK/ENDE
    static const char SPX_SHUTDOWN;                   //! Schaltet den SPX nach UPLOAD aus *upload*
    static const char SPX_START_YMODEM;               //! starte YMODEM *upload*
    static const char SPX_START_FW_PROGRAMM;          //! programmieren starten *upload*
    static const char SPX_START_EXEC_ADDR;            //! startet ab angegebener Addr *upload*
    static const char SPX_SET_UPDATE_ADDR;            //! setzt die Addr, ab wlcher das Update geschrieben wird *upload*
    static const char SPX_CHECK_UPLOADED_FILE;        //! gibt die übertragene Datei als Hexdump aus *upload*
  };
}

#endif  // SPXCOMMANDDEF_HPP
