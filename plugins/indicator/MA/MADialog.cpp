/*
 *  Qtstalker stock charter
 *
 *  Copyright (C) 2001-2005 Stefan S. Stratigakos
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

#include "MADialog.h"
#include "HelpWindow.h"
#include <qlayout.h>
#include <qvalidator.h>

MADialog::MADialog (QString d, bool f) : QTabDialog (0, "MADialog", TRUE)
{
  helpFile = d;
  customFlag = f;

  QWidget *w = new QWidget(this);
  
  QVBoxLayout *vbox = new QVBoxLayout(w);
  vbox->setMargin(5);
  vbox->setSpacing(0);
  
  QGridLayout *grid = new QGridLayout(vbox, 3, 2);
  grid->setMargin(5);
  grid->setSpacing(5);
  grid->setColStretch(1, 1);

  QLabel *label = new QLabel(tr("MA Type"), w);
  grid->addWidget(label, 0, 0);

  maType = new QComboBox(w);
  connect(maType, SIGNAL(activated(int)), this, SLOT(typeChanged(int)));
  grid->addWidget(maType, 0, 1);

  label = new QLabel(tr("Color"), w);
  grid->addWidget(label, 1, 0);

  QColor c("red");
  colorButton = new ColorButton(w, c);
  grid->addWidget(colorButton, 1, 1);

  label = new QLabel(tr("Line Type"), w);
  grid->addWidget(label, 2, 0);

  lineType = new QComboBox(w);
  grid->addWidget(lineType, 2, 1);

  label = new QLabel(tr("Label"), w);
  grid->addWidget(label, 3, 0);

  text = new QLineEdit(w);
  grid->addWidget(text, 3, 1);

  label = new QLabel(tr("Period"), w);
  grid->addWidget(label, 4, 0);

  period = new QSpinBox(1, 99999, 1, w);
  grid->addWidget(period, 4, 1);

  if (! customFlag)
  {
    label = new QLabel(tr("Input"), w);
    grid->addWidget(label, 5, 0);

    input = new QComboBox(w);
    grid->addWidget(input, 5, 1);
  }
  else
  {
    label = new QLabel(tr("Input"), w);
    grid->addWidget(label, 5, 0);

    QString s("1");
    customInput = new FormulaInput(w, FALSE, s);
    grid->addWidget(customInput, 5, 1);
  }

  freqLabel = new QLabel(tr("Freq"), w);
  grid->addWidget(freqLabel, 6, 0);

  QDoubleValidator *dv = new QDoubleValidator(.009, 99999999, 4, w, 0);
  
  freq = new QLineEdit(w);
  freq->setValidator(dv);
  grid->addWidget(freq, 6, 1);

  widthLabel = new QLabel(tr("Width"), w);
  grid->addWidget(widthLabel, 7, 0);

  dv = new QDoubleValidator(.009, 99999999, 4, w, 0);
  
  width = new QLineEdit(w);
  width->setValidator(dv);
  grid->addWidget(width, 7, 1);

  addTab(w, tr("Parms"));

  setOkButton();
  setCancelButton();
  setHelpButton();
  QObject::connect(this, SIGNAL(helpButtonPressed()), this, SLOT(help()));
}

MADialog::~MADialog ()
{
}

void MADialog::setColor (QColor d)
{
  colorButton->setColor(d);
}

QColor MADialog::getColor ()
{
  return colorButton->getColor();
}

void MADialog::setMAType (QStringList l, int d)
{
  maType->insertStringList(l, -1);
  maType->setCurrentItem(d);
  typeChanged(0);
}

int MADialog::getMAType ()
{
  return maType->currentItem();
}

void MADialog::setLabel (QString d)
{
  text->setText(d);
}

QString MADialog::getLabel ()
{
  return text->text();
}

void MADialog::setLineType (QStringList l, int d)
{
  lineType->insertStringList(l, -1);
  lineType->setCurrentItem(d);
}

int MADialog::getLineType ()
{
  return lineType->currentItem();
}

void MADialog::setPeriod (int d)
{
  period->setValue(d);
}

int MADialog::getPeriod ()
{
  return period->value();
}

void MADialog::setInput (QStringList l, int d)
{
  if (! customFlag)
  {
    input->insertStringList(l, -1);
    input->setCurrentItem(d);
  }
}

int MADialog::getInput ()
{
  return input->currentItem();
}

void MADialog::setCustomInput (QString d)
{
  if (customFlag)
    customInput->setInput(d);
}

QString MADialog::getCustomInput ()
{
  return customInput->getInput();
}

void MADialog::setFreq (double d)
{
  freq->setText(QString::number(d));
}

double MADialog::getFreq ()
{
  return freq->text().toDouble();
}

void MADialog::setWidth (double d)
{
  width->setText(QString::number(d));
}

double MADialog::getWidth ()
{
  return width->text().toDouble();
}

void MADialog::help ()
{
  HelpWindow *hw = new HelpWindow(this, helpFile);
  hw->show();
}

void MADialog::typeChanged (int)
{
  if (! maType->currentText().compare("Lowpass"))
  {
    freqLabel->show();
    freq->show();
    widthLabel->show();
    width->show();
  }
  else
  {
    freqLabel->hide();
    freq->hide();
    widthLabel->hide();
    width->hide();
  }
}


