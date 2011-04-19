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

#include "STOCH_SLOWWidget.h"
#include "Globals.h"
#include "Command.h"
#include "MAType.h"

#include <QtDebug>
#include <QStringList>
#include <QFormLayout>

STOCH_SLOWWidget::STOCH_SLOWWidget (QWidget *p) : PluginWidget (p)
{
  createGeneralPage();
}

void STOCH_SLOWWidget::createGeneralPage ()
{
  QFormLayout *form = new QFormLayout;
  _vbox->addLayout(form);

  // slowk
  _slowk = new LineEdit;
  form->addRow("NAME_SLOWK", _slowk);

  // slowd
  _slowd = new LineEdit;
  form->addRow("NAME_SLOWD", _slowd);

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
  _fkperiod = new QSpinBox;
  _fkperiod->setRange(1, 9999);
  form->addRow("PERIOD_FASTK", _fkperiod);

  // period slowk
  _skperiod = new QSpinBox;
  _skperiod->setRange(1, 9999);
  form->addRow("PERIOD_SLOWK", _skperiod);

  // period slowd
  _sdperiod = new QSpinBox;
  _sdperiod->setRange(1, 9999);
  form->addRow("PERIOD_SLOWD", _sdperiod);

  // slowk ma type
  MAType mat;
  _ktype = new QComboBox;
  _ktype->addItems(mat.list());
  form->addRow("MA_TYPE_SLOWK", _ktype);

  // slowd ma type
  _dtype = new QComboBox;
  _dtype->addItems(mat.list());
  form->addRow("MA_TYPE_SLOWD", _dtype);
}

void STOCH_SLOWWidget::setCommand (QString d)
{
  Command command(d);
  if (command.parm("PLUGIN") != "STOCH_SLOW")
    return;

  _slowk->setText(command.parm("NAME_SLOWK"));
  _slowd->setText(command.parm("NAME_SLOWD"));
  _hinput->setText(command.parm("INPUT_HIGH"));
  _linput->setText(command.parm("INPUT_LOW"));
  _cinput->setText(command.parm("INPUT_CLOSE"));
  _fkperiod->setValue(command.parm("PERIOD_FASTK").toInt());
  _skperiod->setValue(command.parm("PERIOD_SLOWK").toInt());
  _sdperiod->setValue(command.parm("PERIOD_SLOWD").toInt());
  _ktype->setCurrentIndex(_ktype->findText(command.parm("MA_TYPE_SLOWK")));
  _dtype->setCurrentIndex(_dtype->findText(command.parm("MA_TYPE_SLOWD")));
}

void STOCH_SLOWWidget::commands (QStringList &l, int tab)
{
  QStringList cl;
  cl << "PLUGIN=STOCH_SLOW";
  cl << "NAME_SLOWK=" + _slowk->text();
  cl << "NAME_SLOWD=" + _slowd->text();
  cl << "INPUT_HIGH=" + _hinput->text();
  cl << "INPUT_LOW=" + _linput->text();
  cl << "INPUT_CLOSE=" + _cinput->text();
  cl << "PERIOD_FASTK=" + _fkperiod->text();
  cl << "PERIOD_SLOWK=" + _skperiod->text();
  cl << "PERIOD_SLOWD=" + _sdperiod->text();
  cl << "MA_TYPE_SLOWK=" + _ktype->currentText();
  cl << "MA_TYPE_SLOWD=" + _dtype->currentText();
  cl << "TAB=" + QString::number(tab);
  l.append(cl.join(","));
}
