/*
 *  Qtstalker stock charter
 * 
 *  Copyright (C) 2001,2002 Stefan S. Stratigakos
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
#include <qtabwidget.h>
#include "Config.h"
#include "SettingView.h"
#include "ChartDb.h"
#include "EditDialog.h"

class EditChartDialog : public EditDialog
{
  Q_OBJECT

  public:
    EditChartDialog (Config *, QString);
    ~EditChartDialog ();

  public slots:
    void deleteRecord ();
    void saveRecord ();
    void dateChanged (const QDate &);

  private:
    SettingView *detailList;
    SettingView *recordList;
    Setting *details;
    Setting *record;
    QDateEdit *date;
    QString chartPath;
    ChartDb *db;
    QTabWidget *tab;
};

#endif
