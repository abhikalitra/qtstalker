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

#include "FIDialog.h"
#include "MAType.h"
#include "Globals.h"
#include "FI.h"

#include <QtDebug>
#include <QStringList>

FIDialog::FIDialog (QWidget *p, Setting *set) : Dialog (p)
{
  _settings = set;
  _keySize = "FIDialog_window_size";
  _keyPos = "FIDialog_window_position";

  QStringList l;
  l << "QtStalker" + g_session + ":" << "FI" << tr("Indicator") << _settings->data("NAME");
  setWindowTitle(l.join(" "));

  createGeneralPage();

  loadSettings();
}

void FIDialog::createGeneralPage ()
{
  // ma type
  MAType mat;
  QStringList l = mat.list();

  _maType = new QComboBox;
  _maType->addItems(l);
  _maType->setCurrentIndex(_maType->findText(_settings->data(FI::_SMOOTHING_TYPE), Qt::MatchExactly));
  _form->addRow(tr("Smoothing Type"), _maType);

  // period
  _period = new QSpinBox;
  _period->setRange(1, 100000);
  _period->setValue(_settings->getInt(FI::_SMOOTHING));
  _form->addRow(tr("Smoothing"), _period);

  // color
  _color = new ColorButton(this, QColor(_settings->data(FI::_COLOR)));
  _color->setColorButton();
  _form->addRow(tr("Color"), _color);

  // plot style
  Curve c;
  c.list(l, 1);

  _style = new QComboBox;
  _style->addItems(l);
  _style->setCurrentIndex(_style->findText(_settings->data(FI::_STYLE), Qt::MatchExactly));
  _form->addRow(tr("Style"), _style);

  // make room unused
  _message->hide();
}

void FIDialog::done ()
{
  _settings->setData(FI::_SMOOTHING_TYPE, _maType->currentText());
  _settings->setData(FI::_SMOOTHING, _period->value());
  _settings->setData(FI::_COLOR, _color->color().name());
  _settings->setData(FI::_STYLE, _style->currentText());

  saveSettings();

  accept();
}
