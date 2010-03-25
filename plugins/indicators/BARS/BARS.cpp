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

#include "BARS.h"
#include "PrefDialog.h"
#include "MATH1.h"

#include <QtDebug>
#include <QObject>


BARS::BARS ()
{
  indicator = "BARS";

  settings.setData(UpColor, "green");
  settings.setData(DownColor, "red");
  settings.setData(NeutralColor, "blue");
  settings.setData(BarsLabel, indicator);
  settings.setData(MAColor, "red");
  settings.setData(MA2Color, "yellow");
  settings.setData(MA3Color, "blue");
  settings.setData(MAPlot, "Line");
  settings.setData(MA2Plot, "Line");
  settings.setData(MA3Plot, "Line");
  settings.setData(MALabel, "MA1");
  settings.setData(MA2Label, "MA2");
  settings.setData(MA3Label, "MA3");
  settings.setData(MAPeriod, 20);
  settings.setData(MA2Period, 50);
  settings.setData(MA3Period, 200);
  settings.setData(MAType, "SMA");
  settings.setData(MA2Type, "SMA");
  settings.setData(MA3Type, "SMA");
}

int BARS::getIndicator (Indicator &ind, BarData *data)
{
  QString s;
  settings.getData(UpColor, s);
  QColor up(s);

  settings.getData(DownColor, s);
  QColor down(s);

  settings.getData(NeutralColor, s);
  QColor neutral(s);

  PlotLine *line = getBARS(data, up, down, neutral);
  if (! line)
    return 1;

  settings.getData(BarsLabel, s);
  line->setLabel(s);

  ind.addLine(line);

  MATH1 m;
  QStringList maList;
  m.getMAList(maList);
  
  int period = settings.getInt(MAPeriod);
  if (period > 1)
  {
    settings.getData(MAType, s);
    int type = maList.indexOf(s);
    PlotLine *ma = m.getMA(line, period, type);
    if (ma)
    {
      settings.getData(MAColor, s);
      ma->setColor(s);
      settings.getData(MAPlot, s);
      ma->setPlugin(s);
      settings.getData(MALabel, s);
      ma->setLabel(s);
      ind.addLine(ma);
    }
  }

  period = settings.getInt(MA2Period);
  if (period > 1)
  {
    settings.getData(MA2Type, s);
    int type = maList.indexOf(s);
    PlotLine *ma = m.getMA(line, period, type);
    if (ma)
    {
      settings.getData(MA2Color, s);
      ma->setColor(s);
      settings.getData(MA2Plot, s);
      ma->setPlugin(s);
      settings.getData(MA2Label, s);
      ma->setLabel(s);
      ind.addLine(ma);
    }
  }

  period = settings.getInt(MA3Period);
  if (period > 1)
  {
    settings.getData(MA3Type, s);
    int type = maList.indexOf(s);
    PlotLine *ma = m.getMA(line, period, type);
    if (ma)
    {
      settings.getData(MA3Color, s);
      ma->setColor(s);
      settings.getData(MA3Plot, s);
      ma->setPlugin(s);
      settings.getData(MA3Label, s);
      ma->setLabel(s);
      ind.addLine(ma);
    }
  }

  return 0;
}

int BARS::getCUS (QStringList &set, QHash<QString, PlotLine *> &tlines, BarData *data)
{
  // INDICATOR,PLUGIN,BARS,<NAME>,<BAR_UP_COLOR>,<BAR_DOWN_COLOR>,<BAR_NEUTRAL_COLOR>

  if (set.count() != 7)
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

  QColor barUpColor(set[4]);
  if (! barUpColor.isValid())
  {
    qDebug() << indicator << "::calculate: invalid bar up color" << set[4];
    return 1;
  }

  QColor barDownColor(set[5]);
  if (! barDownColor.isValid())
  {
    qDebug() << indicator << "::calculate: invalid bar down color" << set[5];
    return 1;
  }

  QColor barNeutralColor(set[6]);
  if (! barNeutralColor.isValid())
  {
    qDebug() << indicator << "::calculate: invalid bar neutral color" << set[6];
    return 1;
  }

  PlotLine *line = getBARS(data, barUpColor, barDownColor, barNeutralColor);
  tlines.insert(set[3], line);
  return 0;
}

PlotLine * BARS::getBARS (BarData *data, QColor &_up, QColor &_down, QColor &_neutral)
{
  MATH1 m;
  PlotLine *line = m.getBARS(data, _up, _down, _neutral);
  QString s = "OHLC";
  line->setPlugin(s);
  return line;
}

int BARS::dialog (int)
{
  int page = 0;
  QString k, d;
  PrefDialog *dialog = new PrefDialog;
  dialog->setWindowTitle(QObject::tr("Edit Indicator"));

  k = QObject::tr("Settings");
  dialog->addPage(page, k);

  settings.getData(UpColor, d);
  dialog->addColorItem(UpColor, page, QObject::tr("Up"), d);

  settings.getData(DownColor, d);
  dialog->addColorItem(DownColor, page, QObject::tr("Down"), d);

  settings.getData(NeutralColor, d);
  dialog->addColorItem(NeutralColor, page, QObject::tr("Neutral"), d);

  settings.getData(BarsLabel, d);
  dialog->addTextItem(BarsLabel, page, QObject::tr("Label"), d);

  MATH1 m;
  QStringList maList;
  m.getMAList(maList);
  
  page++;
  k = QObject::tr("MA 1");
  dialog->addPage(page, k);

  settings.getData(MAColor, d);
  dialog->addColorItem(MAColor, page, QObject::tr("Color"), d);

  settings.getData(MAPlot, d);
  dialog->addComboItem(MAPlot, page, QObject::tr("Plot"), plotList, d);

  settings.getData(MALabel, d);
  dialog->addTextItem(MALabel, page, QObject::tr("Label"), d);

  dialog->addIntItem(MAPeriod, page, QObject::tr("Period"), settings.getInt(MAPeriod), 1, 100000);

  settings.getData(MAType, d);
  dialog->addComboItem(MAType, page, QObject::tr("Type"), maList, d);

  page++;
  k = QObject::tr("MA 2");
  dialog->addPage(page, k);

  settings.getData(MA2Color, d);
  dialog->addColorItem(MA2Color, page, QObject::tr("Color"), d);

  settings.getData(MA2Plot, d);
  dialog->addComboItem(MA2Plot, page, QObject::tr("Plot"), plotList, d);

  settings.getData(MA2Label, d);
  dialog->addTextItem(MA2Label, page, QObject::tr("Label"), d);

  dialog->addIntItem(MA2Period, page, QObject::tr("Period"), settings.getInt(MA2Period), 1, 100000);

  settings.getData(MA2Type, d);
  dialog->addComboItem(MA2Type, page, QObject::tr("Type"), maList, d);

  page++;
  k = QObject::tr("MA 3");
  dialog->addPage(page, k);

  settings.getData(MA3Color, d);
  dialog->addColorItem(MA3Color, page, QObject::tr("Color"), d);

  settings.getData(MA3Plot, d);
  dialog->addComboItem(MA3Plot, page, QObject::tr("Plot"), plotList, d);

  settings.getData(MA3Label, d);
  dialog->addTextItem(MA3Label, page, QObject::tr("Label"), d);

  dialog->addIntItem(MA3Period, page, QObject::tr("Period"), settings.getInt(MA3Period), 1, 100000);

  settings.getData(MA3Type, d);
  dialog->addComboItem(MA3Type, page, QObject::tr("Type"), maList, d);

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
  BARS *o = new BARS;
  return ((IndicatorPlugin *) o);
}


