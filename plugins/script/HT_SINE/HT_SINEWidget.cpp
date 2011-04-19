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

#include "HT_SINEWidget.h"
#include "Globals.h"
#include "Command.h"

#include <QtDebug>
#include <QStringList>
#include <QFormLayout>

HT_SINEWidget::HT_SINEWidget (QWidget *p) : PluginWidget (p)
{
  createGeneralPage();
}

void HT_SINEWidget::createGeneralPage ()
{
  QFormLayout *form = new QFormLayout;
  _vbox->addLayout(form);

  // sine
  _sine = new LineEdit;
  form->addRow("NAME_SINE", _sine);

  // lead
  _lead = new LineEdit;
  form->addRow("NAME_LEAD", _lead);

  // input
  _input = new LineEdit;
  form->addRow("INPUT", _input);
}

void HT_SINEWidget::setCommand (QString d)
{
  Command command(d);
  if (command.parm("PLUGIN") != "HT_SINE")
    return;

  _sine->setText(command.parm("NAME_SINE"));
  _lead->setText(command.parm("NAME_LEAD"));
  _input->setText(command.parm("INPUT"));
}

void HT_SINEWidget::commands (QStringList &l, int tab)
{
  QStringList cl;
  cl << "PLUGIN=HT_SINE";
  cl << "NAME_SINE=" + _sine->text();
  cl << "NAME_LEAD=" + _lead->text();
  cl << "INPUT=" + _input->text();
  cl << "TAB=" + QString::number(tab);
  l.append(cl.join(","));
}
