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

#include "HT_PHASORDialog.h"
#include "Globals.h"
#include "HT_PHASOR.h"
#include "InputType.h"

#include <QtDebug>
#include <QStringList>

HT_PHASORDialog::HT_PHASORDialog (QWidget *p, Setting *set) : Dialog (p)
{
  _settings = set;
  _keySize = "HT_PHASORDialog_window_size";
  _keyPos = "HT_PHASORDialog_window_position";

  QStringList l;
  l << "QtStalker" + g_session + ":" << "HT_PHASOR" << tr("Indicator") << _settings->data("NAME");
  setWindowTitle(l.join(" "));

  createGeneralPage();
  createPhasePage();
  createQuadPage();

  loadSettings();
}

void HT_PHASORDialog::createGeneralPage ()
{
  _tabs = new QTabWidget;
  _vbox->insertWidget(0, _tabs);

  QWidget *w = new QWidget;

  QFormLayout *form = new QFormLayout;
  w->setLayout(form);

  // input
  InputType it;
  QStringList l = it.list();

  _input = new QComboBox;
  _input->addItems(l);
  _input->setCurrentIndex(_input->findText(_settings->data(HT_PHASOR::_INPUT), Qt::MatchExactly));
  form->addRow(tr("Input"), _input);

  // make room unused
  _message->hide();
  
  _tabs->addTab(w, tr("General"));  
}

void HT_PHASORDialog::createPhasePage ()
{
  QWidget *w = new QWidget;

  QFormLayout *form = new QFormLayout;
  w->setLayout(form);

  // color
  _phaseColor = new ColorButton(this, QColor(_settings->data(HT_PHASOR::_COLOR_PHASE)));
  _phaseColor->setColorButton();
  form->addRow(tr("Color"), _phaseColor);

  // plot style
  QStringList l;
  Curve c;
  c.list(l, 1);

  _phaseStyle = new QComboBox;
  _phaseStyle->addItems(l);
  _phaseStyle->setCurrentIndex(_phaseStyle->findText(_settings->data(HT_PHASOR::_STYLE_PHASE), Qt::MatchExactly));
  form->addRow(tr("Style"), _phaseStyle);

  _tabs->addTab(w, "PHASE");
}

void HT_PHASORDialog::createQuadPage ()
{
  QWidget *w = new QWidget;

  QFormLayout *form = new QFormLayout;
  w->setLayout(form);

  // color
  _quadColor = new ColorButton(this, QColor(_settings->data(HT_PHASOR::_COLOR_QUAD)));
  _quadColor->setColorButton();
  form->addRow(tr("Color"), _quadColor);

  // plot style
  QStringList l;
  Curve c;
  c.list(l, 1);

  _quadStyle = new QComboBox;
  _quadStyle->addItems(l);
  _quadStyle->setCurrentIndex(_quadStyle->findText(_settings->data(HT_PHASOR::_STYLE_QUAD), Qt::MatchExactly));
  form->addRow(tr("Style"), _quadStyle);

  _tabs->addTab(w, "QUAD");
}

void HT_PHASORDialog::done ()
{
  _settings->setData(HT_PHASOR::_COLOR_PHASE, _phaseColor->color().name());
  _settings->setData(HT_PHASOR::_COLOR_QUAD, _quadColor->color().name());
  _settings->setData(HT_PHASOR::_STYLE_PHASE, _phaseStyle->currentText());
  _settings->setData(HT_PHASOR::_STYLE_QUAD, _quadStyle->currentText());
  _settings->setData(HT_PHASOR::_INPUT, _input->currentText());

  saveSettings();

  accept();
}
