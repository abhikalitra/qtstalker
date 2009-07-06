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

#include <QLayout>
#include <QInputDialog>
#include <QMessageBox>
#include <QDateTime>
#include <QProgressDialog>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QGroupBox>
#include <QPushButton>
#include <QtDebug>

#include "Scanner.h"
#include "BarData.h"
#include "DataBase.h"
#include "IndicatorPlugin.h"




Scanner::Scanner (QString n) : QDialog (0, 0)
{
  rule.setName(n);
  
  QString s = "Qtstalker Scanner";
  s.append(": ");
  s.append(n);
  setWindowTitle(s);

  QVBoxLayout *vbox = new QVBoxLayout;
  vbox->setMargin(10);
  vbox->setSpacing(5);
  setLayout(vbox);

  fileBox = new QGroupBox;
  fileBox->setCheckable(TRUE);
  fileBox->setFlat(FALSE);
  fileBox->setTitle(tr("All Symbols"));
  connect(fileBox, SIGNAL(toggled(bool)), this, SLOT(allSymbolsToggled(bool)));
  vbox->addWidget(fileBox);

  QHBoxLayout *hbox = new QHBoxLayout;
  hbox->setMargin(10);
  hbox->setSpacing(2);
  fileBox->setLayout(hbox);

  fileList = new QListWidget;
  hbox->addWidget(fileList);

  QVBoxLayout *tvbox = new QVBoxLayout;
  tvbox->setMargin(0);
  tvbox->setSpacing(2);
  hbox->addLayout(tvbox);

  addFileButton = new QToolButton;
  addFileButton->setText("A");
  connect(addFileButton, SIGNAL(clicked()), this, SLOT(getSymbols()));
  tvbox->addWidget(addFileButton);

  deleteFileButton = new QToolButton;
  deleteFileButton->setText("D");
  connect(deleteFileButton, SIGNAL(clicked()), this, SLOT(deleteSymbols()));
  tvbox->addWidget(deleteFileButton);

  tvbox->addStretch(1);

  QGridLayout *grid = new QGridLayout;
  grid->setColumnStretch(1, 1);
  grid->setMargin(0);
  grid->setSpacing(5);
  vbox->addLayout(grid);
  
  QLabel *label = new QLabel(tr("Bar Length"));
  grid->addWidget(label, 0, 0);

  BarData bd(n);
  period = new QComboBox;
  QStringList l;
  bd.getBarLengthList(l);
  period->addItems(l);
  period->setCurrentIndex(period->findText("Daily"));
  grid->addWidget(period, 0, 1);

  label = new QLabel(tr("Bars"));
  grid->addWidget(label, 1, 0);

  bars = new QSpinBox;
  bars->setRange(1, 99999999);
  bars->setValue(100);
  grid->addWidget(bars, 1, 1);

  formula = new FormulaEdit();
  vbox->addWidget(formula);

  buttonBox = new QDialogButtonBox;
  vbox->addWidget(buttonBox);

  QPushButton *button = buttonBox->addButton(QDialogButtonBox::Ok);
  connect(button, SIGNAL(clicked()), this, SLOT(scan()));

  button = buttonBox->addButton(QDialogButtonBox::Save);
  connect(button, SIGNAL(clicked()), this, SLOT(saveRule()));

  button = buttonBox->addButton(QDialogButtonBox::Cancel);
  connect(button, SIGNAL(clicked()), this, SLOT(reject()));

  vbox->addStretch(1);

  loadRule();
}

void Scanner::scan ()
{
  DataBase db;
  QStringList symbolList, resultList;

  if (! fileBox->isChecked())
  {
    int loop;
    for (loop = 0; loop < fileList->count(); loop++)
    {
      QListWidgetItem *item = fileList->item(loop);
      symbolList.append(item->text());
    }
  }
  else
    db.getAllChartsList(symbolList);

  Indicator i;
  rule.getIndicator(i);
  QString indicatorName;
  i.getName(indicatorName);
  IndicatorPlugin ip;
  ip.setName(indicatorName);

  this->setEnabled(FALSE);

  QProgressDialog prog(tr("Scanning..."), tr("Cancel"), 1, symbolList.count(), this);
  prog.show();
  
  emit signalMessage(QString("Scanning..."));
  
  int loop;
  for (loop = 0; loop < symbolList.count(); loop++)
  {
    prog.setValue(loop);
    emit signalMessage(QString());
    if (prog.wasCanceled())
    {
      emit signalMessage(QString("Scan cancelled"));
      break;
    }

    BarData *data = new BarData(symbolList[loop]);
    data->setBarsRequested(bars->value());
    data->setBarLength((BarData::BarLength) period->currentIndex());
    db.getChart(data);

    ip.setIndicatorInput(data);

    QList<PlotLine *> plotList;
    ip.calculate(plotList);
    if (! plotList.count())
    {
      qDebug() << "Scanner::scan: empty plotlist";
      delete data;
      continue;
    }
    
    PlotLine *line = plotList.at(0);
    if (line && line->getSize() > 0)
    {
      if (line->getData(line->getSize() - 1) > 0)
      {
        resultList.append(symbolList[loop]);
qDebug() << symbolList[loop];
      }
    }
    
    delete data;
    
    emit signalMessage(QString());
  }
 
  QString s;
  rule.getName(s);
  QString s2 = tr("Scanner_") + s;
  db.setGroupList(s2, resultList);
  
  if (! prog.wasCanceled())
    emit signalMessage(QString("Scan complete"));
  
  this->setEnabled(TRUE);

  emit signalScanComplete();
}

void Scanner::saveRule ()
{
  if (! fileBox->isChecked())
  {
    int loop;
    QStringList l;
    for (loop = 0; loop < fileList->count(); loop++)
    {
      QListWidgetItem *item = fileList->item(loop);
      l.append(item->text());
    }

    rule.setFileList(l);

    QString s("0");
    rule.setAllSymbols(s);
  }
  else
  {
    QString s("1");
    rule.setAllSymbols(s);
  }

  QString s = period->currentText();
  rule.setBarLength(s);

  s = bars->text();
  rule.setBars(s);  

  Indicator i;
  formula->getIndicator(i);
  s = "Scanner";
  i.setType(s);
  rule.getName(s);
  s.prepend("Scanner_");
  i.setName(s);
  rule.setIndicator(i);    

  DataBase db;
  db.setScanner(rule);

  emit signalUpdate();
}

void Scanner::loadRule ()
{
  DataBase db;
  QStringList l;
  db.getScannerList(l);

  QString s;
  rule.getName(s);
  if (! l.contains(s))
    return;

  db.getScanner(rule);

  fileList->clear();
  rule.getFileList(l);
  fileList->addItems(l);

  rule.getAllSymbols(s);
  fileBox->setChecked(s.toInt());
  
  rule.getBarLength(s);
  period->setCurrentIndex(period->findText(s));

  rule.getBars(s);
  bars->setValue(s.toInt());

  Indicator i;
  rule.getIndicator(i);
  formula->setIndicator(i);
}

void Scanner::exitDialog ()
{
  reject();
}

void Scanner::getSymbols ()
{
  bool ok;
  QString s = QInputDialog::getText(this, tr("Select symbols"), tr("Symbol contains"), QLineEdit::Normal, QString(), &ok, 0);
  if (s.isEmpty())
    return;

  QStringList l;
  DataBase db;
  db.getAllChartsList(l);

  int loop;
  for (loop = 0; loop < l.count(); loop++)
  {
    if (l[loop].contains(s))
      fileList->addItem(l[loop]);
  }
}

void Scanner::allSymbolsToggled (bool d)
{
  if (d)
  {
    fileList->setEnabled(FALSE);
    addFileButton->setEnabled(FALSE);
    deleteFileButton->setEnabled(FALSE);
  }
  else
  {
    fileList->setEnabled(TRUE);
    addFileButton->setEnabled(TRUE);
    deleteFileButton->setEnabled(TRUE);
  }
}

void Scanner::deleteSymbols ()
{
}


