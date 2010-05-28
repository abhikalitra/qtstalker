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
#include "MAFactory.h"
#include "BARSUtils.h"
#include "PlotFactory.h"

#include <QtDebug>
#include <QObject>

BARS::BARS ()
{
  _indicator = "BARS";

  _settings.setData(UpColor, "green");
  _settings.setData(DownColor, "red");
  _settings.setData(NeutralColor, "blue");
  _settings.setData(BarsLabel, _indicator);
  _settings.setData(MAColor, "red");
  _settings.setData(MA2Color, "yellow");
  _settings.setData(MA3Color, "blue");
  _settings.setData(MAPlot, "Line");
  _settings.setData(MA2Plot, "Line");
  _settings.setData(MA3Plot, "Line");
  _settings.setData(MALabel, "MA1");
  _settings.setData(MA2Label, "MA2");
  _settings.setData(MA3Label, "MA3");
  _settings.setData(MAPeriod, 20);
  _settings.setData(MA2Period, 50);
  _settings.setData(MA3Period, 200);
  _settings.setData(MAType, "SMA");
  _settings.setData(MA2Type, "SMA");
  _settings.setData(MA3Type, "SMA");
}

int BARS::getIndicator (Indicator &ind, BarData *data)
{
  QString s;
  _settings.getData(UpColor, s);
  QColor up(s);

  _settings.getData(DownColor, s);
  QColor down(s);

  _settings.getData(NeutralColor, s);
  QColor neutral(s);

  BARSUtils b;
  PlotLine *line = b.getBARS(data, up, down, neutral);
  if (! line)
    return 1;

  _settings.getData(BarsLabel, s);
  line->setLabel(s);

  s = "0";
  ind.setLine(s, line);
  ind.addPlotOrder(s);

  int period = _settings.getInt(MAPeriod);
  if (period > 1)
  {
    _settings.getData(MAType, s);
    MAFactory mafac;
    int type = mafac.typeFromString(s);

    _settings.getData(MAColor, s);
    QColor color(s);

    _settings.getData(MAPlot, s);
    PlotFactory plfac;
    int lineType = plfac.typeFromString(s);
    
    PlotLine *ma = mafac.ma(line, period, type, lineType, color);
    if (ma)
    {
      _settings.getData(MALabel, s);
      ma->setLabel(s);
      
      s = "1";
      ind.setLine(s, ma);
      ind.addPlotOrder(s);
    }
  }

  period = _settings.getInt(MA2Period);
  if (period > 1)
  {
    _settings.getData(MA2Type, s);
    MAFactory mafac;
    int type = mafac.typeFromString(s);

    _settings.getData(MA2Color, s);
    QColor color(s);

    _settings.getData(MA2Plot, s);
    PlotFactory plfac;
    int lineType = plfac.typeFromString(s);

    PlotLine *ma = mafac.ma(line, period, type, lineType, color);
    if (ma)
    {
      _settings.getData(MA2Label, s);
      ma->setLabel(s);
      
      s = "2";
      ind.setLine(s, ma);
      ind.addPlotOrder(s);
    }
  }

  period = _settings.getInt(MA3Period);
  if (period > 1)
  {
    _settings.getData(MA3Type, s);
    MAFactory mafac;
    int type = mafac.typeFromString(s);

    _settings.getData(MA3Color, s);
    QColor color(s);

    _settings.getData(MA3Plot, s);
    PlotFactory plfac;
    int lineType = plfac.typeFromString(s);

    PlotLine *ma = mafac.ma(line, period, type, lineType, color);
    if (ma)
    {
      _settings.getData(MA3Label, s);
      ma->setLabel(s);
      
      s = "3";
      ind.setLine(s, ma);
      ind.addPlotOrder(s);
    }
  }

  return 0;
}

int BARS::getCUS (QStringList &set, Indicator &ind, BarData *data)
{
  // INDICATOR,PLUGIN,BARS,<NAME>,<BAR_UP_COLOR>,<BAR_DOWN_COLOR>,<BAR_NEUTRAL_COLOR>
  //     0       1     2     3          4              5                  6

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

  QColor barUpColor(set[4]);
  if (! barUpColor.isValid())
  {
    qDebug() << _indicator << "::getCUS: invalid bar up color" << set[4];
    return 1;
  }

  QColor barDownColor(set[5]);
  if (! barDownColor.isValid())
  {
    qDebug() << _indicator << "::getCUS: invalid bar down color" << set[5];
    return 1;
  }

  QColor barNeutralColor(set[6]);
  if (! barNeutralColor.isValid())
  {
    qDebug() << _indicator << "::getCUS: invalid bar neutral color" << set[6];
    return 1;
  }

  BARSUtils b;
  PlotLine *line = b.getBARS(data, barUpColor, barDownColor, barNeutralColor);
  line->setLabel(set[3]);
  ind.setLine(set[3], line);
  return 0;
}

int BARS::dialog (int)
{
  int page = 0;
  QString k, d;
  PrefDialog *dialog = new PrefDialog;
  dialog->setWindowTitle(QObject::tr("Edit Indicator"));

  k = QObject::tr("Settings");
  dialog->addPage(page, k);

  _settings.getData(UpColor, d);
  dialog->addColorItem(UpColor, page, QObject::tr("Up"), d);

  _settings.getData(DownColor, d);
  dialog->addColorItem(DownColor, page, QObject::tr("Down"), d);

  _settings.getData(NeutralColor, d);
  dialog->addColorItem(NeutralColor, page, QObject::tr("Neutral"), d);

  _settings.getData(BarsLabel, d);
  dialog->addTextItem(BarsLabel, page, QObject::tr("Label"), d);

  QStringList maList;
  MAFactory mau;
  mau.list(maList);
  
  PlotFactory fac;
  QStringList plotList;
  fac.list(plotList, TRUE);

  page++;
  k = QObject::tr("MA 1");
  dialog->addPage(page, k);

  _settings.getData(MAColor, d);
  dialog->addColorItem(MAColor, page, QObject::tr("Color"), d);

  _settings.getData(MAPlot, d);
  dialog->addComboItem(MAPlot, page, QObject::tr("Plot"), plotList, d);

  _settings.getData(MALabel, d);
  dialog->addTextItem(MALabel, page, QObject::tr("Label"), d);

  dialog->addIntItem(MAPeriod, page, QObject::tr("Period"), _settings.getInt(MAPeriod), 1, 100000);

  _settings.getData(MAType, d);
  dialog->addComboItem(MAType, page, QObject::tr("Type"), maList, d);

  page++;
  k = QObject::tr("MA 2");
  dialog->addPage(page, k);

  _settings.getData(MA2Color, d);
  dialog->addColorItem(MA2Color, page, QObject::tr("Color"), d);

  _settings.getData(MA2Plot, d);
  dialog->addComboItem(MA2Plot, page, QObject::tr("Plot"), plotList, d);

  _settings.getData(MA2Label, d);
  dialog->addTextItem(MA2Label, page, QObject::tr("Label"), d);

  dialog->addIntItem(MA2Period, page, QObject::tr("Period"), _settings.getInt(MA2Period), 1, 100000);

  _settings.getData(MA2Type, d);
  dialog->addComboItem(MA2Type, page, QObject::tr("Type"), maList, d);

  page++;
  k = QObject::tr("MA 3");
  dialog->addPage(page, k);

  _settings.getData(MA3Color, d);
  dialog->addColorItem(MA3Color, page, QObject::tr("Color"), d);

  _settings.getData(MA3Plot, d);
  dialog->addComboItem(MA3Plot, page, QObject::tr("Plot"), plotList, d);

  _settings.getData(MA3Label, d);
  dialog->addTextItem(MA3Label, page, QObject::tr("Label"), d);

  dialog->addIntItem(MA3Period, page, QObject::tr("Period"), _settings.getInt(MA3Period), 1, 100000);

  _settings.getData(MA3Type, d);
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


