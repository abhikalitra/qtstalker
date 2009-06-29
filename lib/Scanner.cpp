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
#include <QDir>
#include <QProgressDialog>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QTextStream>
#include <stdlib.h>
#include <QLabel>
#include <QGroupBox>
#include <QPushButton>
#include <QtDebug>
#include "Scanner.h"
#include "BarData.h"
#include "SymbolDialog.h"
#include "IndicatorPlugin.h"


Scanner::Scanner (QString n) : QDialog (0, 0)
{
  scannerName = n;
  
  QString s = "Qtstalker Scanner";
  s.append(": ");
  s.append(scannerName);
  setWindowTitle(s);

  QWidget *w = new QWidget(this);
  
  QVBoxLayout *vbox = new QVBoxLayout(w);
  vbox->setMargin(10);
  vbox->setSpacing(5);

  QGridLayout *grid = new QGridLayout(w);
  grid->setColumnStretch(1, 1);
  grid->setSpacing(5);
  vbox->addLayout(grid);
  
  allSymbols = new QCheckBox(tr("All symbols"), w);
  connect(allSymbols, SIGNAL(toggled(bool)), this, SLOT(allSymbolsToggled(bool)));
  grid->addWidget(allSymbols, 0, 0);

  fileButton = new QPushButton(tr("0 Symbols"), w);
  connect(fileButton, SIGNAL(clicked()), this, SLOT(getSymbols()));
  grid->addWidget(fileButton, 0, 1);

  QLabel *label = new QLabel(tr("Charts or Groups"), w);
  grid->addWidget(label, 1, 0);

  basePath = new QComboBox(w);
  basePath->addItem(tr("Chart"));
  basePath->addItem(tr("Group"));
  grid->addWidget(basePath, 1, 1);

  label = new QLabel(tr("Bar Length"), w);
  grid->addWidget(label, 2, 0);

  BarData bd(scannerName);
  period = new QComboBox(w);
  bd.getBarLengthList(barLengthList);
  period->addItems(barLengthList);
  period->setCurrentIndex(period->findText("Daily"));
  grid->addWidget(period, 2, 1);

  label = new QLabel(tr("Bars"), w);
  grid->addWidget(label, 3, 0);

  bars = new QSpinBox(w);
  bars->setRange(1, 99999999);
  bars->setValue(100);
  grid->addWidget(bars, 3, 1);
  
  list = new FormulaEdit(w, s);
  vbox->addWidget(list);

  buttonBox = new QDialogButtonBox(w);
  QPushButton *button = buttonBox->addButton(QDialogButtonBox::Ok);
  connect(button, SIGNAL(clicked()), this, SLOT(scan()));
  button = buttonBox->addButton(QDialogButtonBox::Save);
  connect(button, SIGNAL(clicked()), this, SLOT(saveRule()));
  button = buttonBox->addButton(QDialogButtonBox::Cancel);
  connect(button, SIGNAL(clicked()), this, SLOT(reject()));
  button = buttonBox->addButton(QDialogButtonBox::Help);
  connect(button, SIGNAL(clicked()), this, SLOT(slotHelp()));
  vbox->addWidget(buttonBox);

  setLayout(vbox);
  
  loadRule();
}

Scanner::~Scanner ()
{
}

void Scanner::scan ()
{
  if (! fileList.count() && ! allSymbols->isChecked())
  {
    QMessageBox::information(this,
                             tr("Qtstalker: Error"),
			     tr("No symbols selected."));
    return;
  }

  // open the CUS plugin
  QString iplugin("CUS");
  IndicatorPlugin *plug = new IndicatorPlugin;
  QString s;
//  list->getText(s);
  QStringList l = s.split("\n");
//  plug->setCustomFunction(l);
  
  this->setEnabled(FALSE);
  
  // clear dir for scan symbols
  QDir dir;
//  config->getData(Config::GroupPath, s);
  s.append("/Scanner");
  if (! dir.exists(s))
    dir.mkdir(s);
  s.append("/" + scannerName);
  if (! dir.exists(s))
    dir.mkdir(s);
  else
  {
    int loop;
    dir.setPath(s);
    for (loop = 2; loop < (int) dir.count(); loop++)
    {
      QString s2 = dir.absolutePath() + "/" + dir[loop];
      if (! dir.remove(s2))
        qDebug() << s2 << " not removed";
    }
  }
  
  if (allSymbols->isChecked())
  {
    QString ts;
//    if (! basePath->currentText().compare(tr("Chart")))
//      config->getData(Config::DataPath, ts);
//    else
//      config->getData(Config::GroupPath, ts);
//    Traverse trav(Traverse::File);
//    trav.traverse(ts);
//    trav.getList(fileList);
  }
  
  QProgressDialog prog(tr("Scanning..."),
                       tr("Cancel"),
                       1,
		       fileList.count(),
		       this);
  prog.show();
  
  int minBars = bars->value();
  
  emit message(QString("Scanning..."));
  
  int loop;
  for (loop = 0; loop < (int) fileList.count(); loop++)
  {
    prog.setValue(loop);
    emit message(QString());
    if (prog.wasCanceled())
    {
      emit message(QString("Scan cancelled"));
      break;
    }

    QFileInfo fi(fileList[loop]);
    if (fi.isDir())
      continue;

    QDir dir;
    if (! dir.exists(fileList[loop]))
      continue;
//    db.open(fileList[loop], chartIndex);

//    db.setBarRange(minBars);
//    db.setBarLength((BarData::BarLength) barLengthList.indexOf(period->currentText()));

    BarData *recordList = new BarData(fileList[loop]);
    QDateTime dt = QDateTime::currentDateTime();
//    db.getHistory(recordList, dt);
    
    // load the CUS plugin and calculate
    plug->setIndicatorInput(recordList);
//    plug->calculate();
//    if (! plug->getLines())
//    {
//      delete recordList;
//      continue;
//    }
    
//    PlotLine *line = plug->getLine(0);
    PlotLine *line = 0;
    if (line && line->getSize() > 0)
    {
      if (line->getData(line->getSize() - 1) > 0)
      {
        QString ts;
        QString s = "ln -s \"" + fileList[loop] + "\" " + ts + "/Scanner/" + scannerName;
        system((char *) s.data());
      }
    }
    
    delete recordList;
    
    emit message(QString());
  }
  
  if (! prog.wasCanceled())
    emit message(QString("Scan complete"));
  
  this->setEnabled(TRUE);

  emit scanComplete();
}

void Scanner::saveRule ()
{
  QString s;
//  config->getData(Config::ScannerPath, s);
  s.append("/" + scannerName);

  QFile f(s);
  if (! f.open(QIODevice::WriteOnly))
    return;
  QTextStream stream(&f);
  
  stream << "allSymbols=" << QString::number(allSymbols->isChecked()) << "\n";
  stream << "compression=" << period->currentText() << "\n";
  stream << "bars=" << bars->text() << "\n";
  stream << "basepath=" << basePath->currentText() << "\n";
  
  int loop;
  for (loop = 0; loop < (int) fileList.count(); loop++)
    stream << "symbol=" << fileList[loop] << "\n";
  
//  list->getText(s);
  QStringList l = s.split("\n");
  stream << "script=" << l.join("|") << "\n";
  
  f.close();
}

void Scanner::loadRule ()
{
  QString s;
//  config->getData(Config::ScannerPath, s);
  s.append("/" + scannerName);

  QFile f(s);
  if (! f.open(QIODevice::ReadOnly))
    return;
  QTextStream stream(&f);

  fileList.clear();
  
  while(stream.atEnd() == 0)
  {
    s = stream.readLine();
    QByteArray ba;
    ba.append(s);
    ba = ba.trimmed();
    s = ba;

    if (! s.length())
      continue;
      
    QString key;
    QString dat;
    if (s.contains("="))
    {
      int t = s.indexOf("=", 0, Qt::CaseSensitive);
      key = s.left(t);
      dat = s.right(s.length() - t - 1);
    }
    else
      continue;

    if (! key.compare("allSymbols"))
    {
      allSymbols->setChecked(dat.toInt());
      continue;
    }
    
    if (! key.compare("compression"))
    {
      period->setCurrentIndex(period->findText(dat));
      continue;
    }
    
    if (! key.compare("symbol"))
    {
      fileList.append(dat);
      continue;
    }

    if (! key.compare("bars"))
    {
      bars->setValue(dat.toInt());
      continue;
    }

    if (! key.compare("basepath"))
    {
      if (! dat.compare(tr("Chart")))
        basePath->setCurrentIndex(0);
      else
        basePath->setCurrentIndex(1);
      continue;
    }
    
    if (! key.compare("script"))
    {
//      QStringList l2 = dat.split("|");
//      int loop;
//      for (loop = 0; loop < (int) l2.count(); loop++)
//        list->setLine(l2[loop]);
    }
  }

  fileButton->setText(QString::number(fileList.count()) + " Symbols");
  
  f.close();
}

void Scanner::exitDialog ()
{
  emit exitScanner();
  reject();
}

void Scanner::getSymbols ()
{
  QString s;
  QString s2("*");
  SymbolDialog *dialog = new SymbolDialog(this,
                                          s,
  					  s,
					  s2,
					  QFileDialog::ExistingFiles);
  dialog->setWindowTitle(tr("Select symbols to scan"));
  
  int rc = dialog->exec();

  if (rc == QDialog::Accepted)
    fileList = dialog->selectedFiles();
    
  fileButton->setText(QString::number(fileList.count()) + " Symbols");
    
  delete dialog;
}

void Scanner::allSymbolsToggled (bool d)
{
  if (d)
    fileButton->setEnabled(FALSE);
  else
    fileButton->setEnabled(TRUE);
}


