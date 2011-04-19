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

#include "BBANDSWidget.h"
#include "Globals.h"
#include "Command.h"
#include "MAType.h"

#include <QtDebug>
#include <QStringList>
#include <QFormLayout>

BBANDSWidget::BBANDSWidget (QWidget *p) : PluginWidget (p)
{
  createGeneralPage();
}

void BBANDSWidget::createGeneralPage ()
{
  QFormLayout *form = new QFormLayout;
  _vbox->addLayout(form);

  // upper
  _upper = new LineEdit;
  form->addRow("NAME_UPPER", _upper);

  // middle
  _middle = new LineEdit;
  form->addRow("NAME_MIDDLE", _middle);

  // lower
  _lower = new LineEdit;
  form->addRow("NAME_LOWER", _lower);

  // input
  _input = new LineEdit;
  form->addRow("INPUT", _input);

  // ma type
  MAType mat;
  _type = new QComboBox;
  _type->addItems(mat.list());
  form->addRow("MA_TYPE", _type);

  // period
  _period = new QSpinBox;
  _period->setRange(1, 9999);
  form->addRow("PERIOD", _period);

  // up dev
  _udev = new QDoubleSpinBox;
  _udev->setRange(0, 9999);
  form->addRow("DEV_UP", _udev);
  
  // down dev
  _ldev = new QDoubleSpinBox;
  _ldev->setRange(0, 9999);
  form->addRow("DEV_DOWN", _ldev);
}

void BBANDSWidget::setCommand (QString d)
{
  Command command(d);
  if (command.parm("PLUGIN") != "BBANDS")
    return;

  _upper->setText(command.parm("NAME_UPPER"));
  _middle->setText(command.parm("NAME_MIDDLE"));
  _lower->setText(command.parm("NAME_LOWER"));
  _input->setText(command.parm("INPUT"));
  _period->setValue(command.parm("PERIOD").toInt());
  _type->setCurrentIndex(_type->findText(command.parm("MA_TYPE")));
  _udev->setValue(command.parm("DEV_UP").toDouble());
  _ldev->setValue(command.parm("DEV_DOWN").toDouble());
}

void BBANDSWidget::commands (QStringList &l, int tab)
{
  QStringList cl;
  cl << "PLUGIN=BBANDS";
  cl << "NAME_UPPER=" + _upper->text();
  cl << "NAME_MIDDLE=" + _middle->text();
  cl << "NAME_LOWER=" + _lower->text();
  cl << "INPUT=" + _input->text();
  cl << "PERIOD=" + _period->text();
  cl << "MA_TYPE=" + _type->currentText();
  cl << "DEV_UP=" + _udev->text();
  cl << "DEV_DOWN=" + _ldev->text();
  cl << "TAB=" + QString::number(tab);
  l.append(cl.join(","));
}
