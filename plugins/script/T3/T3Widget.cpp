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

#include "T3Widget.h"
#include "Globals.h"
#include "Command.h"

#include <QtDebug>
#include <QStringList>
#include <QFormLayout>

T3Widget::T3Widget (QWidget *p) : PluginWidget (p)
{
  createGeneralPage();
}

void T3Widget::createGeneralPage ()
{
  QFormLayout *form = new QFormLayout;
  _vbox->addLayout(form);

  // output
  _output = new LineEdit;
  form->addRow("NAME", _output);

  // input
  _input = new LineEdit;
  form->addRow("INPUT", _input);

  // period
  _period = new QSpinBox;
  _period->setRange(1, 9999);
  form->addRow("PERIOD", _period);

  // vfactor
  _vfactor = new QDoubleSpinBox;
  _vfactor->setRange(0, 9999);
  form->addRow("VFACTOR", _vfactor);
}

void T3Widget::setCommand (QString d)
{
  Command command(d);
  if (command.parm("PLUGIN") != "T3")
    return;

  _output->setText(command.parm("NAME"));
  _input->setText(command.parm("INPUT"));
  _period->setValue(command.parm("PERIOD").toInt());
  _vfactor->setValue(command.parm("VFACTOR").toDouble());
}

void T3Widget::commands (QStringList &l, int tab)
{
  QStringList cl;
  cl << "PLUGIN=T3";
  cl << "NAME=" + _output->text();
  cl << "INPUT=" + _input->text();
  cl << "PERIOD=" + _period->text();
  cl << "VFACTOR=" + _vfactor->text();
  cl << "TAB=" + QString::number(tab);
  l.append(cl.join(","));
}
