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

#include "SZWidget.h"
#include "Globals.h"
#include "Command.h"
#include "SZ.h"

#include <QtDebug>
#include <QStringList>
#include <QFormLayout>

SZWidget::SZWidget (QWidget *p) : PluginWidget (p)
{
  createGeneralPage();
}

void SZWidget::createGeneralPage ()
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

  // method
  SZ sz;
  _method = new QComboBox;
  _method->addItems(sz.list());
  form->addRow("METHOD", _method);

  // period
  _period = new QSpinBox;
  _period->setRange(1, 9999);
  form->addRow("PERIOD", _period);

  // no decline
  _ndperiod = new QSpinBox;
  _ndperiod->setRange(1, 9999);
  form->addRow("PERIOD_NO_DECLINE", _ndperiod);

  // coeff
  _coeff = new QDoubleSpinBox;
  _coeff->setRange(0, 9999);
  form->addRow("COEFFICIENT", _coeff);
}

void SZWidget::setCommand (QString d)
{
  Command command(d);
  if (command.parm("PLUGIN") != "SZ")
    return;

  _output->setText(command.parm("NAME"));
  _hinput->setText(command.parm("INPUT_HIGH"));
  _linput->setText(command.parm("INPUT_LOW"));
  _method->setCurrentIndex(_method->findText(command.parm("METHOD")));
  _period->setValue(command.parm("PERIOD").toInt());
  _ndperiod->setValue(command.parm("PERIOD_NO_DECLINE").toInt());
  _coeff->setValue(command.parm("COEFFICIENT").toDouble());
}

void SZWidget::commands (QStringList &l, int tab)
{
  QStringList cl;
  cl << "PLUGIN=SZ";
  cl << "NAME=" + _output->text();
  cl << "INPUT_HIGH=" + _hinput->text();
  cl << "INPUT_LOW=" + _linput->text();
  cl << "METHOD=" + _method->currentText();
  cl << "PERIOD=" + _period->text();
  cl << "PERIOD_NO_DECLINE=" + _ndperiod->text();
  cl << "COEFFICIENT=" + _coeff->text();
  cl << "TAB=" + QString::number(tab);
  l.append(cl.join(","));
}
