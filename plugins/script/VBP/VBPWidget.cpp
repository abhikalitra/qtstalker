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

#include "VBPWidget.h"
#include "Globals.h"
#include "Command.h"

#include <QtDebug>
#include <QStringList>
#include <QFormLayout>

VBPWidget::VBPWidget (QWidget *p) : PluginWidget (p)
{
  createGeneralPage();
}

void VBPWidget::createGeneralPage ()
{
  QFormLayout *form = new QFormLayout;
  _vbox->addLayout(form);

  // output
  _output = new LineEdit;
  form->addRow("NAME", _output);

  // close input
  _cinput = new LineEdit;
  form->addRow("INPUT_CLOSE", _cinput);

  // cvol input
  _vinput = new LineEdit;
  form->addRow("INPUT_VOLUME", _vinput);

  // up color
  _ucolor = new ColorButton(this, QColor(Qt::red));
  form->addRow("COLOR_UP", _ucolor);

  // down color
  _dcolor = new ColorButton(this, QColor(Qt::red));
  form->addRow("COLOR_DOWN", _dcolor);
}

void VBPWidget::setCommand (QString d)
{
  Command command(d);
  if (command.parm("PLUGIN") != "VBP")
    return;

  _output->setText(command.parm("NAME"));
  _cinput->setText(command.parm("INPUT_CLOSE"));
  _vinput->setText(command.parm("INPUT_VOLUME"));
  _ucolor->setColor(QColor(command.parm("COLOR_UP")));
  _dcolor->setColor(QColor(command.parm("COLOR_DOWN")));
}

void VBPWidget::commands (QStringList &l, int tab)
{
  QStringList cl;
  cl << "PLUGIN=VBP";
  cl << "NAME=" + _output->text();
  cl << "INPUT_CLOSE=" + _cinput->text();
  cl << "INPUT_VOLUME=" + _vinput->text();
  cl << "COLOR_UP=" + _ucolor->color().name();
  cl << "COLOR_DOWN=" + _dcolor->color().name();
  cl << "TAB=" + QString::number(tab);
  l.append(cl.join(","));
}
