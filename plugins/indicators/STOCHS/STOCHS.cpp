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

#include "STOCHS.h"
#include "MAFactory.h"
#include "STOCHUtils.h"
#include "PlotFactory.h"

#include <QtDebug>

STOCHS::STOCHS ()
{
  indicator = "STOCHS";

  settings.setData(SlowKColor, "red");
  settings.setData(SlowDColor, "yellow");
  settings.setData(Ref1Color, "white");
  settings.setData(Ref2Color, "white");
  settings.setData(SlowKPlot, "Line");
  settings.setData(SlowDPlot, "Dash");
  settings.setData(SlowKLabel, "SLOWK");
  settings.setData(SlowDLabel, "SLOWD");
  settings.setData(FastKPeriod, 5);
  settings.setData(SlowKPeriod, 3);
  settings.setData(SlowDPeriod, 3);
  settings.setData(SlowKMA, "SMA");
  settings.setData(SlowDMA, "SMA");
  settings.setData(Ref1, 20);
  settings.setData(Ref2, 80);
}

int STOCHS::getIndicator (Indicator &ind, BarData *data)
{
  // create first ref line
  QString s = "Horizontal";
  PlotFactory fac;
  PlotLine *line = fac.plot(s);
  if (! line)
    return 1;

  settings.getData(Ref1Color, s);
  QColor color(s);

  line->setData(0, new PlotLineBar(color, (double) settings.getInt(Ref1)));
  ind.addLine(line);

  // create second ref line
  s = "Horizontal";
  line = fac.plot(s);
  if (! line)
    return 1;

  settings.getData(Ref2Color, s);
  color.setNamedColor(s);

  line->setData(0, new PlotLineBar(color, (double) settings.getInt(Ref2)));
  ind.addLine(line);

  // create the fastk line
  int period = settings.getInt(FastKPeriod);

  settings.getData(SlowKColor, s);
  color.setNamedColor(s);

  settings.getData(SlowKPlot, s);
  int lineType = fac.typeFromString(s);

  STOCHUtils fastk;
  PlotLine *fk = fastk.fastK(data, period, lineType, color);
  if (! fk)
    return 1;

  // create the slowk line
  period = settings.getInt(SlowKPeriod);

  MAFactory mau;
  settings.getData(SlowKMA, s);
  int maType = mau.typeFromString(s);

  settings.getData(SlowKColor, s);
  color.setNamedColor(s);

  settings.getData(SlowKPlot, s);
  lineType = fac.typeFromString(s);

  PlotLine *sk = mau.ma(fk, period, maType, lineType, color);
  if (!sk)
  {
    delete fk;
    return 1;
  }
  
  settings.getData(SlowKLabel, s);
  sk->setLabel(s);
  ind.addLine(sk);

  // create the slowd line
  period = settings.getInt(SlowDPeriod);

  settings.getData(SlowDMA, s);
  maType = mau.typeFromString(s);

  settings.getData(SlowDColor, s);
  color.setNamedColor(s);

  settings.getData(SlowDPlot, s);
  lineType = fac.typeFromString(s);

  PlotLine *sd = mau.ma(sk, period, maType, lineType, color);
  if (!sd)
  {
    delete fk;
    return 1;
  }

  settings.getData(SlowDLabel, s);
  sd->setLabel(s);
  ind.addLine(sd);

  delete fk;

  return 0;
}

int STOCHS::getCUS (QStringList &set, QHash<QString, PlotLine *> &tlines, BarData *data)
{
  // INDICATOR,PLUGIN,STOCHS,<NAME SLOWK>,<NAME SLOWD>,<FASTK PERIOD>,<SLOWK PERIOD>,<SLOWK MA TYPE>,<SLOWD PERIOD>,<SLOWD MA TYPE>,<SLOWK PLOT TYPE>,<SLOWD PLOT TYPE>,<SLOWK COLOR>,<SLOWD COLOR>
  //     0       1      2         3            4             5              6               7              8              9                10                11               12            13

  if (set.count() != 14)
  {
    qDebug() << indicator << "::getCUS: invalid settings count" << set.count();
    return 1;
  }

  PlotLine *tl = tlines.value(set[3]);
  if (tl)
  {
    qDebug() << indicator << "::getCUS: duplicate name" << set[3];
    return 1;
  }

  tl = tlines.value(set[4]);
  if (tl)
  {
    qDebug() << indicator << "::getCUS: duplicate name" << set[4];
    return 1;
  }

  bool ok;
  int fkp = set[5].toInt(&ok);
  if (! ok)
  {
    qDebug() << indicator << "::getCUS: invalid fastk period" << set[5];
    return 1;
  }

  int skp = set[6].toInt(&ok);
  if (! ok)
  {
    qDebug() << indicator << "::getCUS: invalid slowk period" << set[6];
    return 1;
  }

  MAFactory mau;
  int kma = mau.typeFromString(set[7]);
  if (kma == -1)
  {
    qDebug() << indicator << "::getCUS: invalid slowk ma" << set[7];
    return 1;
  }

  int sdp = set[8].toInt(&ok);
  if (! ok)
  {
    qDebug() << indicator << "::getCUS: invalid slowd period" << set[8];
    return 1;
  }

  int dma = mau.typeFromString(set[9]);
  if (dma == -1)
  {
    qDebug() << indicator << "::getCUS: invalid slowd ma" << set[9];
    return 1;
  }

  PlotFactory fac;
  int klineType = fac.typeFromString(set[10]);
  if (klineType == -1)
  {
    qDebug() << indicator << "::getCUS: invalid slowk plot type" << set[10];
    return 1;
  }

  int dlineType = fac.typeFromString(set[11]);
  if (dlineType == -1)
  {
    qDebug() << indicator << "::getCUS: invalid slowd plot type" << set[11];
    return 1;
  }

  QColor kcolor(set[12]);
  if (! kcolor.isValid())
  {
    qDebug() << indicator << "::getCUS: invalid slowk color" << set[12];
    return 1;
  }

  QColor dcolor(set[13]);
  if (! dcolor.isValid())
  {
    qDebug() << indicator << "::getCUS: invalid slowd color" << set[13];
    return 1;
  }

  STOCHUtils fastk;
  PlotLine *fk = fastk.fastK(data, fkp, klineType, kcolor);
  if (! fk)
    return 1;

  PlotLine *sk = mau.ma(fk, skp, kma, klineType, kcolor);
  if (! sk)
  {
    delete fk;
    return 1;
  }

  PlotLine *sd = mau.ma(sk, sdp, dma, dlineType, dcolor);
  if (! sd)
  {
    delete fk;
    delete sk;
    return 1;
  }

  sk->setLabel(set[3]);
  sd->setLabel(set[4]);

  tlines.insert(set[3], sk);
  tlines.insert(set[4], sd);

  delete fk;

  return 0;
}

int STOCHS::dialog (int)
{
  int page = 0;
  QString k, d;
  PrefDialog *dialog = new PrefDialog;
  dialog->setWindowTitle(QObject::tr("Edit Indicator"));

  k = QObject::tr("SlowK");
  dialog->addPage(page, k);

  settings.getData(SlowKColor, d);
  dialog->addColorItem(SlowKColor, page, QObject::tr("Color"), d);

  PlotFactory fac;
  QStringList plotList;
  fac.list(plotList, TRUE);

  settings.getData(SlowKPlot, d);
  dialog->addComboItem(SlowKPlot, page, QObject::tr("Plot"), plotList, d);

  settings.getData(SlowKLabel, d);
  dialog->addTextItem(SlowKLabel, page, QObject::tr("Label"), d);

  dialog->addIntItem(FastKPeriod, page, QObject::tr("FastK Period"), settings.getInt(FastKPeriod), 1, 100000);

  dialog->addIntItem(SlowKPeriod, page, QObject::tr("Period"), settings.getInt(SlowKPeriod), 1, 100000);

  QStringList maList;
  MAFactory mau;
  mau.list(maList);

  settings.getData(SlowKMA, d);
  dialog->addComboItem(SlowKMA, page, QObject::tr("MA Type"), maList, d);

  page++;
  k = QObject::tr("SlowD");
  dialog->addPage(page, k);

  settings.getData(SlowDColor, d);
  dialog->addColorItem(SlowDColor, page, QObject::tr("Color"), d);

  settings.getData(SlowDPlot, d);
  dialog->addComboItem(SlowDPlot, page, QObject::tr("Plot"), plotList, d);

  settings.getData(SlowDLabel, d);
  dialog->addTextItem(SlowDLabel, page, QObject::tr("Label"), d);

  dialog->addIntItem(SlowDPeriod, page, QObject::tr("Period"), settings.getInt(SlowDPeriod), 1, 100000);

  settings.getData(SlowDMA, d);
  dialog->addComboItem(SlowDMA, page, QObject::tr("MA Type"), maList, d);

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
  STOCHS *o = new STOCHS;
  return ((IndicatorPlugin *) o);
}


