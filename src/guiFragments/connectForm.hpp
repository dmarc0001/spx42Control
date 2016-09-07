﻿#ifndef CONNECTFORM_HPP
#define CONNECTFORM_HPP

#include <QWidget>

#include "../logging/Logger.hpp"

namespace Ui {
  class ConnectForm;
}

namespace spx42
{
  class ConnectFragment : public QWidget
  {
    private:
      Q_OBJECT
      Ui::ConnectForm *ui;
      QWidget *parentWidget;
      Logger *lg;
      static int counter;
      static int count;

    public:
      explicit ConnectFragment(QWidget *parent, Logger *logger );
      ~ConnectFragment();

    private slots:
      void connectButtonSlot(void);

  };
}
#endif // CONNECTFORM_HPP
