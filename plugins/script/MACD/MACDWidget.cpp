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

#include "MACDWidget.h"
#include "Globals.h"
#include "Command.h"
#include "MAType.h"

#include <QtDebug>
#include <QStringList>
#include <QFormLayout>

MACDWidget::MACDWidget (QWidget *p) : PluginWidget (p)
{
  createGeneralPage();
}

void MACDWidget::createGeneralPage ()
{
  QFormLayout *form = new QFormLayout;
  _vbox->addLayout(form);

  // macd
  _macd = new LineEdit;
  form->addRow("NAME_MACD", _macd);

  // signal
  _signal = new LineEdit;
  form->addRow("NAME_SIGNAL", _signal);

  // hist
  _hist = new LineEdit;
  form->addRow("NAME_HIST", _hist);

  // input
  _input = new LineEdit;
  form->addRow("INPUT", _input);

  // period fast
  _fperiod = new QSpinBox;
  _fperiod->setRange(1, 9999);
  form->addRow("PERIOD_FAST", _fperiod);

  // period slow
  _speriod = new QSpinBox;
  _speriod->setRange(1, 9999);
  form->addRow("PERIOD_SLOW", _speriod);

  // period signal
  _sigperiod = new QSpinBox;
  _sigperiod->setRange(1, 9999);
  form->addRow("PERIOD_SIGNAL", _sigperiod);

  // fast ma
  MAType mat;
  _fma = new QComboBox;
  _fma->addItems(mat.list());
  form->addRow("MA_TYPE_FAST", _fma);

  // slow ma
  _sma = new QComboBox;
  _sma->addItems(mat.list());
  form->addRow("MA_TYPE_SLOW", _sma);

  // signal ma
  _sigma = new QComboBox;
  _sigma->addItems(mat.list());
  form->addRow("MA_TYPE_SIGNAL", _sigma);
}

void MACDWidget::setCommand (QString d)
{
  Command command(d);
  if (command.parm("PLUGIN") != "MACD")
    return;

  _macd->setText(command.parm("NAME_MACD"));
  _signal->setText(command.parm("NAME_SIGNAL"));
  _hist->setText(command.parm("NAME_HIST"));
  _input->setText(command.parm("INPUT"));
  _fma->setCurrentIndex(_fma->findText(command.parm("MA_TYPE_FAST")));
  _sma->setCurrentIndex(_fma->findText(command.parm("MA_TYPE_SLOW")));
  _sigma->setCurrentIndex(_fma->findText(command.parm("MA_TYPE_SIGNAL")));
  _fperiod->setValue(command.parm("PERIOD_FAST").toInt());
  _speriod->setValue(command.parm("PERIOD_SLOW").toInt());
  _sigperiod->setValue(command.parm("PERIOD_SIGNAL").toInt());
}

void MACDWidget::commands (QStringList &l, int tab)
{
  QStringList cl;
  cl << "PLUGIN=MACD";
  cl << "NAME_MACD=" + _macd->text();
  cl << "NAME_SIGNAL=" + _signal->text();
  cl << "NAME_HIST=" + _hist->text();
  cl << "INPUT=" + _input->text();
  cl << "PERIOD_FAST=" + _fperiod->text();
  cl << "PERIOD_SLOW=" + _speriod->text();
  cl << "PERIOD_SIGNAL=" + _sigperiod->text();
  cl << "MA_TYPE_FAST=" + _fma->currentText();
  cl << "MA_TYPE_SLOW=" + _sma->currentText();
  cl << "MA_TYPE_SIGNAL=" + _sigma->currentText();
  cl << "TAB=" + QString::number(tab);
  l.append(cl.join(","));
}
