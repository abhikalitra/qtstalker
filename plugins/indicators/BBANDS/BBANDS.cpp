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

#include "BBANDS.h"
#include "ta_libc.h"

#include <QtDebug>
#include <QColor>

BBANDS::BBANDS ()
{
  indicator = "BBANDS";

  settings.setData(UpColor, "red");
  settings.setData(MidColor, "red");
  settings.setData(DownColor, "red");
  settings.setData(UpPlot, "Line");
  settings.setData(MidPlot, "Line");
  settings.setData(DownPlot, "Line");
  settings.setData(UpLabel, "BBANDSU");
  settings.setData(MidLabel, "BBANDSM");
  settings.setData(DownLabel, "BBANDSL");
  settings.setData(UpDeviation, 2);
  settings.setData(DownDeviation, 2);
  settings.setData(Input, "Close");
  settings.setData(Period, 20);
  settings.setData(MAType, "SMA");
}

int BBANDS::getIndicator (Indicator &ind, BarData *data)
{
  QString s;
  settings.getData(Input, s);
  PlotLine *in = data->getInput(data->getInputType(s));
  if (! in)
  {
    qDebug() << indicator << "::calculate: input not found" << s;
    return 1;
  }

  int period = settings.getInt(Period);
  double udev = settings.getDouble(UpDeviation);
  double ldev = settings.getDouble(DownDeviation);

  QStringList maList;
  getMAList(maList);
  
  settings.getData(MAType, s);
  int ma = maList.indexOf(s);

  QList<PlotLine *> l;
  int rc = getBBANDS(in, period, udev, ldev, ma, l);
  if (rc)
  {
    delete in;
    return 1;
  }

  if (l.count() != 3)
  {
    delete in;
    qDeleteAll(l);
    return 1;
  }

  QColor up("green");
  QColor down("red");
  QColor neutral("blue");
  PlotLine *bars = getLocalBARS(data, up, down, neutral);
  if (! bars)
  {
    delete in;
    qDeleteAll(l);
    return 1;
  }
  
  ind.addLine(bars);

  // upper line
  PlotLine *line = l.at(0);
  settings.getData(UpColor, s);
  line->setColor(s);
  settings.getData(UpPlot, s);
  line->setPlugin(s);
  settings.getData(UpLabel, s);
  line->setLabel(s);
  ind.addLine(line);

  // middle line
  line = l.at(1);
  settings.getData(MidColor, s);
  line->setColor(s);
  settings.getData(MidPlot, s);
  line->setPlugin(s);
  settings.getData(MidLabel, s);
  line->setLabel(s);
  ind.addLine(line);

  // lower line
  line = l.at(2);
  settings.getData(DownColor, s);
  line->setColor(s);
  settings.getData(DownPlot, s);
  line->setPlugin(s);
  settings.getData(DownLabel, s);
  line->setLabel(s);
  ind.addLine(line);

  delete in;

  return 0;
}

int BBANDS::getCUS (QStringList &set, QHash<QString, PlotLine *> &tlines, BarData *data)
{
  // INDICATOR,PLUGIN,BBANDS,<INPUT>,<NAME_UPPER>,<NAME_MIDDLE>,<NAME_LOWER>,<PERIOD>,<UPPER_DEVIATION>,<LOWER_DEVIATION>,<MA_TYPE>

  if (set.count() != 11)
  {
    qDebug() << indicator << "::calculate: invalid settings count" << set.count();
    return 1;
  }

  PlotLine *tl = tlines.value(set[4]);
  if (tl)
  {
    qDebug() << indicator << "::calculate: duplicate name" << set[4];
    return 1;
  }

  tl = tlines.value(set[5]);
  if (tl)
  {
    qDebug() << indicator << "::calculate: duplicate name" << set[5];
    return 1;
  }

  tl = tlines.value(set[6]);
  if (tl)
  {
    qDebug() << indicator << "::calculate: duplicate name" << set[6];
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
  int period = set[7].toInt(&ok);
  if (! ok)
  {
    qDebug() << indicator << "::calculate: invalid period settings" << set[7];
    return 1;
  }

  double upDev = set[8].toDouble(&ok);
  if (! ok)
  {
    qDebug() << indicator << "::calculate: invalid up deviation settings" << set[8];
    return 1;
  }

  double lowDev = set[9].toDouble(&ok);
  if (! ok)
  {
    qDebug() << indicator << "::calculate: invalid lower deviation settings" << set[9];
    return 1;
  }

  QStringList maList;
  getMAList(maList);
  int ma = maList.indexOf(set[10]);
  if (ma == -1)
  {
    qDebug() << indicator << "::calculate: invalid ma settings" << set[10];
    return 1;
  }

  QList<PlotLine *> l;
  int rc = getBBANDS(in, period, upDev, lowDev, ma, l);
  if (rc)
  {
    delete in;
    return 1;
  }

  if (l.count() != 3)
  {
    delete in;
    qDeleteAll(l);
    return 1;
  }

  PlotLine *line = l.at(0);
  tlines.insert(set[4], line);
  line = l.at(1);
  tlines.insert(set[5], line);
  line = l.at(2);
  tlines.insert(set[6], line);

  return 0;
}

int BBANDS::getBBANDS (PlotLine *in, int period, double udev, double ldev, int ma, QList<PlotLine *> &_l)
{
  int size = in->count();
  TA_Real input[size];
  TA_Real upper[size];
  TA_Real middle[size];
  TA_Real lower[size];
  int loop;
  for (loop = 0; loop < size; loop++)
    input[loop] = (TA_Real) in->getData(loop);

  TA_Integer outBeg;
  TA_Integer outNb;
  TA_RetCode rc = TA_BBANDS(0, size - 1, &input[0], period, udev, ldev, (TA_MAType) ma, &outBeg, &outNb, &upper[0], &middle[0], &lower[0]);
  if (rc != TA_SUCCESS)
  {
    qDebug() << indicator << "::calculate: TA-Lib error" << rc;
    return 1;
  }

  PlotLine *u = new PlotLine;
  PlotLine *m = new PlotLine;
  PlotLine *l = new PlotLine;
  for (loop = 0; loop < outNb; loop++)
  {
    u->append(upper[loop]);
    m->append(middle[loop]);
    l->append(lower[loop]);
  }

  _l.append(u);
  _l.append(m);
  _l.append(l);

  return 0;
}

int BBANDS::dialog (int)
{
  int page = 0;
  QString k, d;
  PrefDialog *dialog = new PrefDialog;
  dialog->setWindowTitle(QObject::tr("Edit Indicator"));

  k = QObject::tr("Settings");
  dialog->addPage(page, k);

  settings.getData(Input, d);
  dialog->addComboItem(Input, page, QObject::tr("Input"), inputList, d);

  dialog->addIntItem(Period, page, QObject::tr("Period"), settings.getInt(Period), 2, 100000);

  dialog->addDoubleItem(UpDeviation, page, QObject::tr("Upper Deviation"), settings.getDouble(UpDeviation), -100000, 100000);

  dialog->addDoubleItem(DownDeviation, page, QObject::tr("Lower Deviation"), settings.getDouble(DownDeviation), -100000, 100000);

  QStringList maList;
  getMAList(maList);
  
  settings.getData(MAType, d);
  dialog->addComboItem(MAType, page, QObject::tr("MA Type"), maList, d);

  page++;
  k = QObject::tr("Upper");
  dialog->addPage(page, k);

  settings.getData(UpColor, d);
  dialog->addColorItem(UpColor, page, QObject::tr("Color"), d);

  settings.getData(UpPlot, d);
  dialog->addComboItem(UpPlot, page, QObject::tr("Plot"), plotList, d);

  settings.getData(UpLabel, d);
  dialog->addTextItem(UpLabel, page, QObject::tr("Label"), d);

  page++;
  k = QObject::tr("Mid");
  dialog->addPage(page, k);

  settings.getData(MidColor, d);
  dialog->addColorItem(MidColor, page, QObject::tr("Color"), d);

  settings.getData(MidPlot, d);
  dialog->addComboItem(MidPlot, page, QObject::tr("Plot"), plotList, d);

  settings.getData(MidLabel, d);
  dialog->addTextItem(MidLabel, page, QObject::tr("Label"), d);

  page++;
  k = QObject::tr("Lower");
  dialog->addPage(page, k);

  settings.getData(DownColor, d);
  dialog->addColorItem(DownColor, page, QObject::tr("Color"), d);

  settings.getData(DownPlot, d);
  dialog->addComboItem(DownPlot, page, QObject::tr("Plot"), plotList, d);

  settings.getData(DownLabel, d);
  dialog->addTextItem(DownLabel, page, QObject::tr("Label"), d);

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
  BBANDS *o = new BBANDS;
  return ((IndicatorPlugin *) o);
}


