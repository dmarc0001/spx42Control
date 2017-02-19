#include "SPX42Config.hpp"

namespace spx42
{
  /**
   * @brief Standartkonstruktor
   */
  SPX42Config::SPX42Config() :
    QObject(Q_NULLPTR),
    sendSignals( true )
  {
    reset();
    connect( &spxLicense, &SPX42License::licenseChangedPrivateSig, this, &SPX42Config::licenseChangedPrivateSlot );
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

  SPX42License& SPX42Config::getLicense()
  {
    return spxLicense;
  }

  void SPX42Config::setLicense(const SPX42License value)
  {
    if( spxLicense != value )
    {
      spxLicense.setLicType( value.getLicType());
      if( sendSignals )
      {
        emit licenseChangedSig( spxLicense );
      }
    }
  }


  QString SPX42Config::getLicName() const
  {
    switch( static_cast<int>(spxLicense.getLicType()))
    {
      case static_cast<int>(LicenseType::LIC_NITROX):
        return( tr("NITROX") );
      case static_cast<int>(LicenseType::LIC_NORMOXIX):
        return( tr("NORMOXIC TMX") );
      case static_cast<int>(LicenseType::LIC_FULLTMX):
        return( tr("FULL TMX") );
      case static_cast<int>(LicenseType::LIC_MIL):
        return( tr("MILITARY") );
    }
    return( tr("UNKNOWN") );
  }

  SPX42Gas& SPX42Config::getGasAt( int num )
  {
    if( num < 0 || num > 7 )
    {
      return( gasList[0] );
    }
    return( gasList[num]);
  }

  void SPX42Config::reset(void)
  {
    isValid = false;
    spxLicense.setLicType( LicenseType::LIC_NITROX );
    spxLicense.setLicInd( IndividualLicense::LIC_NONE );
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
    emit licenseChangedSig( spxLicense );
  }

  QString SPX42Config::getSerialNumber() const
  {
    return( serialNumber );
  }

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

  DecompressionPreset SPX42Config::getCurrentDecoGradientPresetType()
  {
    return( currentPreset );
  }

  DecoGradient SPX42Config::getCurrentDecoGradientValue()
  {
    return( decoPresets.value( static_cast<int>(currentPreset) ));
  }

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


  void SPX42Config::licenseChangedPrivateSlot(void)
  {
    if( sendSignals )
    {
      emit licenseChangedSig( spxLicense );
    }
  }

  DecompressionDynamicGradient SPX42Config::getIsDecoDynamicGradients(void)
  {
    return( decoDynamicGradient );
  }

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

  DecompressionDeepstops SPX42Config::getIstDeepstopsEnabled( void )
  {
    return( decoDeepstopsEnabled );
  }

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

  DisplayBrightness SPX42Config::getDisplayBrightness( void )
  {
    return( displayBrightness );
  }

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

  DeviceTemperaturUnit SPX42Config::getUnitsTemperatur( void )
  {
    return( unitTemperature );
  }

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

  DeviceLenghtUnit SPX42Config::getUnitsLength( void )
  {
    return( unitLength );
  }

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

  DeviceWaterType SPX42Config::getUnitsWaterType( void )
  {
    return( unitWaterType );
  }

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

  DeviceSetpointAuto SPX42Config::getSetpointAuto( void )
  {
    return( setpointAuto );
  }

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

  DeviceSetpointValue SPX42Config::getSetpointValue( void )
  {
    return( setpointValue );
  }

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

  DeviceIndividualSensors SPX42Config::getIndividualSensorsOn( void )
  {
    return( individualSensorsOn );
  }

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


  DeviceIndividualPSCR SPX42Config::getIndividualPscrMode( void )
  {
    return( individualPSCROn );
  }

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

  DeviceIndividualSensorCount SPX42Config::getIndividualSensorsCount( void )
  {
    return( individualSensorCount );
  }

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

  DeviceIndividualAcoustic SPX42Config::getIndividualAcoustic( void )
  {
    return( individualAcustic );
  }

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

  DeviceIndividualLogInterval SPX42Config::getIndividualLogInterval( void )
  {
    return( individualLogInterval );
  }

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

}
