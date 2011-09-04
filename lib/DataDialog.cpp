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

#include <QtDebug>

DataDialog::DataDialog (QWidget *p, Data *d) : Dialog (p)
{
  _settings = d;

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

void DataDialog::addTab (QString label)
{
  QWidget *w = new QWidget(this);

  QFormLayout *form = new QFormLayout;
  form->setSpacing(5);
  w->setLayout(form);
  _formList.insert(_tabs->count(), form);
  _tabs->addTab(w, label);
}

int DataDialog::setText (int tab, QString key, QString label, QString text, QString tt)
{
  QFormLayout *form = _formList.value(tab);
  if (! form)
    return 1;

  LineEdit *w = new LineEdit(0);
  w->setText(text);
  w->setToolTip(tt);
  form->addRow(label, w);
  _widgets.insert(key, (void *) w);
  _types.insert(key, _STRING);
  return 0;
}

int DataDialog::setColor (int tab, QString key, QString label, QColor c, QString tt)
{
  QFormLayout *form = _formList.value(tab);
  if (! form)
    return 1;

  ColorButton *w = new ColorButton(0, c);
  w->setToolTip(tt);
  form->addRow(label, w);
  _widgets.insert(key, (void *) w);
  _types.insert(key, _COLOR);

  return 0;
}

int DataDialog::setInteger (int tab, QString key, QString label, int v, int h, int l, QString tt)
{
  QFormLayout *form = _formList.value(tab);
  if (! form)
    return 1;

  QSpinBox *w = new QSpinBox;
  w->setToolTip(tt);
  w->setRange(l, h);
  w->setValue(v);
  form->addRow(label, w);
  _widgets.insert(key, (void *) w);
  _types.insert(key, _INTEGER);

  return 0;
}

int DataDialog::setDouble (int tab, QString key, QString label, double v, double h, double l, QString tt)
{
  QFormLayout *form = _formList.value(tab);
  if (! form)
    return 1;

  QDoubleSpinBox *w = new QDoubleSpinBox;
  w->setToolTip(tt);
  w->setRange(l, h);
  w->setValue(v);
  form->addRow(label, w);
  _widgets.insert(key, (void *) w);
  _types.insert(key, _DOUBLE);

  return 0;
}

int DataDialog::setBool (int tab, QString key, QString label, bool v, QString tt)
{
  QFormLayout *form = _formList.value(tab);
  if (! form)
    return 1;

  QCheckBox *w = new QCheckBox;
  w->setChecked(v);
  w->setToolTip(tt);
  form->addRow(label, w);
  _widgets.insert(key, (void *) w);
  _types.insert(key, _BOOL);

  return 0;
}

int DataDialog::setList (int tab, QString key, QString label, QString v, QStringList l, QString tt)
{
  QFormLayout *form = _formList.value(tab);
  if (! form)
    return 1;

  QComboBox *w = new QComboBox;
  w->setToolTip(tt);
  w->addItems(l);
  w->setCurrentIndex(w->findText(v));
  form->addRow(label, w);
  _widgets.insert(key, (void *) w);
  _types.insert(key, _LIST);

  return 0;
}

int DataDialog::setDateTime (int tab, QString key, QString label, QDateTime v, QString tt)
{
  QFormLayout *form = _formList.value(tab);
  if (! form)
    return 1;

  QDateTimeEdit *w = new QDateTimeEdit;
  w->setToolTip(tt);
  w->setCalendarPopup(TRUE);
  w->setDisplayFormat("yyyy.MM.dd HH:mm:ss");
  w->setDateTime(v);
  form->addRow(label, w);
  _widgets.insert(key, (void *) w);
  _types.insert(key, _DATETIME);

  return 0;
}

int DataDialog::setFile (int tab, QString key, QString label, QStringList v, QString tt)
{
  QFormLayout *form = _formList.value(tab);
  if (! form)
    return 1;

  FileButton *w = new FileButton(0);
  w->setFiles(v);
  w->setToolTip(tt);
  form->addRow(label, w);
  _widgets.insert(key, (void *) w);
  _types.insert(key, _FILE);

  return 0;
}

void DataDialog::done ()
{
  if (! _settings)
  {
    saveSettings();
//  emit signalDone();
    accept();
    return;
  }

  QHashIterator<QString, int> it(_types);
  while (it.hasNext())
  {
    it.next();

    QVariant v = _settings->get(it.key());

    switch ((Type) it.value())
    {
      case _STRING:
      {
        LineEdit *w = (LineEdit *) _widgets.value(it.key());
        v.setValue(w->text());
        break;
      }
      case _COLOR:
      {
        ColorButton *w = (ColorButton *) _widgets.value(it.key());
        v.setValue(w->color().name());
        break;
      }
      case _INTEGER:
      {
        QSpinBox *w = (QSpinBox *) _widgets.value(it.key());
        v.setValue(w->value());
        break;
      }
      case _DOUBLE:
      {
        QDoubleSpinBox *w = (QDoubleSpinBox *) _widgets.value(it.key());
        v.setValue(w->value());
        break;
      }
      case _BOOL:
      {
        QCheckBox *w = (QCheckBox *) _widgets.value(it.key());
        v.setValue(w->isChecked());
        break;
      }
      case _DATETIME:
      {
        QDateTimeEdit *w = (QDateTimeEdit *) _widgets.value(it.key());
        v.setValue(w->dateTime());
        break;
      }
      case _LIST:
      {
        QComboBox *w = (QComboBox *) _widgets.value(it.key());
        v.setValue(w->currentText());
        break;
      }
      case _FILE:
      {
        FileButton *w = (FileButton *) _widgets.value(it.key());
        v.setValue(w->files());
        break;
      }
      default:
        break;
    }

    _settings->set(it.key(), v);
  }

  saveSettings();

//  emit signalDone();

  accept();
}
