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

#include "STOCHRSI.h"
#include "ta_libc.h"
#include "MATH1.h"

#include <QtDebug>


STOCHRSI::STOCHRSI ()
{
  indicator = "STOCHRSI";

  settings.setData(FastkColor, "red");
  settings.setData(FastdColor, "yellow");
  settings.setData(Ref1Color, "white");
  settings.setData(Ref2Color, "white");
  settings.setData(FastkPlot, "Line");
  settings.setData(FastdPlot, "Dash");
  settings.setData(FastkLabel, "STORSIK");
  settings.setData(FastdLabel, "STORSID");
  settings.setData(FastkPeriod, 5);
  settings.setData(FastdPeriod, 3);
  settings.setData(FastdMA, "SMA");
  settings.setData(Ref1, 25);
  settings.setData(Ref2, 75);
  settings.setData(Input, "SMA");
  settings.setData(Period, 14);
}

int STOCHRSI::getIndicator (Indicator &ind, BarData *data)
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
  int fastk = settings.getInt(FastkPeriod);
  int fastd = settings.getInt(FastdPeriod);

  MATH1 m;
  QStringList maList;
  m.getMAList(maList);
  
  settings.getData(FastdMA, s);
  int ma = maList.indexOf(s);

  QList<PlotLine *> l;
  int rc = getSTOCHRSI(in, period, fastk, fastd, ma, l);
  if (rc || l.count() != 2)
  {
    qDeleteAll(l);
    delete in;
    return 1;
  }

  PlotLine *ref1 = new PlotLine;
  s = "Horizontal";
  ref1->setPlugin(s);
  settings.getData(Ref1Color, s);
  ref1->setColor(s);
  ref1->append(settings.getInt(Ref1));
  ind.addLine(ref1);

  PlotLine *ref2 = new PlotLine;
  s = "Horizontal";
  ref2->setPlugin(s);
  settings.getData(Ref2Color, s);
  ref2->setColor(s);
  ref2->append(settings.getInt(Ref2));
  ind.addLine(ref2);

  // fastk line
  PlotLine *line = l.at(0);
  settings.getData(FastkColor, s);
  line->setColor(s);
  settings.getData(FastkPlot, s);
  line->setPlugin(s);
  settings.getData(FastkLabel, s);
  line->setLabel(s);
  ind.addLine(line);

  // fastd line
  line = l.at(1);
  settings.getData(FastdColor, s);
  line->setColor(s);
  settings.getData(FastdPlot, s);
  line->setPlugin(s);
  settings.getData(FastdLabel, s);
  line->setLabel(s);
  ind.addLine(line);

  delete in;

  return 0;
}

int STOCHRSI::getCUS (QStringList &set, QHash<QString, PlotLine *> &tlines, BarData *data)
{
  // INDICATOR,STOCHRSI,<INPUT>,<NAME_FASTK>,<NAME_FASTD>,<PERIOD>,<FASTK_PERIOD>,<FASTD_PERIOD>,
  // <FASTD_MA_TYPE>

  if (set.count() != 9)
  {
    qDebug() << indicator << "::calculate: invalid settings count" << set.count();
    return 1;
  }

  PlotLine *in = tlines.value(set[2]);
  if (! in)
  {
    in = data->getInput(data->getInputType(set[2]));
    if (! in)
    {
      qDebug() << indicator << "::calculate: input not found" << set[2];
      return 1;
    }

    tlines.insert(set[2], in);
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
  int period = set[5].toInt(&ok);
  if (! ok)
  {
    qDebug() << indicator << "::calculate: invalid period" << set[5];
    return 1;
  }

  int fkp = set[6].toInt(&ok);
  if (! ok)
  {
    qDebug() << indicator << "::calculate: invalid fastkk period" << set[6];
    return 1;
  }

  int fdp = set[7].toInt(&ok);
  if (! ok)
  {
    qDebug() << indicator << "::calculate: invalid fastd period" << set[7];
    return 1;
  }

  MATH1 m;
  QStringList maList;
  m.getMAList(maList);
  
  int ma = maList.indexOf(set[8]);
  if (ma == -1)
  {
    qDebug() << indicator << "::calculate: invalid ma settings" << set[8];
    return 1;
  }

  QList<PlotLine *> l;
  int rc = getSTOCHRSI(in, period, fkp, fdp, ma, l);
  if (rc || l.count() != 2)
  {
    qDeleteAll(l);
    return 1;
  }

  tlines.insert(set[3], l.at(0));
  tlines.insert(set[4], l.at(1));

  return 0;
}

int STOCHRSI::getSTOCHRSI (PlotLine *in, int period, int fkp, int fdp, int ma, QList<PlotLine *> &l)
{
  int size = in->count();
  TA_Real input[size];
  TA_Real out[size];
  TA_Real out2[size];
  int loop;
  for (loop = 0; loop < size; loop++)
    input[loop] = (TA_Real) in->getData(loop);

  TA_Integer outBeg;
  TA_Integer outNb;
  TA_RetCode rc = TA_STOCHRSI(0, in->count() - 1, &input[0], period, fkp, fdp, (TA_MAType) ma, &outBeg, &outNb, &out[0], &out2[0]);
  if (rc != TA_SUCCESS)
  {
    qDebug() << indicator << "::calculate: TA-Lib error" << rc;
    return 1;
  }

  PlotLine *line = new PlotLine;
  PlotLine *line2 = new PlotLine;
  for (loop = 0; loop < outNb; loop++)
  {
    line->append(out[loop]);
    line2->append(out2[loop]);
  }

  l.append(line);
  l.append(line2);

  return 0;
}

int STOCHRSI::dialog (int)
{
  int page = 0;
  QString k, d;
  PrefDialog *dialog = new PrefDialog;
  dialog->setWindowTitle(QObject::tr("Edit Indicator"));

  k = QObject::tr("Settings");
  dialog->addPage(page, k);

  dialog->addIntItem(Period, page, QObject::tr("Period"), settings.getInt(Period), 2, 100000);

  settings.getData(Input, d);
  dialog->addComboItem(Input, page, QObject::tr("Input"), inputList, d);

  page++;
  k = QObject::tr("FastK");
  dialog->addPage(page, k);

  settings.getData(FastkColor, d);
  dialog->addColorItem(FastkColor, page, QObject::tr("Color"), d);

  settings.getData(FastkPlot, d);
  dialog->addComboItem(FastkPlot, page, QObject::tr("Plot"), plotList, d);

  settings.getData(FastkLabel, d);
  dialog->addTextItem(FastkLabel, page, QObject::tr("Label"), d);

  dialog->addIntItem(FastkPeriod, page, QObject::tr("Period"), settings.getInt(FastkPeriod), 1, 100000);

  page++;
  k = QObject::tr("FastD");
  dialog->addPage(page, k);

  settings.getData(FastdColor, d);
  dialog->addColorItem(FastdColor, page, QObject::tr("Color"), d);

  settings.getData(FastdPlot, d);
  dialog->addComboItem(FastdPlot, page, QObject::tr("Plot"), plotList, d);

  settings.getData(FastdLabel, d);
  dialog->addTextItem(FastdLabel, page, QObject::tr("Label"), d);

  dialog->addIntItem(FastdPeriod, page, QObject::tr("Period"), settings.getInt(FastdPeriod), 1, 100000);

  MATH1 m;
  QStringList maList;
  m.getMAList(maList);
  
  settings.getData(FastdMA, d);
  dialog->addComboItem(FastdMA, page, QObject::tr("MA Type"), maList, d);

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
  STOCHRSI *o = new STOCHRSI;
  return ((IndicatorPlugin *) o);
}


