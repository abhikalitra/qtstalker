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

#include "POWidget.h"
#include "Globals.h"
#include "Command.h"
#include "PO.h"
#include "MAType.h"

#include <QtDebug>
#include <QStringList>
#include <QFormLayout>

POWidget::POWidget (QWidget *p) : PluginWidget (p)
{
  createGeneralPage();
}

void POWidget::createGeneralPage ()
{
  QFormLayout *form = new QFormLayout;
  _vbox->addLayout(form);

  // output
  _output = new LineEdit;
  form->addRow("NAME", _output);

  // input
  _input = new LineEdit;
  form->addRow("INPUT", _input);

  // fast period
  _fast = new QSpinBox;
  _fast->setRange(1, 9999);
  form->addRow("PERIOD_FAST", _fast);

  // slow period
  _slow = new QSpinBox;
  _slow->setRange(1, 9999);
  form->addRow("PERIOD_SLOW", _slow);

  // ma type
  MAType mat;
  _type = new QComboBox;
  _type->addItems(mat.list());
  form->addRow("MA_TYPE", _type);

  // method
  PO po;
  _method = new QComboBox;
  _method->addItems(po.list());
  form->addRow("METHOD", _method);
}

void POWidget::setCommand (QString d)
{
  Command command(d);
  if (command.parm("PLUGIN") != "PO")
    return;

  _output->setText(command.parm("NAME"));
  _input->setText(command.parm("INPUT"));
  _fast->setValue(command.parm("PERIOD_FAST").toInt());
  _slow->setValue(command.parm("PERIOD_SLOW").toInt());
  _type->setCurrentIndex(_type->findText(command.parm("MA_TYPE")));
  _method->setCurrentIndex(_type->findText(command.parm("METHOD")));
}

void POWidget::commands (QStringList &l, int tab)
{
  QStringList cl;
  cl << "PLUGIN=PO";
  cl << "NAME=" + _output->text();
  cl << "INPUT=" + _input->text();
  cl << "PERIOD_FAST=" + _fast->text();
  cl << "PERIOD_SLOW=" + _slow->text();
  cl << "MA_TYPE=" + _type->currentText();
  cl << "METHOD=" + _method->currentText();
  cl << "TAB=" + QString::number(tab);
  l.append(cl.join(","));
}
