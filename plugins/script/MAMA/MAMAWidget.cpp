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

#include "MAMAWidget.h"
#include "Globals.h"
#include "Command.h"

#include <QtDebug>
#include <QStringList>
#include <QFormLayout>

MAMAWidget::MAMAWidget (QWidget *p) : PluginWidget (p)
{
  createGeneralPage();
}

void MAMAWidget::createGeneralPage ()
{
  QFormLayout *form = new QFormLayout;
  _vbox->addLayout(form);

  // mama
  _mama = new LineEdit;
  form->addRow("NAME_MAMA", _mama);

  // fama
  _fama = new LineEdit;
  form->addRow("NAME_FAMA", _fama);

  // input
  _input = new LineEdit;
  form->addRow("INPUT", _input);

  // fast
  _fast = new QDoubleSpinBox;
  _fast->setRange(0, 9999);
  form->addRow("LIMIT_FAST", _fast);

  // slow
  _slow = new QDoubleSpinBox;
  _slow->setRange(0, 9999);
  form->addRow("LIMIT_SLOW", _slow);
}

void MAMAWidget::setCommand (QString d)
{
  Command command(d);
  if (command.parm("PLUGIN") != "MAMA")
    return;

  _mama->setText(command.parm("NAME_MAMA"));
  _fama->setText(command.parm("NAME_FAMA"));
  _input->setText(command.parm("INPUT"));
  _fast->setValue(command.parm("LIMIT_FAST").toDouble());
  _slow->setValue(command.parm("LIMIT_SLOW").toDouble());
}

void MAMAWidget::commands (QStringList &l, int tab)
{
  QStringList cl;
  cl << "PLUGIN=MAMA";
  cl << "NAME_MAMA=" + _mama->text();
  cl << "NAME_FAMA=" + _fama->text();
  cl << "INPUT=" + _input->text();
  cl << "LIMIT_FAST=" + _fast->text();
  cl << "LIMIT_SLOW=" + _slow->text();
  cl << "TAB=" + QString::number(tab);
  l.append(cl.join(","));
}
