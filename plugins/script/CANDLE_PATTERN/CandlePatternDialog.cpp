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

#include "CandlePatternDialog.h"
#include "CANDLE_PATTERN.h"
#include "Globals.h"

#include <QtDebug>
#include <QStringList>

CandlePatternDialog::CandlePatternDialog (QWidget *p, Setting *set) : Dialog (p)
{
  _settings = set;
  _keySize = "CandlePatternDialog_window_size";
  _keyPos = "CandlePatternDialog_window_position";

  QStringList l;
  l << "QtStalker" + g_session + ":" << "CANDLE_PATTERN" << tr("Indicator") << _settings->data("NAME");
  setWindowTitle(l.join(" "));

  createGeneralPage();

  loadSettings();
}

void CandlePatternDialog::createGeneralPage ()
{
  _tabs = new QTabWidget;
  _vbox->insertWidget(0, _tabs);

  QWidget *w = new QWidget;

  QFormLayout *form = new QFormLayout;
  w->setLayout(form);

  // candle color
  _candleColor = new ColorButton(this, QColor(_settings->data(CANDLE_PATTERN::_COLOR)));
  _candleColor->setColorButton();
  form->addRow(tr("Candle Color"), _candleColor);

  // penetration
  _pen = new QDoubleSpinBox;
  _pen->setRange(0, 100);
  _pen->setValue(_settings->getDouble(CANDLE_PATTERN::_PEN));
  form->addRow(tr("Penetration"), _pen);

  // make room unused
  _message->hide();

  _tabs->addTab(w, tr("General"));
}

void CandlePatternDialog::done ()
{
  _settings->setData(CANDLE_PATTERN::_PEN, _pen->value());
  _settings->setData(CANDLE_PATTERN::_COLOR, _candleColor->color().name());

  saveSettings();

  accept();
}
