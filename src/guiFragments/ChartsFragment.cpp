#include "ChartsFragment.hpp"
#include "ui_ChartsFragment.h"

namespace spx
{
  ChartsFragment::ChartsFragment( QWidget *parent,
                                  std::shared_ptr< Logger > logger,
                                  std::shared_ptr< SPX42Database > spx42Database,
                                  std::shared_ptr< SPX42Config > spxCfg )
      : QWidget( parent ), IFragmentInterface( logger, spx42Database, spxCfg ), ui( new Ui::ChartsFragment )
  {
    lg->debug( "ChartsFragment::ChartsFragment..." );
    ui->setupUi( this );
    ui->transferProgressBar->setVisible( false );
    onConfLicChangedSlot();
    connect( spxConfig.get(), &SPX42Config::licenseChangedSig, this, &ChartsFragment::onConfLicChangedSlot );
  }

  ChartsFragment::~ChartsFragment()
  {
    lg->debug( "ConnectFragment::~ConnectFragment..." );
  }

  void ChartsFragment::onOnlineStatusChangedSlot( bool isOnline )
  {
    // TODO: was machen
  }

  void ChartsFragment::onConfLicChangedSlot( void )
  {
    lg->debug(
        QString( "ChartsFragment::confLicChangedSlot -> set: %1" ).arg( static_cast< int >( spxConfig->getLicense().getLicType() ) ) );
    ui->tabHeaderLabel->setText(
        QString( tr( "LOGCHARTS SPX42 Serial [%1] Lic: %2" ).arg( spxConfig->getSerialNumber() ).arg( spxConfig->getLicName() ) ) );
  }

  void ChartsFragment::onCloseDatabaseSlot( void )
  {
    // TODO: implementieren
  }
}
