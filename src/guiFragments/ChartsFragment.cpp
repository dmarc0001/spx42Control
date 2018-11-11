#include "ChartsFragment.hpp"
#include "ui_ChartsFragment.h"

namespace spx
{
  ChartsFragment::ChartsFragment( QWidget *parent,
                                  std::shared_ptr< Logger > logger,
                                  std::shared_ptr< SPX42Database > spx42Database,
                                  std::shared_ptr< SPX42Config > spxCfg,
                                  std::shared_ptr< SPX42RemotBtDevice > remSPX42 )
      : QWidget( parent ), IFragmentInterface( logger, spx42Database, spxCfg, remSPX42 ), ui( new Ui::ChartsFragment )
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
    deactivateTab();
  }

  void ChartsFragment::deactivateTab( void )
  {
    disconnect( spxConfig.get(), nullptr, this, nullptr );
  }

  void ChartsFragment::onOnlineStatusChangedSlot( bool )
  {
    // TODO: was machen
  }

  void ChartsFragment::onSocketErrorSlot( QBluetoothSocket::SocketError )
  {
    // TODO: implementieren
  }

  void ChartsFragment::onConfLicChangedSlot()
  {
    lg->debug( QString( "ChartsFragment::onConfLicChangedSlot -> set: %1" )
                   .arg( static_cast< int >( spxConfig->getLicense().getLicType() ) ) );
    ui->tabHeaderLabel->setText(
        QString( tr( "LOGCHARTS SPX42 Serial [%1] Lic: %2" ).arg( spxConfig->getSerialNumber() ).arg( spxConfig->getLicName() ) ) );
  }

  void ChartsFragment::onCloseDatabaseSlot()
  {
    // TODO: implementieren
  }
}
