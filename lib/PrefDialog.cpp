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

#include "PrefDialog.h"
#include <qlabel.h>

PrefDialog::PrefDialog () : QTabDialog (0, "PrefDialog", TRUE)
{
  widgetList.setAutoDelete(TRUE);
  gridList.setAutoDelete(TRUE);
  colorButtonList.setAutoDelete(TRUE);
  intList.setAutoDelete(TRUE);
  floatList.setAutoDelete(TRUE);
  checkList.setAutoDelete(TRUE);
  fontButtonList.setAutoDelete(TRUE);
  textList.setAutoDelete(TRUE);
  comboList.setAutoDelete(TRUE);
  dateList.setAutoDelete(TRUE);
  fileList.setAutoDelete(TRUE);
  symbolList.setAutoDelete(TRUE);
  dvList.setAutoDelete(TRUE);
  
  resize(300, 200);
  
  setOkButton();
  setCancelButton();
}

PrefDialog::~PrefDialog ()
{
}

void PrefDialog::createPage (QString name)
{
  QWidget *w = new QWidget(this);
  widgetList.append(w);
  
  QGridLayout *grid = new QGridLayout(w, 1, 2);
  grid->setMargin(5);
  grid->setSpacing(5);
  gridList.append(grid);
  grid->setColStretch(1, 1);

  addTab(w, name);
}

void PrefDialog::addColorItem (QString name, int page, QColor color)
{
  QWidget *w = widgetList.at(page - 1);
  
  QGridLayout *grid = gridList.at(page - 1);
  grid->expand(grid->numRows() + 1, grid->numCols());
  
  QLabel *label = new QLabel(name, w);
  grid->addWidget(label, grid->numRows() - 2, 0);
  
  ColorButton *button = new ColorButton(w, color);
  grid->addWidget(button, grid->numRows() - 2, 1);
  button->setColorButton();
  colorButtonList.insert(name, button);
}

QColor PrefDialog::getColor (QString name)
{
  QColor color;
  
  ColorButton *button = colorButtonList[name];
  if (button)
    color = button->getColor();
    
  return color;
}

void PrefDialog::addFloatItem (QString name, int page, double num)
{
  addFloatItem(name, page, num, -9999999999.0, 9999999999.0);
}

void PrefDialog::addFloatItem (QString name, int page, double num, double low, double high)
{
  QWidget *w = widgetList.at(page - 1);
  
  QGridLayout *grid = gridList.at(page - 1);
  grid->expand(grid->numRows() + 1, grid->numCols());
  
  QLabel *label = new QLabel(name, w);
  grid->addWidget(label, grid->numRows() - 2, 0);
  
  QDoubleValidator *dv = new QDoubleValidator(low, high, 4, this, 0);
  dvList.insert(name, dv);
  
  QLineEdit *edit = new QLineEdit(QString::number(num), w);
  edit->setValidator(dv);
  grid->addWidget(edit, grid->numRows() - 2, 1);
  floatList.insert(name, edit);
}

double PrefDialog::getFloat (QString name)
{
  double num = 0;
  
  QLineEdit *edit = floatList[name];
  if (edit)
    num = edit->text().toFloat();
    
  return num;
}

void PrefDialog::addIntItem (QString name, int page, int num)
{
  addIntItem(name, page, num, -999999999, 999999999);
}

void PrefDialog::addIntItem (QString name, int page, int num, int min, int max)
{
  QWidget *w = widgetList.at(page - 1);
  
  QGridLayout *grid = gridList.at(page - 1);
  grid->expand(grid->numRows() + 1, grid->numCols());
  
  QLabel *label = new QLabel(name, w);
  grid->addWidget(label, grid->numRows() - 2, 0);
  
  QSpinBox *spin = new QSpinBox(min, max, 1, w);
  spin->setValue(num);
  grid->addWidget(spin, grid->numRows() - 2, 1);
  intList.insert(name, spin);
}

int PrefDialog::getInt (QString name)
{
  int num = 0;
  
  QSpinBox *spin = intList[name];
  if (spin)
    num = spin->value();
    
  return num;
}

void PrefDialog::addCheckItem (QString name, int page, QString flag)
{
  if (! flag.compare("True"))
    addCheckItem(name, page, TRUE);
  else
    addCheckItem(name, page, FALSE);
}

void PrefDialog::addCheckItem (QString name, int page, bool flag)
{
  QWidget *w = widgetList.at(page - 1);
  
  QGridLayout *grid = gridList.at(page - 1);
  grid->expand(grid->numRows() + 1, grid->numCols());
  
  QCheckBox *check = new QCheckBox(name, w);
  check->setChecked(flag);
  grid->addWidget(check, grid->numRows() - 2, 0);
  checkList.insert(name, check);
}

bool PrefDialog::getCheck (QString name)
{
  bool flag = FALSE;
  
  QCheckBox *check = checkList[name];
  if (check)
    flag = check->isChecked();
    
  return flag;
}

QString PrefDialog::getCheckString (QString name)
{
  QString flag = FALSE;
  
  QCheckBox *check = checkList[name];
  if (check)
  {
    if (check->isChecked())
      flag = "True";
    else
      flag = "False";
  }    
    
  return flag;
}

void PrefDialog::addFontItem (QString name, int page, QFont font)
{
  QWidget *w = widgetList.at(page - 1);
  
  QGridLayout *grid = gridList.at(page - 1);
  grid->expand(grid->numRows() + 1, grid->numCols());
  
  QLabel *label = new QLabel(name, w);
  grid->addWidget(label, grid->numRows() - 2, 0);
  
  FontButton *button = new FontButton(w, font);
  grid->addWidget(button, grid->numRows() - 2, 1);
  fontButtonList.insert(name, button);
}

QFont PrefDialog::getFont (QString name)
{
  QFont font;
  
  FontButton *button = fontButtonList[name];
  if (button)
    font = button->getFont();
    
  return font;
}

void PrefDialog::addTextItem (QString name, int page, QString t)
{
  QWidget *w = widgetList.at(page - 1);
  
  QGridLayout *grid = gridList.at(page - 1);
  grid->expand(grid->numRows() + 1, grid->numCols());
  
  QLabel *label = new QLabel(name, w);
  grid->addWidget(label, grid->numRows() - 2, 0);

  QLineEdit *edit = new QLineEdit(t, w);
  grid->addWidget(edit, grid->numRows() - 2, 1);
  textList.insert(name, edit);
}

QString PrefDialog::getText (QString name)
{
  QString s;
  
  QLineEdit *edit = textList[name];
  if (edit)
    s = edit->text();
    
  return s;
}

void PrefDialog::addComboItem (QString name, int page, QStringList l, QString s)
{
  QWidget *w = widgetList.at(page - 1);
  
  QGridLayout *grid = gridList.at(page - 1);
  grid->expand(grid->numRows() + 1, grid->numCols());
  
  QLabel *label = new QLabel(name, w);
  grid->addWidget(label, grid->numRows() - 2, 0);

  QComboBox *combo = new QComboBox(w);
  combo->insertStringList(l, -1);
  if (s.length())
    combo->setCurrentText(s);
  grid->addWidget(combo, grid->numRows() - 2, 1);
  comboList.insert(name, combo);
}

void PrefDialog::addComboItem (QString name, int page, QStringList l, int s)
{
  QWidget *w = widgetList.at(page - 1);
  
  QGridLayout *grid = gridList.at(page - 1);
  grid->expand(grid->numRows() + 1, grid->numCols());
  
  QLabel *label = new QLabel(name, w);
  grid->addWidget(label, grid->numRows() - 2, 0);

  QComboBox *combo = new QComboBox(w);
  combo->insertStringList(l, -1);
  combo->setCurrentItem(s);
  grid->addWidget(combo, grid->numRows() - 2, 1);
  comboList.insert(name, combo);
}

QString PrefDialog::getCombo (QString name)
{
  QString s;
  
  QComboBox *combo = comboList[name];
  if (combo)
    s = combo->currentText();
    
  return s;
}

int PrefDialog::getComboIndex (QString name)
{
  int i = 0;
  
  QComboBox *combo = comboList[name];
  if (combo)
    i = combo->currentItem();
    
  return i;
}

void PrefDialog::addDateItem (QString name, int page, QDateTime dt)
{
  QWidget *w = widgetList.at(page - 1);
  
  QGridLayout *grid = gridList.at(page - 1);
  grid->expand(grid->numRows() + 1, grid->numCols());
  
  QLabel *label = new QLabel(name, w);
  grid->addWidget(label, grid->numRows() - 2, 0);

  QDateEdit *date = new QDateEdit(dt.date(), w);
  date->setAutoAdvance(TRUE);
  date->setOrder(QDateEdit::YMD);
  grid->addWidget(date, grid->numRows() - 2, 1);
  dateList.insert(name, date);
}

QDateTime PrefDialog::getDate (QString name)
{
  QDateTime dt;
  
  QDateEdit *date = dateList[name];
  if (date)
    dt.setDate(date->date());
    
  return dt;
}

void PrefDialog::addFileItem (QString name, int page)
{
  QWidget *w = widgetList.at(page - 1);
  
  QGridLayout *grid = gridList.at(page - 1);
  grid->expand(grid->numRows() + 1, grid->numCols());
  
  QLabel *label = new QLabel(name, w);
  grid->addWidget(label, grid->numRows() - 2, 0);

  FileButton *button = new FileButton(w);
  grid->addWidget(button, grid->numRows() - 2, 1);
  fileList.insert(name, button);
}

QStringList PrefDialog::getFile (QString name)
{
  QStringList l;
  
  FileButton *button = fileList[name];
  if (button)
    l = button->getFile();
    
  return l;
}

void PrefDialog::addSymbolItem (QString name, int page, QString path, QString symbol)
{
  QWidget *w = widgetList.at(page - 1);
  
  QGridLayout *grid = gridList.at(page - 1);
  grid->expand(grid->numRows() + 1, grid->numCols());
  
  QLabel *label = new QLabel(name, w);
  grid->addWidget(label, grid->numRows() - 2, 0);

  SymbolButton *button = new SymbolButton(w, path, symbol);
  grid->addWidget(button, grid->numRows() - 2, 1);
  symbolList.insert(name, button);
}

QString PrefDialog::getSymbol (QString name)
{
  QString s;
  
  SymbolButton *button = symbolList[name];
  if (button)
    s = button->getPath();
    
  return s;
}

