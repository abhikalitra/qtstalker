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

#include "AD.h"

#include <QtDebug>


AD::AD ()
{
  indicator = "AD";
  methodKey = QObject::tr("Method");
  fpKey = QObject::tr("Fast Period");
  spKey = QObject::tr("Slow Period");
  oscColorKey = QObject::tr("OSC Color");
  oscPlotKey = QObject::tr("OSC Plot");
  oscLabelKey = QObject::tr("OSC Label");

  methodList << "AD";
  methodList << "ADOSC";

  QString d;
  d = "red";
  settings.setData(colorKey, d);
  settings.setData(oscColorKey, d);

  d = "Line";
  settings.setData(plotKey, d);

  d = "Histogram Bar";
  settings.setData(oscPlotKey, d);

  settings.setData(labelKey, indicator);

  d = "ADOSC";
  settings.setData(oscLabelKey, d);

  settings.setData(fpKey, 3);
  settings.setData(spKey, 10);

  d = "AD";
  settings.setData(methodKey, d);
}

int AD::getIndicator (Indicator &ind, BarData *data)
{
  QString s;
  settings.getData(methodKey, s);
  int method = methodList.indexOf(s);

  PlotLine *line = 0;
  switch (method)
  {
    case 1:
    {
      int fp = settings.getInt(fpKey);
      int sp = settings.getInt(spKey);
      line = getADOSC(data, fp, sp);
      settings.getData(oscColorKey, s);
      line->setColor(s);
      settings.getData(oscPlotKey, s);
      line->setType(s);
      settings.getData(oscLabelKey, s);
      line->setLabel(s);
      break;
    }
    default:
      line = getAD(data);
      settings.getData(colorKey, s);
      line->setColor(s);
      settings.getData(plotKey, s);
      line->setType(s);
      settings.getData(labelKey, s);
      line->setLabel(s);
      break;
  }

  if (! line)
    return 1;

  ind.addLine(line);

  return 0;
}

int AD::getCUS (QStringList &set, QHash<QString, PlotLine *> &tlines, BarData *data)
{
  QString s;
  int method = methodList.indexOf(set[2]);

  int rc = 1;
  switch (method)
  {
    case 0:
      rc = getCUS_AD(set, tlines, data);
      break;
    case 1:
      rc = getCUS_ADOSC(set, tlines, data);
      break;
  }

  return rc;
}

int AD::getCUS_AD (QStringList &set, QHash<QString, PlotLine *> &tlines, BarData *data)
{
  // INDICATOR,AD,AD,<NAME>

  if (set.count() != 4)
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

  PlotLine *line = getAD(data);
  if (! line)
    return 1;

  tlines.insert(set[3], line);

  return 0;
}

int AD::getCUS_ADOSC (QStringList &set, QHash<QString, PlotLine *> &tlines, BarData *data)
{
  // INDICATOR,AD,ADOSC,<NAME>,<FAST_PERIOD>,<SLOW_PERIOD>

  if (set.count() != 6)
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
  int fast = set[4].toInt(&ok);
  if (! ok)
  {
    qDebug() << indicator << "::calculate: invalid fast period" << set[4];
    return 1;
  }

  int slow = set[5].toInt(&ok);
  if (! ok)
  {
    qDebug() << indicator << "::calculate: invalid slow period" << set[5];
    return 1;
  }

  PlotLine *line = getADOSC(data, fast, slow);
  if (! line)
    return 1;

  tlines.insert(set[3], line);

  return 0;
}

PlotLine * AD::getAD (BarData *data)
{
  int size = data->count();
  TA_Real high[size];
  TA_Real low[size];
  TA_Real close[size];
  TA_Real volume[size];
  TA_Real out[size];
  int loop;
  for (loop = 0; loop < size; loop++)
  {
    high[loop] = (TA_Real) data->getHigh(loop);
    low[loop] = (TA_Real) data->getLow(loop);
    close[loop] = (TA_Real) data->getClose(loop);
    volume[loop] = (TA_Real) data->getVolume(loop);
  }

  TA_Integer outBeg;
  TA_Integer outNb;
  TA_RetCode rc = TA_AD(0, size - 1, &high[0], &low[0], &close[0], &volume[0], &outBeg, &outNb, &out[0]);
  if (rc != TA_SUCCESS)
  {
    qDebug() << indicator << "::calculate: : TA-Lib error" << rc;
    return 0;
  }

  PlotLine *line = new PlotLine;
  for (loop = 0; loop < outNb; loop++)
    line->append(out[loop]);

  return line;
}

PlotLine * AD::getADOSC (BarData *data, int fast, int slow)
{
  int size = data->count();
  TA_Real high[size];
  TA_Real low[size];
  TA_Real close[size];
  TA_Real volume[size];
  TA_Real out[size];
  int loop;
  for (loop = 0; loop < size; loop++)
  {
    high[loop] = (TA_Real) data->getHigh(loop);
    low[loop] = (TA_Real) data->getLow(loop);
    close[loop] = (TA_Real) data->getClose(loop);
    volume[loop] = (TA_Real) data->getVolume(loop);
  }

  TA_Integer outBeg;
  TA_Integer outNb;
  TA_RetCode rc = TA_ADOSC(0, size - 1, &high[0], &low[0], &close[0], &volume[0], fast, slow, &outBeg, &outNb, &out[0]);
  if (rc != TA_SUCCESS)
  {
    qDebug() << indicator << "::calculate: : TA-Lib error" << rc;
    return 0;
  }

  PlotLine *line = new PlotLine;
  for (loop = 0; loop < outNb; loop++)
    line->append(out[loop]);

  return line;
}

int AD::dialog ()
{
  int page = 0;
  QString k, d;
  PrefDialog *dialog = new PrefDialog;
  dialog->setWindowTitle(QObject::tr("Edit Indicator"));

  k = QObject::tr("Settings");
  dialog->addPage(page, k);

  settings.getData(colorKey, d);
  QColor c(d);
  dialog->addColorItem(page, colorKey, c);

  settings.getData(plotKey, d);
  dialog->addComboItem(page, plotKey, plotList, d);

  settings.getData(labelKey, d);
  dialog->addTextItem(page, labelKey, d);

  settings.getData(methodKey, d);
  dialog->addComboItem(page, methodKey, methodList, d);

  page++;
  k = QObject::tr("OSC");
  dialog->addPage(page, k);

  settings.getData(oscColorKey, d);
  dialog->addColorItem(page, oscColorKey, d);

  settings.getData(oscPlotKey, d);
  dialog->addComboItem(page, oscPlotKey, plotList, d);

  settings.getData(oscLabelKey, d);
  dialog->addTextItem(page, oscLabelKey, d);

  dialog->addIntItem(page, fpKey, settings.getInt(fpKey), 1, 100000);

  dialog->addIntItem(page, spKey, settings.getInt(spKey), 1, 100000);

  int rc = dialog->exec();
  if (rc == QDialog::Rejected)
  {
    delete dialog;
    return rc;
  }

  QStringList keys;
  settings.getKeyList(keys);
  int loop;
  for (loop = 0; loop < keys.count(); loop++)
  {
    QString d;
    dialog->getItem(keys[loop], d);
    if (! d.isEmpty())
      settings.setData(keys[loop], d);
  }

  delete dialog;
  return rc;
}


