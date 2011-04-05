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
#include "InputType.h"

#include <QtDebug>
#include <QStringList>
#include <QFormLayout>

HT_PHASORDialog::HT_PHASORDialog (QWidget *p, Setting *set) : QWidget (p)
{
  _settings = set;
  createGeneralPage();
  createPhasePage();
  createQuadPage();
}

void HT_PHASORDialog::createGeneralPage ()
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

void HT_PHASORDialog::createPhasePage ()
{
  QWidget *w = new QWidget;

  QFormLayout *form = new QFormLayout;
  w->setLayout(form);

  // output
  _phaseOutput = new QLineEdit(_settings->data("OUTPUT_PHASE"));
  form->addRow(tr("Output"), _phaseOutput);

  // color
  _phaseColor = new ColorButton(this, QColor(_settings->data("COLOR_PHASE")));
  _phaseColor->setColorButton();
  form->addRow(tr("Color"), _phaseColor);

  // plot style
  Curve c;
  QStringList l = c.list();

  _phaseStyle = new QComboBox;
  _phaseStyle->addItems(l);
  _phaseStyle->setCurrentIndex(_phaseStyle->findText(_settings->data("STYLE_PHASE"), Qt::MatchExactly));
  form->addRow(tr("Style"), _phaseStyle);

  // z
  _zPhase = new QSpinBox;
  _zPhase->setRange(-1, 99);
  _zPhase->setValue(_settings->getInt("Z_PHASE"));
  form->addRow(tr("Plot Order"), _zPhase);
  
  _tabs->addTab(w, "PHASE");
}

void HT_PHASORDialog::createQuadPage ()
{
  QWidget *w = new QWidget;

  QFormLayout *form = new QFormLayout;
  w->setLayout(form);

  // output
  _quadOutput = new QLineEdit(_settings->data("OUTPUT_QUAD"));
  form->addRow(tr("Output"), _quadOutput);

  // color
  _quadColor = new ColorButton(this, QColor(_settings->data("COLOR_QUAD")));
  _quadColor->setColorButton();
  form->addRow(tr("Color"), _quadColor);

  // plot style
  Curve c;
  QStringList l = c.list();

  _quadStyle = new QComboBox;
  _quadStyle->addItems(l);
  _quadStyle->setCurrentIndex(_quadStyle->findText(_settings->data("STYLE_QUAD"), Qt::MatchExactly));
  form->addRow(tr("Style"), _quadStyle);

  // z
  _zQuad = new QSpinBox;
  _zQuad->setRange(-1, 99);
  _zQuad->setValue(_settings->getInt("Z_QUAD"));
  form->addRow(tr("Plot Order"), _zQuad);

  _tabs->addTab(w, "QUAD");
}

void HT_PHASORDialog::save ()
{
  _settings->setData("COLOR_PHASE", _phaseColor->color().name());
  _settings->setData("COLOR_QUAD", _quadColor->color().name());
  _settings->setData("STYLE_PHASE", _phaseStyle->currentText());
  _settings->setData("STYLE_QUAD", _quadStyle->currentText());
  _settings->setData("INPUT", _input->currentText());
  _settings->setData("OUTPUT_PHASE", _phaseOutput->text());
  _settings->setData("Z_PHASE", _zPhase->text());
  _settings->setData("OUTPUT_QUAD", _quadOutput->text());
  _settings->setData("Z_QUAD", _zQuad->text());
}
