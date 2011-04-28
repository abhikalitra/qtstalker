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

#include "PluginWidget.h"
#include "Globals.h"
#include "LineEdit.h"
#include "ColorButton.h"

#include <QtDebug>
#include <QComboBox>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QCheckBox>
#include <QDateTimeEdit>

PluginWidget::PluginWidget (QWidget *p) : QWidget (p)
{
  _form = new QFormLayout;
  setLayout(_form);
}

void PluginWidget::setWidgets (Setting *set)
{
  _settings = set;
  QStringList keys = _settings->data("KEYS").split(",", QString::SkipEmptyParts);
  int loop = 0;
  for (; loop < keys.count(); loop++)
  {
    QString s = keys.at(loop) + ":TYPE";
    QString type = _settings->data(s);

    if (type == "TEXT")
    {
      LineEdit *le = new LineEdit(this);
      le->setText(_settings->data(keys.at(loop)));
      _form->addRow(keys.at(loop), le);
      _widgets.insert(keys.at(loop), (void *) le);
    }
    else if (type == "LIST")
    {
      QComboBox *cb = new QComboBox(this);

      QString s = keys.at(loop) + ":LIST";
      cb->addItems(_settings->data(s).split(",", QString::SkipEmptyParts));

      cb->setCurrentIndex(cb->findText(_settings->data(keys.at(loop))));
      _form->addRow(keys.at(loop), cb);
      _widgets.insert(keys.at(loop), (void *) cb);
    }
    else if (type == "COLOR")
    {
      ColorButton *cb = new ColorButton(this, QColor(_settings->data(keys.at(loop))));
      _form->addRow(keys.at(loop), cb);
      _widgets.insert(keys.at(loop), (void *) cb);
    }
    else if (type == "INTEGER")
    {
      QSpinBox *sb = new QSpinBox(this);
      sb->setRange(-999999, 999999);
      sb->setValue(_settings->getInt(keys.at(loop)));
      _form->addRow(keys.at(loop), sb);
      _widgets.insert(keys.at(loop), (void *) sb);
    }
    else if (type == "DOUBLE")
    {
      QDoubleSpinBox *sb = new QDoubleSpinBox(this);
      sb->setRange(-99999999, 99999999);
      sb->setValue(_settings->getDouble(keys.at(loop)));
      _form->addRow(keys.at(loop), sb);
      _widgets.insert(keys.at(loop), (void *) sb);
    }
    else if (type == "CHECK")
    {
      QCheckBox *w = new QCheckBox(this);
      bool b = _settings->getInt(keys.at(loop));
      w->setChecked(b);
      _form->addRow(keys.at(loop), w);
      _widgets.insert(keys.at(loop), (void *) w);
    }
    else if (type == "DATE")
    {
      QDateTimeEdit *w = new QDateTimeEdit(this);
      w->setCalendarPopup(TRUE);
      w->setDisplayFormat("yyyy.MM.dd HH:mm:ss");
      w->setDateTime(_settings->dateTime(keys.at(loop)));
      _form->addRow(keys.at(loop), w);
      _widgets.insert(keys.at(loop), (void *) w);
    }
  }
}

void PluginWidget::save ()
{
  QStringList keys = _settings->data("KEYS").split(",", QString::SkipEmptyParts);
  int loop = 0;
  for (; loop < keys.count(); loop++)
  {
    QString s = keys.at(loop) + ":TYPE";
    QString type = _settings->data(s);

    if (type == "TEXT")
    {
      LineEdit *w = (LineEdit *) _widgets.value(keys.at(loop));
      _settings->setData(keys.at(loop), w->text());
    }
    else if (type == "LIST")
    {
      QComboBox *w = (QComboBox *) _widgets.value(keys.at(loop));
      _settings->setData(keys.at(loop), w->currentText());
    }
    else if (type == "COLOR")
    {
      ColorButton *w = (ColorButton *) _widgets.value(keys.at(loop));
      _settings->setData(keys.at(loop), w->color().name());
    }
    else if (type == "INTEGER")
    {
      QSpinBox *w = (QSpinBox *) _widgets.value(keys.at(loop));
      _settings->setData(keys.at(loop), w->text());
    }
    else if (type == "DOUBLE")
    {
      QDoubleSpinBox *w = (QDoubleSpinBox *) _widgets.value(keys.at(loop));
      _settings->setData(keys.at(loop), w->text());
    }
    else if (type == "CHECK")
    {
      QCheckBox *w = (QCheckBox *) _widgets.value(keys.at(loop));
      int d = w->isChecked();
      _settings->setData(keys.at(loop), d);
    }
    else if (type == "DATE")
    {
      QDateTimeEdit *w = (QDateTimeEdit *) _widgets.value(keys.at(loop));
      _settings->setData(keys.at(loop), w->dateTime());
    }
  }
}
