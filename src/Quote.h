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

#ifndef QUOTE_HPP
#define QUOTE_HPP

#include <qstring.h>
#include <qcombobox.h>
#include <qtextedit.h>
#include <qtabdialog.h>
#include <qcheckbox.h>
#include "Config.h"
#include "Toolbar.h"

class QuoteDialog : public QTabDialog
{
  Q_OBJECT

  signals:
    void chartUpdated ();
    void message (QString);

  public:
    QuoteDialog (Config *);
    ~QuoteDialog ();
    void enableGUI ();
    void disableGUI ();

  public slots:
    void getQuotes ();
    void ruleChanged (int);
    void downloadComplete ();
    void cancelDownload ();
    void pluginSettings ();
    void printStatusLogMessage (QString);
    void printDataLogMessage (QString);

  private:
    Config *config;
    QComboBox *ruleCombo;
    QString plugin;
    QTextEdit *dataLog;
    QTextEdit *statusLog;
    QPushButton *settingButton;
    Toolbar *toolbar;
    QCheckBox *showData;
};

#endif
