/*
 *  Qtstalker stock charter
 *
 *  Copyright (C) 2001-2004 Stefan S. Stratigakos
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
#include "FileButton.h"

class CSVDialog : public QTabDialog
{
  Q_OBJECT
  
  public:
    CSVDialog ();
    ~CSVDialog ();
    void createMainPage ();
    void createDatePage ();

    QString getType ();
    QStringList getFiles ();
    QString getSymbol ();
    void setFuturesSymbol (QStringList, QString);
    QString getFuturesSymbol ();
    void setFuturesMonth (QStringList, QString);
    QString getFuturesMonth ();
    void setFormat (QStringList, QString);
    QString getFormat ();
    void setDelimiter (QStringList, QString);
    QString getDelimiter ();
    void setDateFormat (QStringList, QString);
    QString getDateFormat ();
    void setStartDate (QDateTime);
    QDateTime getStartDate ();
    void setEndDate (QDateTime);
    QDateTime getEndDate ();
    void setDateRange (bool);
    bool getDateRange ();
    
  public slots:
    void typeChanged (int);
    void dateRangeChanged (bool);
          
  private:
    FileButton *file;
    QLineEdit *symbol;
    QComboBox *type;
    QComboBox *futuresSymbol;
    QComboBox *futuresMonth;
    QComboBox *format;
    QComboBox *delimiter;
    QComboBox *dateFormat;
    QDateEdit *sdate;
    QDateEdit *edate;
    QCheckBox *dateRange;
};

