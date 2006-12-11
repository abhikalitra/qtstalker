/*
 *  Qtstalker stock charter
 *
 *  Copyright (C) 2001-2006 Stefan S. Stratigakos
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

#include "IndicatorSummary.h"
#include "DbPlugin.h"
#include "IndicatorPlugin.h"
#include "Setting.h"
#include "DataWindow.h"
#include "PrefDialog.h"
#include "Traverse.h"
#include <qdir.h>
#include <qdatetime.h>
#include <qfileinfo.h>
#include <qmessagebox.h>

IndicatorSummary::IndicatorSummary (QStringList &l, int mb, BarData::BarLength bl)
{
  indicatorList = l;
  minBars = mb;
  barLength = bl;

  indicators.setAutoDelete(TRUE);
}

IndicatorSummary::~IndicatorSummary ()
{
}

void IndicatorSummary::run ()
{
  PrefDialog *dialog = new PrefDialog;
  dialog->setCaption(QObject::tr("Indicator Summary Parms"));
  
  QString pl = QObject::tr("Details");
  QString sl = QObject::tr("Group");
  
  dialog->createPage(pl);

  // get the groups list
  QString s;
  config.getData(Config::GroupPath, s);
  Traverse trav(Traverse::Dir);
  trav.traverse(s);
  emit signalWakeup();
  QStringList l;
  trav.getList(l);
  dialog->addComboItem(sl, pl, l, l[0]);
  
  int rc = dialog->exec();
  
  if (rc != QDialog::Accepted)
  {
    delete dialog;
    return;
  }

  loadIndicators();

  QString group;
  dialog->getCombo(sl, group);
  delete dialog;

  QPtrList<Setting> setList;
  setList.setAutoDelete(TRUE);

  int loop;
  QDir dir(group);
  for (loop = 2; loop < (int) dir.count(); loop++)
  {
    s = dir.absPath() + "/" + dir[loop];
    QFileInfo fi(s);
    if (fi.isDir())
      continue;

    DbPlugin db;
    if (db.openChart(s))
      continue;

    db.setBarRange(minBars);
    db.setBarLength(barLength);

    BarData *recordList = new BarData(s);
    QDateTime dt = QDateTime::currentDateTime();
    db.getHistory(recordList, dt);
    db.close();

    emit signalWakeup();

    Setting *fd = new Setting;
    setList.append(fd);
    QString ts = "Symbol";
    s = dir[loop];
    fd->setData(ts, s);

    int loop2;
    for (loop2 = 0; loop2 < (int) indicators.count(); loop2++)
    {
      Indicator *i = indicators.at(loop2);
      i->getType(s);
      IndicatorPlugin *plug = config.getIndicatorPlugin(s);
      if (! plug)
        continue;

      i->getFile(s);
      plug->loadIndicatorSettings(s);
      plug->clearOutput();
      plug->setIndicatorInput(recordList);
      plug->calculate();

      emit signalWakeup();
  
      Indicator *ri = plug->getIndicator();
      int loop3;
      for (loop3 = 0; loop3 < ri->getLines(); loop3++)
      {
        PlotLine *line = ri->getLine(loop3);
        if (! line)
          continue;

        if (line->getSize() < 1)
          continue;

        if (line->getType() == PlotLine::Bar || line->getType() == PlotLine::Candle ||
            line->getType() == PlotLine::Horizontal || line->getType() == PlotLine::Invisible)
          continue;

        line->strip(line->getData(line->getSize() - 1), 4, s);
        line->getLabel(ts);
        fd->setData(ts, s);
      }

      emit signalWakeup();
    }

    delete recordList;
  }

  if (! setList.count())
  {
    QMessageBox::information(0, tr("Qtstalker: Warning"), tr("No files in group."));
    return;
  }

  createDataWindow(setList, group);
}

void IndicatorSummary::loadIndicators ()
{
  int loop;
  for (loop = 0; loop < (int) indicatorList.count(); loop++)
  {
    Setting set;
    config.getIndicator(indicatorList[loop], set);
    if (! set.count())
      continue;
    
    Indicator *i = new Indicator;
    i->setIndicator(set, indicatorList[loop]);
    if (! i->getEnable())
    {
      delete i;
      continue;
    }

    QString plugin;
    i->getType(plugin);  
    IndicatorPlugin *plug = config.getIndicatorPlugin(plugin);
    if (! plug)
    {
      delete i;
      continue;
    }

    indicators.append(i);
  }
}

void IndicatorSummary::createDataWindow (QPtrList<Setting> &list, QString &group)
{
  DataWindow *dw = new DataWindow(0);
  QFileInfo fi(group);
  QString s = QObject::tr("Indicator Summary ") + fi.fileName();
  dw->setCaption(s);

  int loop;
  QStringList klist;
  for (loop = 0; loop < (int) list.count(); loop++)
  {
    Setting *set = list.at(loop);
    QStringList l;
    set->getKeyList(l);

    int loop2;
    for (loop2 = 0; loop2 < (int) l.count(); loop2++)
    {
      if (klist.findIndex(l[loop2]) == -1)
        klist.append(l[loop2]);
    }
  }

  s = tr("Symbol");
  dw->setHeader(0, s);
  klist.remove(s);
  klist.sort();
  klist.prepend(s);
  for (loop = 0; loop < (int) klist.count(); loop++)
  {
    s = klist[loop];
    dw->setHeader(loop, s);
  }

  for (loop = 0; loop < (int) list.count(); loop++)
  {
    Setting *set = list.at(loop);
    int loop2;
    for (loop2 = 0; loop2 < (int) klist.count(); loop2++)
    {
      s = klist[loop2];
      QString s2;
      set->getData(s, s2);
      if (! s2.length())
        continue;
      dw->setData(loop, loop2, s2);
    }
  }

  dw->show();
}

