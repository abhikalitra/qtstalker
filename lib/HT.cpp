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

#include "HT.h"
#include "BARS.h"

#include <QtDebug>


HT::HT ()
{
  indicator = "HT";
  methodKey = QObject::tr("Method");
  phaseColorKey = QObject::tr("Phase Color");
  quadColorKey = QObject::tr("Quad Color");
  sineColorKey = QObject::tr("Sine Color");
  leadColorKey = QObject::tr("Lead Color");
  phasePlotKey = QObject::tr("Phase Plot");
  quadPlotKey = QObject::tr("Quad Plot");
  sinePlotKey = QObject::tr("Sine Plot");
  leadPlotKey = QObject::tr("Lead Plot");
  phaseLabelKey = QObject::tr("Phase Label");
  quadLabelKey = QObject::tr("Quad Label");
  sineLabelKey = QObject::tr("Sine Label");
  leadLabelKey = QObject::tr("Lead Label");

  methodList << "DCPERIOD";
  methodList << "DCPHASE";
  methodList << "TRENDLINE";
  methodList << "TRENDMODE";
  methodList << "PHASE";
  methodList << "QUAD";
  methodList << "SINE";
  methodList << "LEAD";

  guiMethodList << "DCPERIOD";
  guiMethodList << "DCPHASE";
  guiMethodList << "TRENDLINE";
  guiMethodList << "TRENDMODE";
  guiMethodList << "PHASOR";
  guiMethodList << "SINE";

  QString d;
  d = "red";
  settings.setData(colorKey, d);
  settings.setData(phaseColorKey, d);
  settings.setData(sineColorKey, d);

  d = "yellow";
  settings.setData(quadColorKey, d);
  settings.setData(leadColorKey, d);

  d = "Line";
  settings.setData(plotKey, d);
  settings.setData(phasePlotKey, d);
  settings.setData(quadPlotKey, d);
  settings.setData(sinePlotKey, d);
  settings.setData(leadPlotKey, d);

  settings.setData(labelKey, indicator);

  d = "PHASE";
  settings.setData(phaseLabelKey, d);

  d = "QUAD";
  settings.setData(quadLabelKey, d);

  d = "SINE";
  settings.setData(sineLabelKey, d);

  d = "LEAD";
  settings.setData(leadLabelKey, d);

  d = "Close";
  settings.setData(inputKey, d);

  d = "TRENDLINE";
  settings.setData(methodKey, d);
}

int HT::getIndicator (Indicator &ind, BarData *data)
{
  QString s;
  settings.getData(inputKey, s);
  PlotLine *in = data->getInput(data->getInputType(s));
  if (! in)
  {
    qDebug() << indicator << "::calculate: input not found" << s;
    return 1;
  }

  settings.getData(methodKey, s);
  int method = guiMethodList.indexOf(s);

  switch (method)
  {
    case 4: // phasor
    {
      int t = methodList.indexOf(QString("PHASE"));
      PlotLine *phase = getHT(in, t);
      if (! phase)
      {
        delete in;
        return 1;
      }
      settings.getData(phaseColorKey, s);
      phase->setColor(s);
      settings.getData(phasePlotKey, s);
      phase->setType(s);
      settings.getData(phaseLabelKey, s);
      phase->setLabel(s);

      t = methodList.indexOf(QString("QUAD"));
      PlotLine *quad = getHT(in, t);
      if (! quad)
      {
        delete in;
	delete phase;
        return 1;
      }
      settings.getData(quadColorKey, s);
      quad->setColor(s);
      settings.getData(quadPlotKey, s);
      quad->setType(s);
      settings.getData(quadLabelKey, s);
      quad->setLabel(s);

      ind.addLine(phase);
      ind.addLine(quad);
      break;
    }
    case 5: // sine
    {
      int t = methodList.indexOf(QString("SINE"));
      PlotLine *sine = getHT(in, t);
      if (! sine)
      {
        delete in;
        return 1;
      }
      settings.getData(sineColorKey, s);
      sine->setColor(s);
      settings.getData(sinePlotKey, s);
      sine->setType(s);
      settings.getData(sineLabelKey, s);
      sine->setLabel(s);

      t = methodList.indexOf(QString("LEAD"));
      PlotLine *lead = getHT(in, t);
      if (! lead)
      {
        delete in;
	delete sine;
        return 1;
      }
      settings.getData(leadColorKey, s);
      lead->setColor(s);
      settings.getData(leadPlotKey, s);
      lead->setType(s);
      settings.getData(leadLabelKey, s);
      lead->setLabel(s);

      ind.addLine(sine);
      ind.addLine(lead);
      break;
    }
    default:
    {
      PlotLine *line = getHT(in, method);
      if (! line)
      {
        delete in;
        return 1;
      }

      if (method != 3)
      {
        BARS bars;
        bars.getIndicator(ind, data);
      }

      settings.getData(colorKey, s);
      line->setColor(s);
      settings.getData(plotKey, s);
      line->setType(s);
      settings.getData(labelKey, s);
      line->setLabel(s);
      ind.addLine(line);
      break;
    }
  }

  delete in;
  return 0;
}

int HT::getCUS (QStringList &set, QHash<QString, PlotLine *> &tlines, BarData *data)
{
  // INDICATOR,HT,METHOD,<NAME>,<INPUT>

  if (set.count() != 5)
  {
    qDebug() << indicator << "::calculate: invalid settings count" << set.count();
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

  PlotLine *line = getHT(in, method);
  if (! line)
    return 1;

  tlines.insert(set[3], line);

  return 0;
}

PlotLine * HT::getHT (PlotLine *in, int method)
{
  int size = in->getSize();
  TA_Integer outBeg;
  TA_Integer outNb;
  TA_Real input[size];
  TA_Real out[size];
  int loop;
  for (loop = 0; loop < size; loop++)
    input[loop] = (TA_Real) in->getData(loop);

  TA_RetCode rc = TA_SUCCESS;
  switch (method)
  {
    case 0:
      rc = TA_HT_DCPERIOD (0, size - 1, &input[0], &outBeg, &outNb, &out[0]);
      break;
    case 1:
      rc = TA_HT_DCPHASE (0, size - 1, &input[0], &outBeg, &outNb, &out[0]);
      break;
    case 2:
      rc = TA_HT_TRENDLINE (0, size - 1, &input[0], &outBeg, &outNb, &out[0]);
      break;
    case 3:
    {
      TA_Integer iout[size];
      rc = TA_HT_TRENDMODE (0, size - 1, &input[0], &outBeg, &outNb, &iout[0]);
      if (rc != TA_SUCCESS)
      {
        qDebug() << indicator << "::calculate TA-Lib error" << rc;
        return 0;
      }
      PlotLine *line = new PlotLine;
      for (loop = 0; loop < outNb; loop++)
        line->append(iout[loop]);
      return line;
      break;
    }
    case 4: // phase line
    {
      TA_Real out2[size];
      rc = TA_HT_PHASOR (0, size - 1, &input[0], &outBeg, &outNb, &out[0], &out2[0]);
      break;
    }
    case 5: // quad line
    {
      TA_Real out2[size];
      rc = TA_HT_PHASOR (0, size - 1, &input[0], &outBeg, &outNb, &out2[0], &out[0]);
      break;
    }
    case 6: // sine line
    {
      TA_Real out2[size];
      rc = TA_HT_SINE (0, size - 1, &input[0], &outBeg, &outNb, &out[0], &out2[0]);
      break;
    }
    case 7: // lead line
    {
      TA_Real out2[size];
      rc = TA_HT_SINE (0, size - 1, &input[0], &outBeg, &outNb, &out2[0], &out[0]);
      break;
    }
    default:
      break;
  }

  if (rc != TA_SUCCESS)
  {
    qDebug() << indicator << "::calculate TA-Lib error" << rc;
    return 0;
  }

  PlotLine *line = new PlotLine;
  for (loop = 0; loop < outNb; loop++)
    line->append(out[loop]);

  return line;
}

int HT::dialog ()
{
  int page = 0;
  QString k, d;
  PrefDialog *dialog = new PrefDialog;
  dialog->setWindowTitle(QObject::tr("Edit Indicator"));

  k = QObject::tr("Settings");
  dialog->addPage(page, k);

  settings.getData(colorKey, d);
  dialog->addColorItem(page, colorKey, d);

  settings.getData(plotKey, d);
  dialog->addComboItem(page, plotKey, plotList, d);

  settings.getData(labelKey, d);
  dialog->addTextItem(page, labelKey, d);

  settings.getData(inputKey, d);
  dialog->addComboItem(page, inputKey, inputList, d);

  settings.getData(methodKey, d);
  dialog->addComboItem(page, methodKey, guiMethodList, d);

  page++;
  k = QObject::tr("Phasor");
  dialog->addPage(page, k);

  settings.getData(phaseColorKey, d);
  dialog->addColorItem(page, phaseColorKey, d);

  settings.getData(quadColorKey, d);
  dialog->addColorItem(page, quadColorKey, d);

  settings.getData(phasePlotKey, d);
  dialog->addComboItem(page, phasePlotKey, plotList, d);

  settings.getData(quadPlotKey, d);
  dialog->addComboItem(page, quadPlotKey, plotList, d);

  settings.getData(phaseLabelKey, d);
  dialog->addTextItem(page, phaseLabelKey, d);

  settings.getData(quadLabelKey, d);
  dialog->addTextItem(page, quadLabelKey, d);

  page++;
  k = QObject::tr("Sine Wave");
  dialog->addPage(page, k);

  settings.getData(sineColorKey, d);
  dialog->addColorItem(page, sineColorKey, d);

  settings.getData(leadColorKey, d);
  dialog->addColorItem(page, leadColorKey, d);

  settings.getData(sinePlotKey, d);
  dialog->addComboItem(page, sinePlotKey, plotList, d);

  settings.getData(leadPlotKey, d);
  dialog->addComboItem(page, leadPlotKey, plotList, d);

  settings.getData(sineLabelKey, d);
  dialog->addTextItem(page, sineLabelKey, d);

  settings.getData(leadLabelKey, d);
  dialog->addTextItem(page, leadLabelKey, d);

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


