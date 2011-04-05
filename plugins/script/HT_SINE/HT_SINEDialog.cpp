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
#include "InputType.h"

#include <QtDebug>
#include <QStringList>
#include <QFormLayout>

HT_SINEDialog::HT_SINEDialog (QWidget *p, Setting *set) : QWidget (p)
{
  _settings = set;
  createGeneralPage();
  createSinePage();
  createLeadPage();
}

void HT_SINEDialog::createGeneralPage ()
{
  QVBoxLayout *vbox = new QVBoxLayout;
  setLayout(vbox);

  _tabs = new QTabWidget;
  vbox->addWidget(_tabs);

  QWidget *w = new QWidget;

  QFormLayout *form = new QFormLayout;
  w->setLayout(form);

  // input
  InputType it;
  QStringList l = it.list();
  l.append(_settings->data("INPUT"));
  l.removeDuplicates();

  _input = new QComboBox;
  _input->addItems(l);
  _input->setCurrentIndex(_input->findText(_settings->data("INPUT"), Qt::MatchExactly));
  form->addRow(tr("Input"), _input);

  _tabs->addTab(w, tr("General"));  
}

void HT_SINEDialog::createSinePage ()
{
  QWidget *w = new QWidget;

  QFormLayout *form = new QFormLayout;
  w->setLayout(form);

  // output
  _sineOutput = new QLineEdit(_settings->data("OUTPUT_SINE"));
  form->addRow(tr("Output"), _sineOutput);

  // color
  _sineColor = new ColorButton(this, QColor(_settings->data("COLOR_SINE")));
  _sineColor->setColorButton();
  form->addRow(tr("Color"), _sineColor);

  // plot style
  Curve c;
  QStringList l = c.list();

  _sineStyle = new QComboBox;
  _sineStyle->addItems(l);
  _sineStyle->setCurrentIndex(_sineStyle->findText(_settings->data("STYLE_SINE"), Qt::MatchExactly));
  form->addRow(tr("Style"), _sineStyle);

  // z
  _zSine = new QSpinBox;
  _zSine->setRange(-1, 99);
  _zSine->setValue(_settings->getInt("Z_SINE"));
  form->addRow(tr("Plot Order"), _zSine);

  _tabs->addTab(w, "SINE");
}

void HT_SINEDialog::createLeadPage ()
{
  QWidget *w = new QWidget;

  QFormLayout *form = new QFormLayout;
  w->setLayout(form);

  // output
  _leadOutput = new QLineEdit(_settings->data("OUTPUT_LEAD"));
  form->addRow(tr("Output"), _leadOutput);

  // color
  _leadColor = new ColorButton(this, QColor(_settings->data("COLOR_LEAD")));
  _leadColor->setColorButton();
  form->addRow(tr("Color"), _leadColor);

  // plot style
  Curve c;
  QStringList l = c.list();

  _leadStyle = new QComboBox;
  _leadStyle->addItems(l);
  _leadStyle->setCurrentIndex(_leadStyle->findText(_settings->data("STYLE_LEAD"), Qt::MatchExactly));
  form->addRow(tr("Style"), _leadStyle);

  // z
  _zLead = new QSpinBox;
  _zLead->setRange(-1, 99);
  _zLead->setValue(_settings->getInt("Z"));
  form->addRow(tr("Plot Order"), _zLead);

  _tabs->addTab(w, "LEAD");
}

void HT_SINEDialog::save ()
{
  _settings->setData("COLOR_SINE", _sineColor->color().name());
  _settings->setData("COLOR_LEAD", _leadColor->color().name());
  _settings->setData("STYLE_SINE", _sineStyle->currentText());
  _settings->setData("STYLE_LEAD", _leadStyle->currentText());
  _settings->setData("INPUT", _input->currentText());
  _settings->setData("OUTPUT_SINE", _sineOutput->text());
  _settings->setData("Z_SINE", _zSine->text());
  _settings->setData("OUTPUT_LEAD", _leadOutput->text());
  _settings->setData("Z_LEAD", _zLead->text());
}
