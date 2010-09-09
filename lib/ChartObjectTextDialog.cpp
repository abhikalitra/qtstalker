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

#include "ChartObjectTextDialog.h"
#include "Config.h"
#include "ChartObject.h"

#include <QtDebug>
#include <QLayout>
#include <QLabel>
#include <QString>

ChartObjectTextDialog::ChartObjectTextDialog ()
{
  setWindowTitle(tr("Edit Text Chart Object"));

  createMainPage();
}

void ChartObjectTextDialog::createMainPage ()
{
  QWidget *w = new QWidget;

  QGridLayout *grid = new QGridLayout;
  grid->setSpacing(2);
  grid->setColumnStretch(1, 1);
  w->setLayout(grid);

  int row = 0;
  int col = 0;

  // date
  QLabel *label = new QLabel(tr("Date"));
  grid->addWidget(label, row, col++);

  _date = new QDateTimeEdit;
  _date->setCalendarPopup(TRUE);
  grid->addWidget(_date, row++, col--);


  // color
  label = new QLabel(tr("Color"));
  grid->addWidget(label, row, col++);

  QColor c(Qt::red);
  _color = new ColorButton(this, c);
  grid->addWidget(_color, row++, col--);
  _color->setColorButton();


  // font
  label = new QLabel(tr("Font"));
  grid->addWidget(label, row, col++);

  QFont f;
  _font = new FontButton(this, f);
  grid->addWidget(_font, row++, col--);
  _font->setFontButton();

  
  // price
  label = new QLabel(tr("Price"));
  grid->addWidget(label, row, col++);

  _price = new QDoubleSpinBox;
  _price->setRange(0.0, 99999999.0);
  grid->addWidget(_price, row++, col--);


  // label
  label = new QLabel(tr("Text"));
  grid->addWidget(label, row, col++);

  _label = new QLineEdit;
  grid->addWidget(_label, row++, col--);


  // default
  label = new QLabel(tr("Set as default"));
  grid->addWidget(label, row, col++);

  _default = new QCheckBox;
  grid->addWidget(_default, row++, col--);

  grid->setRowStretch(row, 1);

  _tabs->addTab(w, tr("Settings"));
}

void ChartObjectTextDialog::done ()
{
  if (_default->isChecked())
  {
    Config config;
    config.setData(Config::DefaultChartObjectTextColor, _color->color());
    config.setData(Config::DefaultChartObjectTextFont, _font->font());
  }

  _settings.color = _color->color();
  _settings.date = _date->dateTime();
  _settings.price = _price->value();
  _settings.text = _label->text();
  _settings.font = _font->font();

  emit signalDone(_settings);

  accept();
}

void ChartObjectTextDialog::setSettings (ChartObjectSettings &d)
{
  _settings = d;
  
  _color->setColor(_settings.color);
  _date->setDateTime(_settings.date);
  _price->setValue(_settings.price);
  _font->setFont(_settings.font);
  _label->setText(_settings.text);
}

