#include "gasForm.hpp"
#include "ui_gasFragment.h"

namespace spx42
{
  int GasFragment::counter = 0;
  int GasFragment::count = 0;

  GasFragment::GasFragment(QWidget *parent, Logger *logger) :
    QWidget(parent),
    ui(new Ui::GasForm),
    parentWidget( parent ),
    lg(logger)
  {
    lg->debug( QString("GasForm::GasForm <%1> count: <%2>").arg(++counter, 4, 10, QChar('0')).arg(++count, 4, 10, QChar('0')) );
    ui->setupUi(parent);
  }

  GasFragment::~GasFragment()
  {
    lg->debug( QString("GasForm::~GasForm <%1> count: <%2>").arg(counter, 4, 10, QChar('0')).arg(--count, 4, 10, QChar('0')) );
    delete ui;
  }
}
