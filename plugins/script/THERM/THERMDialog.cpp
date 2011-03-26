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

#include "THERMDialog.h"
#include "THERM.h"
#include "Globals.h"
#include "MAType.h"

#include <QtDebug>
#include <QStringList>

THERMDialog::THERMDialog (QWidget *p, Setting *set) : Dialog (p)
{
  _settings = set;
  _keySize = "THERMDialog_window_size";
  _keyPos = "THERMDialog_window_position";

  QStringList l;
  l << "QtStalker" + g_session + ":" << "THERM" << tr("Indicator") << _settings->data("NAME");
  setWindowTitle(l.join(" "));

  createGeneralPage();

  loadSettings();
}

void THERMDialog::createGeneralPage ()
{
  _tabs = new QTabWidget;
  _vbox->insertWidget(0, _tabs);

  QWidget *w = new QWidget;

  QFormLayout *form = new QFormLayout;
  w->setLayout(form);

  // color
  _color = new ColorButton(this, QColor(_settings->data(THERM::_COLOR)));
  _color->setColorButton();
  form->addRow(tr("Color"), _color);

  // plot style
  QStringList l;
  Curve c;
  c.list(l, 1);

  _style = new QComboBox;
  _style->addItems(l);
  _style->setCurrentIndex(_style->findText(_settings->data(THERM::_STYLE), Qt::MatchExactly));
  form->addRow(tr("Style"), _style);

  // smoothing
  _smoothing = new QSpinBox;
  _smoothing->setRange(1, 100000);
  _smoothing->setValue(_settings->getInt(THERM::_SMOOTHING));
  _form->addRow(tr("Smoothing"), _smoothing);

  // type
  MAType mat;
  l = mat.list();

  _smoothingType = new QComboBox;
  _smoothingType->addItems(l);
  _smoothingType->setCurrentIndex(_smoothingType->findText(_settings->data(THERM::_SMOOTHING_TYPE), Qt::MatchExactly));
  _form->addRow(tr("Smoothing Type"), _smoothingType);

  // make room unused
  _message->hide();

  _tabs->addTab(w, "THERM");
}

void THERMDialog::done ()
{
  _settings->setData(THERM::_COLOR, _color->color().name());
  _settings->setData(THERM::_STYLE, _style->currentText());
  _settings->setData(THERM::_SMOOTHING, _smoothing->value());
  _settings->setData(THERM::_SMOOTHING_TYPE, _smoothingType->currentText());

  saveSettings();

  accept();
}
