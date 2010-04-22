/*
 *  Qtstalker stock charter
 *
 *  Copyright (C) 2001-2010 Stefan S. Stratigakos
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

#include "STOCH.h"
#include "MAUtils.h"
#include "STOCHUtils.h"

#include <QtDebug>

STOCH::STOCH ()
{
  indicator = "STOCH";

  settings.setData(FastKColor, "red");
  settings.setData(FastDColor, "yellow");
  settings.setData(Ref1Color, "white");
  settings.setData(Ref2Color, "white");
  settings.setData(FastKPlot, "Line");
  settings.setData(FastDPlot, "Dash");
  settings.setData(FastKLabel, "FASTK");
  settings.setData(FastDLabel, "FASTD");
  settings.setData(FastKPeriod, 5);
  settings.setData(FastDPeriod, 3);
  settings.setData(FastDMA, "SMA");
  settings.setData(Ref1, 20);
  settings.setData(Ref2, 80);
}

int STOCH::getIndicator (Indicator &ind, BarData *data)
{
  // create first ref line
  PlotLine *ref1 = new PlotLine;
  QString s = "Horizontal";
  ref1->setPlugin(s);
  ref1->append(settings.getInt(Ref1));
  settings.getData(Ref1Color, s);
  ref1->setColor(s);
  ind.addLine(ref1);

  // create second ref line
  PlotLine *ref2 = new PlotLine;
  s = "Horizontal";
  ref2->setPlugin(s);
  ref2->append(settings.getInt(Ref2));
  settings.getData(Ref2Color, s);
  ref2->setColor(s);
  ind.addLine(ref2);

  // create the fastk line
  int period = settings.getInt(FastKPeriod);
  STOCHUtils fastk;
  PlotLine *fk = fastk.getFastK(data, period);
  if (! fk)
    return 1;

  settings.getData(FastKColor, s);
  fk->setColor(s);
  settings.getData(FastKPlot, s);
  fk->setPlugin(s);
  settings.getData(FastKLabel, s);
  fk->setLabel(s);
  ind.addLine(fk);
  
  // get the MA list
  QStringList maList;
  MAUtils mau;
  mau.getMAList(maList);

  // create the fastd line
  period = settings.getInt(FastDPeriod);
    
  settings.getData(FastDMA, s);
  int maType = maList.indexOf(s);
    
  PlotLine *fd = mau.getMA(fk, period, maType);
  if (! fd)
    return 1;
  
  settings.getData(FastDColor, s);
  fd->setColor(s);
  settings.getData(FastDPlot, s);
  fd->setPlugin(s);
  settings.getData(FastDLabel, s);
  fd->setLabel(s);
  ind.addLine(fd);

  return 0;
}

int STOCH::getCUS (QStringList &set, QHash<QString, PlotLine *> &tlines, BarData *data)
{
  // INDICATOR,PLUGIN,STOCH,<NAME FASTK>,<NAME FASTD>,<FASTK PERIOD>,<FASTD PERIOD>,<FASTD MA TYPE>
  //     0        1    2         3            4              5             6              7

  if (set.count() != 8)
  {
    qDebug() << indicator << "::calculate: invalid settings count" << set.count();
    return 1;
  }

  PlotLine *tl = tlines.value(set[3]);
  if (tl)
  {
    qDebug() << indicator << "::calculate: duplicate name" << set[3];
    return 1;
  }

  tl = tlines.value(set[4]);
  if (tl)
  {
    qDebug() << indicator << "::calculate: duplicate name" << set[4];
    return 1;
  }

  bool ok;
  int fkp = set[5].toInt(&ok);
  if (! ok)
  {
    qDebug() << indicator << "::calculate: invalid fastk period" << set[5];
    return 1;
  }

  int fdp = set[6].toInt(&ok);
  if (! ok)
  {
    qDebug() << indicator << "::calculate: invalid fastd period" << set[6];
    return 1;
  }

  QStringList maList;
  MAUtils mau;
  mau.getMAList(maList);
  int ma = maList.indexOf(set[7]);
  if (ma == -1)
  {
    qDebug() << indicator << "::calculate: invalid fastd ma" << set[7];
    return 1;
  }

  STOCHUtils fastk;
  PlotLine *fk = fastk.getFastK(data, fkp);
  if (! fk)
    return 1;

  PlotLine *fd = mau.getMA(fk, fdp, ma);
  if (! fd)
  {
    delete fk;
    return 1;
  }

  tlines.insert(set[3], fk);
  tlines.insert(set[4], fd);

  return 0;
}

int STOCH::dialog (int)
{
  int page = 0;
  QString k, d;
  PrefDialog *dialog = new PrefDialog;
  dialog->setWindowTitle(QObject::tr("Edit Indicator"));

  k = QObject::tr("FastK");
  dialog->addPage(page, k);

  settings.getData(FastKColor, d);
  dialog->addColorItem(FastKColor, page, QObject::tr("Color"), d);

  settings.getData(FastKPlot, d);
  dialog->addComboItem(FastKPlot, page, QObject::tr("Plot"), plotList, d);

  settings.getData(FastKLabel, d);
  dialog->addTextItem(FastKLabel, page, QObject::tr("Label"), d);

  dialog->addIntItem(FastKPeriod, page, QObject::tr("Period"), settings.getInt(FastKPeriod), 1, 100000);

  page++;
  k = QObject::tr("FastD");
  dialog->addPage(page, k);

  settings.getData(FastDColor, d);
  dialog->addColorItem(FastDColor, page, QObject::tr("Color"), d);

  settings.getData(FastDPlot, d);
  dialog->addComboItem(FastDPlot, page, QObject::tr("Plot"), plotList, d);

  settings.getData(FastDLabel, d);
  dialog->addTextItem(FastDLabel, page, QObject::tr("Label"), d);

  dialog->addIntItem(FastDPeriod, page, QObject::tr("Period"), settings.getInt(FastDPeriod), 1, 100000);

  QStringList maList;
  MAUtils mau;
  mau.getMAList(maList);

  settings.getData(FastDMA, d);
  dialog->addComboItem(FastDMA, page, QObject::tr("MA Type"), maList, d);

  page++;
  k = QObject::tr("Ref");
  dialog->addPage(page, k);

  settings.getData(Ref1Color, d);
  dialog->addColorItem(Ref1Color, page, QObject::tr("Ref. 1 Color"), d);

  settings.getData(Ref2Color, d);
  dialog->addColorItem(Ref2Color, page, QObject::tr("Ref. 2 Color"), d);

  dialog->addIntItem(Ref1, page, QObject::tr("Ref. 1"), settings.getInt(Ref1), 0, 100);

  dialog->addIntItem(Ref2, page, QObject::tr("Ref. 2"), settings.getInt(Ref2), 0, 100);

  int rc = dialog->exec();
  if (rc == QDialog::Rejected)
  {
    delete dialog;
    return rc;
  }

  getDialogSettings(dialog);

  delete dialog;
  return rc;
}

//*************************************************************
//*************************************************************
//*************************************************************

IndicatorPlugin * createIndicatorPlugin ()
{
  STOCH *o = new STOCH;
  return ((IndicatorPlugin *) o);
}


