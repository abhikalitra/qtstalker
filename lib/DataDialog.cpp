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
#include "DataFactory.h"
#include "DataSetting.h"
#include "SettingString.h"
#include "SettingInteger.h"
#include "SettingDouble.h"
#include "SettingBool.h"
#include "SettingColor.h"
#include "SettingDateTime.h"
#include "SettingFactory.h"
#include "LineEdit.h"
#include "ColorButton.h"
#include "FileButton.h"

#include <QtDebug>

DataDialog::DataDialog (QWidget *p) : Dialog (p)
{
  _keySize = "data_dialog_window_size";
  _keyPos = "data_dialog_window_position";

  createGUI();
}

void DataDialog::createGUI ()
{
  _tabs = new QTabWidget;
  _vbox->insertWidget(1, _tabs);

  _message->hide();
}

void DataDialog::addTab (int pos, QString label)
{
  QFormLayout *form = _formList.value(pos);
  if (form)
    return;

  QWidget *w = new QWidget(this);

  form = new QFormLayout;
  form->setSpacing(5);
  w->setLayout(form);
  _formList.insert(pos, form);

  _tabs->insertTab(pos, w, label);
}

void DataDialog::set (Data *d)
{
  if (d->type() != DataFactory::_DATA_SETTING)
    return;

  int tab = d->get(DataSetting::_TAB)->toInteger();

  Setting *vset = d->get(DataSetting::_VALUE);
  if (! vset)
    return;

  Setting *set = d->get(DataSetting::_LABEL);
  if (! set)
    return;
  QString key = set->toString();

  switch ((SettingFactory::Type) vset->type())
  {
    case SettingFactory::_STRING:
      setText(tab, key, vset->toString(), QString());
      _settings.insert(key, d);
      break;
    case SettingFactory::_INTEGER:
      setInteger(tab, key, vset->toInteger(), 999999, -999999, QString());
      _settings.insert(key, d);
      break;
    case SettingFactory::_DOUBLE:
      setDouble(tab, key, vset->toDouble(), 99999999.0, -99999999.0, QString());
      _settings.insert(key, d);
      break;
    case SettingFactory::_LIST:
      setList(tab, key, vset->toString(), vset->toList(), QString());
      _settings.insert(key, d);
      break;
    case SettingFactory::_COLOR:
      setColor(tab, key, vset->toColor(), QString());
      _settings.insert(key, d);
      break;
    case SettingFactory::_DATETIME:
      setDateTime(tab, key, vset->toDateTime(), QString());
      _settings.insert(key, d);
      break;
    case SettingFactory::_BOOL:
      setBool(tab, key, vset->toBool(), QString());
      _settings.insert(key, d);
      break;
    case SettingFactory::_FILE:
      setFile(tab, key, vset->toList(), QString());
      _settings.insert(key, d);
      break;
    default:
      break;
  }
}

int DataDialog::setText (int tab, QString key, QString text, QString tt)
{
  QFormLayout *form = _formList.value(tab);
  if (! form)
    return 1;

  LineEdit *w = new LineEdit(0);
  w->setText(text);
  w->setToolTip(tt);
  form->addRow(key, w);
  _widgets.insert(key, (void *) w);

  return 0;
}

int DataDialog::setColor (int tab, QString key, QColor c, QString tt)
{
  QFormLayout *form = _formList.value(tab);
  if (! form)
    return 1;

  ColorButton *w = new ColorButton(0, c);
  w->setToolTip(tt);
  form->addRow(key, w);
  _widgets.insert(key, (void *) w);

  return 0;
}

int DataDialog::setInteger (int tab, QString key, int v, int h, int l, QString tt)
{
  QFormLayout *form = _formList.value(tab);
  if (! form)
    return 1;

  QSpinBox *w = new QSpinBox;
  w->setToolTip(tt);
  w->setRange(l, h);
  w->setValue(v);
  form->addRow(key, w);
  _widgets.insert(key, (void *) w);

  return 0;
}

int DataDialog::setDouble (int tab, QString key, double v, double h, double l, QString tt)
{
  QFormLayout *form = _formList.value(tab);
  if (! form)
    return 1;

  QDoubleSpinBox *w = new QDoubleSpinBox;
  w->setToolTip(tt);
  w->setRange(l, h);
  w->setValue(v);
  form->addRow(key, w);
  _widgets.insert(key, (void *) w);

  return 0;
}

int DataDialog::setBool (int tab, QString key, bool v, QString tt)
{
  QFormLayout *form = _formList.value(tab);
  if (! form)
    return 1;

  QCheckBox *w = new QCheckBox;
  w->setChecked(v);
  w->setToolTip(tt);
  form->addRow(key, w);
  _widgets.insert(key, (void *) w);

  return 0;
}

int DataDialog::setList (int tab, QString key, QString v, QStringList l, QString tt)
{
  QFormLayout *form = _formList.value(tab);
  if (! form)
    return 1;

  QComboBox *w = new QComboBox;
  w->setToolTip(tt);
  w->addItems(l);
  w->setCurrentIndex(w->findText(v));
  form->addRow(key, w);
  _widgets.insert(key, (void *) w);

  return 0;
}

int DataDialog::setDateTime (int tab, QString key, QDateTime v, QString tt)
{
  QFormLayout *form = _formList.value(tab);
  if (! form)
    return 1;

  QDateTimeEdit *w = new QDateTimeEdit;
  w->setToolTip(tt);
  w->setCalendarPopup(TRUE);
  w->setDisplayFormat("yyyy.MM.dd HH:mm:ss");
  w->setDateTime(v);
  form->addRow(key, w);
  _widgets.insert(key, (void *) w);

  return 0;
}

int DataDialog::setFile (int tab, QString key, QStringList v, QString tt)
{
  QFormLayout *form = _formList.value(tab);
  if (! form)
    return 1;

  FileButton *w = new FileButton(0);
  w->setFiles(v);
  w->setToolTip(tt);
  form->addRow(key, w);
  _widgets.insert(key, (void *) w);

  return 0;
}

void DataDialog::done ()
{
  if (! _settings.count())
  {
    saveSettings();
    accept();
    return;
  }

  QHashIterator<QString, Data *> it(_settings);
  while (it.hasNext())
  {
    it.next();
    Data *d = it.value();
    Setting *set = d->get(DataSetting::_VALUE);

    switch ((SettingFactory::Type) set->type())
    {
      case SettingFactory::_STRING:
      {
        LineEdit *w = (LineEdit *) _widgets.value(it.key());
        set->set(w->text());
        break;
      }
      case SettingFactory::_COLOR:
      {
        ColorButton *w = (ColorButton *) _widgets.value(it.key());
        set->set(w->color());
        break;
      }
      case SettingFactory::_INTEGER:
      {
        QSpinBox *w = (QSpinBox *) _widgets.value(it.key());
        set->set(w->value());
        break;
      }
      case SettingFactory::_DOUBLE:
      {
        QDoubleSpinBox *w = (QDoubleSpinBox *) _widgets.value(it.key());
        set->set(w->value());
        break;
      }
      case SettingFactory::_BOOL:
      {
        QCheckBox *w = (QCheckBox *) _widgets.value(it.key());
        set->set(w->isChecked());
        break;
      }
      case SettingFactory::_DATETIME:
      {
        QDateTimeEdit *w = (QDateTimeEdit *) _widgets.value(it.key());
        set->set(w->dateTime());
        break;
      }
      case SettingFactory::_LIST:
      {
        QComboBox *w = (QComboBox *) _widgets.value(it.key());
        set->set(w->currentText());
        break;
      }
      case SettingFactory::_FILE:
      {
        FileButton *w = (FileButton *) _widgets.value(it.key());
        set->set(w->files());
        break;
      }
      default:
        break;
    }
  }

  saveSettings();

  accept();
}
