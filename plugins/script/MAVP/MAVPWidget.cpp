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

#include "MAVPWidget.h"
#include "Globals.h"
#include "Command.h"
#include "MAType.h"

#include <QtDebug>
#include <QStringList>
#include <QFormLayout>

MAVPWidget::MAVPWidget (QWidget *p) : PluginWidget (p)
{
  createGeneralPage();
}

void MAVPWidget::createGeneralPage ()
{
  QFormLayout *form = new QFormLayout;
  _vbox->addLayout(form);

  // output
  _output = new LineEdit;
  form->addRow("NAME", _output);

  // input
  _input = new LineEdit;
  form->addRow("INPUT", _input);

  // input 2
  _input2 = new LineEdit;
  form->addRow("INPUT2", _input2);

  // min
  _min = new QSpinBox;
  _min->setRange(1, 9999);
  form->addRow("PERIOD_MIN", _min);

  // max
  _max = new QSpinBox;
  _max->setRange(1, 9999);
  form->addRow("PERIOD_MAX", _max);

  // ma type
  MAType mat;
  _type = new QComboBox;
  _type->addItems(mat.list());
  form->addRow("MA_TYPE", _type);
}

void MAVPWidget::setCommand (QString d)
{
  Command command(d);
  if (command.parm("PLUGIN") != "MAVP")
    return;

  _output->setText(command.parm("NAME"));
  _input->setText(command.parm("INPUT"));
  _input2->setText(command.parm("INPUT2"));
  _min->setValue(command.parm("PERIOD_MIN").toInt());
  _max->setValue(command.parm("PERIOD_MAX").toInt());
  _type->setCurrentIndex(_type->findText(command.parm("MA_TYPE")));
}

void MAVPWidget::commands (QStringList &l, int tab)
{
  QStringList cl;
  cl << "PLUGIN=MAVP";
  cl << "NAME=" + _output->text();
  cl << "INPUT=" + _input->text();
  cl << "INPUT2=" + _input2->text();
  cl << "PERIOD_MIN=" + _min->text();
  cl << "PERIOD_MAX=" + _max->text();
  cl << "MA_TYPE=" + _type->currentText();
  cl << "TAB=" + QString::number(tab);
  l.append(cl.join(","));
}
