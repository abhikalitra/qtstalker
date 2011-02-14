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

#ifndef YAHOO_DIALOG_DIALOG_HPP
#define YAHOO_DIALOG_DIALOG_HPP

#include <QDialog>
#include <QPushButton>
#include <QDateTimeEdit>
#include <QCheckBox>
#include <QStringList>
#include <QTextEdit>

#include "Command.h"

class YahooDialog : public QDialog
{
  Q_OBJECT

  signals:
    void signalStop ();

  public:
    YahooDialog (Command *);
    void createGUI ();
    void loadSettings ();
    void saveSettings ();
      
  public slots:
    void selectSymbolsDialog ();
    void setSymbols (QStringList);
    void done ();
    void cancel ();
    void help ();
    void downloadDone ();
    void allSymbolsToggled (bool);
    void autoDateToggled (bool);

  private:
    QDateTimeEdit *_sdate;
    QDateTimeEdit *_edate;
    QCheckBox *_adjustment;
    QCheckBox *_allSymbols;
    QCheckBox *_autoDate;
    QPushButton *_selectSymbolsButton;
    QStringList _symbolList;
    QPushButton *_okButton;
    QPushButton *_cancelButton;
    QString _helpFile;
    Command *_command;
    QTextEdit *_log;
};

#endif
