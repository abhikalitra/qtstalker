/*
 *  Qtstalker stock charter
 *
 *  Copyright (C) 2001-2010 Stefan S. Stratigakos
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

#ifndef YAHOO_DIALOG_HPP
#define YAHOO_DIALOG_HPP

#include <QDialog>
#include <QTextEdit>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QDateTimeEdit>
#include <QCheckBox>

#include "YahooThread.h"

class YahooDialog : public QDialog
{
  Q_OBJECT

  public:
    enum Type
    {
      History,
      Info
    };

    YahooDialog ();
    void createMainPage ();
    void loadSettings ();
    void saveSettings ();
      
  public slots:
    void editSymbols ();
    void cancelButton ();
    void downloadDone ();
    void startHistory ();
    void startInfo ();

  private:
    QTextEdit *_log;
    QDialogButtonBox *_buttonBox;
    QPushButton *_symbolsButton;
    QPushButton *_histButton;
    QPushButton *_infoButton;
    QPushButton *_cancelButton;
    QDateTimeEdit *_sdate;
    QDateTimeEdit *_edate;
    QCheckBox *_adjustment;
    YahooThread *_thread;
    Type _type;
    int _runningFlag;
};

#endif
