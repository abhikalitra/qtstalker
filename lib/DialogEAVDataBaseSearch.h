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

#ifndef DIALOG_EAV_DATA_BASE_SEARCH_HPP
#define DIALOG_EAV_DATA_BASE_SEARCH_HPP

#include <QStringList>
#include <QListWidget>
#include <QComboBox>
#include <QGroupBox>

#include "Dialog.h"
#include "WidgetLineEdit.h"

class DialogEAVDataBaseSearch : public Dialog
{
  Q_OBJECT

  signals:
    void signalDone (QStringList);

  public:
    DialogEAVDataBaseSearch (QWidget *);
    void createGUI ();

  public slots:
    void search ();
    void done ();
//    void searchSelectionChanged ();
//    void symbolSelectionChanged ();
//    void searchButtonPressed ();
//    void addButtonPressed ();
//    void deleteButtonPressed ();

  private:
    QGroupBox *_gbox1;
    QGroupBox *_gbox2;
    QComboBox *_keys1;
    QComboBox *_keys2;
    QComboBox *_op1;
    QComboBox *_op2;
    WidgetLineEdit *_value1;
    WidgetLineEdit *_value2;
    QListWidget *_nameList;
    QPushButton *_searchButton;
};

#endif
