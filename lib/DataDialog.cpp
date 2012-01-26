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

#include "DataDialog.h"
#include "LineEdit.h"
#include "ColorButton.h"
#include "FileButton.h"
#include "SymbolButton.h"

#include <QtDebug>
#include <QComboBox>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QCheckBox>
#include <QDateTimeEdit>

DataDialog::DataDialog (QWidget *p, QString id, Entity e) : Dialog (p)
{
  _id = id;
  _settings = e;
  _keySize = "data_dialog_window_size";
  _keyPos = "data_dialog_window_position";

  createGUI();
  
  setGUI();
}

void DataDialog::createGUI ()
{
  _tabs = new QTabWidget;
  _vbox->insertWidget(1, _tabs);

  _message->hide();
}

void DataDialog::addTab (int pos)
{
  QFormLayout *form = _formList.value(pos);
  if (form)
    return;

  QWidget *w = new QWidget(this);

  form = new QFormLayout;
  form->setSpacing(5);
  w->setLayout(form);
  _formList.insert(pos, form);

  _tabs->insertTab(pos, w, QString::number(pos));
}

void DataDialog::setGUI ()
{
  QList<QString> keys;
  _settings.dkeys(keys);
  qSort(keys);
  
  int loop = 0;
  for (; loop < keys.size(); loop++)
  {
    Data d;
    _settings.toData(keys.at(loop), d);

    int tab = d.tab();
    QString key = keys.at(loop);
    QString label = d.label();
    if (label.isEmpty())
      continue;

    switch ((TypeData::Key) d.type())
    {
      case TypeData::_STRING:
        setText(tab, key, label, d.toString(), QString());
        break;
      case TypeData::_INTEGER:
        setInteger(tab, key, label, d.toInteger(), 999999, -999999, QString());
        break;
      case TypeData::_DOUBLE:
        setDouble(tab, key, label, d.toDouble(), 99999999.0, -99999999.0, QString());
        break;
      case TypeData::_LIST:
        setList(tab, key, label, d.toString(), d.toList(), QString());
        break;
      case TypeData::_COLOR:
        setColor(tab, key, label, d.toColor(), QString());
        break;
      case TypeData::_DATETIME:
        setDateTime(tab, key, label, d.toDateTime(), QString());
        break;
      case TypeData::_BOOL:
        setBool(tab, key, label, d.toBool(), QString());
        break;
      case TypeData::_FILE:
        setFile(tab, key, label, d.toList(), QString());
        break;
      case TypeData::_SYMBOL:
        setSymbol(tab, key, label, d.toList(), QString());
        break;
      default:
        break;
    }
  }
}

void DataDialog::setSettings (Entity d)
{
  _settings = d;
  setGUI();
}

Entity DataDialog::settings ()
{
  return _settings;
}

int DataDialog::setText (int tab, QString key, QString label, QString text, QString tt)
{
  QFormLayout *form = _formList.value(tab);
  if (! form)
  {
    addTab(tab);
    form = _formList.value(tab);
  }

  LineEdit *w = new LineEdit(this);
  w->setText(text);
  w->setToolTip(tt);
  form->addRow(label, w);
  _widgets.insert(key, (void *) w);

  return 0;
}

int DataDialog::setColor (int tab, QString key, QString label, QColor c, QString tt)
{
  QFormLayout *form = _formList.value(tab);
  if (! form)
  {
    addTab(tab);
    form = _formList.value(tab);
  }

  ColorButton *w = new ColorButton(this, c);
  w->setToolTip(tt);
  form->addRow(label, w);
  _widgets.insert(key, (void *) w);

  return 0;
}

int DataDialog::setInteger (int tab, QString key, QString label, int v, int h, int l, QString tt)
{
  QFormLayout *form = _formList.value(tab);
  if (! form)
  {
    addTab(tab);
    form = _formList.value(tab);
  }

  QSpinBox *w = new QSpinBox;
  w->setToolTip(tt);
  w->setRange(l, h);
  w->setValue(v);
  form->addRow(label, w);
  _widgets.insert(key, (void *) w);

  return 0;
}

int DataDialog::setDouble (int tab, QString key, QString label, double v, double h, double l, QString tt)
{
  QFormLayout *form = _formList.value(tab);
  if (! form)
  {
    addTab(tab);
    form = _formList.value(tab);
  }

  QDoubleSpinBox *w = new QDoubleSpinBox;
  w->setToolTip(tt);
  w->setRange(l, h);
  w->setValue(v);
  form->addRow(label, w);
  _widgets.insert(key, (void *) w);

  return 0;
}

int DataDialog::setBool (int tab, QString key, QString label, bool v, QString tt)
{
  QFormLayout *form = _formList.value(tab);
  if (! form)
  {
    addTab(tab);
    form = _formList.value(tab);
  }

  QCheckBox *w = new QCheckBox;
  w->setChecked(v);
  w->setToolTip(tt);
  form->addRow(label, w);
  _widgets.insert(key, (void *) w);

  return 0;
}

int DataDialog::setList (int tab, QString key, QString label, QString v, QStringList l, QString tt)
{
  QFormLayout *form = _formList.value(tab);
  if (! form)
  {
    addTab(tab);
    form = _formList.value(tab);
  }

  QComboBox *w = new QComboBox;
  w->setToolTip(tt);
  w->addItems(l);
  w->setCurrentIndex(w->findText(v));
  form->addRow(label, w);
  _widgets.insert(key, (void *) w);

  return 0;
}

int DataDialog::setDateTime (int tab, QString key, QString label, QDateTime v, QString tt)
{
  QFormLayout *form = _formList.value(tab);
  if (! form)
  {
    addTab(tab);
    form = _formList.value(tab);
  }

  QDateTimeEdit *w = new QDateTimeEdit;
  w->setToolTip(tt);
  w->setCalendarPopup(TRUE);
  w->setDisplayFormat("yyyy.MM.dd HH:mm:ss");
  w->setDateTime(v);
  form->addRow(label, w);
  _widgets.insert(key, (void *) w);

  return 0;
}

int DataDialog::setFile (int tab, QString key, QString label, QStringList v, QString tt)
{
  QFormLayout *form = _formList.value(tab);
  if (! form)
  {
    addTab(tab);
    form = _formList.value(tab);
  }

  FileButton *w = new FileButton(this);
  w->setFiles(v);
  w->setToolTip(tt);
  form->addRow(label, w);
  _widgets.insert(key, (void *) w);

  return 0;
}

int DataDialog::setSymbol (int tab, QString key, QString label, QStringList v, QString tt)
{
  QFormLayout *form = _formList.value(tab);
  if (! form)
  {
    addTab(tab);
    form = _formList.value(tab);
  }

  SymbolButton *w = new SymbolButton(this);
  w->setSymbols(v);
  w->setToolTip(tt);
  form->addRow(label, w);
  _widgets.insert(key, (void *) w);

  return 0;
}

void DataDialog::done ()
{
  QList<QString> keys;
  _settings.dkeys(keys);
//qDebug() << "DataDialog::done" << keys;
  
  int loop = 0;
  for (; loop < keys.size(); loop++)
  {
    Data d;
    _settings.toData(keys.at(loop), d);
    
    switch ((TypeData::Key) d.type())
    {
      case TypeData::_STRING:
      {
        LineEdit *w = (LineEdit *) _widgets.value(keys.at(loop));
	if (! w)
	  continue;
        d.set(w->text());
	_settings.set(keys.at(loop), d);
        break;
      }
      case TypeData::_COLOR:
      {
        ColorButton *w = (ColorButton *) _widgets.value(keys.at(loop));
	if (! w)
	  continue;
        d.set(w->color());
	_settings.set(keys.at(loop), d);
        break;
      }
      case TypeData::_INTEGER:
      {
        QSpinBox *w = (QSpinBox *) _widgets.value(keys.at(loop));
	if (! w)
	  continue;
        d.set(w->value());
	_settings.set(keys.at(loop), d);
        break;
      }
      case TypeData::_DOUBLE:
      {
        QDoubleSpinBox *w = (QDoubleSpinBox *) _widgets.value(keys.at(loop));
	if (! w)
	  continue;
        d.set(w->value());
	_settings.set(keys.at(loop), d);
        break;
      }
      case TypeData::_BOOL:
      {
        QCheckBox *w = (QCheckBox *) _widgets.value(keys.at(loop));
	if (! w)
	  continue;
        d.set(w->isChecked());
	_settings.set(keys.at(loop), d);
        break;
      }
      case TypeData::_DATETIME:
      {
        QDateTimeEdit *w = (QDateTimeEdit *) _widgets.value(keys.at(loop));
	if (! w)
	  continue;
        d.set(w->dateTime());
	_settings.set(keys.at(loop), d);
        break;
      }
      case TypeData::_LIST:
      {
        QComboBox *w = (QComboBox *) _widgets.value(keys.at(loop));
	if (! w)
	  continue;
        d.set(w->currentText());
	_settings.set(keys.at(loop), d);
        break;
      }
      case TypeData::_FILE:
      {
        FileButton *w = (FileButton *) _widgets.value(keys.at(loop));
	if (! w)
	  continue;
        d.set(w->files());
	_settings.set(keys.at(loop), d);
        break;
      }
      case TypeData::_SYMBOL:
      {
        SymbolButton *w = (SymbolButton *) _widgets.value(keys.at(loop));
	if (! w)
	  continue;
        d.set(w->symbols());
	_settings.set(keys.at(loop), d);
        break;
      }
      default:
        break;
    }
  }

  _saveFlag++;
  
  saveSettings();

  accept();
}

void DataDialog::setTabTitle (int pos, QString label)
{
  _tabs->setTabText(pos, label);
}
