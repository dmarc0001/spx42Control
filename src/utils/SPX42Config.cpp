#include "SPX42Config.hpp"

namespace spx
{
  /**
   * @brief Standartkonstruktor
   */
  SPX42Config::SPX42Config() :
    QObject(Q_NULLPTR),
    sendSignals( true )
  {
    reset();
    //connect( &spxLicense, &SPX42License::licenseChangedPrivateSig, this, &SPX42Config::licenseChangedPrivateSlot );
    //
    // DECO Presets füllen
    //
    decoPresets.insert( static_cast<int>(DecompressionPreset::DECO_KEY_V_CONSERVATIVE), ProjectConst::DECO_VAL_V_CONSERVATIVE );
    decoPresets.insert( static_cast<int>(DecompressionPreset::DECO_KEY_CONSERVATIVE), ProjectConst::DECO_VAL_CONSERVATIVE );
    decoPresets.insert( static_cast<int>(DecompressionPreset::DECO_KEY_MODERATE), ProjectConst::DECO_VAL_MODERATE );
    decoPresets.insert( static_cast<int>(DecompressionPreset::DECO_KEY_AGRESSIVE), ProjectConst::DECO_VAL_AGRESSIVE );
    decoPresets.insert( static_cast<int>(DecompressionPreset::DECO_KEY_V_AGRESSIVE), ProjectConst::DECO_VAL_V_AGRESSIVE );
    decoPresets.insert( static_cast<int>(DecompressionPreset::DECO_KEY_CUSTOM), ProjectConst::DECO_VAL_CUSTOM );
  }

  /**
   * @brief Gibt die aktuelle SPX42 Lizenz zurück
   * @return Lizenz
   */
  SPX42License SPX42Config::getLicense()
  {
    return( spxLicense );
  }

  /**
   * @brief Setze die neue SPX42 Lizenz
   * @param value neue Lizenz
   */
  void SPX42Config::setLicense(const LicenseType value)
  {
    if( spxLicense.getLicType() != value )
    {
      spxLicense.setLicType( value );
      if( sendSignals )
      {
        emit licenseChangedSig( spxLicense );
      }
    }
  }

  /**
   * @brief Setze oder lösche Individual-Lizenz
   * @param value
   */
  void SPX42Config::setLicense(const IndividualLicense value)
  {
    if( spxLicense.getLicInd() != value )
    {
      spxLicense.setLicInd( value );
      if( sendSignals )
      {
        emit licenseChangedSig( spxLicense );
      }
    }
  }

  /**
   * @brief gib den Lizenznamen als Sting zurück
   * @return
   */
  QString SPX42Config::getLicName() const
  {
    QString individualName = "";
    if( spxLicense.getLicInd() == IndividualLicense::LIC_INDIVIDUAL )
    {
      individualName = tr(" (I)");
    }
    switch( static_cast<int>(spxLicense.getLicType()))
    {
      case static_cast<int>(LicenseType::LIC_NITROX):
        return( tr("NITROX").append(individualName) );
      case static_cast<int>(LicenseType::LIC_NORMOXIX):
        return( tr("NORMOXIC TMX").append(individualName) );
      case static_cast<int>(LicenseType::LIC_FULLTMX):
        return( tr("FULL TMX").append(individualName) );
      case static_cast<int>(LicenseType::LIC_MIL):
        return( tr("MILITARY").append(individualName) );
    }
    return( tr("UNKNOWN") );
  }

  /**
   * @brief Gib ein gesetztes Gas zurück
   * @param num Nummer des Gases (0..7)
   * @return Das Gas
   */
  SPX42Gas &SPX42Config::getGasAt( int num )
  {
    if( num < 0 || num > 7 )
    {
      return( gasList[0] );
    }
    return( gasList[num] );
  }

  /**
   * @brief Setze das Objekt auf einen definierten Grundzustand
   */
  void SPX42Config::reset(void)
  {
    isValid = false;
    spxLicense.setLicType( LicenseType::LIC_NITROX );
    spxLicense.setLicInd( IndividualLicense::LIC_NONE);
    serialNumber = "0000000000";
    for( int i=0; i<8; i++)
    {
      gasList[0].reset();
    }
    currentPreset = DecompressionPreset::DECO_KEY_CONSERVATIVE;
    decoDeepstopsEnabled = DecompressionDeepstops::DEEPSTOPS_ENABLED;
    decoDynamicGradient = DecompressionDynamicGradient::DYNAMIC_GRADIENT_ON;
    displayBrightness = DisplayBrightness::BRIGHT_100;
    displayOrientation = DisplayOrientation::LANDSCAPE;
    unitTemperature = DeviceTemperaturUnit::CELSIUS;
    unitLength = DeviceLenghtUnit::METRIC;
    unitWaterType = DeviceWaterType::FRESHWATER;
    setpointAuto = DeviceSetpointAuto::AUTO_06;
    setpointValue = DeviceSetpointValue::SETPOINT_10;
    individualSensorsOn = DeviceIndividualSensors::SENSORS_ON;
    individualPSCROn = DeviceIndividualPSCR::PSCR_OFF;
    individualSensorCount = DeviceIndividualSensorCount::SENSOR_COUNT_03;
    individualAcustic = DeviceIndividualAcoustic::ACOUSTIC_ON;
    individualLogInterval = DeviceIndividualLogInterval::INTERVAL_20;
    individualTempStick = DeviceIndividualTempstick::TEMPSTICK01;
    emit licenseChangedSig( spxLicense );
  }

  /**
   * @brief Eefrage die gespeicherte SPX42 Seriennummer
   * @return
   */
  QString SPX42Config::getSerialNumber() const
  {
    return( serialNumber );
  }

  /**
   * @brief Setzte die Seriennummer für den SPX42
   * @param serial Seriennummer als String
   */
  void SPX42Config::setSerialNumber(const QString &serial)
  {
    if( serialNumber != serial )
    {
      serialNumber = serial;
      if( sendSignals  )
      {
        emit serialNumberChangedSig( serialNumber );
      }
    }
  }

  /**
   * @brief Setzte die Dekompressions-Voreinstellung
   * @param presetType der Typ des Presets
   * @param low LOW Gradient, wenn Preset == CUSTOM
   * @param high HIGH Gradient, wenn Preset == CUSTOM
   */
  void SPX42Config::setCurrentPreset( DecompressionPreset presetType, qint8 low, qint8 high )
  {
    //
    // wenn CUSTOM gegeben ist, dann die Werte eintragen
    //
    if( presetType == DecompressionPreset::DECO_KEY_CUSTOM )
    {
      //
      // insert macht ein update....
      //
      decoPresets.insert( static_cast<int>(DecompressionPreset::DECO_KEY_CUSTOM), DecoGradient(low,high) );
      if( sendSignals )
      {
        emit decoGradientChangedSig( getCurrentDecoGradientValue() );
      }
      return;
    }
    //
    // oder wenn sich der Typ geändert hat
    //
    if( currentPreset != presetType )
    {
      currentPreset = presetType;
      if( sendSignals )
      {
        emit decoGradientChangedSig( getCurrentDecoGradientValue() );
      }
    }
    // oder nix geändert, nix machen
  }

  /**
   * @brief Setzte die Dekompressions-Voreinstellung
   * @param presetType der Typ des Presets
   * @param dGraient Einstellung für Deko-Gradient
   */
  void SPX42Config::setCurrentPreset( DecompressionPreset presetType, const DecoGradient& dGradient )
  {
    //
    // wenn CUSTOM gegeben ist, dann die Werte eintragen
    //
    if( presetType == DecompressionPreset::DECO_KEY_CUSTOM )
    {
      //
      // insert macht ein update....
      //
      decoPresets.insert( static_cast<int>(DecompressionPreset::DECO_KEY_CUSTOM), dGradient );
      if( sendSignals )
      {
        emit decoGradientChangedSig( getCurrentDecoGradientValue() );
      }
      return;
    }
    //
    // oder wenn sich der Typ geändert hat
    //
    if( currentPreset != presetType )
    {
      currentPreset = presetType;
      if( sendSignals )
      {
        emit decoGradientChangedSig( getCurrentDecoGradientValue() );
      }
    }
    // oder nix geändert, nix machen
  }

  /**
   * @brief Gib das aktuelle Preset zurück
   * @return
   */
  DecompressionPreset SPX42Config::getCurrentDecoGradientPresetType()
  {
    return( currentPreset );
  }

  /**
   * @brief Gib die Gradienten zurück
   * @return Die Gradienten als Objekt DecoGradient
   */
  DecoGradient SPX42Config::getCurrentDecoGradientValue() const
  {
    return( decoPresets.value( static_cast<int>(currentPreset) ));
  }

  /**
   * @brief Gib die Grdienten aür einen bestimmten Typ zurück
   * @param presetType der Tp, für den die Gradienten gesucht werden
   * @return Die Gradienten als Objekt DecoGradient
   */
  DecoGradient SPX42Config::getPresetValues( DecompressionPreset presetType ) const
  {
    //
    // gib einfach das Wertepaar zurück
    //
    return( decoPresets.value( static_cast<int>(presetType) ) );
  }

  /**
   * @brief Ermittle das Preset für angegebene Werte
   * @param Gradient low
   * @param Gradient high
   * @return Preset-Typ
   */
  DecompressionPreset SPX42Config::getPresetForGradient( qint8 low, qint8 high )
  {
    for( DecoGradientHash::iterator it = decoPresets.begin(); it != decoPresets.end(); it++ )
    {
      if( it.value().first == low && it.value().second == high )
      {
        return( static_cast<DecompressionPreset>(it.key()));
      }
    }
    return( DecompressionPreset::DECO_KEY_CUSTOM );
  }

  /**
   * @brief Slot, wenn sich die Lizenz geändert hat
   */
  //void SPX42Config::licenseChangedPrivateSlot(SPX42License &lic)
  //{
  //  if( spxLicense != lic )
  //  {
  //    spxLicense = lic;
  //    if( sendSignals )
  //    {
  //      emit licenseChangedSig( spxLicense );
  //    }
  //  }
  //}

  /**
   * @brief gib aktuelle dynamische Gradienten zurück
   * @return Gradientenobjekt
   */
  DecompressionDynamicGradient SPX42Config::getIsDecoDynamicGradients(void)
  {
    return( decoDynamicGradient );
  }

  /**
   * @brief setzte dynamische Gadienten an/aus
   * @param isDynamicEnabled
   */
  void SPX42Config::setIsDecoDynamicGradients(DecompressionDynamicGradient isDynamicEnabled )
  {
    if( decoDynamicGradient != isDynamicEnabled )
    {
      decoDynamicGradient = isDynamicEnabled;
      if( sendSignals )
      {
        emit decoDynamicGradientStateChangedSig( decoDynamicGradient );
      }
    }
  }

  /**
   * @brief erfrage ob tiefe Dekostops erlaubt sind
   * @return sind Dekostops erlaubt?
   */
  DecompressionDeepstops SPX42Config::getIstDeepstopsEnabled( void )
  {
    return( decoDeepstopsEnabled );
  }

  /**
   * @brief SPX42Config::setIsDeepstopsEnabled
   * @param isEnabled
   */
  void SPX42Config::setIsDeepstopsEnabled(DecompressionDeepstops isEnabled )
  {
    if( decoDeepstopsEnabled != isEnabled )
    {
      decoDeepstopsEnabled = isEnabled;
      if( sendSignals )
      {
        emit decoDeepStopsEnabledSig( decoDeepstopsEnabled );
      }
    }
  }

  /**
   * @brief Gib die einsgestellte Displayhelligkeit zurück
   * @return Helligkeit als Objekt
   */
  DisplayBrightness SPX42Config::getDisplayBrightness( void )
  {
    return( displayBrightness );
  }

  /**
   * @brief Setzte Display Helligkeit
   * @param brightness die Helligkeit
   */
  void SPX42Config::setDisplayBrightness( DisplayBrightness brightness )
  {
    if( displayBrightness != brightness )
    {
      displayBrightness = brightness;
      if( sendSignals )
      {
        emit displayBrightnessChangedSig( displayBrightness );
      }
    }
  }

  /**
   * @brief Gib die Helligkeit als Objekt zurück
   * @return Helligkeit als Objekt
   */
  DisplayOrientation SPX42Config::getDisplayOrientation( void )
  {
    return( displayOrientation );
  }

  void SPX42Config::setDisplayOrientation( DisplayOrientation orientation )
  {
    if( displayOrientation != orientation )
    {
      displayOrientation = orientation;
      if( sendSignals )
      {
        emit displayOrientationChangedSig( displayOrientation );
      }
    }
  }

  /**
   * @brief gib die Temperatureinheit als Objekt zurück
   * @return Temperatureinheit als Objekt
   */
  DeviceTemperaturUnit SPX42Config::getUnitsTemperatur( void )
  {
    return( unitTemperature );
  }

  /**
   * @brief Setzte die Temperatureiheit
   * @param tUnit Wert der Einheit
   */
  void SPX42Config::setUnitsTemperatur( DeviceTemperaturUnit tUnit )
  {
    if( unitTemperature != tUnit )
    {
      unitTemperature = tUnit;
      if( sendSignals )
      {
        emit unitsTemperaturChangedSig( unitTemperature );
      }
    }
  }

  /**
   * @brief Gib die Einheit der Länge zurück
   * @return Einheit der Länge als Objekt
   */
  DeviceLenghtUnit SPX42Config::getUnitsLength( void )
  {
    return( unitLength );
  }

  /**
   * @brief Setzte die Einheit der Längen
   * @param lUnit Einhait als Objekt
   */
  void SPX42Config::setUnitsLength( DeviceLenghtUnit lUnit )
  {
    if( unitLength != lUnit )
    {
      unitLength = lUnit;
      if( sendSignals )
      {
        emit unitsLengtChangedSig( unitLength );
      }
    }
  }

  /**
   * @brief Gib den Wassertyp zurück
   * @return Wassertyp als Objekt
   */
  DeviceWaterType SPX42Config::getUnitsWaterType( void )
  {
    return( unitWaterType );
  }

  /**
   * @brief setzte den Wassertyp
   * @param wUnit Wassertyp als Objekt
   */
  void SPX42Config::setUnitsWaterType( DeviceWaterType wUnit )
  {
    if( unitWaterType != wUnit )
    {
      unitWaterType = wUnit;
      if( sendSignals )
      {
        emit untisWaterTypeChangedSig( unitWaterType );
      }
    }
  }

  /**
   * @brief Gib den Autosetpoint zurück
   * @return Autosetpoint als Obhekt
   */
  DeviceSetpointAuto SPX42Config::getSetpointAuto( void )
  {
    return( setpointAuto );
  }

  /**
   * @brief setzte den Autosetpoint
   * @param aSetpoint autosetpoint als Objekt
   */
  void SPX42Config::setSetpointAuto( DeviceSetpointAuto aSetpoint )
  {
    if( setpointAuto != aSetpoint )
    {
      setpointAuto = aSetpoint;
      if( sendSignals )
      {
        emit setpointAutoChangeSig( setpointAuto );
      }
    }
  }

  /**
   * @brief Gib den Setpoint zurück
   * @return der Setpoint als Obhekt
   */
  DeviceSetpointValue SPX42Config::getSetpointValue( void )
  {
    return( setpointValue );
  }

  /**
   * @brief setzte den Setpoint
   * @param ppo2 als Objekt
   */
  void SPX42Config::setSetpointValue( DeviceSetpointValue ppo2 )
  {
    if( setpointValue != ppo2 )
    {
      setpointValue = ppo2;
      if( sendSignals )
      {
        emit setpointValueChangedSig( setpointValue );
      }
    }
  }

  /**
   * @brief gib die Einstellung Sensoren ein/aus zurück
   * @return Einstellung der Sensoren (ein/aus)
   */
  DeviceIndividualSensors SPX42Config::getIndividualSensorsOn( void )
  {
    return( individualSensorsOn );
  }

  /**
   * @brief setzte die Einstellunge Sensoren (ein/aus)
   * @param onOff Wert der Einstellung
   */
  void SPX42Config::setIndividualSensorsOn( DeviceIndividualSensors onOff )
  {
    if( individualSensorsOn != onOff )
    {
      individualSensorsOn = onOff;
      if( sendSignals )
      {
        emit individualSensorsOnChangedSig( individualSensorsOn );
      }
    }
  }

  /**
   * @brief SPX42Config::getIndividualPscrMode
   * @return Wert der Einstellung als Objekt
   */
  DeviceIndividualPSCR SPX42Config::getIndividualPscrMode( void )
  {
    return( individualPSCROn );
  }

  /**
   * @brief Setzte die Einstellung für Passive Semiclosed Rebreather (ein/aus)
   * @param pscrMode Einstellung als Objekt
   */
  void SPX42Config::setIndividualPscrMode( DeviceIndividualPSCR pscrMode )
  {
    if( individualPSCROn != pscrMode )
    {
      if( sendSignals )
      {
        emit individualPscrModeChangedSig( individualPSCROn );
      }
    }
  }

  /**
   * @brief gib die Anzahl der Sensoren zurück
   * @return Anzahl der Sensoren als Objekt
   */
  DeviceIndividualSensorCount SPX42Config::getIndividualSensorsCount( void )
  {
    return( individualSensorCount );
  }

  /**
   * @brief Setzte die Anzahl der aktiven Sensoren
   * @param sCount Anzahl als Objekt
   */
  void SPX42Config::setIndividualSensorsCount( DeviceIndividualSensorCount sCount )
  {
    if( individualSensorCount != sCount )
    {
      if( sendSignals )
      {
        emit individualSensorsCountChangedSig( individualSensorCount );
      }
    }
  }

  /**
   * @brief Gib die Einstellung für akustische Warnungen zurück
   * @return Einstellung für akustische Warnungen als Objekt
   */
  DeviceIndividualAcoustic SPX42Config::getIndividualAcoustic( void )
  {
    return( individualAcustic );
  }

  /**
   * @brief Setze Einstellung zr akustichen Warnung (ein/aus)
   * @param acoustic Einstellung als Objekt
   */
  void SPX42Config::setIndividualAcoustic( DeviceIndividualAcoustic acoustic )
  {
    if( individualAcustic != acoustic )
    {
      individualAcustic = acoustic;
      if( sendSignals )
      {
        emit individualAcousticChangedSig( individualAcustic );
      }
    }
  }

  /**
   * @brief gib die Einstellung für das Loginterval zurück
   * @return Einstellung als Objekt
   */
  DeviceIndividualLogInterval SPX42Config::getIndividualLogInterval( void )
  {
    return( individualLogInterval );
  }

  /**
   * @brief Setzte Einstellung für das Loginterval
   * @param logInterval Interval als Objjekt
   */
  void SPX42Config::setIndividualLogInterval( DeviceIndividualLogInterval logInterval )
  {
    if( individualLogInterval != logInterval )
    {
      individualLogInterval = logInterval;
      if( sendSignals )
      {
        emit individualLogIntervalChangedSig( individualLogInterval );
      }
    }
  }

  /**
   * @brief Erfrage die Einstellungen für den Tempstick
   * @return Einstellung des Tempsticks
   */
  DeviceIndividualTempstick SPX42Config::getIndividualTempStick( void )
  {
    return( individualTempStick );
  }

  /**
   * @brief Setze Einstellung für Typ des Tempertursticks
   * @param tStick Einstellung als Objekt
   */
  void SPX42Config::setIndividualTempStick( DeviceIndividualTempstick tStick )
  {
    if( individualTempStick != tStick )
    {
      individualTempStick = tStick;
      if( sendSignals )
      {
        emit individualTempstickChangedSig( individualTempStick );
      }
    }
  }

}
