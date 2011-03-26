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

#include "VARDialog.h"
#include "Globals.h"
#include "VAR.h"
#include "InputType.h"

#include <QtDebug>
#include <QStringList>

VARDialog::VARDialog (QWidget *p, Setting *set) : Dialog (p)
{
  _settings = set;
  _keySize = "VARDialog_window_size";
  _keyPos = "VARDialog_window_position";

  QStringList l;
  l << "QtStalker" + g_session + ":" << "VAR" << tr("Indicator") << _settings->data("NAME");
  setWindowTitle(l.join(" "));

  createGeneralPage();

  loadSettings();
}

void VARDialog::createGeneralPage ()
{
  // input
  InputType it;
  QStringList l = it.list();

  _input = new QComboBox;
  _input->addItems(l);
  _input->setCurrentIndex(_input->findText(_settings->data(VAR::_INPUT), Qt::MatchExactly));
  _form->addRow(tr("Input"), _input);

  // period
  _period = new QSpinBox;
  _period->setRange(1, 100000);
  _period->setValue(_settings->getInt(VAR::_PERIOD));
  _form->addRow(tr("Period"), _period);

  // color
  _color = new ColorButton(this, QColor(_settings->data(VAR::_COLOR)));
  _color->setColorButton();
  _form->addRow(tr("Color"), _color);

  // plot style
  Curve c;
  c.list(l, 1);

  _style = new QComboBox;
  _style->addItems(l);
  _style->setCurrentIndex(_style->findText(_settings->data(VAR::_STYLE), Qt::MatchExactly));
  _form->addRow(tr("Style"), _style);

  // make room unused
  _message->hide();
}

void VARDialog::done ()
{
  _settings->setData(VAR::_INPUT, _input->currentText());
  _settings->setData(VAR::_COLOR, _color->color().name());
  _settings->setData(VAR::_STYLE, _style->currentText());
  _settings->setData(VAR::_PERIOD, _period->value());

  saveSettings();

  accept();
}
