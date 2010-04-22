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

#include "CCI.h"
#include "MAUtils.h"

#include <QtDebug>
#include <cmath>


CCI::CCI ()
{
  indicator = "CCI";

  settings.setData(Color, "red");
  settings.setData(Plot, "Line");
  settings.setData(Label, indicator);
  settings.setData(Smoothing, 10);
  settings.setData(SmoothingType, "SMA");
  settings.setData(Period, 20);
  settings.setData(Ref1, 100);
  settings.setData(Ref2, -100);
  settings.setData(Ref1Color, "white");
  settings.setData(Ref2Color, "white");
}

int CCI::getIndicator (Indicator &ind, BarData *data)
{
  QString s;
  int period = settings.getInt(Period);
  int smoothing = settings.getInt(Smoothing);

  QStringList maList;
  MAUtils mau;
  mau.getMAList(maList);
  
  settings.getData(SmoothingType, s);
  int type = maList.indexOf(s);

  PlotLine *line = getCCI(data, period, smoothing, type);
  if (! line)
    return 1;

  // create the ref1 line
  int ref = settings.getInt(Ref1);
  PlotLine *rline = new PlotLine;
  s = "Horizontal";
  rline->setPlugin(s);
  rline->append(ref);
  settings.getData(Ref1Color, s);
  rline->setColor(s);
  ind.addLine(rline);

  // create the ref2 line
  ref = settings.getInt(Ref2);
  rline = new PlotLine;
  s = "Horizontal";
  rline->setPlugin(s);
  rline->append(ref);
  settings.getData(Ref2Color, s);
  rline->setColor(s);
  ind.addLine(rline);

  settings.getData(Plot, s);
  line->setPlugin(s);
  settings.getData(Label, s);
  line->setLabel(s);
  settings.getData(Color, s);
  line->setColor(s);
  ind.addLine(line);

  return 0;
}

int CCI::getCUS (QStringList &set, QHash<QString, PlotLine *> &tlines, BarData *data)
{
  // INDICATOR,PLUGIN,CCI,<NAME>,<PERIOD>,<SMOOTHING_PERIOD>,<SMOOTHING_TYPE>

  if (set.count() != 7)
  {
    qDebug() << indicator << "::calculate: invalid parm count" << set.count();
    return 1;
  }

  PlotLine *tl = tlines.value(set[3]);
  if (tl)
  {
    qDebug() << indicator << "::calculate: duplicate name" << set[3];
    return 1;
  }

  bool ok;
  int period = set[4].toInt(&ok);
  if (! ok)
  {
    qDebug() << indicator << "::calculate: invalid period parm" << set[4];
    return 1;
  }

  int smoothing = set[5].toInt(&ok);
  if (! ok)
  {
    qDebug() << indicator << "::calculate: invalid smoothing period" << set[5];
    return 1;
  }

  QStringList maList;
  MAUtils mau;
  mau.getMAList(maList);
  int ma = maList.indexOf(set[6]);
  if (ma == -1)
  {
    qDebug() << indicator << "::calculate: invalid smoothing type" << set[6];
    return 1;
  }

  PlotLine *line = getCCI(data, period, smoothing, ma);
  if (! line)
    return 1;

  tlines.insert(set[3], line);

  return 0;
}

PlotLine * CCI::getCCI (BarData *data, int period, int smoothing, int type)
{
  if (data->count() < period)
    return 0;
  
  int size = data->count();
  int loop = 0;
  PlotLine *tp = new PlotLine;
  for (; loop < size; loop++)
  {
    Bar *bar = data->getBar(loop);
    tp->append((bar->getHigh() + bar->getLow() + bar->getClose()) / 3);
  }

  MAUtils mau;
  PlotLine *smatp = mau.getMA(tp, period, MAUtils::SMA);
  if (! smatp)
  {
    delete tp;
    return 0;
  }

  int tpLoop = tp->count() - 1;
  int smatpLoop = smatp->count() - 1;
  PlotLine *cci = new PlotLine;
  while (tpLoop >= period && smatpLoop >= period)
  {
    double t = 0;
    int count = 0;
    for (; count < period; count++)
      t += fabs(tp->getData(tpLoop - count) - smatp->getData(smatpLoop - count));
    double b = (t / (double) period) * 0.015;

    double a = tp->getData(tpLoop) - smatp->getData(smatpLoop);
    cci->prepend(a / b);

    tpLoop--;
    smatpLoop--;
  }
  
  if (smoothing > 1)
  {
    QStringList maList;
    mau.getMAList(maList);
    PlotLine *ma = mau.getMA(cci, smoothing, type);
    delete cci;
    cci = ma;
  }

  delete tp;
  delete smatp;
  
  return cci;
}

int CCI::dialog (int)
{
  int page = 0;
  QString k, d;
  PrefDialog *dialog = new PrefDialog;
  dialog->setWindowTitle(QObject::tr("Edit Indicator"));

  k = QObject::tr("Settings");
  dialog->addPage(page, k);

  settings.getData(Color, d);
  dialog->addColorItem(Color, page, QObject::tr("Color"), d);

  settings.getData(Plot, d);
  dialog->addComboItem(Plot, page, QObject::tr("Plot"), plotList, d);

  settings.getData(Label, d);
  dialog->addTextItem(Label, page, QObject::tr("Label"), d);

  dialog->addIntItem(Period, page, QObject::tr("Period"), settings.getInt(Period), 2, 100000);

  dialog->addIntItem(Smoothing, page, QObject::tr("Smoothing"), settings.getInt(Smoothing), 1, 100000);

  QStringList maList;
  MAUtils mau;
  mau.getMAList(maList);

  settings.getData(SmoothingType, d);
  dialog->addComboItem(SmoothingType, page, QObject::tr("Smoothing Type"), maList, d);

  page++;
  k = QObject::tr("Ref Lines");
  dialog->addPage(page, k);

  settings.getData(Ref1Color, d);
  dialog->addColorItem(Ref1Color, page, QObject::tr("Ref 1 Color"), d);

  settings.getData(Ref2Color, d);
  dialog->addColorItem(Ref2Color, page, QObject::tr("Ref 2 Color"), d);

  dialog->addIntItem(Ref1, page, QObject::tr("Ref 1"), settings.getInt(Ref1), -100000, 100000);

  dialog->addIntItem(Ref2, page, QObject::tr("Ref 2"), settings.getInt(Ref2), -100000, 100000);

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
  CCI *o = new CCI;
  return ((IndicatorPlugin *) o);
}


