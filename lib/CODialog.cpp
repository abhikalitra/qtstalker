/*
 *  Qtstalker stock charter
 *
 *  Copyright (C) 2001-2007 Stefan S. Stratigakos
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


#include "CODialog.h"
#include "DataBase.h"
#include "Config.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QIcon>
#include <QFont>
#include <QMessageBox>
#include <QtDebug>
#include <QLabel>





CODialog::CODialog (QWidget *w, COSettings *c) : QDialog (w, 0)
{
  co = c;
  
  QVBoxLayout *vbox = new QVBoxLayout;
  setLayout(vbox);

  tabs = new QTabWidget;
  vbox->addWidget(tabs);

  buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel | QDialogButtonBox::Help);
  connect(buttonBox, SIGNAL(accepted()), this, SLOT(slotSave()));
  connect(buttonBox, SIGNAL(rejected()), this, SLOT(cancelPressed()));
  vbox->addWidget(buttonBox);

  QString s;
  co->getData(COSettings::COCOType, s);
  switch ((COSettings::COType) s.toInt())
  {
    case COSettings::COBuyArrow:
      createArrowPage();
      setWindowTitle(tr("Edit Buy Arrow"));
      break;
    case COSettings::COSellArrow:
      createArrowPage();
      setWindowTitle(tr("Edit Sell Arrow"));
      break;
    case COSettings::COHorizontalLine:
      createHorizontalLinePage();
      break;
    case COSettings::COText:
      createTextPage();
      break;
    case COSettings::COFiboLine:
      createFiboLinePage();
      break;
    case COSettings::COTrendLine:
      createTrendLinePage();
      break;
    case COSettings::COVerticalLine:
      createVerticalLinePage();
      break;
    default:
      break;
  }
  
  modified = 0;
}

void CODialog::slotModified ()
{
  modified = TRUE;
}

void CODialog::createArrowPage ()
{
  QWidget *w = new QWidget;

  QVBoxLayout *vbox = new QVBoxLayout;
  w->setLayout(vbox);
  
  QGridLayout *grid = new QGridLayout;
  grid->setMargin(5);
  grid->setSpacing(5);
  grid->setColumnStretch(1, 2);
  vbox->addLayout(grid);

  int row = 0;
  int col = 0;

  // color
  QLabel *label = new QLabel(tr("Color"));
  grid->addWidget(label, row, col++);
  
  QColor color;
  co->getData(COSettings::COColor, color);
  colorButton = new ColorButton(w, color);
  grid->addWidget(colorButton, row++, col--);
  connect(colorButton, SIGNAL(valueChanged()), this, SLOT(slotModified()));

  // price
  label = new QLabel(tr("Price"));
  grid->addWidget(label, row, col++);
  
  dspin = new QDoubleSpinBox;
  grid->addWidget(dspin, row++, col--);
  double td = 0;
  co->getData(COSettings::COValue, td);
  dspin->setValue(td);
  connect(dspin, SIGNAL(valueChanged(double)), this, SLOT(slotModified()));
  
  // default
  label = new QLabel(tr("Set Default"));
  grid->addWidget(label, row, col++);
  
  checkDefault = new QCheckBox;
  grid->addWidget(checkDefault, row++, col--);
  connect(checkDefault, SIGNAL(toggled(bool)), this, SLOT(slotModified()));
  
  grid->setRowStretch(row, 2);

  tabs->addTab(w, tr("Parms"));
}

void CODialog::createHorizontalLinePage ()
{
  setWindowTitle(tr("Edit Horizontal Line"));
      
  QWidget *w = new QWidget;

  QVBoxLayout *vbox = new QVBoxLayout;
  w->setLayout(vbox);
  
  QGridLayout *grid = new QGridLayout;
  grid->setMargin(5);
  grid->setSpacing(5);
  grid->setColumnStretch(1, 2);
  vbox->addLayout(grid);

  int row = 0;
  int col = 0;

  // color
  QLabel *label = new QLabel(tr("Color"));
  grid->addWidget(label, row, col++);
  
  QColor color;
  co->getData(COSettings::COColor, color);
  colorButton = new ColorButton(w, color);
  grid->addWidget(colorButton, row++, col--);
  connect(colorButton, SIGNAL(valueChanged()), this, SLOT(slotModified()));

  // price
  label = new QLabel(tr("Price"));
  grid->addWidget(label, row, col++);
  
  dspin = new QDoubleSpinBox;
  grid->addWidget(dspin, row++, col--);
  double td = 0;
  co->getData(COSettings::COValue, td);
  dspin->setValue(td);
  connect(dspin, SIGNAL(valueChanged(double)), this, SLOT(slotModified()));
  
  // text
  label = new QLabel(tr("Text"));
  grid->addWidget(label, row, col++);
  
  text = new QLineEdit;
  grid->addWidget(text, row++, col--);
  QString s;
  co->getData(COSettings::COLabel, s);
  text->setText(s);
  connect(text, SIGNAL(textChanged(const QString &)), this, SLOT(slotModified()));
  
  // default
  label = new QLabel(tr("Set Default"));
  grid->addWidget(label, row, col++);
  
  checkDefault = new QCheckBox;
  grid->addWidget(checkDefault, row++, col--);
  connect(checkDefault, SIGNAL(toggled(bool)), this, SLOT(slotModified()));
  
  grid->setRowStretch(row, 2);

  tabs->addTab(w, tr("Parms"));
}

void CODialog::createTextPage ()
{
  setWindowTitle(tr("Edit Text"));
      
  QWidget *w = new QWidget;

  QVBoxLayout *vbox = new QVBoxLayout;
  w->setLayout(vbox);
  
  QGridLayout *grid = new QGridLayout;
  grid->setMargin(5);
  grid->setSpacing(5);
  grid->setColumnStretch(1, 2);
  vbox->addLayout(grid);

  int row = 0;
  int col = 0;

  // color
  QLabel *label = new QLabel(tr("Color"));
  grid->addWidget(label, row, col++);
  
  QColor color;
  co->getData(COSettings::COColor, color);
  colorButton = new ColorButton(w, color);
  grid->addWidget(colorButton, row++, col--);
  connect(colorButton, SIGNAL(valueChanged()), this, SLOT(slotModified()));

  // price
  label = new QLabel(tr("Price"));
  grid->addWidget(label, row, col++);
  
  dspin = new QDoubleSpinBox;
  grid->addWidget(dspin, row++, col--);
  double td = 0;
  co->getData(COSettings::COValue, td);
  dspin->setValue(td);
  connect(dspin, SIGNAL(valueChanged(double)), this, SLOT(slotModified()));
  
  // text
  label = new QLabel(tr("Text"));
  grid->addWidget(label, row, col++);
  
  text = new QLineEdit;
  grid->addWidget(text, row++, col--);
  QString s;
  co->getData(COSettings::COLabel, s);
  text->setText(s);
  connect(text, SIGNAL(textChanged(const QString &)), this, SLOT(slotModified()));
  
  // font
  label = new QLabel(tr("Font"));
  grid->addWidget(label, row, col++);

  QFont font;
  co->getData(COSettings::COFont, font);
  fontButton = new FontButton(w, font);
  grid->addWidget(fontButton, row++, col--);
  connect(fontButton, SIGNAL(valueChanged()), this, SLOT(slotModified()));
  
  // default
  label = new QLabel(tr("Set Default"));
  grid->addWidget(label, row, col++);
  
  checkDefault = new QCheckBox;
  grid->addWidget(checkDefault, row++, col--);
  connect(checkDefault, SIGNAL(toggled(bool)), this, SLOT(slotModified()));
  
  grid->setRowStretch(row, 2);

  tabs->addTab(w, tr("Parms"));
}

void CODialog::createFiboLinePage ()
{
  setWindowTitle(tr("Edit Fibo Line"));
  
  QWidget *w = new QWidget;

  QVBoxLayout *vbox = new QVBoxLayout;
  w->setLayout(vbox);
  
  QGridLayout *grid = new QGridLayout;
  grid->setMargin(5);
  grid->setSpacing(5);
  grid->setColumnStretch(1, 2);
  vbox->addLayout(grid);

  int row = 0;
  int col = 0;

  // color
  QLabel *label = new QLabel(tr("Color"));
  grid->addWidget(label, row, col++);
  
  QColor color;
  co->getData(COSettings::COColor, color);
  colorButton = new ColorButton(w, color);
  grid->addWidget(colorButton, row++, col--);
  connect(colorButton, SIGNAL(valueChanged()), this, SLOT(slotModified()));
  
  // line1
  label = new QLabel(tr("Line 1"));
  grid->addWidget(label, row, col++);
  
  dspin = new QDoubleSpinBox;
  grid->addWidget(dspin, row++, col--);
  double td = 0;
  co->getData(COSettings::COLine1, td);
  dspin->setValue(td);
  connect(dspin, SIGNAL(valueChanged(double)), this, SLOT(slotModified()));
  
  // line2
  label = new QLabel(tr("Line 2"));
  grid->addWidget(label, row, col++);
  
  dspin2 = new QDoubleSpinBox;
  grid->addWidget(dspin2, row++, col--);
  co->getData(COSettings::COLine2, td);
  dspin2->setValue(td);
  connect(dspin2, SIGNAL(valueChanged(double)), this, SLOT(slotModified()));
  
  // line3
  label = new QLabel(tr("Line 3"));
  grid->addWidget(label, row, col++);
  
  dspin3 = new QDoubleSpinBox;
  grid->addWidget(dspin3, row++, col--);
  co->getData(COSettings::COLine3, td);
  dspin3->setValue(td);
  connect(dspin3, SIGNAL(valueChanged(double)), this, SLOT(slotModified()));
  
  // line4
  label = new QLabel(tr("Line 4"));
  grid->addWidget(label, row, col++);
  
  dspin4 = new QDoubleSpinBox;
  grid->addWidget(dspin4, row++, col--);
  co->getData(COSettings::COLine4, td);
  dspin4->setValue(td);
  connect(dspin4, SIGNAL(valueChanged(double)), this, SLOT(slotModified()));
  
  // line5
  label = new QLabel(tr("Line 5"));
  grid->addWidget(label, row, col++);
  
  dspin5 = new QDoubleSpinBox;
  grid->addWidget(dspin5, row++, col--);
  co->getData(COSettings::COLine5, td);
  dspin5->setValue(td);
  connect(dspin5, SIGNAL(valueChanged(double)), this, SLOT(slotModified()));
  
  // line6
  label = new QLabel(tr("Line 6"));
  grid->addWidget(label, row, col++);
  
  dspin6 = new QDoubleSpinBox;
  grid->addWidget(dspin6, row++, col--);
  co->getData(COSettings::COLine6, td);
  dspin6->setValue(td);
  connect(dspin6, SIGNAL(valueChanged(double)), this, SLOT(slotModified()));
  
  // high
  label = new QLabel(tr("High Point"));
  grid->addWidget(label, row, col++);
  
  dspin7 = new QDoubleSpinBox;
  grid->addWidget(dspin7, row++, col--);
  co->getData(COSettings::COHigh, td);
  dspin7->setValue(td);
  connect(dspin7, SIGNAL(valueChanged(double)), this, SLOT(slotModified()));
  
  // low
  label = new QLabel(tr("Low Point"));
  grid->addWidget(label, row, col++);
  
  dspin8 = new QDoubleSpinBox;
  grid->addWidget(dspin8, row++, col--);
  co->getData(COSettings::COLow, td);
  dspin8->setValue(td);
  connect(dspin8, SIGNAL(valueChanged(double)), this, SLOT(slotModified()));
  
  // extend
  label = new QLabel(tr("Extend"));
  grid->addWidget(label, row, col++);
  
  check = new QCheckBox;
  int ti = 0;
  co->getData(COSettings::COExtend, ti);
  check->setChecked((bool) ti);
  grid->addWidget(check, row++, col--);
  connect(check, SIGNAL(toggled(bool)), this, SLOT(slotModified()));
  
  // default
  label = new QLabel(tr("Set Default"));
  grid->addWidget(label, row, col++);
  
  checkDefault = new QCheckBox;
  grid->addWidget(checkDefault, row++, col--);
  connect(checkDefault, SIGNAL(toggled(bool)), this, SLOT(slotModified()));
  
  grid->setRowStretch(row, 2);

  tabs->addTab(w, tr("Parms"));
}

void CODialog::createTrendLinePage ()
{
  setWindowTitle(tr("Edit Trend Line"));

  QWidget *w = new QWidget;

  QVBoxLayout *vbox = new QVBoxLayout;
  w->setLayout(vbox);
  
  QGridLayout *grid = new QGridLayout;
  grid->setMargin(5);
  grid->setSpacing(5);
  grid->setColumnStretch(1, 2);
  vbox->addLayout(grid);

  int row = 0;
  int col = 0;

  // color
  QLabel *label = new QLabel(tr("Color"));
  grid->addWidget(label, row, col++);
  
  QColor color;
  co->getData(COSettings::COColor, color);
  colorButton = new ColorButton(w, color);
  grid->addWidget(colorButton, row++, col--);
  connect(colorButton, SIGNAL(valueChanged()), this, SLOT(slotModified()));

  // start price
  label = new QLabel(tr("Start Price"));
  grid->addWidget(label, row, col++);
  
  dspin = new QDoubleSpinBox;
  grid->addWidget(dspin, row++, col--);
  double td = 0;
  co->getData(COSettings::COValue, td);
  dspin->setValue(td);
  connect(dspin, SIGNAL(valueChanged(double)), this, SLOT(slotModified()));
  
  // end price
  label = new QLabel(tr("End Price"));
  grid->addWidget(label, row, col++);
  
  dspin2 = new QDoubleSpinBox;
  grid->addWidget(dspin2, row++, col--);
  co->getData(COSettings::COValue2, td);
  dspin2->setValue(td);
  connect(dspin2, SIGNAL(valueChanged(double)), this, SLOT(slotModified()));
  
  // bar field
  label = new QLabel(tr("Bar Field"));
  grid->addWidget(label, row, col++);
  
  QStringList l;
  l.append(tr("Open"));
  l.append(tr("High"));
  l.append(tr("Low"));
  l.append(tr("Close"));

  combo = new QComboBox;
  grid->addWidget(combo, row++, col--);
  combo->addItems(l);
  QString s;
  co->getData(COSettings::COBarField, s);
  combo->setCurrentIndex(l.indexOf(s));
  connect(combo, SIGNAL(currentIndexChanged(int)), this, SLOT(slotModified()));
  
  // use bar
  label = new QLabel(tr("Use Bar"));
  grid->addWidget(label, row, col++);
  
  check = new QCheckBox;
  int ti = 0;
  co->getData(COSettings::COUseBar, ti);
  check->setChecked((bool) ti);
  grid->addWidget(check, row++, col--);
  connect(check, SIGNAL(toggled(bool)), this, SLOT(slotModified()));
  
  // extend
  label = new QLabel(tr("Extend"));
  grid->addWidget(label, row, col++);
  
  check2 = new QCheckBox;
  co->getData(COSettings::COExtend, ti);
  check2->setChecked((bool) ti);
  grid->addWidget(check2, row++, col--);
  connect(check2, SIGNAL(toggled(bool)), this, SLOT(slotModified()));
  
  // default
  label = new QLabel(tr("Set Default"));
  grid->addWidget(label, row, col++);
  
  checkDefault = new QCheckBox;
  grid->addWidget(checkDefault, row++, col--);
  connect(checkDefault, SIGNAL(toggled(bool)), this, SLOT(slotModified()));
  
  grid->setRowStretch(row, 2);

  tabs->addTab(w, tr("Parms"));
}

void CODialog::createVerticalLinePage ()
{
  setWindowTitle(tr("Edit Vertical Line"));
      
  QWidget *w = new QWidget;

  QVBoxLayout *vbox = new QVBoxLayout;
  w->setLayout(vbox);
  
  QGridLayout *grid = new QGridLayout;
  grid->setMargin(5);
  grid->setSpacing(5);
  grid->setColumnStretch(1, 2);
  vbox->addLayout(grid);

  int row = 0;
  int col = 0;

  // color
  QLabel *label = new QLabel(tr("Color"));
  grid->addWidget(label, row, col++);
  
  QColor color;
  co->getData(COSettings::COColor, color);
  colorButton = new ColorButton(w, color);
  grid->addWidget(colorButton, row++, col--);
  connect(colorButton, SIGNAL(valueChanged()), this, SLOT(slotModified()));

  // default
  label = new QLabel(tr("Set Default"));
  grid->addWidget(label, row, col++);
  
  checkDefault = new QCheckBox;
  grid->addWidget(checkDefault, row++, col--);
  connect(checkDefault, SIGNAL(toggled(bool)), this, SLOT(slotModified()));
  
  grid->setRowStretch(row, 2);

  tabs->addTab(w, tr("Parms"));
}

void CODialog::cancelPressed()
{
  if (modified)
  {
    int rc = QMessageBox::warning(this,
    			        tr("Qtstalker: Warning"),
			        tr("Item modified. Are you sure you want to discard changes?"),
			        QMessageBox::Yes,
			        QMessageBox::No,
			        QMessageBox::NoButton);
    if (rc == QMessageBox::No)
      return;
  }

  reject();
}

void CODialog::slotSave()
{
  // save color
  QColor color;
  colorButton->getColor(color);
  co->setData(COSettings::COColor, color);
  
  Config config;
  double td = 0;
  int ti = 0;
  QFont font;
  QString s;
  co->getData(COSettings::COCOType, s);
  switch ((COSettings::COType) s.toInt())
  {
    case COSettings::COBuyArrow:
      if (checkDefault->isChecked())
	config.setData(Config::DefaultBuyArrowColor, color);
      
      td = dspin->value();
      co->setData(COSettings::COValue, td);
      break;
    case COSettings::COSellArrow:
      if (checkDefault->isChecked())
	config.setData(Config::DefaultSellArrowColor, color);
      
      td = dspin->value();
      co->setData(COSettings::COValue, td);
      break;
    case COSettings::COHorizontalLine:
      if (checkDefault->isChecked())
	config.setData(Config::DefaultHorizontalLineColor, color);
      
      td = dspin->value();
      co->setData(COSettings::COValue, td);

      s = text->text();
      co->setData(COSettings::COLabel, s);
      break;
    case COSettings::COText:
      if (checkDefault->isChecked())
	config.setData(Config::DefaultTextColor, color);
      
      td = dspin->value();
      co->setData(COSettings::COValue, td);

      s = text->text();
      co->setData(COSettings::COLabel, s);
      if (checkDefault->isChecked())
	config.setData(Config::DefaultTextLabel, s);
      
      fontButton->getFont(font);
      co->setData(COSettings::COFont, font);
      if (checkDefault->isChecked())
	config.setData(Config::DefaultTextFont, font);
      break;
    case COSettings::COFiboLine:
      if (checkDefault->isChecked())
	config.setData(Config::DefaultFiboLineColor, color);
      
      td = dspin->value();
      co->setData(COSettings::COLine1, td);
      if (checkDefault->isChecked())
	config.setData(Config::DefaultFiboLine1, td);

      td = dspin2->value();
      co->setData(COSettings::COLine2, td);
      if (checkDefault->isChecked())
	config.setData(Config::DefaultFiboLine2, td);

      td = dspin3->value();
      co->setData(COSettings::COLine3, td);
      if (checkDefault->isChecked())
	config.setData(Config::DefaultFiboLine3, td);

      td = dspin4->value();
      co->setData(COSettings::COLine4, td);
      if (checkDefault->isChecked())
	config.setData(Config::DefaultFiboLine4, td);

      td = dspin5->value();
      co->setData(COSettings::COLine5, td);
      if (checkDefault->isChecked())
	config.setData(Config::DefaultFiboLine5, td);

      td = dspin6->value();
      co->setData(COSettings::COLine6, td);
      if (checkDefault->isChecked())
	config.setData(Config::DefaultFiboLine6, td);

      td = dspin7->value();
      co->setData(COSettings::COHigh, td);

      td = dspin8->value();
      co->setData(COSettings::COLow, td);

      ti = (int) check->isChecked();
      co->setData(COSettings::COExtend, ti);
      break;
    case COSettings::COTrendLine:
      if (checkDefault->isChecked())
	config.setData(Config::DefaultTrendLineColor, color);
      
      td = dspin->value();
      co->setData(COSettings::COValue, td);

      td = dspin2->value();
      co->setData(COSettings::COValue2, td);

      s = combo->currentText();
      co->setData(COSettings::COBarField, s);
      if (checkDefault->isChecked())
	config.setData(Config::DefaultTrendLineBar, s);
      
      ti = (int) check->isChecked();
      co->setData(COSettings::COUseBar, ti);
      if (checkDefault->isChecked())
	config.setData(Config::DefaultTrendLineUseBar, ti);
      
      ti = (int) check2->isChecked();
      co->setData(COSettings::COExtend, ti);
      if (checkDefault->isChecked())
	config.setData(Config::DefaultTrendLineExtend, ti);
      break;
    case COSettings::COVerticalLine:
      if (checkDefault->isChecked())
	config.setData(Config::DefaultVerticalLineColor, color);
      break;
    default:
      break;
  }
  
  accept();
}


