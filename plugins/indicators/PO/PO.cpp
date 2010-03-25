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

#include "PO.h"
#include "ta_libc.h"
#include "MATH1.h"

#include <QtDebug>


PO::PO ()
{
  indicator = "PO";

  methodList << "APO";
  methodList << "PPO";

  settings.setData(Color, "red");
  settings.setData(Plot, "HistogramBar");
  settings.setData(Label, indicator);
  settings.setData(Input, "Close");
  settings.setData(FastPeriod, 12);
  settings.setData(SlowPeriod, 26);
  settings.setData(MAType, "SMA");
  settings.setData(Method, "APO");
}

int PO::getIndicator (Indicator &ind, BarData *data)
{
  QString s;
  settings.getData(Input, s);
  PlotLine *in = data->getInput(data->getInputType(s));
  if (! in)
  {
    qDebug() << indicator << "::calculate: input not found" << s;
    return 1;
  }

  int fast = settings.getInt(FastPeriod);
  int slow = settings.getInt(SlowPeriod);

  MATH1 m;
  QStringList maList;
  m.getMAList(maList);
  
  settings.getData(MAType, s);
  int ma = maList.indexOf(s);

  settings.getData(Method, s);
  int method = methodList.indexOf(s);

  PlotLine *line = getPO(in, fast, slow, ma, method);
  if (! line)
  {
    delete in;
    return 1;
  }

  settings.getData(Color, s);
  line->setColor(s);
  settings.getData(Plot, s);
  line->setPlugin(s);
  settings.getData(Label, s);
  line->setLabel(s);
  ind.addLine(line);

  delete in;

  return 0;
}

int PO::getCUS (QStringList &set, QHash<QString, PlotLine *> &tlines, BarData *data)
{
  // INDICATOR,PLUGIN,PO,METHOD,<NAME>,<INPUT>,<FAST_PERIOD>,<SLOW_PERIOD>,<MA_TYPE>

  if (set.count() != 9)
  {
    qDebug() << indicator << "::calculate: invalid parm count" << set.count();
    return 1;
  }

  int method = methodList.indexOf(set[3]);
  if (method == -1)
  {
    qDebug() << indicator << "::calculate: invalid method" << set[3];
    return 1;
  }

  PlotLine *tl = tlines.value(set[4]);
  if (tl)
  {
    qDebug() << indicator << "::calculate: duplicate name" << set[4];
    return 1;
  }

  PlotLine *in = tlines.value(set[5]);
  if (! in)
  {
    in = data->getInput(data->getInputType(set[5]));
    if (! in)
    {
      qDebug() << indicator << "::calculate: input not found" << set[5];
      return 1;
    }

    tlines.insert(set[5], in);
  }

  bool ok;
  int fast = set[6].toInt(&ok);
  if (! ok)
  {
    qDebug() << indicator << "::calculate: invalid fast period" << set[6];
    return 1;
  }

  int slow = set[7].toInt(&ok);
  if (! ok)
  {
    qDebug() << indicator << "::calculate: invalid slow period" << set[7];
    return 1;
  }

  MATH1 m;
  QStringList maList;
  m.getMAList(maList);
  int ma = maList.indexOf(set[8]);
  if (ma == -1)
  {
    qDebug() << indicator << "::calculate: invalid ma" << set[8];
    return 1;
  }

  PlotLine *line = getPO(in, fast, slow, ma, method);
  if (! line)
    return 1;

  tlines.insert(set[4], line);

  return 0;
}

PlotLine * PO::getPO (PlotLine *in, int fast, int slow, int ma, int method)
{
  TA_Integer outBeg;
  TA_Integer outNb;
  TA_Real input[in->count()];
  TA_Real out[in->count()];
  int loop;
  for (loop = 0; loop < in->count(); loop++)
    input[loop] = (TA_Real) in->getData(loop);

  TA_RetCode rc = TA_SUCCESS;
  switch (method)
  {
    case 0:
      rc = TA_APO(0, in->count() - 1, &input[0], fast, slow, (TA_MAType) ma, &outBeg, &outNb, &out[0]);
      break;
    case 1:
      rc = TA_PPO(0, in->count() - 1, &input[0], fast, slow, (TA_MAType) ma, &outBeg, &outNb, &out[0]);
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

int PO::dialog (int)
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

  settings.getData(Input, d);
  dialog->addComboItem(Input, page, QObject::tr("Input"), inputList, d);

  dialog->addIntItem(FastPeriod, page, QObject::tr("Fast Period"), settings.getInt(FastPeriod), 2, 100000);

  dialog->addIntItem(SlowPeriod, page, QObject::tr("Slow Period"), settings.getInt(SlowPeriod), 2, 100000);

  MATH1 m;
  QStringList maList;
  m.getMAList(maList);

  settings.getData(MAType, d);
  dialog->addComboItem(MAType, page, QObject::tr("MA Type"), maList, d);

  settings.getData(Method, d);
  dialog->addComboItem(Method, page, QObject::tr("Method"), methodList, d);

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
  PO *o = new PO;
  return ((IndicatorPlugin *) o);
}


