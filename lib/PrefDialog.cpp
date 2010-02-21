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
#include "PlotLine.h"
#include "IndicatorBase.h"
#include "BarData.h"

#include <QHBoxLayout>
#include <QGridLayout>
#include <QFrame>
#include <QLabel>
#include <QVBoxLayout>
#include <QtDebug>



PrefDialog::PrefDialog (QWidget *w) : QDialog (w, 0)
{
  init();
}

PrefDialog::PrefDialog () : QDialog (0, 0)
{
  init();
}

void PrefDialog::init ()
{
  QVBoxLayout *vbox = new QVBoxLayout;
  setLayout(vbox);

  tabs = new QTabWidget;
  vbox->addWidget(tabs);

  vbox->insertStretch(-1, 1);

  buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel | QDialogButtonBox::Help);
  connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
  connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
//  connect(buttonBox, SIGNAL(helpRequested()), this, SLOT(help()));
  vbox->addWidget(buttonBox);

  keys.clear();
}

void PrefDialog::addPage (int page, QString &title)
{
  QWidget *w = new QWidget(this);

  QVBoxLayout *vbox = new QVBoxLayout;
  w->setLayout(vbox);

  QGridLayout *grid = new QGridLayout;
  grid->setSpacing(5);
  grid->setColumnStretch(1, 1);
  vbox->addLayout(grid);

  vbox->addStretch(1);

  tabs->addTab(w, title);

  gridList.insert(page, grid);
}

void PrefDialog::addColorItem (int key, int page, QString name, QString &color)
{
  QColor c(color);
  addColorItem(key, page, name, c);
}

void PrefDialog::addColorItem (int key, int page, QString name, QColor &color)
{
  QGridLayout *grid = gridList.value(page);
  if (! grid)
  {
    qDebug() << "PrefDialog::addColorItem: page number not found";
    return;
  }

  QLabel *label = new QLabel(name);
  grid->addWidget(label, grid->rowCount(), 0);

  ColorButton *button = new ColorButton(this, color);
  grid->addWidget(button, grid->rowCount() - 1, 1);
  button->setColorButton();
  colorButtonList.insert(key, button);

  keys.append(key);
}

void PrefDialog::getColor (int key, QColor &color)
{
  ColorButton *button = colorButtonList.value(key);
  if (button)
    button->getColor(color);
}

void PrefDialog::addDoubleItem (int key, int page, QString name, double num)
{
  addDoubleItem(key, page, name, num, -9999999999.0, 9999999999.0);
}

void PrefDialog::addDoubleItem (int key, int page, QString name, double num, double low, double high)
{
  QGridLayout *grid = gridList.value(page);
  if (! grid)
  {
    qDebug() << "PrefDialog::addDoubleItem: page number not found";
    return;
  }

  QLabel *label = new QLabel(name);
  grid->addWidget(label, grid->rowCount(), 0);

  QDoubleSpinBox *edit = new QDoubleSpinBox;
  edit->setMinimum(low);
  edit->setMaximum(high);
  edit->setValue(num);
  grid->addWidget(edit, grid->rowCount() - 1, 1);
  doubleList.insert(key, edit);

  keys.append(key);
}

double PrefDialog::getDouble (int key)
{
  double num = 0;
  QDoubleSpinBox *edit = doubleList.value(key);
  if (edit)
    num = edit->value();
  return num;
}

void PrefDialog::addIntItem (int key, int page, QString name, int num)
{
  addIntItem(key, page, name, num, -999999999, 999999999);
}

void PrefDialog::addIntItem (int key, int page, QString name, int num, int min, int max)
{
  QGridLayout *grid = gridList.value(page);
  if (! grid)
  {
    qDebug() << "PrefDialog::addIntItem: page number not found";
    return;
  }

  QLabel *label = new QLabel(name);
  grid->addWidget(label, grid->rowCount(), 0);

  QSpinBox *spin = new QSpinBox;
  spin->setMinimum(min);
  spin->setMaximum(max);
  spin->setValue(num);
  grid->addWidget(spin, grid->rowCount() - 1, 1);
  intList.insert(key, spin);

  keys.append(key);
}

int PrefDialog::getInt (int key)
{
  int num = 0;
  QSpinBox *spin = intList.value(key);
  if (spin)
    num = spin->value();
  return num;
}

void PrefDialog::addCheckItem (int key, int page, QString name, QString &flag)
{
  if (! flag.compare("True"))
    addCheckItem(key, page, name, TRUE);
  else
    addCheckItem(key, page, name, FALSE);
}

void PrefDialog::addCheckItem (int key, int page, QString name, int flag)
{
  QGridLayout *grid = gridList.value(page);
  if (! grid)
  {
    qDebug() << "PrefDialog::addCheckItem: page number not found";
    return;
  }

  QLabel *label = new QLabel(name);
  grid->addWidget(label, grid->rowCount(), 0);

  QCheckBox *check = new QCheckBox;
  check->setChecked(flag);
  grid->addWidget(check, grid->rowCount() - 1, 1);
  checkList.insert(key, check);

  keys.append(key);
}

int PrefDialog::getCheck (int key)
{
  int flag = FALSE;
  QCheckBox *check = checkList.value(key);
  if (check)
    flag = check->isChecked();
  return flag;
}

void PrefDialog::getCheckString (int key, QString &flag)
{
  flag.truncate(0);
  QCheckBox *check = checkList.value(key);
  if (check)
  {
    if (check->isChecked())
      flag = "TRUE";
    else
      flag = "FALSE";
  }
}

void PrefDialog::addFontItem (int key, int page, QString name, QFont &font)
{
  QGridLayout *grid = gridList.value(page);
  if (! grid)
  {
    qDebug() << "PrefDialog::addFontItem: page number not found";
    return;
  }

  QLabel *label = new QLabel(name);
  grid->addWidget(label, grid->rowCount(), 0);

  FontButton *button = new FontButton(this, font);
  grid->addWidget(button, grid->rowCount() - 1, 1);
  fontButtonList.insert(key, button);

  keys.append(key);
}

void PrefDialog::getFont (int key, QFont &font)
{
  FontButton *button = fontButtonList.value(key);
  if (button)
    button->getFont(font);
}

void PrefDialog::addTextItem (int key, int page, QString name, QString &t)
{
  QGridLayout *grid = gridList.value(page);
  if (! grid)
  {
    qDebug() << "PrefDialog::addTextItem: page number not found";
    return;
  }

  QLabel *label = new QLabel(name);
  grid->addWidget(label, grid->rowCount(), 0);

  QLineEdit *edit = new QLineEdit(t);
  grid->addWidget(edit, grid->rowCount() - 1, 1);
  textList.insert(key, edit);

  keys.append(key);
}

void PrefDialog::getText (int key, QString &s)
{
  s.clear();
  QLineEdit *edit = textList.value(key);
  if (edit)
    s = edit->text();
}

void PrefDialog::addComboItem (int key, int page, QString name, QStringList &l, QString &s)
{
  QGridLayout *grid = gridList.value(page);
  if (! grid)
  {
    qDebug() << "PrefDialog::addComboItem: page number not found";
    return;
  }

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
  comboList.insert(key, combo);

  keys.append(key);
}

void PrefDialog::addComboItem (int key, int page, QString name, QStringList &l, int s)
{
  QGridLayout *grid = gridList.value(page);
  if (! grid)
  {
    qDebug() << "PrefDialog::addComboItem: page number not found";
    return;
  }

  QLabel *label = new QLabel(name);
  grid->addWidget(label, grid->rowCount(), 0);

  QComboBox *combo = new QComboBox;
  combo->addItems(l);
  combo->setCurrentIndex(s);
  grid->addWidget(combo, grid->rowCount() - 1, 1);
  comboList.insert(key, combo);

  keys.append(key);
}

void PrefDialog::getCombo (int key, QString &s)
{
  s.clear();
  QComboBox *combo = comboList.value(key);
  if (combo)
    s = combo->currentText();
}

int PrefDialog::getComboIndex (int key)
{
  int i = 0;
  QComboBox *combo = comboList.value(key);
  if (combo)
    i = combo->currentIndex();
  return i;
}

QComboBox * PrefDialog::getComboWidget (int key)
{
  return comboList.value(key);
}

void PrefDialog::addDateItem (int key, int page, QString name, QDateTime &dt)
{
  QGridLayout *grid = gridList.value(page);
  if (! grid)
  {
    qDebug() << "PrefDialog::addDateItem: page number not found";
    return;
  }

  QLabel *label = new QLabel(name);
  grid->addWidget(label, grid->rowCount(), 0);

  QDateEdit *date = new QDateEdit(dt.date());
  grid->addWidget(date, grid->rowCount() - 1, 1);
  dateList.insert(key, date);

  keys.append(key);
}

void PrefDialog::getDate (int key, QDateTime &dt)
{
  QDateEdit *date = dateList.value(key);
  if (date)
    dt.setDate(date->date());
}

void PrefDialog::addTimeItem (int key, int page, QString name, QDateTime &dt)
{
  QGridLayout *grid = gridList.value(page);
  if (! grid)
  {
    qDebug() << "PrefDialog::addTimeItem: page number not found";
    return;
  }

  QLabel *label = new QLabel(name);
  grid->addWidget(label, grid->rowCount(), 0);

  QTimeEdit *time = new QTimeEdit(dt.time());
  grid->addWidget(time, grid->rowCount() - 1, 1);
  timeList.insert(key, time);

  keys.append(key);
}

void PrefDialog::getTime (int key, QDateTime &dt)
{
  QTimeEdit *time = timeList.value(key);
  if (time)
    dt.setTime(time->time());
}

void PrefDialog::getItem (int key, QString &s)
{
  s.clear();

  // check for text
  if (textList.contains(key))
  {
    getText(key, s);
    return;
  }

  // check for combo
  if (comboList.contains(key))
  {
    getCombo(key, s);
    return;
  }

  // check for int
  if (intList.contains(key))
  {
    QSpinBox *spin = intList.value(key);
    if (spin)
      s = spin->text();
    return;
  }

  // check for double
  if (doubleList.contains(key))
  {
    QDoubleSpinBox *spin = doubleList.value(key);
    if (spin)
      s = spin->cleanText();
    return;
  }

  // check color
  if (colorButtonList.contains(key))
  {
    QColor color;
    getColor(key, color);
    if (color.isValid())
    {
      s = color.name();
      return;
    }
  }

  // check check
  if (checkList.contains(key))
  {
    QCheckBox *check = checkList.value(key);
    if (check)
      s = QString::number(check->isChecked());
  }

  // check for textedit
  if (textEditList.contains(key))
  {
    getTextEdit(key, s);
    return;
  }
}

void PrefDialog::getKeyList (QList<int> &l)
{
  l = keys;
}

void PrefDialog::addTextEditItem (int key, int page, QString name, QString &t)
{
  QGridLayout *grid = gridList.value(page);
  if (! grid)
  {
    qDebug() << "PrefDialog::addTextEditItem: page number not found";
    return;
  }

  QLabel *label = new QLabel(name);
  grid->addWidget(label, grid->rowCount(), 0);

  QTextEdit *edit = new QTextEdit;
  edit->setText(t);
  grid->addWidget(edit, grid->rowCount() - 1, 1);
  textEditList.insert(key, edit);

  keys.append(key);
}

void PrefDialog::getTextEdit (int key, QString &s)
{
  s.clear();
  QTextEdit *edit = textEditList.value(key);
  if (edit)
    s = edit->toPlainText();
}

