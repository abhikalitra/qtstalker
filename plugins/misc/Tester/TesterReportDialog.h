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

#ifndef TESTER_REPORT_DIALOG_HPP
#define TESTER_REPORT_DIALOG_HPP

#include <QDialog>
#include <QStringList>
#include <QTextEdit>
#include <QTreeWidget>

#include "TesterReport.h"

class TesterReportDialog : public QDialog
{
  Q_OBJECT
  
  public:
    TesterReportDialog (QString);
    ~TesterReportDialog ();
    void loadSettings ();
    void updateTrades (TesterReport &);
    void updateReport (TesterReport &);
    void saveSettings ();
          
  private:
    QString _name;
    QTreeWidget *_tradeList;
    QTextEdit *_report;
};

#endif

