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

#include "InputDialog.h"
#include "Globals.h"

#include <QtDebug>

InputDialog::InputDialog (QWidget *p, Command *c) : Dialog (p)
{
  _command = c;
  _keySize = "input_dialog_window_size";
  _keyPos = "input_dialog_window_position";

  _typeList << "INTEGER" << "DOUBLE" << "TEXT" << "LIST" << "DATE" << "SYMBOL" << "FILE";

  QStringList l;
  l << "QtStalker" << g_session << ":" << _command->parm("TITLE");
  setWindowTitle(l.join(" "));

  createGUI();

  loadSettings();
}

void InputDialog::createGUI ()
{
  int loop = 1;
  for (; loop < 11; loop++)
  {
    QString key = "INPUT" + QString::number(loop);
    QString s = key + "_LABEL";
    QString label = _command->parm(s);
    if (label.isEmpty())
      continue;

    s = key + "_TYPE";
    QString type = _command->parm(s);
    if (type.isEmpty())
      continue;

    s = key + "_VALUE";
    QString value = _command->parm(s);

    switch (_typeList.indexOf(type))
    {
      case 0: // INTEGER
        newInteger(key, label, value);
        break;
      case 1: // DOUBLE
        newDouble(key, label, value);
        break;
      case 2: // TEXT
        newText(key, label, value);
        break;
      case 3: // LIST
        newList(key, label, value);
        break;
      case 4: // DATE
        newDate(key, label, value);
        break;
      case 5: // SYMBOL
        newSymbol(key, label, value);
        break;
      case 6: // FILE
        newFile(key, label, value);
        break;
      default:
        break;
    }
  }
}

void InputDialog::done ()
{
  int loop = 1;
  for (; loop < 11; loop++)
  {
    QString key = "INPUT" + QString::number(loop);
    QString rkey = "INPUT_DIALOG_INPUT" + QString::number(loop) + "_DATA";

    QString s = key + "_TYPE";
    QString type = _command->parm(s);
    if (type.isEmpty())
      continue;

    switch (_typeList.indexOf(type))
    {
      case 0: // INTEGER
      {
        QSpinBox *sb = _integers.value(key);
        if (sb)
          _command->setReturnData(rkey , sb->text());
        break;
      }
      case 1: // DOUBLE
      {
        QDoubleSpinBox *sb = _doubles.value(key);
        if (sb)
          _command->setReturnData(rkey, sb->text());
        break;
      }
      case 2: // TEXT
      {
        QLineEdit *le = _texts.value(key);
        if (le)
          _command->setReturnData(rkey, le->text());
        break;
      }
      case 3: // LIST
      {
        QComboBox *cb = _lists.value(key);
        if (cb)
          _command->setReturnData(rkey, cb->currentText());
        break;
      }
      case 4: // DATE
      {
        QDateTimeEdit *dt = _dates.value(key);
        if (dt)
          _command->setReturnData(rkey, dt->dateTime().toString(Qt::ISODate));
        break;
      }
      case 5: // SYMBOL
      {
        SymbolButton *sb = _symbols.value(key);
        if (sb)
          _command->setReturnData(rkey, sb->symbols().join(";"));
        break;
      }
      case 6: // FILE
      {
        FileButton *fb = _files.value(key);
        if (fb)
          _command->setReturnData(rkey, fb->files().join(";"));
        break;
      }
      default:
        break;
    }
  }

  saveSettings();

  _command->setReturnCode("0");

  accept();
}

void InputDialog::newInteger (QString &key, QString &label, QString &value)
{
  QSpinBox *sb = new QSpinBox(this);
  sb->setRange(-999999, 999999);
  sb->setValue(value.toInt());
  _form->addRow(label, sb);
  _integers.insert(key, sb);
}

void InputDialog::newDouble (QString &key, QString &label, QString &value)
{
  QDoubleSpinBox *sb = new QDoubleSpinBox(this);
  sb->setRange(-99999999, 99999999);
  sb->setValue(value.toDouble());
  _form->addRow(label, sb);
  _doubles.insert(key, sb);
}

void InputDialog::newText (QString &key, QString &label, QString &value)
{
  QLineEdit *le = new QLineEdit(this);
  le->setText(value);
  _form->addRow(label, le);
  _texts.insert(key, le);
}

void InputDialog::newList (QString &key, QString &label, QString &value)
{
  QComboBox *cb = new QComboBox(this);
  cb->addItems(value.split(";", QString::SkipEmptyParts));
  _form->addRow(label, cb);
  _lists.insert(key, cb);
}

void InputDialog::newDate (QString &key, QString &label, QString &value)
{
  QDateTime dt = QDateTime::fromString(value);
  if (! dt.isValid())
    dt = QDateTime::currentDateTime();

  QDateTimeEdit *dte = new QDateTimeEdit(this);
  dte->setCalendarPopup(TRUE);
  dte->setDisplayFormat("yyyy.MM.dd");
  dte->setDateTime(dt);
  _form->addRow(label, dte);
  _dates.insert(key, dte);
}

void InputDialog::newSymbol (QString &key, QString &label, QString &value)
{
  SymbolButton *sb = new SymbolButton(this);
  sb->setSymbols(value.split(";", QString::SkipEmptyParts));
  _form->addRow(label, sb);
  _symbols.insert(key, sb);
}

void InputDialog::newFile (QString &key, QString &label, QString &value)
{
  FileButton *fb = new FileButton(this);
  fb->setFiles(value.split(";", QString::SkipEmptyParts));
  _form->addRow(label, fb);
  _files.insert(key, fb);
}
