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

#include "MFI.h"
#include "MAUtils.h"

#include <QtDebug>


MFI::MFI ()
{
  indicator = "MFI";

  settings.setData(Color, "red");
  settings.setData(Plot, "Line");
  settings.setData(Label, indicator);
  settings.setData(Period, 14);
  settings.setData(Smoothing, 10);
  settings.setData(SmoothingType, "SMA");
  settings.setData(Ref1Color, "white");
  settings.setData(Ref2Color, "white");
  settings.setData(Ref1, 20);
  settings.setData(Ref2, 80);
}

int MFI::getIndicator (Indicator &ind, BarData *data)
{
  QString s;
  int period = settings.getInt(Period);
  int smoothing = settings.getInt(Smoothing);

  QStringList maList;
  MAUtils mau;
  mau.getMAList(maList);
  
  settings.getData(SmoothingType, s);
  int type = maList.indexOf(s);

  PlotLine *line = getMFI(data, period, smoothing, type);
  if (! line)
    return 1;

  PlotLine *ref1 = new PlotLine;
  s = "Horizontal";
  ref1->setPlugin(s);
  ref1->append(settings.getInt(Ref1));
  settings.getData(Ref1Color, s);
  ref1->setColor(s);
  ind.addLine(ref1);

  PlotLine *ref2 = new PlotLine;
  s = "Horizontal";
  ref2->setPlugin(s);
  ref2->append(settings.getInt(Ref2));
  settings.getData(Ref2Color, s);
  ref2->setColor(s);
  ind.addLine(ref2);

  settings.getData(Color, s);
  line->setColor(s);
  settings.getData(Plot, s);
  line->setPlugin(s);
  settings.getData(Label, s);
  line->setLabel(s);
  ind.addLine(line);

  return 0;
}

int MFI::getCUS (QStringList &set, QHash<QString, PlotLine *> &tlines, BarData *data)
{
  // INDICATOR,PLUGIN,MFI,<NAME>,<PERIOD>,<SMOOTHING_PERIOD>,<SMOOTHING_TYPE>

  if (set.count() != 7)
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

  bool ok;
  int period = set[4].toInt(&ok);
  if (! ok)
  {
    qDebug() << indicator << "::calculate: invalid period settings" << set[4];
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

  PlotLine *line = getMFI(data, period, smoothing, ma);
  if (! line)
    return 1;

  tlines.insert(set[3], line);

  return 0;
}

PlotLine * MFI::getMFI (BarData *data, int period, int smoothing, int type)
{
  if (data->count() < period)
    return 0;

  int size = data->count();
  PlotLine *line = new PlotLine;
  int loop = period;
  for (; loop < size; loop++)
  {
    double up = 0;
    double down = 0;
    int count = 0;
    for (; count < period; count++)
    {
      Bar *bar = data->getBar(loop - count);
      Bar *pbar = data->getBar(loop - count - 1);
      double tp = (bar->getHigh() + bar->getLow() + bar->getClose()) / 3;
      double ptp = (pbar->getHigh() + pbar->getLow() + pbar->getClose()) / 3;
      if (tp > ptp)
	up += tp * bar->getVolume();
      else
      {
        if (tp < ptp)
	  down += tp * bar->getVolume();
      }
    }

    double mr = up / down;
    line->append(100 - (100 / (1 + mr)));
  }

  if (smoothing > 1)
  {
    MAUtils mau;
    PlotLine *ma = mau.getMA(line, smoothing, type);
    delete line;
    line = ma;
  }

  return line;
}

int MFI::dialog (int)
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
  dialog->addComboItem(Smoothing, page, QObject::tr("Smoothing Type"), maList, d);

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
  MFI *o = new MFI;
  return ((IndicatorPlugin *) o);
}


