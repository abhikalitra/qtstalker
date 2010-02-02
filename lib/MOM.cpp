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

#include "MOM.h"

#include <QtDebug>


MOM::MOM ()
{
  indicator = "MOM";
  showMAKey = QObject::tr("Show MA");
  showMOMKey = QObject::tr("Show MOM");

  QString d;
  d = "red";
  settings.setData(colorKey, d);

  d = "yellow";
  settings.setData(maColorKey, d);

  d = "Line";
  settings.setData(plotKey, d);
  settings.setData(maPlotKey, d);

  settings.setData(labelKey, indicator);

  d = "MOM_MA";
  settings.setData(maLabelKey, d);

  d = "Close";
  settings.setData(inputKey, d);

  settings.setData(periodKey, 10);
  settings.setData(maPeriodKey, 10);

  d = "SMA";
  settings.setData(maTypeKey, d);

  settings.setData(showMOMKey, 1);
  settings.setData(showMAKey, 1);
}

int MOM::getIndicator (Indicator &ind, BarData *data)
{
  QString s;
  settings.getData(inputKey, s);
  PlotLine *in = data->getInput(data->getInputType(s));
  if (! in)
  {
    qDebug() << indicator << "::calculate: input not found" << s;
    return 1;
  }

  int period = settings.getInt(periodKey);

  PlotLine *ma = 0;
  PlotLine *line = getMOM(in, period);
  if (! line)
  {
    delete in;
    return 1;
  }

  if (settings.getInt(showMAKey))
  {
    int maPeriod = settings.getInt(maPeriodKey);

    settings.getData(maTypeKey, s);
    int type = maList.indexOf(s);

    ma = getMA(line, maPeriod, type);
    if (! ma)
    {
      delete line;
      return 1;
    }

    settings.getData(maColorKey, s);
    ma->setColor(s);

    settings.getData(maPlotKey, s);
    ma->setType(s);

    settings.getData(maLabelKey, s);
    ma->setLabel(s);
  }

  settings.getData(colorKey, s);
  line->setColor(s);

  settings.getData(plotKey, s);
  line->setType(s);

  settings.getData(labelKey, s);
  line->setLabel(s);

  if (settings.getInt(showMOMKey))
    ind.addLine(line);
  else
    delete line;

  if (settings.getInt(showMAKey))
    ind.addLine(ma);

  delete in;

  return 0;
}

int MOM::getCUS (QStringList &set, QHash<QString, PlotLine *> &tlines, BarData *data)
{
  // INDICATOR,MOM,<NAME>,<INPUT>,<PERIOD>

  if (set.count() != 5)
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

  PlotLine *in = tlines.value(set[3]);
  if (! in)
  {
    in = data->getInput(data->getInputType(set[3]));
    if (! in)
    {
      qDebug() << indicator << "::calculate: input not found" << set[3];
      return 1;
    }

    tlines.insert(set[3], in);
  }

  bool ok;
  int period = set[4].toInt(&ok);
  if (! ok)
  {
    qDebug() << indicator << "::calculate: invalid period parm" << set[4];
    return 1;
  }

  PlotLine *line = getMOM(in, period);
  if (! line)
    return 1;

  tlines.insert(set[2], line);

  return 0;
}

PlotLine * MOM::getMOM (PlotLine *in, int period)
{
  int size = in->getSize();
  TA_Real input[size];
  TA_Real out[size];
  int loop;
  for (loop = 0; loop < size; loop++)
    input[loop] = (TA_Real) in->getData(loop);

  TA_Integer outBeg;
  TA_Integer outNb;
  TA_RetCode rc = TA_MOM(0, size - 1, &input[0], period, &outBeg, &outNb, &out[0]);
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

int MOM::dialog ()
{
  int page = 0;
  QString k, d;
  PrefDialog *dialog = new PrefDialog;
  dialog->setWindowTitle(QObject::tr("Edit Indicator"));

  k = QObject::tr("Settings");
  dialog->addPage(page, k);

  settings.getData(colorKey, d);
  dialog->addColorItem(page, colorKey, d);

  settings.getData(plotKey, d);
  dialog->addComboItem(page, plotKey, plotList, d);

  settings.getData(labelKey, d);
  dialog->addTextItem(page, labelKey, d);

  settings.getData(inputKey, d);
  dialog->addComboItem(page, inputKey, inputList, d);

  dialog->addIntItem(page, periodKey, settings.getInt(periodKey), 1, 100000);

  dialog->addCheckItem(page, showMOMKey, settings.getInt(showMOMKey));

  page++;
  k = QObject::tr("MA");
  dialog->addPage(page, k);

  settings.getData(maColorKey, d);
  dialog->addColorItem(page, maColorKey, d);

  settings.getData(maPlotKey, d);
  dialog->addComboItem(page, maPlotKey, plotList, d);

  settings.getData(maLabelKey, d);
  dialog->addTextItem(page, maLabelKey, d);

  dialog->addIntItem(page, maPeriodKey, settings.getInt(maPeriodKey), 2, 100000);

  settings.getData(maTypeKey, d);
  dialog->addComboItem(page, maTypeKey, maList, d);

  dialog->addCheckItem(page, showMAKey, settings.getInt(showMAKey));

  int rc = dialog->exec();
  if (rc == QDialog::Rejected)
  {
    delete dialog;
    return rc;
  }

  dialog->getItem(colorKey, d);
  settings.setData(colorKey, d);

  dialog->getItem(plotKey, d);
  settings.setData(plotKey, d);

  dialog->getItem(labelKey, d);
  settings.setData(labelKey, d);

  dialog->getItem(inputKey, d);
  settings.setData(inputKey, d);

  dialog->getItem(periodKey, d);
  settings.setData(periodKey, d);

  dialog->getItem(showMOMKey, d);
  settings.setData(showMOMKey, d);

  dialog->getItem(maColorKey, d);
  settings.setData(maColorKey, d);

  dialog->getItem(maPlotKey, d);
  settings.setData(maPlotKey, d);

  dialog->getItem(maLabelKey, d);
  settings.setData(maLabelKey, d);

  dialog->getItem(maPeriodKey, d);
  settings.setData(maPeriodKey, d);

  dialog->getItem(maTypeKey, d);
  settings.setData(maTypeKey, d);

  dialog->getItem(showMAKey, d);
  settings.setData(showMAKey, d);

  delete dialog;
  return rc;
}

