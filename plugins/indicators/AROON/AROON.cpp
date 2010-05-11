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

#include <QtDebug>


AROON::AROON ()
{
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

  methodList << "UP";
  methodList << "DOWN";
  methodList << "OSC";

  guiMethodList << "AROON";
  guiMethodList << "OSC";
}

int AROON::getIndicator (Indicator &ind, BarData *data)
{
  QString s;
  settings.getData(Method, s);
  int method = guiMethodList.indexOf(s);

  int period = settings.getInt(Period);

  switch (method)
  {
    case 1:
    {
      PlotLine *line = getOSC(data, period);
      if (! line)
        return 1;

      settings.getData(OSCColor, s);
      line->setColor(s);
      settings.getData(OSCPlot, s);
      line->setPlugin(s);
      settings.getData(OSCLabel, s);
      line->setLabel(s);
      ind.addLine(line);
      break;
    }
    default:
    {
      // get arron up line
      PlotLine *up = getUP(data, period);
      if (! up)
        return 1;

      settings.getData(UpColor, s);
      up->setColor(s);
      settings.getData(UpPlot, s);
      up->setPlugin(s);
      settings.getData(UpLabel, s);
      up->setLabel(s);

      // get aroon down line
      PlotLine *down = getDOWN(data, period);
      if (! down)
      {
        delete up;
        return 1;
      }

      settings.getData(DownColor, s);
      down->setColor(s);
      settings.getData(DownPlot, s);
      down->setPlugin(s);
      settings.getData(DownLabel, s);
      down->setLabel(s);
      ind.addLine(up);
      ind.addLine(down);
      break;
    }
  }

  return 0;
}

int AROON::getCUS (QStringList &set, QHash<QString, PlotLine *> &tlines, BarData *data)
{
  // INDICATOR,PLUGIN,AROON,<METHOD>,*
  //      0       1     2      3

  if (set.count() < 4)
  {
    qDebug() << indicator << "::getCUS: invalid settings count" << set.count();
    return 1;
  }

  int method = methodList.indexOf(set[3]);
  if (method == -1)
  {
    qDebug() << indicator << "::getCUS: invalid method" << set[3];
    return 1;
  }

  int rc = 1;

  switch ((_Method) method)
  {
    case Up:
      rc = getCUSUP(set, tlines, data);
      break;
    case Down:
      rc = getCUSDOWN(set, tlines, data);
      break;
    case OSC:
      rc = getCUSOSC(set, tlines, data);
      break;
    default:
      break;
  }

  return rc;
}

int AROON::getCUSUP (QStringList &set, QHash<QString, PlotLine *> &tlines, BarData *data)
{
  // INDICATOR,PLUGIN,AROON,UP,<NAME>,<PERIOD>

  if (set.count() != 6)
  {
    qDebug() << indicator << "::getCUSUP: invalid settings count" << set.count();
    return 1;
  }

  PlotLine *tl = tlines.value(set[4]);
  if (tl)
  {
    qDebug() << indicator << "::getCUSUP: duplicate name" << set[4];
    return 1;
  }

  bool ok;
  int period = set[5].toInt(&ok);
  if (! ok)
  {
    qDebug() << indicator << "::getCUSUP: invalid period" << set[5];
    return 1;
  }

  PlotLine *line = getUP(data, period);
  if (! line)
    return 1;

  tlines.insert(set[4], line);

  return 0;
}

int AROON::getCUSDOWN (QStringList &set, QHash<QString, PlotLine *> &tlines, BarData *data)
{
  // INDICATOR,PLUGIN,AROON,DOWN,<NAME>,<PERIOD>

  if (set.count() != 6)
  {
    qDebug() << indicator << "::getCUSDOWN: invalid settings count" << set.count();
    return 1;
  }

  PlotLine *tl = tlines.value(set[4]);
  if (tl)
  {
    qDebug() << indicator << "::getCUSDOWN: duplicate name" << set[4];
    return 1;
  }

  bool ok;
  int period = set[5].toInt(&ok);
  if (! ok)
  {
    qDebug() << indicator << "::getCUSDOWN: invalid period" << set[5];
    return 1;
  }

  PlotLine *line = getDOWN(data, period);
  if (! line)
    return 1;

  tlines.insert(set[4], line);

  return 0;
}

int AROON::getCUSOSC (QStringList &set, QHash<QString, PlotLine *> &tlines, BarData *data)
{
  // INDICATOR,PLUGIN,AROON,OSC,<NAME>,<PERIOD>

  if (set.count() != 6)
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

  PlotLine *line = getOSC(data, period);
  if (! line)
    return 1;

  tlines.insert(set[4], line);

  return 0;
}

PlotLine * AROON::getUP (BarData *data, int period)
{
  if (data->count() < period)
    return 0;

  PlotLine *high = data->getInput(BarData::High);
  if (! high)
    return 0;

  int loop = period;
  PlotLine *line = new PlotLine;
  for (; loop < high->count(); loop++)
  {
    int count = 0;
    double max = -9999999;
    int pos = 0;
    for (; count <= period; count++)
    {
      if (high->getData(loop - count) > max)
      {
        pos = count;
	max = high->getData(loop - count);
      }
    }

    double aroon = ((period - pos) / (double) period) * 100;
    line->append(aroon);
  }

  delete high;

  return line;
}

PlotLine * AROON::getDOWN (BarData *data, int period)
{
  if (data->count() < period)
    return 0;

  PlotLine *low = data->getInput(BarData::Low);
  if (! low)
    return 0;

  int loop = period;
  PlotLine *line = new PlotLine;
  for (; loop < low->count(); loop++)
  {
    int count = 0;
    double min = 99999999;
    int pos = 0;
    for (; count <= period; count++)
    {
      if (low->getData(loop - count) < min)
      {
        pos = count;
	min = low->getData(loop - count);
      }
    }

    double aroon = ((period - pos) / (double) period) * 100;
    line->append(aroon);
  }

  delete low;

  return line;
}

PlotLine * AROON::getOSC (BarData *data, int period)
{
  if (data->count() < period)
    return 0;

  PlotLine *up = getUP(data, period);
  if (! up)
    return 0;

  PlotLine *down = getDOWN(data, period);
  if (! down)
  {
    delete up;
    return 0;
  }

  int upLoop = up->count() - 1;
  int downLoop = down->count() - 1;
  PlotLine *line = new PlotLine;

  while (upLoop > -1 && downLoop > -1)
  {
    line->prepend(up->getData(upLoop) - down->getData(downLoop));
    upLoop--;
    downLoop--;
  }

  delete up;
  delete down;

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
  dialog->addComboItem(Method, page, QObject::tr("Method"), guiMethodList, d);

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


