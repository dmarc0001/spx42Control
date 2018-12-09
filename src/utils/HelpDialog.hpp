#ifndef HELPDIALOG_HPP
#define HELPDIALOG_HPP

#include <QtGui/QDialog>

namespace Ui {
  class HelpDialog;
}

class HelpDialog : public QDialog
{
    Q_OBJECT

  public:
    explicit HelpDialog(QWidget *parent = nullptr);
    ~HelpDialog();

  protected:
    void changeEvent(QEvent *e);

  private:
    Ui::HelpDialog *ui;
};

#endif // HELPDIALOG_HPP
