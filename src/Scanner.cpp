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

#include <qlayout.h>
#include <qinputdialog.h>
#include <qmessagebox.h>
#include <qdatetime.h>
#include <qdir.h>
#include <qhgroupbox.h>
#include "Scanner.h"
#include "BarData.h"
#include "SymbolDialog.h"
#include "ChartDb.h"
#include "IndicatorPlugin.h"

Scanner::Scanner (QString n) : QTabDialog (0, 0, FALSE)
{
  scannerName = n;
  
  QString s = "Qtstalker Scanner";
  s.append(": ");
  s.append(scannerName);
  setCaption (s);

  QWidget *w = new QWidget(this);
  
  QVBoxLayout *vbox = new QVBoxLayout(w);
  vbox->setMargin(5);
  vbox->setSpacing(5);
  
  QHGroupBox *gbox = new QHGroupBox(tr("Symbol Selection"), w);  
  vbox->addWidget(gbox);

  allSymbols = new QCheckBox(tr("All symbols"), gbox);
  connect(allSymbols, SIGNAL(toggled(bool)), this, SLOT(allSymbolsToggled(bool)));

  fileButton = new QPushButton(tr("Symbols..."), gbox);
  connect(fileButton, SIGNAL(clicked()), this, SLOT(getSymbols()));

  gbox = new QHGroupBox(tr("Bars/Compression"), w);  
  vbox->addWidget(gbox);
  
  bars = new QSpinBox(1, 9999, 1, gbox);
  bars->setValue(275);
  
  period = new QComboBox(gbox);
  period->insertItem(tr("Daily"), -1);
  period->insertItem(tr("Weekly"), -1);
  period->insertItem(tr("Monthly"), -1);
  
  list = new FormulaEdit(w);
  vbox->addWidget(list);
  
  vbox->addSpacing(10);
  
  progbar = new QProgressBar(0, w);
  progbar->reset();
  vbox->addWidget(progbar);
  
  setDefaultButton(tr("&Scan"));
  connect(this, SIGNAL(defaultButtonPressed()), this, SLOT(scan()));
  
  setApplyButton(tr("&Apply"));  
  connect(this, SIGNAL(applyButtonPressed()), this, SLOT(saveRule()));
  
  setCancelButton(tr("&Cancel"));  
  connect(this, SIGNAL(cancelButtonPressed()), this, SLOT(exitDialog()));
  
  setOkButton(QString::null);  
  
  addTab(w, tr("Parms"));

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

  int loop;
  bool flag = FALSE;
  for (loop = 0; loop < list->getLines(); loop++)
  {
    Setting set;
    set.parse(list->getLine(loop));
    if (! set.getData("plugin").compare("COMP"))
    {
      if (set.getData("plot").toInt())
      {
        flag = TRUE;
	break;
      }
    }
  }
  
  if (! flag)
  {
    QMessageBox::information(this,
                             tr("Qtstalker: Error"),
			     tr("No COMP step or COMP step not checked."));
    return;
  }
  
  // clear dir for scan symbols
  QDir dir;
  QString s = config.getData(Config::GroupPath);
  s.append("/Scanner");
  if (! dir.exists(s, TRUE))
    dir.mkdir(s, TRUE);
  s.append("/");
  s.append(scannerName);
  if (! dir.exists(s, TRUE))
    dir.mkdir(s, TRUE);
  else
  {
    QString s2 =  "rm -r ";
    s2.append(s);
    system (s2);
    dir.mkdir(s, TRUE);
  }
  
  if (allSymbols->isChecked())
  {
    Traverse *trav = new Traverse();
    trav->traverse(config.getData(Config::DataPath) + "/Stocks");
    fileList = trav->getList();
    delete trav;
  }

  progbar->setTotalSteps(fileList.count());
  
  for (loop = 0; loop < (int) fileList.count(); loop++)
  {
    progbar->setProgress(loop);
    emit message(QString());
  
    ChartDb *db = new ChartDb;
    db->openChart(fileList[loop]);

    if (! period->currentText().compare(tr("Daily")))
      db->setBarCompression(BarData::DailyBar);
    else
    {
      if (! period->currentText().compare(tr("Weekly")))
        db->setBarCompression(BarData::WeeklyBar);
      else
        db->setBarCompression(BarData::MonthlyBar);
    }
    
    db->setBarRange(bars->value());

    BarData *recordList = db->getHistory();
    
    // open the CUS plugin   
    IndicatorPlugin *plug2 = config.getIndicatorPlugin("CUS");
    if (! plug2)
    {
      config.closePlugin("CUS");
      delete recordList;
      delete db;
      continue;
    }

    int loop2;
    for (loop2 = 0; loop2 < list->getLines(); loop2++)
      plug2->setCustomFunction(list->getLine(loop2));
  
    // load the CUS plugin and calculate
    plug2->setIndicatorInput(recordList);
    plug2->calculate();
    Indicator *i = plug2->getIndicator();
    if (! i->getLines())
    {
      config.closePlugin("CUS");
      delete recordList;
      delete db;
      continue;
    }
    PlotLine *line = i->getLine(0);
    if (line && line->getSize() > 0)
    {
      if (line->getData(line->getSize() - 1) > 0)
      {
        QString s = "ln -s ";
        s.append(fileList[loop]);
        s.append(" ");
        s.append(config.getData(Config::GroupPath));
        s.append("/Scanner/");
        s.append(scannerName);
        system (s);
      }
    }
    
    config.closePlugin("CUS");
    delete recordList;
    delete db;
  }
  
  progbar->reset();
}

void Scanner::saveRule ()
{
  QString s = config.getData(Config::ScannerPath);
  s.append("/");
  s.append(scannerName);

  QFile f(s);
  if (! f.open(IO_WriteOnly))
    return;
  QTextStream stream(&f);

  int loop;
  for (loop = 0; loop < list->getLines(); loop++)
    stream << list->getLine(loop) << "\n";
  
  f.close();
}

void Scanner::loadRule ()
{
  QString s = config.getData(Config::ScannerPath);
  s.append("/");
  s.append(scannerName);

  QFile f(s);
  if (! f.open(IO_ReadOnly))
    return;
  QTextStream stream(&f);

  while(stream.atEnd() == 0)
  {
    s = stream.readLine();
    s = s.stripWhiteSpace();

    if (! s.length())
      continue;
      
    list->setLine(s);
  }

  f.close();
}

void Scanner::exitDialog ()
{
  reject();
  emit exitScanner();
}

void Scanner::getSymbols ()
{
  SymbolDialog *dialog = new SymbolDialog(this,
  					  config.getData(Config::DataPath),
					  "*",
					  QFileDialog::ExistingFiles);
  dialog->setCaption(tr("Select symbols to scan"));
  
  int rc = dialog->exec();

  if (rc == QDialog::Accepted)
    fileList = dialog->selectedFiles();
    
  delete dialog;
}

void Scanner::allSymbolsToggled (bool d)
{
  if (d)
    fileButton->setEnabled(FALSE);
  else
    fileButton->setEnabled(TRUE);
}

//******************************************************
//**************** TRAVERSE ****************************
//******************************************************

Traverse::Traverse()
{
}

Traverse::~Traverse()
{
}

void Traverse::traverse(QString dirname)
{
  list.clear();
  
  QDir dir(dirname);
  dir.setFilter(QDir::Dirs|QDir::Files);

  const QFileInfoList *fileinfolist = dir.entryInfoList();
  QFileInfoListIterator it(*fileinfolist);
  QFileInfo *fi;
  while((fi = it.current()))
  {
    if(fi->fileName() == "." || fi->fileName() == "..")
    {
      ++it;
      continue;
    }

    if(fi->isDir() && fi->isReadable())
      traverse(fi->absFilePath());
    else
      list.append(fi->absFilePath());

    ++it;
  }
}

QStringList Traverse::getList()
{
  return list;
}


