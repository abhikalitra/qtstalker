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
#include "ta_libc.h"
#include "MATH1.h"

#include <QtDebug>


CCI::CCI ()
{
  indicator = "CCI";

  settings.setData(Color, "red");
  settings.setData(Plot, "Line");
  settings.setData(Label, indicator);
  settings.setData(Smoothing, 10);
  settings.setData(SmoothingType, "SMA");
  settings.setData(Period, 14);
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

  MATH1 m;
  QStringList maList;
  m.getMAList(maList);
  
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
  // INDICATOR,CCI,<NAME>,<PERIOD>,<SMOOTHING_PERIOD>,<SMOOTHING_TYPE>

  if (set.count() != 6)
  {
    qDebug() << indicator << "::calculate: invalid parm count" << set.count();
    return 1;
  }

  PlotLine *tl = tlines.value(set[2]);
  if (tl)
  {
    qDebug() << indicator << "::calculate: duplicate name" << set[2];
    return 1;
  }

  bool ok;
  int period = set[3].toInt(&ok);
  if (! ok)
  {
    qDebug() << indicator << "::calculate: invalid period parm" << set[3];
    return 1;
  }

  int smoothing = set[4].toInt(&ok);
  if (! ok)
  {
    qDebug() << indicator << "::calculate: invalid smoothing period" << set[4];
    return 1;
  }

  MATH1 m;
  QStringList maList;
  m.getMAList(maList);
  int ma = maList.indexOf(set[5]);
  if (ma == -1)
  {
    qDebug() << indicator << "::calculate: invalid smoothing type" << set[5];
    return 1;
  }

  PlotLine *line = getCCI(data, period, smoothing, ma);
  if (! line)
    return 1;

  tlines.insert(set[2], line);

  return 0;
}

PlotLine * CCI::getCCI (BarData *data, int period, int smoothing, int type)
{
  int size = data->count();
  TA_Real high[size];
  TA_Real low[size];
  TA_Real close[size];
  TA_Real out[size];
  int loop;
  for (loop = 0; loop < size; loop++)
  {
    high[loop] = (TA_Real) data->getHigh(loop);
    low[loop] = (TA_Real) data->getLow(loop);
    close[loop] = (TA_Real) data->getClose(loop);
  }

  TA_Integer outBeg;
  TA_Integer outNb;
  TA_RetCode rc = TA_CCI(0, size - 1, &high[0], &low[0], &close[0], period, &outBeg, &outNb, &out[0]);
  if (rc != TA_SUCCESS)
  {
    qDebug() << indicator << "::calculate: TA-Lib error" << rc;
    return 0;
  }

  PlotLine *line = new PlotLine;
  for (loop = 0; loop < outNb; loop++)
    line->append(out[loop]);

  if (smoothing > 1)
  {
    MATH1 m;
    QStringList maList;
    m.getMAList(maList);
    PlotLine *ma = m.getMA(line, smoothing, type);
    if (! ma)
    {
      delete line;
      return 0;
    }

    delete line;
    line = ma;
  }

  return line;
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

  MATH1 m;
  QStringList maList;
  m.getMAList(maList);

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


