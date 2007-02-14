/*
 *  Qtstalker stock charter
 *
 *  Copyright (C) 2001-2007 Stefan S. Stratigakos
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307,
 *  USA.
 */

#include "QuotePlugin.h"
#include "FuturesData.h"
#include "DbPlugin.h"
#include "Setting.h"
#include "Config.h"
#include <qdatetime.h>
#include <qtimer.h>
#include <qtabdialog.h>
#include <qlineedit.h>
#include <qcombobox.h>
#include <qcheckbox.h>
#include <qdatetimeedit.h>
#include <qstringlist.h>
#include <qspinbox.h>
#include <qprogressbar.h>
#include "FileButton.h"

class CSV : public QuotePlugin
{
  Q_OBJECT

  public:
    CSV ();
    virtual ~CSV ();
    void update ();
    void setDelimiter (QString &);
    void getDate (QString &, QString &, Setting &, QDate &);
    void getTime (QString &, QString &);
    bool openDb (QString &, QString &, QString &, bool);
    void loadSettings ();
    void saveSettings ();
    void getRule (Setting &);
    void cancelUpdate ();
    void createMainPage ();
    void updateRules ();

  public slots:
    void parse ();
    void dateRangeChanged (bool);
    void newRule ();
    void editRule ();
    void deleteRule ();
    void help ();

  private:
    Config config;
    QString delim;
    FuturesData fd;
    DbPlugin db;
    QString symbolOveride;
    QString futuresSymbol;
    QString futuresMonth;
    bool cancelFlag;
    QString lastPath;
    QTimer *reloadTimer;
    QString ruleDir;
    FileButton *file;
    QLineEdit *symbol;
    QComboBox *ruleCombo;
    QDateEdit *sdate;
    QDateEdit *edate;
    QCheckBox *dateRange;
    QSpinBox *minutes;
    QProgressBar *progBar;
};

extern "C"
{
  QuotePlugin * createQuotePlugin ();
}
