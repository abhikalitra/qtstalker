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

#include "STOCH_FASTWidget.h"
#include "Globals.h"
#include "Command.h"
#include "MAType.h"

#include <QtDebug>
#include <QStringList>
#include <QFormLayout>

STOCH_FASTWidget::STOCH_FASTWidget (QWidget *p) : PluginWidget (p)
{
  createGeneralPage();
}

void STOCH_FASTWidget::createGeneralPage ()
{
  QFormLayout *form = new QFormLayout;
  _vbox->addLayout(form);

  // fastk
  _fastk = new LineEdit;
  form->addRow("NAME_FASTK", _fastk);

  // fastd
  _fastd = new LineEdit;
  form->addRow("NAME_FASTD", _fastd);

  // high input
  _hinput = new LineEdit;
  form->addRow("INPUT_HIGH", _hinput);

  // low input
  _linput = new LineEdit;
  form->addRow("INPUT_LOW", _linput);

  // close input
  _cinput = new LineEdit;
  form->addRow("INPUT_CLOSE", _cinput);

  // period fastk
  _kperiod = new QSpinBox;
  _kperiod->setRange(1, 9999);
  form->addRow("PERIOD_FASTK", _kperiod);

  // period fastd
  _dperiod = new QSpinBox;
  _dperiod->setRange(1, 9999);
  form->addRow("PERIOD_FASTD", _dperiod);

  // ma type
  MAType mat;
  _type = new QComboBox;
  _type->addItems(mat.list());
  form->addRow("MA_TYPE", _type);
}

void STOCH_FASTWidget::setCommand (QString d)
{
  Command command(d);
  if (command.parm("PLUGIN") != "STOCH_FAST")
    return;

  _fastk->setText(command.parm("NAME_FASTK"));
  _fastd->setText(command.parm("NAME_FASTD"));
  _hinput->setText(command.parm("INPUT_HIGH"));
  _linput->setText(command.parm("INPUT_LOW"));
  _cinput->setText(command.parm("INPUT_CLOSE"));
  _kperiod->setValue(command.parm("PERIOD_FASTK").toInt());
  _dperiod->setValue(command.parm("PERIOD_FASTD").toInt());
  _type->setCurrentIndex(_type->findText(command.parm("MA_TYPE")));
}

void STOCH_FASTWidget::commands (QStringList &l, int tab)
{
  QStringList cl;
  cl << "PLUGIN=STOCH_FAST";
  cl << "NAME_FASTK=" + _fastk->text();
  cl << "NAME_FASTD=" + _fastd->text();
  cl << "INPUT_HIGH=" + _hinput->text();
  cl << "INPUT_LOW=" + _linput->text();
  cl << "INPUT_CLOSE=" + _cinput->text();
  cl << "PERIOD_FASTK=" + _kperiod->text();
  cl << "PERIOD_FASTD=" + _dperiod->text();
  cl << "MA_TYPE=" + _type->currentText();
  cl << "TAB=" + QString::number(tab);
  l.append(cl.join(","));
}
