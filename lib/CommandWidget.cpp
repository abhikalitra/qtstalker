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

#include "CommandWidget.h"
#include "LineEdit.h"
#include "ColorButton.h"
#include "Globals.h"

#include <QtDebug>
#include <QComboBox>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QCheckBox>
#include <QDateTimeEdit>
#include <QSettings>
#include <QToolButton>
#include <QInputDialog>

CommandWidget::CommandWidget (QWidget *p, SettingGroup *sg, Script *script) : QWidget (p)
{
  _settings = sg;
  _script = script;
  _form = new QFormLayout;
  _form->setSpacing(2);
  setLayout(_form);
  _numButtons = 0;
  _setting = 0;

  _buttonGroup = new QButtonGroup(this);
  connect(_buttonGroup, SIGNAL(buttonClicked(int)), this, SLOT(parmDialog(int)));

  setGUI();
}

void CommandWidget::setGUI ()
{
  QList<QString> keys = _settings->keys();

  int loop = 0;
  for (; loop < keys.count(); loop++)
  {
    Setting *set = _settings->get(keys.at(loop));

    switch ((Setting::Type) set->type())
    {
      case Setting::_LIST:
	addList(set);
        break;
      case Setting::_STRING:
	addString(set);
        break;
      case Setting::_COLOR:
	addColor(set);
        break;
      case Setting::_INTEGER:
	addInteger(set);
        break;
      case Setting::_DOUBLE:
	addDouble(set);
        break;
      case Setting::_DATETIME:
	addDateTime(set);
        break;
      case Setting::_BOOL:
	addBool(set);
        break;
      default:
	break;
    }
  }
}

void CommandWidget::addString (Setting *set)
{
  LineEdit *w = new LineEdit(this);
  w->setText(set->getString());
  w->setToolTip(set->tip());
  _widgets.insert(set->key(), (void *) w);

  if (set->inputType() == Setting::_NONE)
  {
    _form->addRow(set->key(), w);
    return;
  }

  QWidget *wi = new QWidget;

  QHBoxLayout *hbox = new QHBoxLayout;
  hbox->setSpacing(2);
  hbox->setMargin(0);
  hbox->addWidget(w);
  wi->setLayout(hbox);

  QToolButton *b = new QToolButton;
  b->setText("...");
  b->setToolTip(tr("Variables..."));
  _buttonGroup->addButton(b, _numButtons);
  hbox->addWidget(b);

  _settingId.insert(_numButtons, set->key());
  _numButtons++;

  _form->addRow(set->key(), wi);
}

void CommandWidget::addBool (Setting *set)
{
  QCheckBox *w = new QCheckBox(this);
  w->setChecked(set->getBool());
  w->setToolTip(set->tip());
  _form->addRow(set->key(), w);
  _widgets.insert(set->key(), (void *) w);
}

void CommandWidget::addInteger (Setting *set)
{
  QSpinBox *w = new QSpinBox(this);
  w->setToolTip(set->tip());
  w->setRange(set->getIntegerLow(), set->getIntegerHigh());
  w->setValue(set->getInteger());
  _form->addRow(set->key(), w);
  _widgets.insert(set->key(), (void *) w);
}

void CommandWidget::addDouble (Setting *set)
{
  QDoubleSpinBox *w = new QDoubleSpinBox(this);
  w->setToolTip(set->tip());
  w->setRange(set->getDoubleLow(), set->getDoubleHigh());
  w->setValue(set->getDouble());
  _form->addRow(set->key(), w);
  _widgets.insert(set->key(), (void *) w);
}

void CommandWidget::addColor (Setting *set)
{
  ColorButton *w = new ColorButton(this, set->getColor());
  w->setToolTip(set->tip());
  _form->addRow(set->key(), w);
  _widgets.insert(set->key(), (void *) w);
}

void CommandWidget::addDateTime (Setting *set)
{
  QDateTimeEdit *w = new QDateTimeEdit(this);
  w->setToolTip(set->tip());
  w->setCalendarPopup(TRUE);
  w->setDisplayFormat("yyyy.MM.dd HH:mm:ss");
  w->setDateTime(set->getDateTime());
  _form->addRow(set->key(), w);
  _widgets.insert(set->key(), (void *) w);
}

void CommandWidget::addList (Setting *set)
{
  QComboBox *w = new QComboBox(this);
  w->setToolTip(set->tip());
  w->addItems(set->getList());
  w->setCurrentIndex(w->findText(set->getString()));
  _form->addRow(set->key(), w);
  _widgets.insert(set->key(), (void *) w);
}

void CommandWidget::save ()
{
  QStringList keys = _settings->keys();
  int loop = 0;
  for (; loop < keys.count(); loop++)
  {
    Setting *v = _settings->get(keys.at(loop));

    switch ((Setting::Type) v->type())
    {
      case Setting::_STRING:
      {
        LineEdit *w = (LineEdit *) _widgets.value(keys.at(loop));
	v->setString(w->text());
	break;
      }
      case Setting::_COLOR:
      {
        ColorButton *w = (ColorButton *) _widgets.value(keys.at(loop));
	v->setColor(w->color());
	break;
      }
      case Setting::_INTEGER:
      {
        QSpinBox *w = (QSpinBox *) _widgets.value(keys.at(loop));
	v->setInteger(w->value());
	break;
      }
      case Setting::_DOUBLE:
      {
        QDoubleSpinBox *w = (QDoubleSpinBox *) _widgets.value(keys.at(loop));
	v->setDouble(w->value());
	break;
      }
      case Setting::_BOOL:
      {
        QCheckBox *w = (QCheckBox *) _widgets.value(keys.at(loop));
	v->setBool(w->isChecked());
	break;
      }
      case Setting::_DATETIME:
      {
        QDateTimeEdit *w = (QDateTimeEdit *) _widgets.value(keys.at(loop));
	v->setDateTime(w->dateTime());
	break;
      }
      case Setting::_LIST:
      {
        QComboBox *w = (QComboBox *) _widgets.value(keys.at(loop));
        v->setString(w->currentText());
        break;
      }
      default:
	break;
    }
  }
}

void CommandWidget::parmDialog (int id)
{
  QString key = _settingId.value(id);
  _setting = _settings->get(key);
  if (! _setting)
    return;

  QStringList keys = scanInputTypes(_setting->inputType());

  QInputDialog *dialog = new QInputDialog(this);
  dialog->setComboBoxItems(keys);
  dialog->setLabelText(tr("Settings"));
  dialog->setWindowTitle(tr("Select Command Setting"));
  connect(dialog, SIGNAL(textValueSelected(const QString &)), this, SLOT(parmDialog2(QString)));
  dialog->show();
}

void CommandWidget::parmDialog2 (QString d)
{
  if (! _setting)
    return;

  _setting->fromString(d);

  switch ((Setting::Type) _setting->type())
  {
    case Setting::_STRING:
    {
      LineEdit *w = (LineEdit *) _widgets.value(_setting->key());
      if (! w)
	break;
      _setting->setString(d);
      w->setText(d);
      break;
    }
    default:
      break;
  }
}

QStringList CommandWidget::scanInputTypes (int type)
{
  QStringList l;
  QStringList runOrder = _script->runOrder();
  int pos = runOrder.indexOf(_settings->stepName());
  if (pos == -1)
    return l;

  int loop = 0;
  for (; loop < pos; loop++)
  {
    SettingGroup *sg = _script->settingGroup(runOrder.at(loop));
    if (! sg)
      continue;

    QStringList keys = sg->keys();
    int loop2 = 0;
    for (; loop2 < keys.count(); loop2++)
    {
      Setting *set = sg->get(keys.at(loop2));
      if (set->outputType() == type)
	l << sg->stepName() + ":" + set->key();
    }
  }

  return l;
}
