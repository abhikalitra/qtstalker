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
#include "stop.xpm"
#include "ok.xpm"
#include "scanner.xpm"
#include "ChartDb.h"
#include "BarData.h"
#include "SymbolDialog.h"

Scanner::Scanner (QString n) : QDialog (0, 0, TRUE)
{
  scannerName = n;
  
  setCaption ("Scanner");

  QVBoxLayout *vbox = new QVBoxLayout(this);
  vbox->setMargin(5);
  vbox->setSpacing(5);
  
  toolbar = new Toolbar(this, 30, 30, FALSE);
  vbox->addWidget(toolbar);

  toolbar->addButton("ok", ok, tr("OK"));
  connect(toolbar->getButton("ok"), SIGNAL(clicked()), this, SLOT(exitDialog()));

  toolbar->addButton("cancel", stop, tr("Cancel"));
  connect(toolbar->getButton("cancel"), SIGNAL(clicked()), this, SLOT(reject()));
  
  toolbar->addButton("scan", scanner, tr("Scan"));
  connect(toolbar->getButton("scan"), SIGNAL(clicked()), this, SLOT(scan()));
  
  QHGroupBox *gbox = new QHGroupBox(tr("Symbol Selection"), this);  
  vbox->addWidget(gbox);

  allSymbols = new QCheckBox(tr("All symbols"), gbox);
  connect(allSymbols, SIGNAL(toggled(bool)), this, SLOT(allSymbolsToggled(bool)));

  fileButton = new QPushButton(tr("Symbols..."), gbox);
  connect(fileButton, SIGNAL(clicked()), this, SLOT(getSymbols()));

  gbox = new QHGroupBox(tr("Compression"), this);  
  vbox->addWidget(gbox);
  
  period = new QComboBox(gbox);
  period->insertItem(tr("Daily"), -1);
  period->insertItem(tr("Weekly"), -1);
  period->insertItem(tr("Monthly"), -1);

  list = new FormulaEdit(this);
  vbox->addWidget(list);

  loadRule();
}

Scanner::~Scanner ()
{
}

void Scanner::scan ()
{
  int loop;
  
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

  for (loop = 0; loop < (int) fileList.count(); loop++)
  {
    ChartDb *db = new ChartDb;
    if (db->openChart(fileList[loop]))
    {
      delete db;
      qDebug("Scanner: Cant open db");
      continue;
    }

    if (! period->currentText().compare(tr("Daily")))
      db->setBarCompression(ChartDb::Daily);
    else
    {
      if (! period->currentText().compare(tr("Weekly")))
        db->setBarCompression(ChartDb::Weekly);
      else
        db->setBarCompression(ChartDb::Monthly);
    }
    
    db->setBarRange(config.getData(Config::Bars).toInt());

    BarData *recordList = db->getHistory();
    
    // open the CUS plugin   
    Plugin *plug = config.getPlugin(Config::IndicatorPluginPath, "CUS");
    if (! plug)
    {
      config.closePlugin("CUS");
      delete recordList;
      delete db;
      continue;
    }

    QStringList l;
    for (loop = 0; loop < list->getLines(); loop++)
      l.append(list->getLine(loop));
  
    // load the CUS plugin and calculate
    plug->setCustomFunction(l.join("|"));
    plug->setIndicatorInput(recordList);
    plug->calculate();
    PlotLine *tline = plug->getIndicatorLine(0);
    if (! tline)
    {
      qDebug("Scanner::scan: no PlotLine returned");
      config.closePlugin("CUS");
      delete recordList;
      delete db;
      continue;
    }
    
    PlotLine *line = new PlotLine;
    line->copy(tline);
    config.closePlugin("CUS");
    
    if (line)
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
    
    delete recordList;
    delete db;
    delete line;
  }
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
  saveRule();
  accept();
  emit exitScanner();
}

void Scanner::getSymbols ()
{
  SymbolDialog *dialog = new SymbolDialog(this,
  					  config.getData(Config::DataPath),
					  "*");
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


