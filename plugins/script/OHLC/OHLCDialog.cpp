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

#include "OHLCDialog.h"
#include "Globals.h"
#include "OHLC.h"

#include <QtDebug>

OHLCDialog::OHLCDialog (QWidget *p, Setting *set) : Dialog (p)
{
  _settings = set;
  _keySize = "OHLCDialog_window_size";
  _keyPos = "OHLCDialog_window_position";

  _styleList << "OHLC" << "Candle";

  QStringList l;
  l << "QtStalker" + g_session + ":" << "OHLC" << tr("Indicator") << _settings->data("NAME");
  setWindowTitle(l.join(" "));
  
  createGeneralPage();

  loadSettings();
}

void OHLCDialog::createGeneralPage ()
{
  // style
  _style = new QComboBox;
  _style->addItems(_styleList);
  _style->setCurrentIndex(_style->findText(_settings->data(OHLC::_STYLE), Qt::MatchExactly));
  _form->addRow(tr("Style"), _style);
  
  // up color
  _upColor = new ColorButton(this, QColor(_settings->data(OHLC::_COLOR_UP)));
  _upColor->setColorButton();
  _form->addRow(tr("Up Color"), _upColor);

  // down color
  _downColor = new ColorButton(this, QColor(_settings->data(OHLC::_COLOR_DOWN)));
  _downColor->setColorButton();
  _form->addRow(tr("Down Color"), _downColor);

  // neutral color
  _neutralColor = new ColorButton(this, QColor(_settings->data(OHLC::_COLOR_NEUTRAL)));
  _neutralColor->setColorButton();
  _form->addRow(tr("Neutral Color"), _neutralColor);

  // label
  _barsLabel = new QLineEdit(_settings->data(OHLC::_LABEL));
  _form->addRow(tr("Label"), _barsLabel);

  // make room unused
  _message->hide();
}

void OHLCDialog::done ()
{
  _settings->setData(OHLC::_COLOR_UP, _upColor->color().name());
  _settings->setData(OHLC::_COLOR_DOWN, _downColor->color().name());
  _settings->setData(OHLC::_COLOR_NEUTRAL, _neutralColor->color().name());
  _settings->setData(OHLC::_LABEL, _barsLabel->text());
  _settings->setData(OHLC::_STYLE, _style->currentText());

  saveSettings();
  
  accept();
}
