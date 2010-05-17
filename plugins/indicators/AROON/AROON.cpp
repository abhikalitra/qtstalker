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

#include "AROON.h"
#include "PlotFactory.h"
#include "ta_libc.h"

#include <QtDebug>

AROON::AROON ()
{
  TA_RetCode rc = TA_Initialize();
  if (rc != TA_SUCCESS)
    qDebug("AROON::error on TA_Initialize");

  indicator = "AROON";
  settings.setData(Method, "AROON");
  settings.setData(DownColor, "red");
  settings.setData(UpColor, "green");
  settings.setData(DownPlot, "Line");
  settings.setData(UpPlot, "Line");
  settings.setData(DownLabel, "AROOND");
  settings.setData(UpLabel, "AROONU");
  settings.setData(OSCColor, "red");
  settings.setData(OSCPlot, "Histogram Bar");
  settings.setData(OSCLabel, "AROONOSC");
  settings.setData(Period, 14);

  methodList << "AROON";
  methodList << "OSC";
}

int AROON::getIndicator (Indicator &ind, BarData *data)
{
  QString s;
  settings.getData(Method, s);
  int method = methodList.indexOf(s);

  int period = settings.getInt(Period);

  switch (method)
  {
    case 1:
    {
      settings.getData(OSCColor, s);
      QColor color(s);

      settings.getData(OSCPlot, s);
      PlotFactory fac;
      int lineType = fac.typeFromString(s);

      PlotLine *line = getOSC(data, period, lineType, color);
      if (! line)
        return 1;

      settings.getData(OSCLabel, s);
      line->setLabel(s);
      ind.addLine(line);
      break;
    }
    default:
    {
      // get arron up line
      settings.getData(UpColor, s);
      QColor ucolor(s);

      settings.getData(DownColor, s);
      QColor dcolor(s);

      PlotFactory fac;
      settings.getData(UpPlot, s);
      int ulineType = fac.typeFromString(s);

      settings.getData(DownPlot, s);
      int dlineType = fac.typeFromString(s);

      QList<PlotLine *> pl;
      if (getAROON(data, period, ulineType, dlineType, ucolor, dcolor, pl))
        return 1;

      PlotLine *line = pl.at(0);
      settings.getData(UpLabel, s);
      line->setLabel(s);
      ind.addLine(line);

      line = pl.at(1);
      settings.getData(DownLabel, s);
      line->setLabel(s);
      ind.addLine(line);
      break;
    }
  }

  return 0;
}

int AROON::getCUS (QStringList &set, QHash<QString, PlotLine *> &tlines, BarData *data)
{
  // INDICATOR,PLUGIN,AROON,<METHOD>,*
  //     0       1      2      3

  if (set.count() < 4)
  {
    qDebug() << indicator << "::getCUS: invalid settings count" << set.count();
    return 1;
  }

  int rc = 1;
  int method = methodList.indexOf(set[3]);
  switch (method)
  {
    case 0:
      rc = getCUSAROON(set, tlines, data);
      break;
    case 1:
      rc = getCUSOSC(set, tlines, data);
      break;
    default:
      break;
  }

  return rc;
}

int AROON::getCUSAROON (QStringList &set, QHash<QString, PlotLine *> &tlines, BarData *data)
{
  // INDICATOR,PLUGIN,AROON,<METHOD>,<UPPER NAME>,<LOWER NAME>,<PERIOD>,<UPPER PLOT TYPE>,<LOWER PLOT TYPE>,<UPPER COLOR>,<LOWER COLOR>
  //     0       1      2      3           4            5          6            7                 8               9              10       

  if (set.count() != 11)
  {
    qDebug() << indicator << "::getCUSAROON: invalid settings count" << set.count();
    return 1;
  }

  PlotLine *tl = tlines.value(set[4]);
  if (tl)
  {
    qDebug() << indicator << "::getCUSAROON: duplicate upper name" << set[4];
    return 1;
  }

  tl = tlines.value(set[5]);
  if (tl)
  {
    qDebug() << indicator << "::getCUSAROON: duplicate lower name" << set[5];
    return 1;
  }

  bool ok;
  int period = set[6].toInt(&ok);
  if (! ok)
  {
    qDebug() << indicator << "::getCUSAROON: invalid period" << set[6];
    return 1;
  }

  PlotFactory fac;
  int ulineType = fac.typeFromString(set[7]);
  if (ulineType == -1)
  {
    qDebug() << indicator << "::getCUSAROON: invalid upper plot type" << set[7];
    return 1;
  }

  int dlineType = fac.typeFromString(set[8]);
  if (dlineType == -1)
  {
    qDebug() << indicator << "::getCUSAROON: invalid lower plot type" << set[8];
    return 1;
  }

  QColor ucolor(set[9]);
  if (! ucolor.isValid())
  {
    qDebug() << indicator << "::getCUSAROON: invalid upper color" << set[9];
    return 1;
  }

  QColor lcolor(set[10]);
  if (! lcolor.isValid())
  {
    qDebug() << indicator << "::getCUSAROON: invalid lower color" << set[10];
    return 1;
  }

  QList<PlotLine *> pl;
  if (getAROON(data, period, ulineType, dlineType, ucolor, lcolor, pl))
    return 1;

  pl.at(0)->setLabel(set[4]);
  pl.at(1)->setLabel(set[5]);
  
  tlines.insert(set[4], pl.at(0));
  tlines.insert(set[5], pl.at(1));

  return 0;
}

int AROON::getCUSOSC (QStringList &set, QHash<QString, PlotLine *> &tlines, BarData *data)
{
  // INDICATOR,PLUGIN,AROON,<METHOD>,<NAME>,<PERIOD>,<PLOT TYPE>,<COLOR>
  //     0       1      2      3       4       5          6         7

  if (set.count() != 8)
  {
    qDebug() << indicator << "::getCUSOSC: invalid settings count" << set.count();
    return 1;
  }

  PlotLine *tl = tlines.value(set[4]);
  if (tl)
  {
    qDebug() << indicator << "::getCUSOSC: duplicate name" << set[4];
    return 1;
  }

  bool ok;
  int period = set[5].toInt(&ok);
  if (! ok)
  {
    qDebug() << indicator << "::getCUSOSC: invalid period" << set[5];
    return 1;
  }

  PlotFactory fac;
  int lineType = fac.typeFromString(set[6]);
  if (lineType == -1)
  {
    qDebug() << indicator << "::getCUSOSC: invalid plot type" << set[6];
    return 1;
  }

  QColor color(set[7]);
  if (! color.isValid())
  {
    qDebug() << indicator << "::getCUSOSC: invalid color" << set[7];
    return 1;
  }

  PlotLine *line = getOSC(data, period, lineType, color);
  if (! line)
    return 1;

  line->setLabel(set[4]);

  tlines.insert(set[4], line);

  return 0;
}

int AROON::getAROON (BarData *data, int period, int ulineType, int llineType, QColor &ucolor, QColor &lcolor,
                     QList<PlotLine *> &pl)
{
  int size = data->count();
  if (size < period)
    return 1;
  
  TA_Real high[size];
  TA_Real low[size];
  TA_Real dout[size];
  TA_Real uout[size];
  TA_Integer outBeg;
  TA_Integer outNb;

  int loop = 0;
  for (; loop < size; loop++)
  {
    Bar *bar = data->getBar(loop);
    high[loop] = (TA_Real) bar->getHigh();
    low[loop] = (TA_Real) bar->getLow();
  }

  TA_RetCode rc = TA_AROON(0,
                           size - 1,
                           &high[0],
                           &low[0],
                           period,
                           &outBeg,
                           &outNb,
                           &dout[0],
                           &uout[0]);
  if (rc != TA_SUCCESS)
  {
    qDebug() << indicator << "::getAROON: TA-Lib error" << rc;
    return 1;
  }

  PlotFactory fac;
  PlotLine *upper = fac.plot(ulineType);
  if (! upper)
    return 1;

  PlotLine *lower = fac.plot(llineType);
  if (! lower)
  {
    delete upper;
    return 1;
  }

  int dataLoop = size - 1;
  int outLoop = outNb - 1;
  while (outLoop > -1 && dataLoop > -1)
  {
    upper->setData(dataLoop, new PlotLineBar(ucolor, uout[outLoop]));
    lower->setData(dataLoop, new PlotLineBar(lcolor, dout[outLoop]));

    dataLoop--;
    outLoop--;
  }

  pl.append(upper);
  pl.append(lower);

  return 0;
}

PlotLine * AROON::getOSC (BarData *data, int period, int lineType, QColor &color)
{
  if (data->count() < period)
    return 0;

  int size = data->count();

  TA_Real high[size];
  TA_Real low[size];
  TA_Real out[size];
  TA_Integer outBeg;
  TA_Integer outNb;

  int loop = 0;
  for (; loop < size; loop++)
  {
    Bar *bar = data->getBar(loop);
    high[loop] = (TA_Real) bar->getHigh();
    low[loop] = (TA_Real) bar->getLow();
  }

  TA_RetCode rc = TA_AROONOSC(0,
                              size - 1,
                              &high[0],
                              &low[0],
                              period,
                              &outBeg,
                              &outNb,
                              &out[0]);
  if (rc != TA_SUCCESS)
  {
    qDebug() << indicator << "::getAROONOSC: TA-Lib error" << rc;
    return 0;
  }

  PlotFactory fac;
  PlotLine *line = fac.plot(lineType);
  if (! line)
    return 0;

  int dataLoop = size - 1;
  int outLoop = outNb - 1;
  while (outLoop > -1 && dataLoop > -1)
  {
    line->setData(dataLoop, new PlotLineBar(color, out[outLoop]));
    dataLoop--;
    outLoop--;
  }

  return line;
}

int AROON::dialog (int)
{
  int page = 0;
  QString k, d;
  PrefDialog *dialog = new PrefDialog;
  dialog->setWindowTitle(QObject::tr("Edit Indicator"));

  k = QObject::tr("General");
  dialog->addPage(page, k);

  dialog->addIntItem(Period, page, QObject::tr("Period"), settings.getInt(Period), 2, 100000);

  settings.getData(Method, d);
  dialog->addComboItem(Method, page, QObject::tr("Method"), methodList, d);

  page++;
  k = QObject::tr("Aroon");
  dialog->addPage(page, k);

  settings.getData(UpColor, d);
  dialog->addColorItem(UpColor, page, QObject::tr("Up Color"), d);

  settings.getData(DownColor, d);
  dialog->addColorItem(DownColor, page, QObject::tr("Down Color"), d);

  PlotFactory fac;
  QStringList plotList;
  fac.list(plotList, TRUE);

  settings.getData(UpPlot, d);
  dialog->addComboItem(UpPlot, page, QObject::tr("Up Plot"), plotList, d);

  settings.getData(DownPlot, d);
  dialog->addComboItem(DownPlot, page, QObject::tr("Down Plot"), plotList, d);

  settings.getData(UpLabel, d);
  dialog->addTextItem(UpLabel, page, QObject::tr("Up Label"), d);

  settings.getData(DownLabel, d);
  dialog->addTextItem(DownLabel, page, QObject::tr("Down Label"), d);

  page++;
  k = QObject::tr("OSC");
  dialog->addPage(page, k);

  settings.getData(OSCColor, d);
  dialog->addColorItem(OSCColor, page, QObject::tr("Color"), d);

  settings.getData(OSCPlot, d);
  dialog->addComboItem(OSCPlot, page, QObject::tr("Plot"), plotList, d);

  settings.getData(OSCLabel, d);
  dialog->addTextItem(OSCLabel, page, QObject::tr("Label"), d);

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
  AROON *o = new AROON;
  return ((IndicatorPlugin *) o);
}


