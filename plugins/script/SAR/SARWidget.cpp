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

#include "SARWidget.h"
#include "Globals.h"
#include "Command.h"

#include <QtDebug>
#include <QStringList>
#include <QFormLayout>

SARWidget::SARWidget (QWidget *p) : PluginWidget (p)
{
  createGeneralPage();
}

void SARWidget::createGeneralPage ()
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

  // initial
  _initial = new QDoubleSpinBox;
  _initial->setRange(0, 9999);
  form->addRow("STEP_INITIAL", _initial);

  // max
  _max = new QDoubleSpinBox;
  _max->setRange(0, 9999);
  form->addRow("STEP_MAX", _max);
}

void SARWidget::setCommand (QString d)
{
  Command command(d);
  if (command.parm("PLUGIN") != "SAR")
    return;

  _output->setText(command.parm("NAME"));
  _hinput->setText(command.parm("INPUT_HIGH"));
  _linput->setText(command.parm("INPUT_LOW"));
  _initial->setValue(command.parm("STEP_INITIAL").toDouble());
  _max->setValue(command.parm("STEP_MAX").toDouble());
}

void SARWidget::commands (QStringList &l, int tab)
{
  QStringList cl;
  cl << "PLUGIN=SAR";
  cl << "NAME=" + _output->text();
  cl << "INPUT_HIGH=" + _hinput->text();
  cl << "INPUT_LOW=" + _linput->text();
  cl << "STEP_INITIAL=" + _initial->text();
  cl << "STEP_MAX=" + _max->text();
  cl << "TAB=" + QString::number(tab);
  l.append(cl.join(","));
}
