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

#include <QtDebug>
#include <QFormLayout>

OHLCDialog::OHLCDialog (QWidget *p, Setting *set) : QWidget (p)
{
  _settings = set;
  _styleList << "Bars" << "Candle";
  createGeneralPage();
}

void OHLCDialog::createGeneralPage ()
{
  QFormLayout *form = new QFormLayout;
  setLayout(form);

  // style
  _style = new QComboBox;
  _style->addItems(_styleList);
  _style->setCurrentIndex(_style->findText(_settings->data("STYLE"), Qt::MatchExactly));
  form->addRow(tr("Style"), _style);
  
  // up color
  _upColor = new ColorButton(this, QColor(_settings->data("COLOR_UP")));
  _upColor->setColorButton();
  form->addRow(tr("Up Color"), _upColor);

  // down color
  _downColor = new ColorButton(this, QColor(_settings->data("COLOR_DOWN")));
  _downColor->setColorButton();
  form->addRow(tr("Down Color"), _downColor);

  // neutral color
  _neutralColor = new ColorButton(this, QColor(_settings->data("COLOR_NEUTRAL")));
  _neutralColor->setColorButton();
  form->addRow(tr("Neutral Color"), _neutralColor);
  
  // z
  _z = new QSpinBox;
  _z->setRange(-1, 99);
  _z->setValue(_settings->getInt("Z"));
  form->addRow(tr("Plot Order"), _z);
}

void OHLCDialog::save ()
{
  _settings->setData("COLOR_UP", _upColor->color().name());
  _settings->setData("COLOR_DOWN", _downColor->color().name());
  _settings->setData("COLOR_NEUTRAL", _neutralColor->color().name());
  _settings->setData("STYLE", _style->currentText());
  _settings->setData("Z", _z->text());
}
