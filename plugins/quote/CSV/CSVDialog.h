/*
 *  Qtstalker stock charter
 *
 *  Copyright (C) 2001-2006 Stefan S. Stratigakos
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

#include <qtabdialog.h>
#include <qlineedit.h>
#include <qcombobox.h>
#include <qcheckbox.h>
#include <qdatetimeedit.h>
#include <qstringlist.h>
#include <qspinbox.h>
#include "FileButton.h"
#include "Toolbar.h"

class CSVDialog : public QTabDialog
{
  Q_OBJECT
  
  public:
    CSVDialog (QWidget *, QString &, QString &);
    ~CSVDialog ();
    void createMainPage ();
    void getFiles (QStringList &);
    void setFiles (QStringList &);
    QString getSymbol ();
    void setSymbol (QString &);
    void setStartDate (QDateTime);
    QDateTime getStartDate ();
    void setEndDate (QDateTime);
    QDateTime getEndDate ();
    void setDateRange (bool);
    bool getDateRange ();
    void updateRules ();
    void setRuleName (QString &);
    QString getRuleName ();
    void setReloadInterval (int);
    int getReloadInterval ();
    
  public slots:
    void dateRangeChanged (bool);
    void newRule ();
    void editRule ();
    void deleteRule ();
    void help ();
          
  private:
    FileButton *file;
    QLineEdit *symbol;
    QComboBox *ruleCombo;
    QDateEdit *sdate;
    QDateEdit *edate;
    QCheckBox *dateRange;
    Toolbar *toolbar;
    QString helpFile;
    QString lastPath;
    QSpinBox *minutes;
    QString ruleDir;
};

