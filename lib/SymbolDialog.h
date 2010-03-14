/*
 *  Qtstalker stock charter
 *
 *  Copyright (C) 2001-2008 Stefan S. Stratigakos
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

#ifndef SYMBOL_DIALOG_HPP
#define SYMBOL_DIALOG_HPP

#include <QStringList>
#include <QString>
#include <QDialog>
#include <QDialogButtonBox>
#include <QTreeWidget>
#include <QToolButton>
#include <QComboBox>
#include <QLineEdit>

#include "Group.h"

class SymbolDialog : public QDialog
{
  Q_OBJECT

  public:
    SymbolDialog (int);
    void setSymbols (QString &ex, QString &ss);
    void getSymbols (Group &);
    void getSymbolSearch (QString &ex, QString &ss);

  public slots:
    void cancelPressed ();
    void deleteButtonPressed ();
    void addButtonPressed ();
    void searchButtonPressed ();

  private:
    int modified;
    QDialogButtonBox *buttonBox;
    QTreeWidget *leftSymbols;
    QTreeWidget *rightSymbols;
    QToolButton *deleteButton;
    QToolButton *addButton;
    QComboBox *exchanges;
    QLineEdit *search;
    QToolButton *searchButton;
};

#endif
