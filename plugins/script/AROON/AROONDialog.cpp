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

#include "AROONDialog.h"
#include "Globals.h"

#include <QtDebug>
#include <QStringList>
#include <QFormLayout>

AROONDialog::AROONDialog (QWidget *p, Setting *set) : QWidget (p)
{
  _settings = set;
  createGeneralPage();
  createAROONUpPage();
  createAROONDownPage();
}

void AROONDialog::createGeneralPage ()
{
  QVBoxLayout *vbox = new QVBoxLayout;
  setLayout(vbox);

  _tabs = new QTabWidget;
  vbox->addWidget(_tabs);

  QWidget *w = new QWidget;

  QFormLayout *form = new QFormLayout;
  w->setLayout(form);

  _period = new QSpinBox;
  _period->setRange(2, 100000);
  _period->setValue(_settings->getInt("PERIOD"));
  form->addRow(tr("Period"), _period);
  
  _tabs->addTab(w, tr("General"));  
}

void AROONDialog::createAROONUpPage ()
{
  QWidget *w = new QWidget;

  QFormLayout *form = new QFormLayout;
  w->setLayout(form);

  // output
  _upOutput = new QLineEdit(_settings->data("OUTPUT_UP"));
  form->addRow(tr("Output"), _upOutput);

  // color
  _upColor = new ColorButton(this, QColor(_settings->data("COLOR_UP")));
  _upColor->setColorButton();
  form->addRow(tr("Color"), _upColor);

  // style
  Curve c;
  QStringList l;
  c.list(l, 1);

  _upStyle = new QComboBox;
  _upStyle->addItems(l);
  _upStyle->setCurrentIndex(_upStyle->findText(_settings->data("STYLE_UP"), Qt::MatchExactly));
  form->addRow(tr("Style"), _upStyle);

  // z
  _zUp = new QSpinBox;
  _zUp->setRange(-1, 99);
  _zUp->setValue(_settings->getInt("Z_UP"));
  form->addRow(tr("Plot Order"), _zUp);
  
  _tabs->addTab(w, tr("Up"));
}

void AROONDialog::createAROONDownPage ()
{
  QWidget *w = new QWidget;

  QFormLayout *form = new QFormLayout;
  w->setLayout(form);

  // output
  _downOutput = new QLineEdit(_settings->data("OUTPUT_DOWN"));
  form->addRow(tr("Output"), _downOutput);

  // color
  _downColor = new ColorButton(this, QColor(_settings->data("COLOR_DOWN")));
  _downColor->setColorButton();
  form->addRow(tr("Color"), _downColor);

  // style
  Curve c;
  QStringList l;
  c.list(l, 1);

  _downStyle = new QComboBox;
  _downStyle->addItems(l);
  _downStyle->setCurrentIndex(_downStyle->findText(_settings->data("STYLE_DOWN"), Qt::MatchExactly));
  form->addRow(tr("Style"), _downStyle);

  // z
  _zDown = new QSpinBox;
  _zDown->setRange(-1, 99);
  _zDown->setValue(_settings->getInt("Z_DOWN"));
  form->addRow(tr("Plot Order"), _zDown);
  
  _tabs->addTab(w, tr("Down"));
}

void AROONDialog::save ()
{
  _settings->setData("COLOR_DOWN", _downColor->color().name());
  _settings->setData("COLOR_UP", _upColor->color().name());
  _settings->setData("STYLE_DOWN", _downStyle->currentText());
  _settings->setData("STYLE_UP", _upStyle->currentText());
  _settings->setData("PERIOD", _period->value());
  _settings->setData("OUTPUT_UP", _upOutput->text());
  _settings->setData("OUTPUT_DOWN", _downOutput->text());
  _settings->setData("Z_UP", _zUp->text());
  _settings->setData("Z_DOWN", _zDown->text());
}
