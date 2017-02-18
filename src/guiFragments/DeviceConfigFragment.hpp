#ifndef DEVICECONFIG_HPP
#define DEVICECONFIG_HPP

#include <memory>
#include <QWidget>

#include "../logging/Logger.hpp"
#include "../utils/SPX42Config.hpp"
#include "IFragmentInterface.hpp"



namespace Ui {
  class DeviceConfig;
}

namespace spx42
{
  // DECOMPRESSION
  // sehr konservativ: L:35 H:70
  // konservativ: L30 H:85
  // moderat L:25 H:85
  // agressiv L: 15 H:90
  // sehr agressiv L:10 H:100
  //
  class DeviceConfigFragment : public QWidget, IFragmentInterface
  {
    private:
      Q_OBJECT
      std::unique_ptr<Ui::DeviceConfig> ui;
      bool areSlotsConnected;                                   //! Ich merke mir, ob die Slots verbunden sind
      bool volatile gradentSlotsIgnore;

    public:
      explicit DeviceConfigFragment(QWidget *parent, std::shared_ptr<Logger> logger , std::shared_ptr<SPX42Config> spxCfg); //! Konstruktor
      ~DeviceConfigFragment();
      void initGuiWithConfig( void );                           //! Initialisiere die GUI mit Werten aus der Config

    protected:
      void changeEvent(QEvent *e);

    private:
      void connectSlots( void );
      void disconnectSlots( void );
      void setGradientPresetWithoutCallback( DecompressionPreset preset );

    private slots:
      void licChangedSlot( SPX42License& lic );                 //! Wenn sich die Lizenz ändert
      void decoComboChangedSlot( int index );                   //! ändert sich der Inhalt der Combobox für Dekompressionseinstellungen
      void decoGradientLowChangedSlot( int low );               //! wenn der Gradient LOW geändert wurde
      void decoGradientHighChangedSlot( int high );             //! wenn der Grsadient HIGH geändert wurde

  };
}
#endif // DEVICECONFIG_HPP
