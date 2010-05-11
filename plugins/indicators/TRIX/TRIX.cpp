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

#include "TRIX.h"
#include "MAUtils.h"
#include "PlotFactory.h"

#include <QtDebug>

TRIX::TRIX ()
{
  indicator = "TRIX";

  settings.setData(Color, "red");
  settings.setData(Plot, "HistogramBar");
  settings.setData(Label, indicator);
  settings.setData(Period, 30);
  settings.setData(Input, "Close");
}

int TRIX::getIndicator (Indicator &ind, BarData *data)
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

  PlotLine *line = getTRIX(in, period);
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

int TRIX::getCUS (QStringList &set, QHash<QString, PlotLine *> &tlines, BarData *data)
{
  // INDICATOR,PLUGIN,TRIX,<NAME>,<INPUT>,<PERIOD>

  if (set.count() != 6)
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

  PlotLine *in = tlines.value(set[4]);
  if (! in)
  {
    in = data->getInput(data->getInputType(set[4]));
    if (! in)
    {
      qDebug() << indicator << "::calculate: input not found" << set[4];
      return 1;
    }

    tlines.insert(set[4], in);
  }

  bool ok;
  int period = set[5].toInt(&ok);
  if (! ok)
  {
    qDebug() << indicator << "::calculate: invalid period parm" << set[5];
    return 1;
  }

  PlotLine *line = getTRIX(in, period);
  if (! line)
    return 1;

  tlines.insert(set[3], line);

  return 0;
}

PlotLine * TRIX::getTRIX (PlotLine *in, int period)
{
  if (in->count() < period)
    return 0;

  MAUtils mau;
  PlotLine *ema1 = mau.getMA(in, period, MAUtils::EMA);
  if (! ema1)
    return 0;
  
  PlotLine *ema2 = mau.getMA(ema1, period, MAUtils::EMA);
  if (! ema2)
  {
    delete ema1;
    return 0;
  }
  
  PlotLine *ema3 = mau.getMA(ema2, period, MAUtils::EMA);
  if (! ema3)
  {
    delete ema1;
    delete ema2;
    return 0;
  }

  PlotLine *line = new PlotLine;
  int loop = 1;
  int size = ema3->count();
  for (; loop < size; loop++)
  {
    double roc = ((ema3->getData(loop) - ema3->getData(loop - 1)) / ema3->getData(loop)) * 100;
    line->append(roc);
  }

  delete ema1;
  delete ema2;
  delete ema3;

  return line;
}

/*
PlotLine * TRIX::getTRIX (PlotLine *in, int period)
{
  int size = in->count();
  TA_Real input[size];
  TA_Real out[size];
  int loop;
  for (loop = 0; loop < size; loop++)
    input[loop] = (TA_Real) in->getData(loop);

  TA_Integer outBeg;
  TA_Integer outNb;
  TA_RetCode rc = TA_TRIX(0, size - 1, &input[0], period, &outBeg, &outNb, &out[0]);
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
*/

int TRIX::dialog (int)
{
  int page = 0;
  QString k, d;
  PrefDialog *dialog = new PrefDialog;
  dialog->setWindowTitle(QObject::tr("Edit Indicator"));

  k = QObject::tr("Settings");
  dialog->addPage(page, k);

  settings.getData(Color, d);
  dialog->addColorItem(Color, page, QObject::tr("Color"), d);

  PlotFactory fac;
  QStringList plotList;
  fac.list(plotList, TRUE);

  settings.getData(Plot, d);
  dialog->addComboItem(Plot, page, QObject::tr("Plot"), plotList, d);

  settings.getData(Label, d);
  dialog->addTextItem(Label, page, QObject::tr("Label"), d);

  dialog->addIntItem(Period, page, QObject::tr("Period"), settings.getInt(Period), 1, 100000);

  BarData bd;
  QStringList inputList;
  bd.getInputFields(inputList);

  settings.getData(Input, d);
  dialog->addComboItem(Input, page, QObject::tr("Input"), inputList, d);

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
  TRIX *o = new TRIX;
  return ((IndicatorPlugin *) o);
}

