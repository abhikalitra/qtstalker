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

#include "HTWidget.h"
#include "Globals.h"
#include "Command.h"
#include "HT.h"

#include <QtDebug>
#include <QStringList>
#include <QFormLayout>

HTWidget::HTWidget (QWidget *p) : PluginWidget (p)
{
  createGeneralPage();
}

void HTWidget::createGeneralPage ()
{
  QFormLayout *form = new QFormLayout;
  _vbox->addLayout(form);

  // output
  _output = new LineEdit;
  form->addRow("NAME", _output);

  // input
  _input = new LineEdit;
  form->addRow("INPUT", _input);

  // method
  HT ht;
  _method = new QComboBox;
  _method->addItems(ht.list());
  form->addRow("METHOD", _method);
}

void HTWidget::setCommand (QString d)
{
  Command command(d);
  if (command.parm("PLUGIN") != "HT")
    return;

  _output->setText(command.parm("NAME"));
  _input->setText(command.parm("INPUT"));
  _method->setCurrentIndex(_method->findText(command.parm("METHOD")));
}

void HTWidget::commands (QStringList &l, int tab)
{
  QStringList cl;
  cl << "PLUGIN=HT";
  cl << "NAME=" + _output->text();
  cl << "INPUT=" + _input->text();
  cl << "METHOD=" + _method->currentText();
  cl << "TAB=" + QString::number(tab);
  l.append(cl.join(","));
}
