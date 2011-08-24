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

#include "CommandDialog.h"
#include "Globals.h"

#include <QtDebug>
#include <QSettings>
#include <QFormLayout>

CommandDialog::CommandDialog (QWidget *p) : Dialog (p)
{
  _settings = 0;

  _keySize = "command_dialog_window_size";
  _keyPos = "command_dialog_window_position";

  QStringList l;
  l << "QtStalker" + g_session + ":" << tr("Insert Command");
  setWindowTitle(l.join(" "));

  createGUI();

  loadSettings();
}

void CommandDialog::createGUI ()
{
  _tabs = new QTabWidget;
  _vbox->insertWidget(1, _tabs);

  _message->hide();
}

void CommandDialog::setWidgets (SettingGroup *settings)
{
  _settings = settings;

  QWidget *base = new QWidget;

  QFormLayout *form = new QFormLayout;
  form->setSpacing(2);
  base->setLayout(form);

  QList<QString> keys = settings->keys();

  int loop = 0;
  for (; loop < keys.count(); loop++)
  {
    Setting *set = settings->get(keys.at(loop));

    switch ((Setting::Type) set->type())
    {
      case Setting::_LIST:
      {
        QComboBox *w = new QComboBox;
        w->setToolTip(set->tip());
        w->addItems(set->getList());
        w->setCurrentIndex(w->findText(set->getString()));
        form->addRow(set->key(), w);
        _comboBox.insert(set->key(), w);
        break;
      }
      case Setting::_STRING:
      {
        LineEdit *w = new LineEdit(0);
        w->setText(set->getString());
        w->setToolTip(set->tip());
        form->addRow(set->key(), w);
        _lineEdit.insert(set->key(), w);
        break;
      }
      case Setting::_COLOR:
      {
        ColorButton *w = new ColorButton(base, set->getColor());
        w->setToolTip(set->tip());
        form->addRow(set->key(), w);
        _colorButton.insert(set->key(), w);
        break;
      }
      case Setting::_INTEGER:
      {
        QSpinBox *w = new QSpinBox;
        w->setToolTip(set->tip());
        w->setRange(set->getIntegerLow(), set->getIntegerHigh());
        w->setValue(set->getInteger());
        form->addRow(set->key(), w);
        _spinBox.insert(set->key(), w);
        break;
      }
      case Setting::_DOUBLE:
      {
        QDoubleSpinBox *w = new QDoubleSpinBox;
        w->setToolTip(set->tip());
        w->setRange(set->getDoubleLow(), set->getDoubleHigh());
        w->setValue(set->getDouble());
        form->addRow(set->key(), w);
        _doubleSpinBox.insert(set->key(), w);
        break;
      }
      case Setting::_DATETIME:
      {
        QDateTimeEdit *w = new QDateTimeEdit;
        w->setToolTip(set->tip());
        w->setCalendarPopup(TRUE);
        w->setDisplayFormat("yyyy.MM.dd HH:mm:ss");
        w->setDateTime(set->getDateTime());
        form->addRow(set->key(), w);
        _dateTimeEdit.insert(set->key(), w);
        break;
      }
      case Setting::_BOOL:
      {
        QCheckBox *w = new QCheckBox;
        w->setChecked(set->getBool());
        w->setToolTip(set->tip());
        form->addRow(set->key(), w);
        _checkBox.insert(set->key(), w);
      }
      case Setting::_FILE:
      {
        FileButton *w = new FileButton(base);
        w->setFiles(set->getFile());
        w->setToolTip(set->tip());
        form->addRow(set->key(), w);
        _fileButton.insert(set->key(), w);
        break;
      }
      default:
	break;
    }
  }

  _tabs->addTab(base, tr("Settings"));
}

void CommandDialog::done ()
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
        LineEdit *w = _lineEdit.value(keys.at(loop));
	v->setString(w->text());
	break;
      }
      case Setting::_COLOR:
      {
        ColorButton *w = _colorButton.value(keys.at(loop));
	v->setColor(w->color());
	break;
      }
      case Setting::_INTEGER:
      {
        QSpinBox *w = _spinBox.value(keys.at(loop));
	v->setInteger(w->value());
	break;
      }
      case Setting::_DOUBLE:
      {
        QDoubleSpinBox *w = _doubleSpinBox.value(keys.at(loop));
	v->setDouble(w->value());
	break;
      }
      case Setting::_BOOL:
      {
        QCheckBox *w = _checkBox.value(keys.at(loop));
	v->setBool(w->isChecked());
	break;
      }
      case Setting::_DATETIME:
      {
        QDateTimeEdit *w = _dateTimeEdit.value(keys.at(loop));
	v->setDateTime(w->dateTime());
	break;
      }
      case Setting::_LIST:
      {
        QComboBox *w = _comboBox.value(keys.at(loop));
        v->setString(w->currentText());
        break;
      }
      case Setting::_FILE:
      {
        FileButton *w = _fileButton.value(keys.at(loop));
        v->setFile(w->files());
        break;
      }
      default:
	break;
    }
  }

  saveSettings();

  accept();
}
