﻿#include "ConnectFragment.hpp"
#include "ui_ConnectFragment.h"

namespace spx42
{
  ConnectFragment::ConnectFragment(QWidget *parent, std::shared_ptr<Logger> logger , std::shared_ptr<SPX42Config> spxCfg) :
    QWidget(parent),
    IFragmentInterface(logger, spxCfg),
    ui(new Ui::ConnectForm)
  {
    lg->debug( "ConnectForm::ConnectForm...");
    ui->setupUi(this);
    ui->connectProgressBar->setVisible(false);
    // FIXME: zur Ansicht einfach Elemente einfügen
    //
    ui->deviceComboBox->addItem("EINS", 1);
    ui->deviceComboBox->addItem("ZWEI", 2);
    ui->deviceComboBox->addItem("DREI", 3);
    ui->deviceComboBox->addItem("VIER", 4);

    connect( ui->connectButton, &QPushButton::clicked, this, &ConnectFragment::connectButtonSlot );
    connect( ui->propertyPushButton, &QPushButton::clicked, this, &ConnectFragment::propertyButtonSlot );
    connect( ui->discoverPushButton, &QPushButton::clicked, this, &ConnectFragment::discoverButtonSlot );
    connect( ui->deviceComboBox, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &ConnectFragment::currentIndexChangedSlot );
  }

  ConnectFragment::~ConnectFragment()
  {
    lg->debug( "ConnectForm::~ConnectForm...");
    //delete ui;
  }

  void ConnectFragment::connectButtonSlot(void)
  {
    lg->debug("ConnectForm::connectButtonSlot -> connect button clicked.");
  }

  void ConnectFragment::propertyButtonSlot( void )
  {
    lg->debug("ConnectForm::propertyButtonSlot -> property button clicked.");
  }

  void ConnectFragment::discoverButtonSlot( void )
  {
    lg->debug("ConnectForm::discoverButtonSlot -> discover button clicked.");
  }

  void ConnectFragment::currentIndexChangedSlot(int index)
  {
    lg->debug( QString("ConnectForm::currentIndexChangedSlot -> index changed to <%1>.").arg(index, 2, 10, QChar('0')));
  }

}
