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
#include "HelpWindow.h"
#include <qlabel.h>

PrefDialog::PrefDialog (QWidget *w) : QTabDialog (w, "PrefDialog", TRUE)
{
  init();
}

PrefDialog::PrefDialog () : QTabDialog (0, "PrefDialog", TRUE)
{
  init();
}

PrefDialog::~PrefDialog ()
{
  widgetList.clear();
  gridList.clear();
  colorButtonList.clear();
  intList.clear();
  floatList.clear();
  checkList.clear();
  fontButtonList.clear();
  textList.clear();
  comboList.clear();
  dateList.clear();
  fileList.clear();
  symbolList.clear();
  dvList.clear();
  formulaInputList.clear();
}

void PrefDialog::init ()
{
  widgetList.setAutoDelete(FALSE);
  gridList.setAutoDelete(FALSE);
  colorButtonList.setAutoDelete(FALSE);
  intList.setAutoDelete(FALSE);
  floatList.setAutoDelete(FALSE);
  checkList.setAutoDelete(FALSE);
  fontButtonList.setAutoDelete(FALSE);
  textList.setAutoDelete(FALSE);
  comboList.setAutoDelete(FALSE);
  dateList.setAutoDelete(FALSE);
  fileList.setAutoDelete(FALSE);
  symbolList.setAutoDelete(FALSE);
  dvList.setAutoDelete(FALSE);
  formulaInputList.setAutoDelete(FALSE);
  
  resize(300, 200);
  
  setOkButton(tr("&OK"));
  setCancelButton(tr("&Cancel"));
  setHelpButton(tr("&Help"));
  
  QObject::connect(this, SIGNAL(helpButtonPressed()), this, SLOT(help()));
}

void PrefDialog::createPage (QString name)
{
  QWidget *w = new QWidget(this);
  widgetList.replace(name, w);
  
  QGridLayout *grid = new QGridLayout(w, 1, 2);
  grid->setMargin(5);
  grid->setSpacing(5);
  gridList.replace(name, grid);
  grid->setColStretch(1, 1);

  addTab(w, name);
}

void PrefDialog::deletePage (QString name)
{
  removePage(widgetList[name]);
}

void PrefDialog::setHelpFile (QString d)
{
  helpFile = d;
}

void PrefDialog::help ()
{
  HelpWindow *hw = new HelpWindow(this, helpFile);
  hw->show();
  reject();
}

void PrefDialog::addColorItem (QString name, QString page, QColor color)
{
  QWidget *w = widgetList[page];
  
  QGridLayout *grid = gridList[page];
  grid->expand(grid->numRows() + 1, grid->numCols());
  
  QLabel *label = new QLabel(name, w);
  grid->addWidget(label, grid->numRows() - 2, 0);
  
  ColorButton *button = new ColorButton(w, color);
  grid->addWidget(button, grid->numRows() - 2, 1);
  button->setColorButton();
  colorButtonList.replace(name, button);
}

QColor PrefDialog::getColor (QString name)
{
  QColor color;
  ColorButton *button = colorButtonList[name];
  if (button)
    color = button->getColor();
  return color;
}

void PrefDialog::addFloatItem (QString name, QString page, double num)
{
  addFloatItem(name, page, num, -9999999999.0, 9999999999.0);
}

void PrefDialog::addFloatItem (QString name, QString page, double num, double low, double high)
{
  QWidget *w = widgetList[page];
  
  QGridLayout *grid = gridList[page];
  grid->expand(grid->numRows() + 1, grid->numCols());
  
  QLabel *label = new QLabel(name, w);
  grid->addWidget(label, grid->numRows() - 2, 0);
  
  QDoubleValidator *dv = new QDoubleValidator(low, high, 4, this, 0);
  dvList.replace(name, dv);
  
  QLineEdit *edit = new QLineEdit(QString::number(num), w);
  edit->setValidator(dv);
  grid->addWidget(edit, grid->numRows() - 2, 1);
  floatList.replace(name, edit);
}

double PrefDialog::getFloat (QString name)
{
  double num = 0;
  QLineEdit *edit = floatList[name];
  if (edit)
    num = edit->text().toFloat();
  return num;
}

void PrefDialog::addIntItem (QString name, QString page, int num)
{
  addIntItem(name, page, num, -999999999, 999999999);
}

void PrefDialog::addIntItem (QString name, QString page, int num, int min, int max)
{
  QWidget *w = widgetList[page];
  
  QGridLayout *grid = gridList[page];
  grid->expand(grid->numRows() + 1, grid->numCols());
  
  QLabel *label = new QLabel(name, w);
  grid->addWidget(label, grid->numRows() - 2, 0);
  
  QSpinBox *spin = new QSpinBox(min, max, 1, w);
  spin->setValue(num);
  grid->addWidget(spin, grid->numRows() - 2, 1);
  intList.replace(name, spin);
}

int PrefDialog::getInt (QString name)
{
  int num = 0;
  QSpinBox *spin = intList[name];
  if (spin)
    num = spin->value();
  return num;
}

void PrefDialog::addCheckItem (QString name, QString page, QString flag)
{
  if (! flag.compare("True"))
    addCheckItem(name, page, TRUE);
  else
    addCheckItem(name, page, FALSE);
}

void PrefDialog::addCheckItem (QString name, QString page, bool flag)
{
  QWidget *w = widgetList[page];
  
  QGridLayout *grid = gridList[page];
  grid->expand(grid->numRows() + 1, grid->numCols());
  
  QCheckBox *check = new QCheckBox(name, w);
  check->setChecked(flag);
  grid->addWidget(check, grid->numRows() - 2, 0);
  checkList.replace(name, check);
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
  QString flag;
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

void PrefDialog::addFontItem (QString name, QString page, QFont font)
{
  QWidget *w = widgetList[page];
  
  QGridLayout *grid = gridList[page];
  grid->expand(grid->numRows() + 1, grid->numCols());
  
  QLabel *label = new QLabel(name, w);
  grid->addWidget(label, grid->numRows() - 2, 0);
  
  FontButton *button = new FontButton(w, font);
  grid->addWidget(button, grid->numRows() - 2, 1);
  fontButtonList.replace(name, button);
}

QFont PrefDialog::getFont (QString name)
{
  QFont font;
  FontButton *button = fontButtonList[name];
  if (button)
    font = button->getFont();
  return font;
}

void PrefDialog::addTextItem (QString name, QString page, QString t)
{
  QWidget *w = widgetList[page];
  
  QGridLayout *grid = gridList[page];
  grid->expand(grid->numRows() + 1, grid->numCols());
  
  QLabel *label = new QLabel(name, w);
  grid->addWidget(label, grid->numRows() - 2, 0);

  QLineEdit *edit = new QLineEdit(t, w);
  grid->addWidget(edit, grid->numRows() - 2, 1);
  textList.replace(name, edit);
}

QString PrefDialog::getText (QString name)
{
  QString s;
  QLineEdit *edit = textList[name];
  if (edit)
    s = edit->text();
  return s;
}

void PrefDialog::addComboItem (QString name, QString page, QStringList l, QString s)
{
  QWidget *w = widgetList[page];
  
  QGridLayout *grid = gridList[page];
  grid->expand(grid->numRows() + 1, grid->numCols());
  
  QLabel *label = new QLabel(name, w);
  grid->addWidget(label, grid->numRows() - 2, 0);

  QComboBox *combo = new QComboBox(w);
  combo->insertStringList(l, -1);
  if (s.length())
    combo->setCurrentText(s);
  grid->addWidget(combo, grid->numRows() - 2, 1);
  comboList.replace(name, combo);
}

void PrefDialog::addComboItem (QString name, QString page, QStringList l, int s)
{
  QWidget *w = widgetList[page];
  
  QGridLayout *grid = gridList[page];
  grid->expand(grid->numRows() + 1, grid->numCols());
  
  QLabel *label = new QLabel(name, w);
  grid->addWidget(label, grid->numRows() - 2, 0);

  QComboBox *combo = new QComboBox(w);
  combo->insertStringList(l, -1);
  combo->setCurrentItem(s);
  grid->addWidget(combo, grid->numRows() - 2, 1);
  comboList.replace(name, combo);
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

QComboBox * PrefDialog::getComboWidget (QString name)
{
  return comboList[name];
}

void PrefDialog::addDateItem (QString name, QString page, QDateTime dt)
{
  QWidget *w = widgetList[page];
  
  QGridLayout *grid = gridList[page];
  grid->expand(grid->numRows() + 1, grid->numCols());
  
  QLabel *label = new QLabel(name, w);
  grid->addWidget(label, grid->numRows() - 2, 0);

  QDateEdit *date = new QDateEdit(dt.date(), w);
  date->setAutoAdvance(TRUE);
  date->setOrder(QDateEdit::YMD);
  grid->addWidget(date, grid->numRows() - 2, 1);
  dateList.replace(name, date);
}

QDateTime PrefDialog::getDate (QString name)
{
  QDateTime dt;
  QDateEdit *date = dateList[name];
  if (date)
    dt.setDate(date->date());
  return dt;
}

void PrefDialog::addFileItem (QString name, QString page)
{
  QWidget *w = widgetList[page];
  
  QGridLayout *grid = gridList[page];
  grid->expand(grid->numRows() + 1, grid->numCols());
  
  QLabel *label = new QLabel(name, w);
  grid->addWidget(label, grid->numRows() - 2, 0);

  FileButton *button = new FileButton(w);
  grid->addWidget(button, grid->numRows() - 2, 1);
  fileList.replace(name, button);
}

QStringList PrefDialog::getFile (QString name)
{
  QStringList l;
  FileButton *button = fileList[name];
  if (button)
    l = button->getFile();
  return l;
}

void PrefDialog::addSymbolItem (QString name, QString page, QString path, QString symbol)
{
  QWidget *w = widgetList[page];
  
  QGridLayout *grid = gridList[page];
  grid->expand(grid->numRows() + 1, grid->numCols());
  
  QLabel *label = new QLabel(name, w);
  grid->addWidget(label, grid->numRows() - 2, 0);

  SymbolButton *button = new SymbolButton(w, path, symbol);
  grid->addWidget(button, grid->numRows() - 2, 1);
  symbolList.replace(name, button);
}

QString PrefDialog::getSymbol (QString name)
{
  QString s;
  SymbolButton *button = symbolList[name];
  if (button)
    s = button->getPath();
  return s;
}

void PrefDialog::addFormulaInputItem (QString name, QString page, bool flag, QString in)
{
  QWidget *w = widgetList[page];
  
  QGridLayout *grid = gridList[page];
  grid->expand(grid->numRows() + 1, grid->numCols());
  
  QLabel *label = new QLabel(name, w);
  grid->addWidget(label, grid->numRows() - 2, 0);

  FormulaInput *fi = new FormulaInput(w, flag, in);
  grid->addWidget(fi, grid->numRows() - 2, 1);
  formulaInputList.replace(name, fi);
}

QString PrefDialog::getFormulaInput (QString name)
{
  QString s;
  FormulaInput *fi = formulaInputList[name];
  if (fi)
    s = fi->getInput();
  return s;
}

