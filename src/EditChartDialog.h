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

#ifndef EDITCHARTDIALOG_HPP
#define EDITCHARTDIALOG_HPP

#include <qstringlist.h>
#include <qdatetimeedit.h>
#include <qtabdialog.h>
#include <qlineedit.h>
#include "ChartDb.h"
#include "Bar.h"
#include "Toolbar.h"

class EditChartDialog : public QTabDialog
{
  Q_OBJECT

  public:
    EditChartDialog (QString);
    ~EditChartDialog ();
    void createDetailsPage ();
    void createDataPage ();
    void clearRecordFields ();

  public slots:
    void deleteRecord ();
    void saveRecord ();
    void slotDateSearch ();
    void saveChart ();
    void textChanged (const QString &);

  private:
    Bar *record;
    QDateTimeEdit *dateSearch;
    ChartDb *db;
    QLineEdit *date;
    QLineEdit *title;
    QLineEdit *open;
    QLineEdit *high;
    QLineEdit *low;
    QLineEdit *close;
    QLineEdit *volume;
    QLineEdit *oi;
    Toolbar *toolbar;
    bool saveRecordFlag;
    bool ignoreSaveRecordFlag;
};

#endif
