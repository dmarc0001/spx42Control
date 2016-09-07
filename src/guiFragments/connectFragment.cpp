#include "connectForm.hpp"
#include "ui_connectFragment.h"

namespace spx42
{
  int ConnectFragment::counter = 0;
  int ConnectFragment::count = 0;

  ConnectFragment::ConnectFragment(QWidget *parent, Logger *logger) :
    QWidget(parent),
    ui(new Ui::ConnectForm),
    parentWidget(parent),
    lg(logger)
  {
    lg->debug( QString("ConnectForm::ConnectForm <%1> count: <%2>").arg(++counter, 4, 10, QChar('0')).arg(++count, 4, 10, QChar('0')) );
    ui->setupUi(parent);
    connect(ui->connectButton, &QPushButton::clicked, this, &ConnectFragment::connectButtonSlot );
  }

  ConnectFragment::~ConnectFragment()
  {
    lg->debug( QString("ConnectForm::~ConnectForm <%1> count: <%2>").arg(counter, 4, 10, QChar('0')).arg(--count, 4, 10, QChar('0')) );
    delete ui;
  }

  void ConnectFragment::connectButtonSlot(void)
  {
    lg->debug("ConnectForm::connectButtonSlot -> connect button clicked.");
  }
}
