/*
 *  Qtstalker stock charter
 *
 *  Copyright (C) 2001-2004 Stefan S. Stratigakos
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

#include "BarDialog.h"
#include "HelpWindow.h"
#include <qinputdialog.h>
#include <qmessagebox.h>
#include <qlayout.h>
#include <qlabel.h>
#include <qframe.h>
#include <qdir.h>

BarDialog::BarDialog (QString d) : QTabDialog (0, "BarDialog", TRUE)
{
  helpFile = d;
  setCaption(tr("Bar Chart Parms"));
  createMainPage();
  createBarPage();
  createPaintBarPage();
  setOkButton();
  setCancelButton();
  setHelpButton();
  QObject::connect(this, SIGNAL(helpButtonPressed()), this, SLOT(help()));
  
  resize(400, 300);
}

BarDialog::~BarDialog ()
{
}

void BarDialog::createMainPage ()
{
  QWidget *w = new QWidget(this);
  
  QVBoxLayout *vbox = new QVBoxLayout(w);
  vbox->setMargin(5);
  vbox->setSpacing(0);
  
  QGridLayout *grid = new QGridLayout(vbox, 3, 2);
  grid->setMargin(5);
  grid->setSpacing(5);
  grid->setColStretch(1, 1);

  QStringList l;
  l.append("Bar");
  l.append("Paint Bar");
  
  QLabel *label = new QLabel(tr("Style"), w);
  grid->addWidget(label, 0, 0);
  
  style = new QComboBox(w);
  style->insertStringList(l, -1);
  grid->addWidget(style, 0, 1);

  label = new QLabel(tr("Min Bar Spacing"), w);
  grid->addWidget(label, 1, 0);
    
  spacing = new QSpinBox(4, 99, 1, w);
  grid->addWidget(spacing, 1, 1);

  addTab(w, tr("Parms"));
}  

void BarDialog::createBarPage ()
{
  QWidget *w = new QWidget(this);
  
  QVBoxLayout *vbox = new QVBoxLayout(w);
  vbox->setMargin(5);
  vbox->setSpacing(0);
  
  QGridLayout *grid = new QGridLayout(vbox, 4, 2);
  grid->setMargin(5);
  grid->setSpacing(5);
  grid->setColStretch(1, 1);
  
  QLabel *label = new QLabel(tr("Bar Up Color"), w);
  grid->addWidget(label, 0, 0);
  
  QColor c("green");
  barUpButton = new ColorButton(w, c);
  grid->addWidget(barUpButton, 0, 1);
  barUpButton->setColorButton();
  
  label = new QLabel(tr("Bar Down Color"), w);
  grid->addWidget(label, 1, 0);
  
  c.setNamedColor("red");
  barDownButton = new ColorButton(w, c);
  grid->addWidget(barDownButton, 1, 1);
  barDownButton->setColorButton();
  
  label = new QLabel(tr("Bar Neutral Color"), w);
  grid->addWidget(label, 2, 0);
  
  c.setNamedColor("blue");
  barNeutralButton = new ColorButton(w, c);
  grid->addWidget(barNeutralButton, 2, 1);
  barNeutralButton->setColorButton();
  
  addTab(w, tr("Bars"));
}

void BarDialog::createPaintBarPage ()
{
  QWidget *w = new QWidget(this);
  
  QVBoxLayout *vbox = new QVBoxLayout(w);
  vbox->setMargin(5);
  vbox->setSpacing(0);
  
  QGridLayout *grid = new QGridLayout(vbox, 2, 2);
  grid->setMargin(5);
  grid->setSpacing(5);
  grid->setColStretch(1, 1);
  
  QLabel *label = new QLabel(tr("Paint Bar Up Color"), w);
  grid->addWidget(label, 0, 0);
  
  QColor c("green");
  paintUpButton = new ColorButton(w, c);
  grid->addWidget(paintUpButton, 0, 1);
  paintUpButton->setColorButton();
  
  label = new QLabel(tr("Paint Bar Down Color"), w);
  grid->addWidget(label, 1, 0);
  
  c.setNamedColor("red");
  paintDownButton = new ColorButton(w, c);
  grid->addWidget(paintDownButton, 1, 1);
  paintDownButton->setColorButton();
  
  vbox->addSpacing(10);
  
  list = new FormulaEdit(w, FormulaEdit::Logic);
  vbox->addWidget(list);
  
  addTab(w, tr("Paint Bars"));
}

int BarDialog::getLines ()
{
  return list->getLines();
}

QString BarDialog::getLine (int row)
{
  return list->getLine(row);
}

void BarDialog::setLine (QString d)
{
  list->setLine(d);
}

void BarDialog::setBarColors (QColor bu, QColor bd, QColor bn)
{
  barUpButton->setColor(bu);
  barDownButton->setColor(bd);
  barNeutralButton->setColor(bn);
}

QColor BarDialog::getBarUpColor ()
{
  return barUpButton->getColor();
}

QColor BarDialog::getBarDownColor ()
{
  return barDownButton->getColor();
}

QColor BarDialog::getBarNeutralColor ()
{
  return barNeutralButton->getColor();
}

void BarDialog::setPaintBarColors (QColor bu, QColor bd)
{
  paintUpButton->setColor(bu);
  paintDownButton->setColor(bd);
}

QColor BarDialog::getPaintUpColor ()
{
  return paintUpButton->getColor();
}

QColor BarDialog::getPaintDownColor ()
{
  return paintDownButton->getColor();
}

void BarDialog::setStyle (QString d)
{
  style->setCurrentText(d);
}

QString BarDialog::getStyle ()
{
  return style->currentText();
}

void BarDialog::setSpacing (int d)
{
  spacing->setValue(d);
}

int BarDialog::getSpacing ()
{
  return spacing->value();
}

void BarDialog::help ()
{
  HelpWindow *hw = new HelpWindow(this, helpFile);
  hw->show();
  reject();
}


