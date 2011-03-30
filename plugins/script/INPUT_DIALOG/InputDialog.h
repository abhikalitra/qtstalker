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

#ifndef PLUGIN_INPUT_DIALOG_DIALOG_HPP
#define PLUGIN_INPUT_DIALOG_DIALOG_HPP

#include <QDoubleSpinBox>
#include <QSpinBox>
#include <QLineEdit>
#include <QHash>
#include <QStringList>
#include <QComboBox>
#include <QDateTimeEdit>

#include "Setting.h"
#include "Dialog.h"
#include "SymbolButton.h"
#include "FileButton.h"

class InputDialog : public Dialog
{
  Q_OBJECT

  public:
    InputDialog (QWidget *, Command *);
    void createGUI ();
    void newInteger (QString &key, QString &label, QString &value);
    void newDouble (QString &key, QString &label, QString &value);
    void newText (QString &key, QString &label, QString &value);
    void newList (QString &key, QString &label, QString &value);
    void newDate (QString &key, QString &label, QString &value);
    void newSymbol (QString &key, QString &label, QString &value);
    void newFile (QString &key, QString &label, QString &value);

  public slots:
    void done ();

  private:
    QStringList _typeList;
    QHash<QString, QDoubleSpinBox *> _doubles;
    QHash<QString, QSpinBox *> _integers;
    QHash<QString, QLineEdit *> _texts;
    QHash<QString, QComboBox *> _lists;
    QHash<QString, QDateTimeEdit *> _dates;
    QHash<QString, SymbolButton *> _symbols;
    QHash<QString, FileButton *> _files;
};

#endif
