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

#include "HT_PHASORWidget.h"
#include "Globals.h"
#include "Command.h"

#include <QtDebug>
#include <QStringList>
#include <QFormLayout>

HT_PHASORWidget::HT_PHASORWidget (QWidget *p) : PluginWidget (p)
{
  createGeneralPage();
}

void HT_PHASORWidget::createGeneralPage ()
{
  QFormLayout *form = new QFormLayout;
  _vbox->addLayout(form);

  // phase
  _phase = new LineEdit;
  form->addRow("NAME_PHASE", _phase);

  // quad
  _quad = new LineEdit;
  form->addRow("INPUT_QUAD", _quad);

  // input
  _input = new LineEdit;
  form->addRow("INPUT", _input);
}

void HT_PHASORWidget::setCommand (QString d)
{
  Command command(d);
  if (command.parm("PLUGIN") != "HT_PHASOR")
    return;

  _phase->setText(command.parm("NAME_PHASE"));
  _quad->setText(command.parm("NAME_QUAD"));
  _input->setText(command.parm("INPUT"));
}

void HT_PHASORWidget::commands (QStringList &l, int tab)
{
  QStringList cl;
  cl << "PLUGIN=HT_PHASOR";
  cl << "NAME_PHASE=" + _phase->text();
  cl << "NAME_QUAD=" + _quad->text();
  cl << "INPUT=" + _input->text();
  cl << "TAB=" + QString::number(tab);
  l.append(cl.join(","));
}
