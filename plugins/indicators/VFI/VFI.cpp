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

#include "VFI.h"
#include "PlotFactory.h"

#include <QtDebug>
#include <cmath>

VFI::VFI ()
{
  _indicator = "VFI";

  _settings.setData(Color, "red");
  _settings.setData(Plot, "Line");
  _settings.setData(Label, _indicator);
  _settings.setData(Period, 100);
}

int VFI::getIndicator (Indicator &ind, BarData *data)
{
  int period = _settings.getInt(Period);

  QString s;
  _settings.getData(Color, s);
  QColor color(s);

  PlotFactory fac;
  _settings.getData(Plot, s);
  int lineType = fac.typeFromString(s);

  PlotLine *line = getVFI(data, period, lineType, color);
  if (! line)
    return 1;

  _settings.getData(Label, s);
  line->setLabel(s);
  
  s = "0";
  ind.setLine(s, line);
  ind.addPlotOrder(s);

  return 0;
}

int VFI::getCUS (QStringList &set, Indicator &ind, BarData *data)
{
  // INDICATOR,PLUGIN,VFI,<NAME>,<PERIOD>,<PLOT TYPE>,<COLOR>
  //     0       1     2    3        4         5         6

  if (set.count() != 7)
  {
    qDebug() << _indicator << "::getCUS: invalid parm count" << set.count();
    return 1;
  }

  PlotLine *tl = ind.line(set[3]);
  if (tl)
  {
    qDebug() << _indicator << "::getCUS: duplicate name" << set[3];
    return 1;
  }

  bool ok;
  int period = set[4].toInt(&ok);
  if (! ok)
  {
    qDebug() << _indicator << "::getCUS: invalid period" << set[4];
    return 1;
  }

  PlotFactory fac;
  int lineType = fac.typeFromString(set[5]);
  if (lineType == -1)
  {
    qDebug() << _indicator << "::getCUS: invalid plot type" << set[5];
    return 1;
  }

  QColor color(set[6]);
  if (! color.isValid())
  {
    qDebug() << _indicator << "::getCUS: invalid color" << set[6];
    return 1;
  }

  PlotLine *line = getVFI(data, period, lineType, color);
  if (! line)
    return 1;

  line->setLabel(set[3]);

  ind.setLine(set[3], line);

  return 0;
}

PlotLine * VFI::getVFI (BarData *data, int period, int lineType, QColor &color)
{
  PlotFactory fac;
  PlotLine *vfi = fac.plot(lineType);
  if (! vfi)
    return 0;

  int loop = period;
  for (; loop < (int) data->count(); loop++)
  {
    double inter = 0.0;
    double sma_vol = 0.0;
    int i;
    Bar *bar = data->getBar(loop - period);
    double close = bar->getClose();
    double high = bar->getHigh();
    double low = bar->getLow();
    double typical = (high+low+close) / 3.0;
    for (i = loop - period + 1; i <= loop; i++)
    {
      bar = data->getBar(i);
      double ytypical = typical;
      close = bar->getClose();
      high = bar->getHigh();
      low = bar->getLow();
      typical = (high + low + close) / 3.0;
      double delta = (log(typical) - log(ytypical));
      inter += delta * delta;
      sma_vol += bar->getVolume();
    }
    inter = 0.2 * sqrt(inter / (double) period) * typical;
    sma_vol /= (double) period;

    bar = data->getBar(loop - period);
    close = bar->getClose();
    high = bar->getHigh();
    low = bar->getLow();
    typical = (high + low + close) / 3.0;
    double t = 0;
    for (i = loop - period + 1; i <= loop; i++)
    {
      bar = data->getBar(i);
      double ytypical = typical;
      double volume = bar->getVolume();
      close = bar->getClose();
      high = bar->getHigh();
      low = bar->getLow();
      typical = (high + low + close) / 3.0;

      if (typical > ytypical + inter)
        t = t + log (1.0 + volume / sma_vol);
      else
      {
        if (typical < ytypical - inter)
          t = t - log (1.0 + volume / sma_vol);
      }
    }

    vfi->setData(loop, new PlotLineBar(color, t));
  }

  return vfi;
}

int VFI::dialog (int)
{
  int page = 0;
  QString k, d;
  PrefDialog *dialog = new PrefDialog;
  dialog->setWindowTitle(QObject::tr("Edit Indicator"));

  k = QObject::tr("Settings");
  dialog->addPage(page, k);

  _settings.getData(Color, d);
  dialog->addColorItem(Color, page, QObject::tr("Color"), d);

  PlotFactory fac;
  QStringList plotList;
  fac.list(plotList, TRUE);

  _settings.getData(Plot, d);
  dialog->addComboItem(Plot, page, QObject::tr("Plot"), plotList, d);

  _settings.getData(Label, d);
  dialog->addTextItem(Label, page, QObject::tr("Label"), d);

  dialog->addIntItem(Period, page, QObject::tr("Period"), _settings.getInt(Period), 1, 100000);

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
  VFI *o = new VFI;
  return ((IndicatorPlugin *) o);
}

