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

#ifndef SCANNER_HPP
#define SCANNER_HPP

#include <qstring.h>
#include <qtabdialog.h>
#include <qcombobox.h>
#include <qpushbutton.h>
#include <qcheckbox.h>
#include <qspinbox.h>
#include "Config.h"
#include "FormulaEdit.h"

class Traverse
{
  public:
    Traverse();
    ~Traverse();
    void traverse(QString dirname);
    QStringList getList();

  private:
    QStringList list;
};

class Scanner : public QTabDialog
{
  Q_OBJECT

  signals:
    void exitScanner ();
    void message (QString);

  public:
    Scanner (QString);
    ~Scanner ();

  public slots:
    void scan ();
    void saveRule ();
    void exitDialog ();
    void loadRule ();
    void getSymbols ();
    void allSymbolsToggled (bool);

  protected:
    FormulaEdit *list;
    QComboBox *period;
    QString scannerName;
    Config config;
    QPushButton *fileButton;
    QStringList fileList;
    QCheckBox *allSymbols;
    QSpinBox *bars;
};

#endif
