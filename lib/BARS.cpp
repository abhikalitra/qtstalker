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

#include "BARS.h"

#include <QtDebug>


BARS::BARS ()
{
  indicator = "BARS";
  ucKey = QObject::tr("Up Color");
  dcKey = QObject::tr("Down Color");
  ncKey = QObject::tr("Neutral Color");

  QString d;
  d = "green";
  settings.setData(ucKey, d);

  d = "red";
  settings.setData(dcKey, d);

  d = "blue";
  settings.setData(ncKey, d);

  settings.setData(labelKey, indicator);
}

int BARS::getIndicator (Indicator &ind, BarData *data)
{
  QString s;
  settings.getData(ucKey, s);
  QColor up(s);

  settings.getData(dcKey, s);
  QColor down(s);

  settings.getData(ncKey, s);
  QColor neutral(s);

  PlotLine *line = getBARS(data, up, down, neutral);
  if (! line)
    return 1;

  line->setType(PlotLine::Bar);

  settings.getData(labelKey, s);
  line->setLabel(s);

  ind.addLine(line);

  return 0;
}

int BARS::getCUS (QStringList &set, QHash<QString, PlotLine *> &tlines, BarData *data)
{
  // INDICATOR,BARS,<NAME>,<BAR_UP_COLOR>,<BAR_DOWN_COLOR>,<BAR_NEUTRAL_COLOR>

  if (set.count() != 6)
  {
    qDebug() << indicator << "::calculate: invalid parm count" << set.count();
    return 1;
  }

  PlotLine *tl = tlines.value(set[2]);
  if (tl)
  {
    qDebug() << indicator << "::calculate: duplicate name" << set[2];
    return 1;
  }

  QColor barUpColor(set[3]);
  if (! barUpColor.isValid())
  {
    qDebug() << indicator << "::calculate: invalid bar up color" << set[3];
    return 1;
  }

  QColor barDownColor(set[4]);
  if (! barDownColor.isValid())
  {
    qDebug() << indicator << "::calculate: invalid bar down color" << set[4];
    return 1;
  }

  QColor barNeutralColor(set[5]);
  if (! barNeutralColor.isValid())
  {
    qDebug() << indicator << "::calculate: invalid bar neutral color" << set[5];
    return 1;
  }

  PlotLine *line = getBARS(data, barUpColor, barDownColor, barNeutralColor);
  tlines.insert(set[2], line);
  return 0;
}

PlotLine * BARS::getBARS (BarData *data, QColor &_up, QColor &_down, QColor &_neutral)
{
  int size = data->count();
  PlotLine *line = new PlotLine;
  int loop;
  for (loop = 0; loop < size; loop++)
  {
    if (loop > 0)
    {
      if (data->getClose(loop) > data->getClose(loop - 1))
        line->append(_up, data->getOpen(loop), data->getHigh(loop), data->getLow(loop), data->getClose(loop), FALSE);
      else
      {
        if (data->getClose(loop) < data->getClose(loop - 1))
          line->append(_down, data->getOpen(loop), data->getHigh(loop), data->getLow(loop), data->getClose(loop), FALSE);
        else
          line->append(_neutral, data->getOpen(loop), data->getHigh(loop), data->getLow(loop), data->getClose(loop), FALSE);
      }
    }
    else
      line->append(_neutral, data->getOpen(loop), data->getHigh(loop), data->getLow(loop), data->getClose(loop), FALSE);
  }

  return line;
}

int BARS::dialog ()
{
  int page = 0;
  QString k, d;
  PrefDialog *dialog = new PrefDialog;
  dialog->setWindowTitle(QObject::tr("Edit Indicator"));

  k = QObject::tr("Settings");
  dialog->addPage(page, k);

  settings.getData(ucKey, d);
  QColor c(d);
  dialog->addColorItem(page, ucKey, c);

  settings.getData(dcKey, d);
  c.setNamedColor(d);
  dialog->addColorItem(page, dcKey, c);

  settings.getData(ncKey, d);
  c.setNamedColor(d);
  dialog->addColorItem(page, ncKey, c);

  settings.getData(labelKey, d);
  dialog->addTextItem(page, labelKey, d);

  int rc = dialog->exec();
  if (rc == QDialog::Rejected)
  {
    delete dialog;
    return rc;
  }

  QStringList keys;
  settings.getKeyList(keys);
  int loop;
  for (loop = 0; loop < keys.count(); loop++)
  {
    QString d;
    dialog->getItem(keys[loop], d);
    if (! d.isEmpty())
      settings.setData(keys[loop], d);
  }

  delete dialog;
  return rc;
}

