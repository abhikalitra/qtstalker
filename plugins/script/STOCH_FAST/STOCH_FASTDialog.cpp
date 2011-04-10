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

#include "STOCH_FASTDialog.h"
#include "Globals.h"
#include "MAType.h"

#include <QtDebug>
#include <QStringList>
#include <QFormLayout>

STOCH_FASTDialog::STOCH_FASTDialog (QWidget *p, Setting *set) : QWidget (p)
{
  _settings = set;
  createKPage();
  createDPage();
  createRefPage();
  createRef2Page();
}

void STOCH_FASTDialog::createKPage ()
{
  QVBoxLayout *vbox = new QVBoxLayout;
  setLayout(vbox);

  _tabs = new QTabWidget;
  vbox->addWidget(_tabs);

  QWidget *w = new QWidget;

  QFormLayout *form = new QFormLayout;
  w->setLayout(form);

  // output
  _koutput = new QLineEdit(_settings->data("OUTPUT_K"));
  form->addRow(tr("Output"), _koutput);

  // period
  _kperiod = new QSpinBox;
  _kperiod->setRange(1, 100000);
  _kperiod->setValue(_settings->getInt("PERIOD_FASTK"));
  form->addRow(tr("Period"), _kperiod);

  // color
  _kcolor = new ColorButton(this, QColor(_settings->data("COLOR_K")));
  _kcolor->setColorButton();
  form->addRow(tr("Color"), _kcolor);

  // plot style
  Curve c;
  QStringList l = c.list();

  _kstyle = new QComboBox;
  _kstyle->addItems(l);
  _kstyle->setCurrentIndex(_kstyle->findText(_settings->data("STYLE_K"), Qt::MatchExactly));
  form->addRow(tr("Style"), _kstyle);

  // z
  _zk = new QSpinBox;
  _zk->setRange(-1, 99);
  _zk->setValue(_settings->getInt("Z_K"));
  form->addRow(tr("Plot Order"), _zk);

  _tabs->addTab(w, "%K");
}

void STOCH_FASTDialog::createDPage ()
{
  QWidget *w = new QWidget;

  QFormLayout *form = new QFormLayout;
  w->setLayout(form);

  // output
  _doutput = new QLineEdit(_settings->data("OUTPUT_D"));
  form->addRow(tr("Output"), _doutput);

  // type
  MAType mat;
  QStringList l = mat.list();

  _maType = new QComboBox;
  _maType->addItems(l);
  _maType->setCurrentIndex(_maType->findText(_settings->data("MA_TYPE_FASTD"), Qt::MatchExactly));
  form->addRow(tr("Type"), _maType);

  // period
  _dperiod = new QSpinBox;
  _dperiod->setRange(1, 100000);
  _dperiod->setValue(_settings->getInt("PERIOD_FASTD"));
  form->addRow(tr("Period"), _dperiod);

  // color
  _dcolor = new ColorButton(this, QColor(_settings->data("COLOR_D")));
  _dcolor->setColorButton();
  form->addRow(tr("Color"), _dcolor);

  // plot style
  Curve c;
  l = c.list();

  _dstyle = new QComboBox;
  _dstyle->addItems(l);
  _dstyle->setCurrentIndex(_dstyle->findText(_settings->data("STYLE_D"), Qt::MatchExactly));
  form->addRow(tr("Style"), _dstyle);

  // z
  _zd = new QSpinBox;
  _zd->setRange(-1, 99);
  _zd->setValue(_settings->getInt("Z_D"));
  form->addRow(tr("Plot Order"), _zd);
  
  _tabs->addTab(w, "%D");
}

void STOCH_FASTDialog::createRefPage ()
{
  QWidget *w = new QWidget;

  QFormLayout *form = new QFormLayout;
  w->setLayout(form);

  // color
  _refColor = new ColorButton(this, QColor(_settings->data("COLOR_REF1")));
  _refColor->setColorButton();
  form->addRow(tr("Color"), _refColor);

  // ref
  _ref = new QDoubleSpinBox;
  _ref->setRange(-100000, 100000);
  _ref->setValue(_settings->getDouble("_REF1"));
  form->addRow(tr("Value"), _ref);

  _tabs->addTab(w, "REF 1");
}

void STOCH_FASTDialog::createRef2Page ()
{
  QWidget *w = new QWidget;

  QFormLayout *form = new QFormLayout;
  w->setLayout(form);

  // color
  _ref2Color = new ColorButton(this, QColor(_settings->data("COLOR_REF2")));
  _ref2Color->setColorButton();
  form->addRow(tr("Color"), _ref2Color);

  // ref
  _ref2 = new QDoubleSpinBox;
  _ref2->setRange(-100000, 100000);
  _ref2->setValue(_settings->getDouble("REF2"));
  form->addRow(tr("Value"), _ref2);

  _tabs->addTab(w, "REF 2");
}

void STOCH_FASTDialog::save ()
{
  _settings->setData("COLOR_K", _kcolor->color().name());
  _settings->setData("COLOR_D", _dcolor->color().name());
  _settings->setData("STYLE_K", _kstyle->currentText());
  _settings->setData("STYLE_D", _dstyle->currentText());
  _settings->setData("PERIOD_FASTD", _dperiod->value());
  _settings->setData("PERIOD_FASTK", _kperiod->value());
  _settings->setData("MA_TYPE_FASTD", _maType->currentText());
  _settings->setData("REF1", _ref->value());
  _settings->setData("REF2", _ref2->value());
  _settings->setData("COLOR_REF1", _refColor->color().name());
  _settings->setData("COLOR_REF2", _ref2Color->color().name());
  _settings->setData("OUTPUT_K", _koutput->text());
  _settings->setData("Z_K", _zk->text());
  _settings->setData("OUTPUT_D", _doutput->text());
  _settings->setData("Z_D", _zd->text());
}