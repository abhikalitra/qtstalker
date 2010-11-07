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

#include "IndicatorPluginDialog.h"
#include "Globals.h"
#include "ColorButton.h"
#include "IndicatorDataBase.h"
#include "Curve.h"
#include "BarData.h"
#include "FileButton.h"
#include "FunctionMA.h"
#include "Setting.h"

#include <QtDebug>
#include <QLayout>
#include <QComboBox>
#include <QLineEdit>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QCheckBox>

IndicatorPluginDialog::IndicatorPluginDialog (Indicator &i)
{
  _indicator = i;
  
  QString s = "Qtstalker" + g_session + ": ";
  s.append(tr("Edit"));
  s.append(": ");
  s.append(_indicator.name());
  s.append(" ");
  s.append(tr("Indicator"));
  setWindowTitle(s);
}

IndicatorPluginDialog::~IndicatorPluginDialog ()
{
}

void IndicatorPluginDialog::done ()
{
  Setting _settings = _indicator.settings();
  
  QHashIterator<int, Type> it(_types);
  while (it.hasNext())
  {
    it.next();

    switch (it.value())
    {
      case _Color:
      {
        ColorButton *cb = (ColorButton *) _widgets.value(it.key());
        _settings.setData(it.key(), cb->color().name());
        break;
      }
      case _Plot:
      case _Combo:
      case _Input:
      case _MA:
      {
        QComboBox *cb = (QComboBox *) _widgets.value(it.key());
        _settings.setData(it.key(), cb->currentText());
        break;
      }
      case _Text:
      {
        QLineEdit *le = (QLineEdit *) _widgets.value(it.key());
        _settings.setData(it.key(), le->text());
        break;
      }
      case _Int:
      {
        QSpinBox *sb = (QSpinBox *) _widgets.value(it.key());
        _settings.setData(it.key(), sb->text());
        break;
      }
      case _Double:
      {
        QDoubleSpinBox *sb = (QDoubleSpinBox *) _widgets.value(it.key());
        _settings.setData(it.key(), sb->text());
        break;
      }
      case _Check:
      {
        QCheckBox *cb = (QCheckBox *) _widgets.value(it.key());
        _settings.setData(it.key(), (int) cb->isChecked());
        break;
      }
      case _File:
      {
        FileButton *fb = (FileButton *) _widgets.value(it.key());
        _settings.setData(it.key(), fb->getFile());
        break;
      }
      default:
        break;
    }
  }

  _indicator.setSettings(_settings);

  IndicatorDataBase db;
  db.transaction();
  db.setIndicator(_indicator);
  db.commit();

  emit signalDone(_indicator);

  accept();
}

int IndicatorPluginDialog::addTab (QString text)
{
  QWidget *w = new QWidget;

  QFormLayout *form = new QFormLayout;
  form->setSpacing(2);
  form->setMargin(5);
  w->setLayout(form);

  int id = _tabs->addTab(w, text);
  
  _forms.insert(id, form);

  return id;
}

void IndicatorPluginDialog::addColor (int tab, int id, QString label, QString color)
{
  QFormLayout *form = _forms.value(tab);
  if (! form)
    return;

  QColor c(color);
  ColorButton *cb = new ColorButton(this, c);
  cb->setColorButton();

  form->addRow(label, cb);

  _widgets.insert(id, (QWidget *) cb);

  _types.insert(id, _Color);
}

void IndicatorPluginDialog::addPlot (int tab, int id, QString label, QString item)
{
  QFormLayout *form = _forms.value(tab);
  if (! form)
    return;

  Curve fac;
  QStringList l;
  fac.list(l, TRUE);

  QComboBox *cb = new QComboBox;
  cb->addItems(l);
  cb->setCurrentIndex(cb->findText(item, Qt::MatchExactly));
  
  form->addRow(label, cb);

  _widgets.insert(id, (QWidget *) cb);

  _types.insert(id, _Plot);
}

void IndicatorPluginDialog::addText (int tab, int id, QString label, QString item)
{
  QFormLayout *form = _forms.value(tab);
  if (! form)
    return;

  QLineEdit *le = new QLineEdit(item);
  
  form->addRow(label, le);

  _widgets.insert(id, (QWidget *) le);

  _types.insert(id, _Text);
}

void IndicatorPluginDialog::addInt (int tab, int id, QString label, int value, int high, int low)
{
  QFormLayout *form = _forms.value(tab);
  if (! form)
    return;

  QSpinBox *sb = new QSpinBox;
  sb->setRange(low, high);
  sb->setValue(value);
  
  form->addRow(label, sb);

  _widgets.insert(id, (QWidget *) sb);

  _types.insert(id, _Int);
}

void IndicatorPluginDialog::addDouble (int tab, int id, QString label, double value, double high, double low)
{
  QFormLayout *form = _forms.value(tab);
  if (! form)
    return;

  QDoubleSpinBox *sb = new QDoubleSpinBox;
  sb->setRange(low, high);
  sb->setValue(value);

  form->addRow(label, sb);

  _widgets.insert(id, (QWidget *) sb);

  _types.insert(id, _Double);
}

void IndicatorPluginDialog::addCombo (int tab, int id, QString label, QStringList list, QString item)
{
  QFormLayout *form = _forms.value(tab);
  if (! form)
    return;

  QComboBox *cb = new QComboBox;
  cb->addItems(list);
  cb->setCurrentIndex(cb->findText(item, Qt::MatchExactly));

  form->addRow(label, cb);

  _widgets.insert(id, (QWidget *) cb);

  _types.insert(id, _Combo);
}

void IndicatorPluginDialog::addInput (int tab, int id, QString label, QString item)
{
  QFormLayout *form = _forms.value(tab);
  if (! form)
    return;

  QStringList l;
  BarData bd;
  bd.getInputFields(l);

  QComboBox *cb = new QComboBox;
  cb->addItems(l);
  cb->setCurrentIndex(cb->findText(item, Qt::MatchExactly));

  form->addRow(label, cb);

  _widgets.insert(id, (QWidget *) cb);

  _types.insert(id, _Input);
}

void IndicatorPluginDialog::addCheck (int tab, int id, QString label, int value)
{
  QFormLayout *form = _forms.value(tab);
  if (! form)
    return;

  QCheckBox *cb = new QCheckBox;
  cb->setChecked(value);

  form->addRow(label, cb);

  _widgets.insert(id, (QWidget *) cb);

  _types.insert(id, _Check);
}

void IndicatorPluginDialog::addFile (int tab, int id, QString label, QString file)
{
  QFormLayout *form = _forms.value(tab);
  if (! form)
    return;

  FileButton *fb = new FileButton(this, file);

  form->addRow(label, fb);

  _widgets.insert(id, (QWidget *) fb);

  _types.insert(id, _File);
}

void IndicatorPluginDialog::addMA (int tab, int id, QString label, QString type)
{
  QFormLayout *form = _forms.value(tab);
  if (! form)
    return;

  FunctionMA mau;
  QStringList l = mau.list();

  QComboBox *cb = new QComboBox;
  cb->addItems(l);
  cb->setCurrentIndex(cb->findText(type, Qt::MatchExactly));

  form->addRow(label, cb);

  _widgets.insert(id, (QWidget *) cb);

  _types.insert(id, _MA);
}

