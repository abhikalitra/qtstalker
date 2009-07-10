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
#include <QProgressDialog>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QLabel>
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
  vbox->setSpacing(10);
  setLayout(vbox);

  QGridLayout *grid = new QGridLayout;
  grid->setColumnStretch(1, 1);
  grid->setMargin(0);
  grid->setSpacing(5);
  vbox->addLayout(grid);
  
  allSymbols = new QCheckBox;
  allSymbols->setText(tr("Use All Symbols"));
  connect(allSymbols, SIGNAL(toggled(bool)), this, SLOT(allSymbolsToggled(bool)));
  grid->addWidget(allSymbols, 0, 0);

  QStringList l;
  symbolButton = new SymbolButton(this, l);
  grid->addWidget(symbolButton, 0, 1);

  QLabel *label = new QLabel(tr("Bar Length"));
  grid->addWidget(label, 1, 0);

  BarData bd(n);
  period = new QComboBox;
  bd.getBarLengthList(l);
  period->addItems(l);
  period->setCurrentIndex(period->findText("Daily"));
  grid->addWidget(period, 1, 1);

  label = new QLabel(tr("Bars"));
  grid->addWidget(label, 2, 0);

  bars = new QSpinBox;
  bars->setRange(1, 99999999);
  bars->setValue(100);
  grid->addWidget(bars, 2, 1);

  grid->setRowStretch(grid->rowCount(), 1);

  QFrame *hline = new QFrame;
  hline->setFrameStyle(QFrame::HLine | QFrame::Sunken);
  vbox->addWidget(hline);

  label = new QLabel(tr("Scanner Rule"));
  vbox->addWidget(label);

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

  if (! allSymbols->isChecked())
    symbolButton->getSymbols(symbolList);
  else
    db.getAllChartsList(symbolList);

  Indicator i;
  rule.getIndicator(i);
  IndicatorPlugin ip;
  ip.setIndicator(i);

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
        resultList.append(symbolList[loop]);
    }
    
    delete data;
    qDeleteAll(plotList);
    
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
  if (! allSymbols->isChecked())
  {
    QStringList l;
    symbolButton->getSymbols(l);
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

  rule.getFileList(l);
  if (l.count())
    symbolButton->setSymbols(l);

  rule.getAllSymbols(s);
  allSymbols->setChecked(s.toInt());
  allSymbolsToggled(s.toInt());
  
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

void Scanner::allSymbolsToggled (bool d)
{
  if (d)
    symbolButton->setEnabled(FALSE);
  else
    symbolButton->setEnabled(TRUE);
}

