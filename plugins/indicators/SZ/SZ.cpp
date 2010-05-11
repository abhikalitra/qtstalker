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

/* The "SafeZone Stop" indicator is described in
   Dr. Alexander Elder's book _Come Into My Trading Room_, p.173 */

#include "SZ.h"
#include "BARSUtils.h"

#include <QtDebug>
#include <cmath>


SZ::SZ ()
{
  indicator = "SZ";

  settings.setData(Color, "red");
  settings.setData(Plot, "Line");
  settings.setData(Label, indicator);
  settings.setData(Period, 10);
  settings.setData(Method, "Long");
  settings.setData(NoDeclinePeriod, 2);
  settings.setData(Coefficient, 2);

  methodList << QObject::tr("Long");
  methodList << QObject::tr("Short");
}

int SZ::getIndicator (Indicator &ind, BarData *data)
{
  QString method;
  settings.getData(Method, method);

  int period = settings.getInt(Period);
  int ndperiod = settings.getInt(NoDeclinePeriod);
  double coeff = settings.getDouble(Coefficient);

  QColor up("green");
  QColor down("red");
  QColor neutral("blue");
  BARSUtils b;
  PlotLine *bars = b.getBARS(data, up, down, neutral);
  if (bars)
    ind.addLine(bars);

  PlotLine *line = getSZ(data, method, period, ndperiod, coeff);
  if (! line)
    return 1;

  QString s;
  settings.getData(Color, s);
  line->setColor(s);
  settings.getData(Plot, s);
  line->setPlugin(s);
  settings.getData(Label, s);
  line->setLabel(s);
  ind.addLine(line);

  return 0;
}

int SZ::getCUS (QStringList &set, QHash<QString, PlotLine *> &tlines, BarData *data)
{
  // INDICATOR,PLUGIN,SZ,<NAME>,<METHOD>,<PERIOD>,<NO_DECLINE_PERIOD>,<COEFFICIENT>

  if (set.count() != 8)
  {
    qDebug() << indicator << "::calculate: invalid parm count" << set.count();
    return 1;
  }

  PlotLine *tl = tlines.value(set[3]);
  if (tl)
  {
    qDebug() << indicator << "::calculate: invalid name" << set[3];
    return 1;
  }

  if (set[4] != "Long" && set[4] != "Short")
  {
    qDebug() << indicator << "::calculate: invalid method" << set[4];
    return 1;
  }

  bool ok;
  int period = set[5].toInt(&ok);
  if (! ok)
  {
    qDebug() << indicator << "::calculate: invalid period" << set[5];
    return 1;
  }
  if (period < 1)
    period = 1;

  int no_decline_period = set[6].toInt(&ok);
  if (! ok)
  {
    qDebug() << indicator << "::calculate: invalid no_decline_period" << set[6];
    return 1;
  }

  double coefficient = set[7].toDouble(&ok);
  if (! ok)
  {
    qDebug() << indicator << "::calculate: invalid coefficient" << set[7];
    return 1;
  }

  PlotLine *line = getSZ(data, set[4], period, no_decline_period, coefficient);
  if (! line)
    return 1;

  tlines.insert(set[3], line);

  return 0;
}

PlotLine * SZ::getSZ (BarData *data, QString &method, int period, int no_decline_period, double coefficient)
{
  int display_uptrend = 0;
  int display_dntrend = 0;
  int position = 1;
  if (method == "Long")
    position = 1;
  else
    position = 2;
  if (position & 1) // long
    display_uptrend = 1;
  if (position & 2) // short
    display_dntrend = 1;

  PlotLine *sz_uptrend = new PlotLine();
  PlotLine *sz_dntrend = new PlotLine();

  double uptrend_stop = 0;
  double dntrend_stop = 0;

  if (no_decline_period < 0)
    no_decline_period = 0;
  if (no_decline_period > 365)
    no_decline_period = 365;

  double old_uptrend_stops[no_decline_period];
  double old_dntrend_stops[no_decline_period];

  int loop;
  for (loop = 0; loop < no_decline_period; loop++)
  {
    old_uptrend_stops[loop] = 0;
    old_dntrend_stops[loop] = 0;
  }

  int start = period + 1;
  for (loop = start; loop < (int) data->count(); loop++)
  {
    // calculate downside/upside penetration for lookback period
    int lbloop;
    int lbstart = loop - period;
    if (lbstart < 2)
      lbstart = 2;
    double uptrend_noise_avg = 0;
    double uptrend_noise_cnt = 0;
    double dntrend_noise_avg = 0;
    double dntrend_noise_cnt = 0;
    for (lbloop = lbstart; lbloop < loop; lbloop++)
    {
      Bar *bar = data->getBar(lbloop);
      Bar *pbar = data->getBar(lbloop - 1);
      double lo_curr = bar->getLow();
      double lo_last = pbar->getLow();
      double hi_curr = bar->getHigh();
      double hi_last = pbar->getHigh();
      if (lo_last > lo_curr)
      {
	uptrend_noise_avg += lo_last - lo_curr;
	uptrend_noise_cnt++;
      }
      if (hi_last < hi_curr)
      {
	dntrend_noise_avg += hi_curr - hi_last;
	dntrend_noise_cnt++;
      }
    }
    // make *_avg into actual averages
    if (uptrend_noise_cnt > 0)
      uptrend_noise_avg /= uptrend_noise_cnt;
    if (dntrend_noise_cnt > 0)
      dntrend_noise_avg /= dntrend_noise_cnt;

    Bar *pbar = data->getBar(loop - 1);
    double lo_last = pbar->getLow();
    double hi_last = pbar->getHigh();
    uptrend_stop = lo_last - coefficient * uptrend_noise_avg;
    dntrend_stop = hi_last + coefficient * dntrend_noise_avg;

    double adjusted_uptrend_stop = uptrend_stop;
    double adjusted_dntrend_stop = dntrend_stop;

    int backloop;
    for (backloop = no_decline_period - 1; backloop >= 0; backloop--)
    {
      if (loop - backloop > start)
      {
	if (old_uptrend_stops[backloop] > adjusted_uptrend_stop)
	  adjusted_uptrend_stop = old_uptrend_stops[backloop];
	if (old_dntrend_stops[backloop] < adjusted_dntrend_stop)
	  adjusted_dntrend_stop = old_dntrend_stops[backloop];
      }
      if (backloop > 0)
      {
	old_uptrend_stops[backloop] = old_uptrend_stops[backloop-1];
	old_dntrend_stops[backloop] = old_dntrend_stops[backloop-1];
      }
    }

    old_uptrend_stops[0] = uptrend_stop;
    old_dntrend_stops[0] = dntrend_stop;

    sz_uptrend->append(adjusted_uptrend_stop);
    sz_dntrend->append(adjusted_dntrend_stop);
  }

  PlotLine *pl = 0;
  if (display_uptrend)
    pl = sz_uptrend;

  if (display_dntrend)
    pl = sz_dntrend;

  return pl;
}

int SZ::dialog (int)
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

  dialog->addIntItem(Period, page, QObject::tr("Period"), settings.getInt(Period), 1, 100000);

  settings.getData(Method, d);
  dialog->addComboItem(Method, page, QObject::tr("Method"), methodList, d);

  dialog->addIntItem(NoDeclinePeriod, page, QObject::tr("No Decline Period"), settings.getInt(NoDeclinePeriod), 1, 100000);

  dialog->addDoubleItem(Coefficient, page, QObject::tr("Coefficient"), settings.getDouble(Coefficient), 0, 100000);

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
  SZ *o = new SZ;
  return ((IndicatorPlugin *) o);
}
