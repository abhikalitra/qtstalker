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

#ifndef SCANNER_HPP
#define SCANNER_HPP

#include <QString>
#include <QDialog>
#include <QDialogButtonBox>
#include <QComboBox>
#include <QPushButton>
#include <QCheckBox>
#include <QSpinBox>
#include <QGroupBox>
#include <QToolButton>
#include <QListWidget>

#include "FormulaEdit.h"
#include "ScannerRule.h"



class Scanner : public QDialog
{
  Q_OBJECT

  signals:
    void signalMessage (QString);
    void signalScanComplete ();
    void signalUpdate ();

  public:
    Scanner (QString);

  public slots:
    void scan ();
    void saveRule ();
    void exitDialog ();
    void loadRule ();
    void getSymbols ();
    void allSymbolsToggled (bool);
    void deleteSymbols ();

  protected:
    QListWidget *fileList;
    FormulaEdit *formula;
    QComboBox *period;
    QSpinBox *bars;
    QDialogButtonBox *buttonBox;    
    QGroupBox *fileBox;
    QToolButton *addFileButton;
    QToolButton *deleteFileButton;
    ScannerRule rule;
};

#endif
