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

#include "PrefDialog.h"
#include "Config.h"
#include <QHBoxLayout>
#include <QGridLayout>
#include <QFrame>
#include <QLabel>
#include <QVBoxLayout>

PrefDialog::PrefDialog (QWidget *w) : QDialog (w, 0)
{
  init();
}

PrefDialog::PrefDialog () : QDialog (0, 0)
{
  init();
}

PrefDialog::~PrefDialog ()
{
  qDeleteAll(colorButtonList);
  qDeleteAll(intList);
  qDeleteAll(doubleList);
  qDeleteAll(checkList);
  qDeleteAll(fontButtonList);
  qDeleteAll(textList);
  qDeleteAll(comboList);
  qDeleteAll(dateList);
  qDeleteAll(fileList);
  qDeleteAll(symbolList);
  qDeleteAll(labelList);
  qDeleteAll(timeList);
}

void PrefDialog::init ()
{
  QVBoxLayout *vbox = new QVBoxLayout;
  setLayout(vbox);

  grid = new QGridLayout;
  vbox->addLayout(grid);
  grid->setMargin(5);
  grid->setSpacing(5);
  grid->setColumnStretch(1, 1);
  
  vbox->insertStretch(-1, 1);

  buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel | QDialogButtonBox::Help);
  connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
  connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
//  connect(buttonBox, SIGNAL(helpRequested()), this, SLOT(help()));
  vbox->addWidget(buttonBox);
}

void PrefDialog::addColorItem (QString &name, QColor &color)
{
  QLabel *label = new QLabel(name);
  grid->addWidget(label, grid->rowCount(), 0);
  
  ColorButton *button = new ColorButton(this, color);
  grid->addWidget(button, grid->rowCount() - 1, 1);
  button->setColorButton();
  colorButtonList.insert(name, button);
}

void PrefDialog::addColorPrefItem (QString &name, QColor &color)
{
  ColorButton *mainButton = new ColorButton(this, color);
  
  // add the preset preferred buttons
  // for now a fixed quantity of 5 pieces with fixed colors
  // may/will later expand with the possibility to make user defineable too
  QString prefButtonName = "PrefColor";
  ColorButton *prefButton;
  QColor prefColor[5]; 
  prefColor[0].setNamedColor("white");
  prefColor[1].setNamedColor("red");
  prefColor[2].setNamedColor("green");
  prefColor[3].setRgb(85,170,255); // something like blue
  prefColor[4].setRgb(255,170,0); // orange
  
  // create pref-buttons and arange in one row(hbox)
  QHBoxLayout *hbox = new QHBoxLayout;
  int i;
  for (i = 0; i < 5; i++)
  {
    prefButton = new ColorButton(this, prefColor[i]);
    prefButton->setDialogOff();
//    prefButton->pix.resize(10, 10);
    prefButton->setFixedSize(10, 10);
    prefButton->setColorButton();
    connect (prefButton, SIGNAL(robPressed(QColor)), mainButton, SLOT(setColor(QColor)));
    hbox->addWidget(prefButton);
    colorButtonList.insert(prefButtonName + QString::number(i, 'f', 0), prefButton);
  
  }
  
  QVBoxLayout *vbox = new QVBoxLayout;
  vbox->addLayout(hbox);
  vbox->addWidget(mainButton);
  
  QLabel *label = new QLabel(name);
  grid->addWidget(label, grid->rowCount(), 0);
  
  grid->addLayout(vbox, grid->rowCount() - 1, 1);
  mainButton->setColorButton();
  colorButtonList.insert(name, mainButton);
}

void PrefDialog::getColor (QString &name, QColor &color)
{
  ColorButton *button = colorButtonList.value(name);
  if (button)
    button->getColor(color);
}

void PrefDialog::addDoubleItem (QString &name, double num)
{
  addDoubleItem(name, num, -9999999999.0, 9999999999.0);
}

void PrefDialog::addDoubleItem (QString &name, double num, double low, double high)
{
  QLabel *label = new QLabel(name);
  grid->addWidget(label, grid->rowCount(), 0);
  
  QDoubleSpinBox *edit = new QDoubleSpinBox;
  edit->setMinimum(low);
  edit->setMaximum(high);
  edit->setValue(num);
  grid->addWidget(edit, grid->rowCount() - 1, 1);
  doubleList.insert(name, edit);
}

double PrefDialog::getDouble (QString &name)
{
  double num = 0;
  QDoubleSpinBox *edit = doubleList.value(name);
  if (edit)
    num = edit->value();
  return num;
}

void PrefDialog::addIntItem (QString &name, int num)
{
  addIntItem(name, num, -999999999, 999999999);
}

void PrefDialog::addIntItem (QString &name, int num, int min, int max)
{
  QLabel *label = new QLabel(name);
  grid->addWidget(label, grid->rowCount(), 0);
  
  QSpinBox *spin = new QSpinBox;
  spin->setMinimum(min);
  spin->setMaximum(max);
  spin->setValue(num);
  grid->addWidget(spin, grid->rowCount() - 1, 1);
  intList.insert(name, spin);
}

int PrefDialog::getInt (QString &name)
{
  int num = 0;
  QSpinBox *spin = intList.value(name);
  if (spin)
    num = spin->value();
  return num;
}

void PrefDialog::addCheckItem (QString &name, QString &flag)
{
  if (! flag.compare("True"))
    addCheckItem(name, TRUE);
  else
    addCheckItem(name, FALSE);
}

void PrefDialog::addCheckItem (QString &name, bool flag)
{
  QLabel *label = new QLabel(name);
  grid->addWidget(label, grid->rowCount(), 0);
  
  QCheckBox *check = new QCheckBox;
  check->setChecked(flag);
  grid->addWidget(check, grid->rowCount() - 1, 1);
  checkList.insert(name, check);
}

bool PrefDialog::getCheck (QString &name)
{
  bool flag = FALSE;
  QCheckBox *check = checkList.value(name);
  if (check)
    flag = check->isChecked();
  return flag;
}

void PrefDialog::getCheckString (QString &name, QString &flag)
{
  flag.truncate(0);
  QCheckBox *check = checkList.value(name);
  if (check)
  {
    if (check->isChecked())
      flag = "True";
    else
      flag = "False";
  }    
}

void PrefDialog::addFontItem (QString &name, QFont &font)
{
  QLabel *label = new QLabel(name);
  grid->addWidget(label, grid->rowCount(), 0);
  
  FontButton *button = new FontButton(this, font);
  grid->addWidget(button, grid->rowCount() - 1, 1);
  fontButtonList.insert(name, button);
}

void PrefDialog::getFont (QString &name, QFont &font)
{
  FontButton *button = fontButtonList.value(name);
  if (button)
    button->getFont(font);
}

void PrefDialog::addTextItem (QString &name, QString &t)
{
  QLabel *label = new QLabel(name);
  grid->addWidget(label, grid->rowCount(), 0);

  QLineEdit *edit = new QLineEdit(t);
  grid->addWidget(edit, grid->rowCount() - 1, 1);
  textList.insert(name, edit);
}

void PrefDialog::getText (QString &name, QString &s)
{
  s.clear();
  QLineEdit *edit = textList.value(name);
  if (edit)
    s = edit->text();
}

void PrefDialog::addComboItem (QString &name, QStringList &l, QString &s)
{
  QLabel *label = new QLabel(name);
  grid->addWidget(label, grid->rowCount(), 0);

  QComboBox *combo = new QComboBox;
  combo->addItems(l);
  if (s.length())
  {
    int t = l.indexOf(s);
    if (t != -1)
      combo->setCurrentIndex(t);
  }
  grid->addWidget(combo, grid->rowCount() - 1, 1);
  comboList.insert(name, combo);
}

void PrefDialog::addComboItem (QString &name, QStringList &l, int s)
{
  QLabel *label = new QLabel(name);
  grid->addWidget(label, grid->rowCount(), 0);

  QComboBox *combo = new QComboBox;
  combo->addItems(l);
  combo->setCurrentIndex(s);
  grid->addWidget(combo, grid->rowCount() - 1, 1);
  comboList.insert(name, combo);
}

void PrefDialog::getCombo (QString &name, QString &s)
{
  s.clear();
  QComboBox *combo = comboList.value(name);
  if (combo)
    s = combo->currentText();
}

int PrefDialog::getComboIndex (QString &name)
{
  int i = 0;
  QComboBox *combo = comboList.value(name);
  if (combo)
    i = combo->currentIndex();
  return i;
}

QComboBox * PrefDialog::getComboWidget (QString &name)
{
  return comboList.value(name);
}

void PrefDialog::addDateItem (QString &name, QDateTime &dt)
{
  QLabel *label = new QLabel(name);
  grid->addWidget(label, grid->rowCount(), 0);

  QDateEdit *date = new QDateEdit(dt.date());
  grid->addWidget(date, grid->rowCount() - 1, 1);
  dateList.insert(name, date);
}

void PrefDialog::getDate (QString &name, QDateTime &dt)
{
  QDateEdit *date = dateList.value(name);
  if (date)
    dt.setDate(date->date());
}

void PrefDialog::addTimeItem (QString &name, QDateTime &dt)
{
  QLabel *label = new QLabel(name);
  grid->addWidget(label, grid->rowCount(), 0);

  QTimeEdit *time = new QTimeEdit(dt.time());
  grid->addWidget(time, grid->rowCount() - 1, 1);
  timeList.insert(name, time);
}

void PrefDialog::getTime (QString &name, QDateTime &dt)
{
  QTimeEdit *time = timeList.value(name);
  if (time)
    dt.setTime(time->time());
}

void PrefDialog::addFileItem (QString &name, QStringList &l, QString &p)
{
  QLabel *label = new QLabel(name);
  grid->addWidget(label, grid->rowCount(), 0);

  FileButton *button = new FileButton(this, l, p);
  grid->addWidget(button, grid->rowCount() - 1, 1);
  fileList.insert(name, button);
}

void PrefDialog::getFile (QString &name, QStringList &l)
{
  l.clear();
  FileButton *button = fileList.value(name);
  if (button)
    button->getFile(l);
}

/*
void PrefDialog::addSymbolItem (QString &name, QString &path, QString &symbol)
{
  QLabel *label = new QLabel(name);
  grid->addWidget(label, grid->rowCount(), 0);

  SymbolButton *button = new SymbolButton(this, path, symbol);
  grid->addWidget(button, grid->rowCount() - 1, 1);
  symbolList.insert(name, button);
}

void PrefDialog::getSymbol (QString &name, QString &s)
{
  s.clear();
  SymbolButton *button = symbolList.value(name);
  if (button)
    button->getPath(s);
}

void PrefDialog::addLabelItem (QString &name, QString &l)
{
  QLabel *label = new QLabel(name);
  grid->addWidget(label, grid->rowCount(), 0);
  
  label = new QLabel(l);
  label->setFrameStyle(QFrame::WinPanel | QFrame::Sunken);
  grid->addWidget(label, grid->rowCount() - 1, 1);
  labelList.insert(name, label);
}
*/

void PrefDialog::getItem (QString &name, QString &s)
{
  s.clear();

  // check for text
  if (textList.contains(name))
  {
    getText(name, s);
    return;
  }

  // check for combo
  if (comboList.contains(name))
  {
    getCombo(name, s);
    return;
  }

  // check for int
  if (intList.contains(name))
  {
    QSpinBox *spin = intList.value(name);
    if (spin)
      s = spin->text();
    return;
  }

  // check for double
  if (doubleList.contains(name))
  {
    QDoubleSpinBox *spin = doubleList.value(name);
    if (spin)
      s = spin->cleanText();
    return;
  }

  // check color
  if (colorButtonList.contains(name))
  {
    QColor color;
    getColor(name, color);
    if (color.isValid())
    {
      s = color.name();
      return;
    }
  }

  // check check
  if (checkList.contains(name))
  {
    QCheckBox *check = checkList.value(name);
    if (check)
      s = QString::number(check->isChecked());
  }
}


