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

#include "BOPDialog.h"
#include "BOP.h"
#include "Globals.h"
#include "MAType.h"

#include <QtDebug>
#include <QStringList>

BOPDialog::BOPDialog (QWidget *p, Setting *set) : Dialog (p)
{
  _settings = set;
  _keySize = "BOPDialog_window_size";
  _keyPos = "BOPDialog_window_position";

  QStringList l;
  l << "QtStalker" + g_session + ":" << "BOP" << tr("Indicator") << _settings->data("NAME");
  setWindowTitle(l.join(" "));

  createGeneralPage();

  loadSettings();
}

void BOPDialog::createGeneralPage ()
{
  // color
  _color = new ColorButton(this, QColor(_settings->data(BOP::_COLOR)));
  _color->setColorButton();
  _form->addRow(tr("Color"), _color);

  // plot style
  Curve c;
  QStringList l;
  c.list(l, 1);

  _style = new QComboBox;
  _style->addItems(l);
  _style->setCurrentIndex(_style->findText(_settings->data(BOP::_STYLE), Qt::MatchExactly));
  _form->addRow(tr("Style"), _style);

  // smoothing
  _smoothing = new QSpinBox;
  _smoothing->setRange(1, 100000);
  _smoothing->setValue(_settings->getInt(BOP::_SMOOTHING));
  _form->addRow(tr("Smoothing"), _smoothing);

  // smoothing type
  MAType mat;
  l = mat.list();

  _smoothingType = new QComboBox;
  _smoothingType->addItems(l);
  _smoothingType->setCurrentIndex(_smoothingType->findText(_settings->data(BOP::_SMOOTHING_TYPE), Qt::MatchExactly));
  _form->addRow(tr("Smoothing Type"), _smoothingType);
  
  // make room unused
  _message->hide();
}

void BOPDialog::done ()
{
  _settings->setData(BOP::_COLOR, _color->color().name());
  _settings->setData(BOP::_STYLE, _style->currentText());
  _settings->setData(BOP::_SMOOTHING, _smoothing->value());
  _settings->setData(BOP::_SMOOTHING_TYPE, _smoothingType->currentText());

  saveSettings();

  accept();
}
