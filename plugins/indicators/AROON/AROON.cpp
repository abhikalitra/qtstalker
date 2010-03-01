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

#include "AROON.h"
#include "ta_libc.h"

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
  settings.setData(OSCPlot, "HistogramBar");
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
      PlotLine *line = getAROON(data, period, 2);
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
      PlotLine *up = getAROON(data, period, 0);
      if (! up)
        return 1;
      settings.getData(UpColor, s);
      up->setColor(s);
      settings.getData(UpPlot, s);
      up->setPlugin(s);
      settings.getData(UpLabel, s);
      up->setLabel(s);

      // get aroon down line
      PlotLine *down = getAROON(data, period, 1);
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
  // INDICATOR,AROON,<METHOD>,<NAME>,<PERIOD>

  if (set.count() != 5)
  {
    qDebug() << indicator << "::calculate: invalid settings count" << set.count();
    return 1;
  }

  int method = methodList.indexOf(set[2]);
  if (method == -1)
  {
    qDebug() << indicator << "::calculate: invalid method" << set[2];
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
    qDebug() << indicator << "::calculate: invalid period" << set[4];
    return 1;
  }

  PlotLine *line = getAROON(data, period, method);
  if (! line)
    return 1;

  tlines.insert(set[3], line);

  return 0;
}

PlotLine * AROON::getAROON (BarData *data, int period, int method)
{
  int size = data->count();
  TA_Real high[size];
  TA_Real low[size];
  TA_Real out[size];
  TA_Real out2[size];
  int loop;
  for (loop = 0; loop < size; loop++)
  {
    high[loop] = (TA_Real) data->getHigh(loop);
    low[loop] = (TA_Real) data->getLow(loop);
  }

  TA_Integer outBeg;
  TA_Integer outNb;
  TA_RetCode rc = TA_SUCCESS;
  switch (method)
  {
    case 0:
      rc = TA_AROON(0, size - 1, &high[0], &low[0], period, &outBeg, &outNb, &out[0], &out2[0]);
      break;
    case 1:
      // we switch out with out2 only here
      rc = TA_AROON(0, size - 1, &high[0], &low[0], period, &outBeg, &outNb, &out2[0], &out[0]);
      break;
    case 2:
      rc = TA_AROONOSC(0, size - 1, &high[0], &low[0], period, &outBeg, &outNb, &out[0]);
      break;
    default:
      break;
  }

  if (rc != TA_SUCCESS)
  {
    qDebug() << indicator << "::calculate: TA-Lib error" << rc;
    return 0;
  }

  PlotLine *line = new PlotLine;
  for (loop = 0; loop < outNb; loop++)
    line->append(out[loop]);

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


