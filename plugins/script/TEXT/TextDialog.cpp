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

#include "TextDialog.h"
#include "Globals.h"
#include "Strip.h"

#include <QtDebug>
#include <QSettings>
#include <QFormLayout>

TextDialog::TextDialog (QWidget *p, Setting *set) : QWidget (p)
{
  _co = set;
  createGUI();
}

void TextDialog::createGUI ()
{
  QFormLayout *form = new QFormLayout;
  setLayout(form);

  // date
  _date = new QDateTimeEdit;
  _date->setCalendarPopup(TRUE);
  _date->setDateTime(_co->dateTime("DATE"));
  form->addRow(tr("Date"), _date);

  // color
  _color = new ColorButton(this, QColor(_co->color("COLOR")));
  form->addRow(tr("Color"), _color);
  
  // font
  _font = new FontButton(this, QFont(_co->font("FONT")));
  form->addRow(tr("Font"), _font);

  // price
  _price = new QDoubleSpinBox;
  _price->setRange(0.0, 99999999.0);
  _price->setDecimals(4);
  _price->setValue(_co->getDouble("PRICE"));
  form->addRow(tr("Price"), _price);

  // text
  _text = new LineEdit(this);
  _text->setText(_co->data("TEXT"));
  form->addRow(tr("Text"), _text);

  // default
  _default = new QCheckBox;
  form->addRow(tr("Set as default"), _default);

  // z
  _z = new QSpinBox;
  _z->setRange(-1, 99);
  _z->setValue(_co->getInt("Z"));
  form->addRow(tr("Plot Order"), _z);
}

void TextDialog::save ()
{
  if (_default->isChecked())
  {
    QSettings settings(g_globalSettings);
    settings.setValue("default_text_color", _color->color().name());
    settings.setValue("default_text_font", _font->font());
    settings.sync();
  }

  _co->setData("COLOR", _color->color());
  _co->setData("PRICE", _price->value());
  _co->setData("DATE", _date->dateTime());

  QString label = _text->text();
  Strip strip;
  strip.verifyText(label);
  _co->setData("TEXT", label);
  
  _co->setData("FONT", _font->font());
  _co->setData("Z", _z->text());
}
