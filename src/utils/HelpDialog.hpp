#ifndef HELPDIALOG_HPP
#define HELPDIALOG_HPP

#include <QDialog>
#include <QObject>
#include <QUrl>
#include <memory>
#include "config/AppConfigClass.hpp"
#include "logging/Logger.hpp"
#include "spx42/SPX42Defs.hpp"

namespace Ui
{
  //
  // Vorwärtsdeklaration für UI
  //
  class HelpDialog;
}

namespace spx
{
  class HelpDialog : public QDialog
  {
    private:
    Q_OBJECT
    std::unique_ptr< Ui::HelpDialog > ui;
    std::shared_ptr< Logger > lg;
    ApplicationTab currentTab;

    public:
    explicit HelpDialog( ApplicationTab tabIdx, QWidget *parent = nullptr, std::shared_ptr< Logger > logger = nullptr );
    ~HelpDialog();

    protected:
    void changeEvent( QEvent *e );
  };
}
#endif  // HELPDIALOG_HPP
