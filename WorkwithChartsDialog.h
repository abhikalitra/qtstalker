/*
 *  Qtstalker stock charter
 *
 *  Copyright (c) 2001,2002 Stefan S. Stratigakos
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

#ifndef WORKWITHCHARTSDIALOG_HPP
#define WORKWITHCHARTSDIALOG_HPP

#include <qstring.h>
#include <qtoolbutton.h>
#include "Config.h"
#include "EditDialog.h"

class WorkwithChartsDialog : public EditDialog
{
  Q_OBJECT

  signals:
    void chartOpened (QString);

  public:
    WorkwithChartsDialog (Config *);
    ~WorkwithChartsDialog ();
    void exportChart (QString);
    void setStartDir (QString);

  public slots:
    void deleteChart ();
    void openSymbol ();
    void editChart ();
    void exportSymbol ();
    void exportAll ();

  private:
    QToolButton *openButton;
    QToolButton *editButton;
    QToolButton *deleteButton;
    QToolButton *exportButton;
    QToolButton *exportAllButton;
};

#endif
