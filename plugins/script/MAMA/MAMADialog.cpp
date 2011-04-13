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

#include "MAMADialog.h"
#include "Globals.h"
#include "InputType.h"

#include <QtDebug>
#include <QStringList>
#include <QFormLayout>

MAMADialog::MAMADialog (QWidget *p, Setting *set) : QWidget (p)
{
  _settings = set;
  createGeneralPage();
  createMAMAPage();
  createFAMAPage();
}

void MAMADialog::createGeneralPage ()
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
  QStringList l;
  l = it.list();
  l.append(_settings->data("INPUT"));
  l.removeDuplicates();

  _input = new QComboBox;
  _input->addItems(l);
  _input->setCurrentIndex(_input->findText(_settings->data("INPUT"), Qt::MatchExactly));
  form->addRow(tr("Input"), _input);

  // fast limit
  _fast = new QDoubleSpinBox;
  _fast->setRange(0, 100000);
  _fast->setDecimals(4);
  _fast->setValue(_settings->getDouble("LIMIT_FAST"));
  form->addRow(tr("Fast Limit"), _fast);

  // slow limit
  _slow = new QDoubleSpinBox;
  _slow->setRange(0, 100000);
  _slow->setDecimals(4);
  _slow->setValue(_settings->getDouble("LIMIT_SLOW"));
  form->addRow(tr("Slow Period"), _slow);

  _tabs->addTab(w, tr("General"));  
}

void MAMADialog::createMAMAPage ()
{
  QWidget *w = new QWidget;

  QFormLayout *form = new QFormLayout;
  w->setLayout(form);

  // name
  _mamaOutput = new QLineEdit(_settings->data("OUTPUT_MAMA"));
  form->addRow(tr("Output"), _mamaOutput);

  // color
  _mamaColor = new ColorButton(this, QColor(_settings->data("COLOR_MAMA")));
  _mamaColor->setColorButton();
  form->addRow(tr("Color"), _mamaColor);

  // plot style
  Curve c;
  QStringList l = c.list();

  _mamaStyle = new QComboBox;
  _mamaStyle->addItems(l);
  _mamaStyle->setCurrentIndex(_mamaStyle->findText(_settings->data("STYLE_MAMA"), Qt::MatchExactly));
  form->addRow(tr("Style"), _mamaStyle);

  // z
  _zMama = new QSpinBox;
  _zMama->setRange(-1, 99);
  _zMama->setValue(_settings->getInt("Z_MAMA"));
  form->addRow(tr("Plot"), _zMama);

  _tabs->addTab(w, "MAMA");
}

void MAMADialog::createFAMAPage ()
{
  QWidget *w = new QWidget;

  QFormLayout *form = new QFormLayout;
  w->setLayout(form);

  // name
  _famaOutput = new QLineEdit(_settings->data("OUTPUT_FAMA"));
  form->addRow(tr("Output"), _famaOutput);

  // color
  _famaColor = new ColorButton(this, QColor(_settings->data("COLOR_FAMA")));
  _famaColor->setColorButton();
  form->addRow(tr("Color"), _famaColor);

  // plot style
  Curve c;
  QStringList l = c.list();

  _famaStyle = new QComboBox;
  _famaStyle->addItems(l);
  _famaStyle->setCurrentIndex(_famaStyle->findText(_settings->data("STYLE_FAMA"), Qt::MatchExactly));
  form->addRow(tr("Style"), _famaStyle);

  // z
  _zFama = new QSpinBox;
  _zFama->setRange(-1, 99);
  _zFama->setValue(_settings->getInt("Z_FAMA"));
  form->addRow(tr("Plot"), _zFama);

  _tabs->addTab(w, "FAMA");
}

void MAMADialog::save ()
{
  _settings->setData("COLOR_MAMA", _mamaColor->color().name());
  _settings->setData("COLOR_FAMA", _famaColor->color().name());
  _settings->setData("STYLE_MAMA", _mamaStyle->currentText());
  _settings->setData("STYLE_FAMA", _famaStyle->currentText());
  _settings->setData("LIMIT_FAST", _fast->value());
  _settings->setData("LIMIT_SLOW", _slow->value());
  _settings->setData("INPUT", _input->currentText());
  _settings->setData("OUTPUT_MAMA", _mamaOutput->text());
  _settings->setData("Z_MAMA", _zMama->text());
  _settings->setData("OUTPUT_FAMA", _famaOutput->text());
  _settings->setData("Z_FAMA", _zFama->text());
}
