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

#include "VOL.h"

#include <QtDebug>


VOL::VOL ()
{
  indicator = "VOL";

  settings.setData(UpColor, "green");
  settings.setData(DownColor, "red");
  settings.setData(NeutralColor, "blue");
  settings.setData(Color, "red");
  settings.setData(MAColor, "yellow");
  settings.setData(Plot, "Histogram Bar");
  settings.setData(MAPlot, "Line");
  settings.setData(Label, indicator);
  settings.setData(MALabel, "VOLMA");
  settings.setData(Method, "VOL");
  settings.setData(MAPeriod, 10);
  settings.setData(MAType, "SMA");

  methodList << "OBV";
  methodList << "VOL";
}

int VOL::getIndicator (Indicator &ind, BarData *data)
{
  QString s;
  settings.getData(Method, s);
  int method = methodList.indexOf(s);

  PlotLine *line = getVOL(data, method);
  if (! line)
    return 1;

  // vol ma
  int period = settings.getInt(MAPeriod);

  settings.getData(MAType, s);
  int type = maList.indexOf(s);

  PlotLine *ma = getMA(line, period, type);
  if (! ma)
  {
    delete line;
    return 1;
  }

  settings.getData(MAColor, s);
  ma->setColor(s);

  settings.getData(MAPlot, s);
  ma->setType(s);

  settings.getData(MALabel, s);
  ma->setLabel(s);

  // vol
  settings.getData(Color, s);
  line->setColor(s);

  settings.getData(Plot, s);
  line->setType(s);

  settings.getData(Label, s);
  line->setLabel(s);

  ind.addLine(line);
  ind.addLine(ma);

  return 0;
}

int VOL::getCUS (QStringList &set, QHash<QString, PlotLine *> &tlines, BarData *data)
{
  // INDICATOR,VOL,METHOD,<NAME>

  if (set.count() != 4)
  {
    qDebug() << indicator << "::calculate: invalid parm count" << set.count();
    return 1;
  }

  int method = methodList.indexOf(set[2]);
  if (method == -1)
  {
    qDebug() << indicator << "::calculate: invalid method" << set[2];
    return 1;
  }

  PlotLine *tl = tlines.value(set[3]);
  if (tl)
  {
    qDebug() << indicator << "::calculate: duplicate name" << set[3];
    return 1;
  }

  PlotLine *line = getVOL(data, method);
  if (! line)
    return 1;

  tlines.insert(set[3], line);

  return 0;
}

PlotLine * VOL::getVOL (BarData *data, int method)
{
  int size = data->count();
  TA_Real out[size];
  TA_Integer outBeg;
  TA_Integer outNb;
  TA_RetCode rc = TA_SUCCESS;
  switch (method)
  {
    case 0:
    {
      TA_Real close[size];
      TA_Real volume[size];
      int loop;
      for (loop = 0; loop < size; loop++)
      {
        close[loop] = (TA_Real) data->getClose(loop);
        volume[loop] = (TA_Real) data->getVolume(loop);
      }
      rc = TA_OBV(0, size - 1, &close[0], &volume[0], &outBeg, &outNb, &out[0]);
      break;
    }
    case 1: // volume;
    {
      PlotLine *line = data->getInput(BarData::Volume);
      if (! line)
      {
        qDebug() << indicator << "::getVOL: no volume data";
        return 0;
      }

      line->setColorFlag(TRUE);
      int loop;
      QColor up("green");
      QColor down("red");
      QColor neutral("blue");
      for (loop = 1; loop < data->count(); loop++)
      {
	if (data->getClose(loop) < data->getClose(loop - 1))
	  line->setColorBar(loop, down);
	else
	{
	  if (data->getClose(loop) > data->getClose(loop - 1))
	    line->setColorBar(loop, up);
	  else
	    line->setColorBar(loop, neutral);
	}
      }
      return line;
      break;
    }
    default:
      break;
  }

  if (rc != TA_SUCCESS)
  {
    qDebug() << indicator << "::calculate: : TA-Lib error" << rc;
    return 0;
  }

  int loop;
  PlotLine *line = new PlotLine;
  for (loop = 0; loop < outNb; loop++)
    line->append(out[loop]);

  return line;
}

int VOL::dialog ()
{
  int page = 0;
  QString k, d;
  PrefDialog *dialog = new PrefDialog;
  dialog->setWindowTitle(QObject::tr("Edit Indicator"));

  k = QObject::tr("Settings");
  dialog->addPage(page, k);

  settings.getData(UpColor, d);
  dialog->addColorItem(UpColor, page, QObject::tr("Up Color"), d);

  settings.getData(DownColor, d);
  dialog->addColorItem(DownColor, page, QObject::tr("Down Color"), d);

  settings.getData(NeutralColor, d);
  dialog->addColorItem(NeutralColor, page, QObject::tr("Neutral Color"), d);

  settings.getData(Plot, d);
  dialog->addComboItem(Plot, page, QObject::tr("Plot"), plotList, d);

  settings.getData(Label, d);
  dialog->addTextItem(Label, page, QObject::tr("Label"), d);

  settings.getData(Method, d);
  dialog->addComboItem(Method, page, QObject::tr("Method"), methodList, d);

  page++;
  k = QObject::tr("MA");
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


