#include "gasFragment.hpp"
#include "ui_gasFragment.h"

namespace spx42
{
  int GasFragment::counter = 0;
  int GasFragment::count = 0;

  GasFragment::GasFragment(QWidget *parent, Logger *logger) :
    QWidget(parent),
    ui(new Ui::GasForm),
    lg(logger)
  {
    lg->debug( "GasForm::GasForm...");
    ui->setupUi(this);
  }

  GasFragment::~GasFragment()
  {
    lg->debug( "GasForm::~GasForm...");
    delete ui;
  }
}
