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

#include "HT_SINEDialog.h"
#include "Globals.h"
#include "HT_SINE.h"
#include "InputType.h"

#include <QtDebug>
#include <QStringList>

HT_SINEDialog::HT_SINEDialog (QWidget *p, Setting *set) : Dialog (p)
{
  _settings = set;
  _keySize = "HT_SINEDialog_window_size";
  _keyPos = "HT_SINEDialog_window_position";

  QStringList l;
  l << "QtStalker" + g_session + ":" << "HT_SINE" << tr("Indicator") << _settings->data("NAME");
  setWindowTitle(l.join(" "));

  createGeneralPage();
  createSinePage();
  createLeadPage();

  loadSettings();
}

void HT_SINEDialog::createGeneralPage ()
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
  _input->setCurrentIndex(_input->findText(_settings->data(HT_SINE::_INPUT), Qt::MatchExactly));
  form->addRow(tr("Input"), _input);

  // make room unused
  _message->hide();
  
  _tabs->addTab(w, tr("General"));  
}

void HT_SINEDialog::createSinePage ()
{
  QWidget *w = new QWidget;

  QFormLayout *form = new QFormLayout;
  w->setLayout(form);

  // color
  _sineColor = new ColorButton(this, QColor(_settings->data(HT_SINE::_COLOR_SINE)));
  _sineColor->setColorButton();
  form->addRow(tr("Color"), _sineColor);

  // plot style
  QStringList l;
  Curve c;
  c.list(l, 1);

  _sineStyle = new QComboBox;
  _sineStyle->addItems(l);
  _sineStyle->setCurrentIndex(_sineStyle->findText(_settings->data(HT_SINE::_STYLE_SINE), Qt::MatchExactly));
  form->addRow(tr("Style"), _sineStyle);

  _tabs->addTab(w, "SINE");
}

void HT_SINEDialog::createLeadPage ()
{
  QWidget *w = new QWidget;

  QFormLayout *form = new QFormLayout;
  w->setLayout(form);

  // color
  _leadColor = new ColorButton(this, QColor(_settings->data(HT_SINE::_COLOR_LEAD)));
  _leadColor->setColorButton();
  form->addRow(tr("Color"), _leadColor);

  // plot style
  QStringList l;
  Curve c;
  c.list(l, 1);

  _leadStyle = new QComboBox;
  _leadStyle->addItems(l);
  _leadStyle->setCurrentIndex(_leadStyle->findText(_settings->data(HT_SINE::_STYLE_LEAD), Qt::MatchExactly));
  form->addRow(tr("Style"), _leadStyle);

  _tabs->addTab(w, "LEAD");
}

void HT_SINEDialog::done ()
{
  _settings->setData(HT_SINE::_COLOR_SINE, _sineColor->color().name());
  _settings->setData(HT_SINE::_COLOR_LEAD, _leadColor->color().name());
  _settings->setData(HT_SINE::_STYLE_SINE, _sineStyle->currentText());
  _settings->setData(HT_SINE::_STYLE_LEAD, _leadStyle->currentText());
  _settings->setData(HT_SINE::_INPUT, _input->currentText());

  saveSettings();

  accept();
}
