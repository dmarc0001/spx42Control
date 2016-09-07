#ifndef CONNECTFORM_HPP
#define CONNECTFORM_HPP

#include <QWidget>

#include "../logging/Logger.hpp"

namespace Ui {
  class ConnectForm;
}

namespace spx42
{
  class ConnectForm : public QWidget
  {
    private:
      Q_OBJECT
      Ui::ConnectForm *ui;
      QWidget *parentWidget;
      Logger *lg;

    public:
      explicit ConnectForm(QWidget *parent, Logger *logger );
      ~ConnectForm();

    private slots:
      void connectButtonSlot(void);

  };
}
#endif // CONNECTFORM_HPP
