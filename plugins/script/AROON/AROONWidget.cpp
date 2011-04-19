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

#include "AROONWidget.h"
#include "Globals.h"
#include "Command.h"

#include <QtDebug>
#include <QStringList>
#include <QFormLayout>

AROONWidget::AROONWidget (QWidget *p) : PluginWidget (p)
{
  createGeneralPage();
}

void AROONWidget::createGeneralPage ()
{
  QFormLayout *form = new QFormLayout;
  _vbox->addLayout(form);

  // upper
  _upper = new LineEdit;
  form->addRow("NAME_UPPER", _upper);

  // lower
  _lower = new LineEdit;
  form->addRow("NAME_LOWER", _lower);

  // high input
  _hinput = new LineEdit;
  form->addRow("INPUT_HIGH", _hinput);

  // low input
  _linput = new LineEdit;
  form->addRow("INPUT_LOW", _linput);

  // period
  _period = new QSpinBox;
  _period->setRange(1, 9999);
  form->addRow("PERIOD", _period);
}

void AROONWidget::setCommand (QString d)
{
  Command command(d);
  if (command.parm("PLUGIN") != "AROON")
    return;

  _upper->setText(command.parm("NAME_UPPER"));
  _lower->setText(command.parm("NAME_LOWER"));
  _hinput->setText(command.parm("INPUT_HIGH"));
  _linput->setText(command.parm("INPUT_LOW"));
  _period->setValue(command.parm("PERIOD").toInt());
}

void AROONWidget::commands (QStringList &l, int tab)
{
  QStringList cl;
  cl << "PLUGIN=AROON";
  cl << "NAME_UPPER=" + _upper->text();
  cl << "NAME_LOWER=" + _lower->text();
  cl << "INPUT_HIGH=" + _hinput->text();
  cl << "INPUT_LOW=" + _linput->text();
  cl << "PERIOD=" + _period->text();
  cl << "TAB=" + QString::number(tab);
  l.append(cl.join(","));
}
