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
#include "MATH1.h"
#include "ta_libc.h"

#include <QtDebug>


HT::HT ()
{
  indicator = "HT";

  settings.setData(Method, "TRENDLINE");
  settings.setData(Color, "red");
  settings.setData(PhaseColor, "red");
  settings.setData(QuadColor, "yellow");
  settings.setData(SineColor, "red");
  settings.setData(LeadColor, "yellow");
  settings.setData(Plot, "Line");
  settings.setData(PhasePlot, "Line");
  settings.setData(QuadPlot, "Line");
  settings.setData(SinePlot, "Line");
  settings.setData(LeadPlot, "Line");
  settings.setData(Label, indicator);
  settings.setData(PhaseLabel, "PHASE");
  settings.setData(QuadLabel, "QUAD");
  settings.setData(SineLabel, "SINE");
  settings.setData(LeadLabel, "LEAD");
  settings.setData(Input, "Close");

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
}

int HT::getIndicator (Indicator &ind, BarData *data)
{
  QString s;
  settings.getData(Input, s);
  PlotLine *in = data->getInput(data->getInputType(s));
  if (! in)
  {
    qDebug() << indicator << "::calculate: input not found" << s;
    return 1;
  }

  settings.getData(Method, s);
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
      settings.getData(PhaseColor, s);
      phase->setColor(s);
      settings.getData(PhasePlot, s);
      phase->setPlugin(s);
      settings.getData(PhaseLabel, s);
      phase->setLabel(s);

      t = methodList.indexOf(QString("QUAD"));
      PlotLine *quad = getHT(in, t);
      if (! quad)
      {
        delete in;
	delete phase;
        return 1;
      }
      settings.getData(QuadColor, s);
      quad->setColor(s);
      settings.getData(QuadPlot, s);
      quad->setPlugin(s);
      settings.getData(QuadLabel, s);
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
      settings.getData(SineColor, s);
      sine->setColor(s);
      settings.getData(SinePlot, s);
      sine->setPlugin(s);
      settings.getData(SineLabel, s);
      sine->setLabel(s);

      t = methodList.indexOf(QString("LEAD"));
      PlotLine *lead = getHT(in, t);
      if (! lead)
      {
        delete in;
	delete sine;
        return 1;
      }
      settings.getData(LeadColor, s);
      lead->setColor(s);
      settings.getData(LeadPlot, s);
      lead->setPlugin(s);
      settings.getData(LeadLabel, s);
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
	MATH1 m;
	QColor up("green");
	QColor down("red");
	QColor neutral("neutral");
	PlotLine *bars = m.getBARS(data, up, down, neutral);
	if (bars)
	  ind.addLine(bars);
      }

      settings.getData(Color, s);
      line->setColor(s);
      settings.getData(Plot, s);
      line->setPlugin(s);
      settings.getData(Label, s);
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
  int size = in->count();
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

int HT::dialog (int)
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

  settings.getData(Method, d);
  dialog->addComboItem(Method, page, QObject::tr("Method"), guiMethodList, d);

  page++;
  k = QObject::tr("Phasor");
  dialog->addPage(page, k);

  settings.getData(PhaseColor, d);
  dialog->addColorItem(PhaseColor, page, QObject::tr("Phase Color"), d);

  settings.getData(QuadColor, d);
  dialog->addColorItem(QuadColor, page, QObject::tr("Quad Color"), d);

  settings.getData(PhasePlot, d);
  dialog->addComboItem(PhasePlot, page, QObject::tr("Phase Plot"), plotList, d);

  settings.getData(QuadPlot, d);
  dialog->addComboItem(QuadPlot, page, QObject::tr("Quad Plot"), plotList, d);

  settings.getData(PhaseLabel, d);
  dialog->addTextItem(PhaseLabel, page, QObject::tr("Phase Label"), d);

  settings.getData(QuadLabel, d);
  dialog->addTextItem(QuadLabel, page, QObject::tr("Quad Label"), d);

  page++;
  k = QObject::tr("Sine Wave");
  dialog->addPage(page, k);

  settings.getData(SineColor, d);
  dialog->addColorItem(SineColor, page, QObject::tr("Sine Color"), d);

  settings.getData(LeadColor, d);
  dialog->addColorItem(LeadColor, page, QObject::tr("Lead Color"), d);

  settings.getData(SinePlot, d);
  dialog->addComboItem(SinePlot, page, QObject::tr("Sine Plot"), plotList, d);

  settings.getData(LeadPlot, d);
  dialog->addComboItem(LeadPlot, page, QObject::tr("Lead Plot"), plotList, d);

  settings.getData(SineLabel, d);
  dialog->addTextItem(SineLabel, page, QObject::tr("Sine Label"), d);

  settings.getData(LeadLabel, d);
  dialog->addTextItem(LeadLabel, page, QObject::tr("Lead Label"), d);

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
  HT *o = new HT;
  return ((IndicatorPlugin *) o);
}


