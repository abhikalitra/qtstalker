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

#include "ULTOSCWidget.h"
#include "Globals.h"
#include "Command.h"

#include <QtDebug>
#include <QStringList>
#include <QFormLayout>

ULTOSCWidget::ULTOSCWidget (QWidget *p) : PluginWidget (p)
{
  createGeneralPage();
}

void ULTOSCWidget::createGeneralPage ()
{
  QFormLayout *form = new QFormLayout;
  _vbox->addLayout(form);

  // output
  _output = new LineEdit;
  form->addRow("NAME", _output);

  // high input
  _hinput = new LineEdit;
  form->addRow("INPUT_HIGH", _hinput);

  // low input
  _linput = new LineEdit;
  form->addRow("INPUT_LOW", _linput);

  // close input
  _cinput = new LineEdit;
  form->addRow("INPUT_CLOSE", _cinput);

  // short period
  _speriod = new QSpinBox;
  _speriod->setRange(1, 9999);
  form->addRow("PERIOD_SHORT", _speriod);

  // med period
  _mperiod = new QSpinBox;
  _mperiod->setRange(1, 9999);
  form->addRow("PERIOD_MED", _mperiod);

  // long period
  _lperiod = new QSpinBox;
  _lperiod->setRange(1, 9999);
  form->addRow("PERIOD_LONG", _lperiod);
}

void ULTOSCWidget::setCommand (QString d)
{
  Command command(d);
  if (command.parm("PLUGIN") != "ULTOSC")
    return;

  _output->setText(command.parm("NAME"));
  _hinput->setText(command.parm("INPUT_HIGH"));
  _linput->setText(command.parm("INPUT_LOW"));
  _cinput->setText(command.parm("INPUT_CLOSE"));
  _speriod->setValue(command.parm("PERIOD_SHORT").toInt());
  _mperiod->setValue(command.parm("PERIOD_MED").toInt());
  _lperiod->setValue(command.parm("PERIOD_LONG").toInt());
}

void ULTOSCWidget::commands (QStringList &l, int tab)
{
  QStringList cl;
  cl << "PLUGIN=ULTOSC";
  cl << "NAME=" + _output->text();
  cl << "INPUT_HIGH=" + _hinput->text();
  cl << "INPUT_LOW=" + _linput->text();
  cl << "INPUT_CLOSE=" + _cinput->text();
  cl << "PERIOD_SHORT=" + _speriod->text();
  cl << "PERIOD_MED=" + _mperiod->text();
  cl << "PERIOD_LONG=" + _lperiod->text();
  cl << "TAB=" + QString::number(tab);
  l.append(cl.join(","));
}
