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

#include "HT.h"
#include "FunctionHT.h"
#include "BARSUtils.h"
#include "PlotStyleFactory.h"

#include <QtDebug>

HT::HT ()
{
  _indicator = "HT";

  _settings.setData(Method, "TRENDLINE");
  _settings.setData(Color, "red");
  _settings.setData(PhaseColor, "red");
  _settings.setData(QuadColor, "yellow");
  _settings.setData(SineColor, "red");
  _settings.setData(LeadColor, "yellow");
  _settings.setData(Plot, "Line");
  _settings.setData(PhasePlot, "Line");
  _settings.setData(QuadPlot, "Line");
  _settings.setData(SinePlot, "Line");
  _settings.setData(LeadPlot, "Line");
  _settings.setData(Label, _indicator);
  _settings.setData(PhaseLabel, "PHASE");
  _settings.setData(QuadLabel, "QUAD");
  _settings.setData(SineLabel, "SINE");
  _settings.setData(LeadLabel, "LEAD");
  _settings.setData(Input, "Close");
}

int HT::getIndicator (Indicator &ind, BarData &data)
{
  QString s;
  _settings.getData(Input, s);
  PlotLine *in = data.getInput(data.getInputType(s));
  if (! in)
  {
    qDebug() << _indicator << "::calculate: input not found" << s;
    return 1;
  }

  FunctionHT f;
  QStringList methodList = f.list();
  
  _settings.getData(Method, s);
  int method = methodList.indexOf(s);

  switch ((FunctionHT::Method) method)
  {
    case FunctionHT::_PHASOR:
    {
      QList<PlotLine *> pl;
      if (f.getPHASE(in, pl))
      {
        delete in;
        return 1;
      }

      PlotLine *line = pl.at(0);

      _settings.getData(PhasePlot, s);
      line->setType(s);

      _settings.getData(PhaseColor, s);
      line->setColor(s);

      _settings.getData(PhaseLabel, s);
      line->setLabel(s);

      s = "0";
      ind.setLine(s, line);
      ind.addPlotOrder(s);

      line = pl.at(1);

      _settings.getData(QuadPlot, s);
      line->setType(s);

      _settings.getData(QuadColor, s);
      line->setColor(s);

      _settings.getData(QuadLabel, s);
      line->setLabel(s);
      
      s = "1";
      ind.setLine(s, line);
      ind.addPlotOrder(s);
      break;
    }
    case FunctionHT::_SINE:
    {
      QList<PlotLine *> pl;
      if (f.getSINE(in, pl))
      {
        delete in;
        return 1;
      }
      
      PlotLine *line = pl.at(0);

      _settings.getData(SinePlot, s);
      line->setType(s);

      _settings.getData(SineColor, s);
      line->setColor(s);

      _settings.getData(SineLabel, s);
      line->setLabel(s);
      
      s = "2";
      ind.setLine(s, line);
      ind.addPlotOrder(s);

      line = pl.at(1);

      _settings.getData(LeadPlot, s);
      line->setType(s);

      _settings.getData(LeadColor, s);
      line->setColor(s);

      _settings.getData(LeadLabel, s);
      line->setLabel(s);
      
      s = "3";
      ind.setLine(s, line);
      ind.addPlotOrder(s);
      break;
    }
    default:
    {
      QColor up("green");
      QColor down("red");
      QColor neutral("blue");
      BARSUtils b;
      PlotLine *bars = b.getBARS(data, up, down, neutral);
      if (bars)
      {
        s = "4";
        ind.setLine(s, bars);
        ind.addPlotOrder(s);
      }

      PlotLine *line = f.getHT(in, method);
      if (! line)
      {
        delete in;
        return 1;
      }

      _settings.getData(Plot, s);
      line->setType(s);

      _settings.getData(Color, s);
      line->setColor(s);

      _settings.getData(Label, s);
      line->setLabel(s);
      
      s = "5";
      ind.setLine(s, line);
      ind.addPlotOrder(s);
      break;
    }
  }

  delete in;
  return 0;
}

int HT::getCUS (QStringList &set, Indicator &ind, BarData &data)
{
  FunctionHT f;
  return f.script(set, ind, data);
}

int HT::dialog (int)
{
  int page = 0;
  QString k, d;
  PrefDialog *dialog = new PrefDialog;
  dialog->setWindowTitle(QObject::tr("Edit Indicator"));

  k = QObject::tr("Settings");
  dialog->addPage(page, k);

  _settings.getData(Color, d);
  dialog->addColorItem(Color, page, QObject::tr("Color"), d);

  PlotStyleFactory fac;
  QStringList plotList;
  fac.list(plotList, TRUE);

  _settings.getData(Plot, d);
  dialog->addComboItem(Plot, page, QObject::tr("Plot"), plotList, d);

  _settings.getData(Label, d);
  dialog->addTextItem(Label, page, QObject::tr("Label"), d, QString());

  BarData bd;
  QStringList inputList;
  bd.getInputFields(inputList);

  _settings.getData(Input, d);
  dialog->addComboItem(Input, page, QObject::tr("Input"), inputList, d);

  FunctionHT f;
  QStringList methodList = f.list();
  
  _settings.getData(Method, d);
  dialog->addComboItem(Method, page, QObject::tr("Method"), methodList, d);

  page++;
  k = QObject::tr("Phasor");
  dialog->addPage(page, k);

  _settings.getData(PhaseColor, d);
  dialog->addColorItem(PhaseColor, page, QObject::tr("Phase Color"), d);

  _settings.getData(QuadColor, d);
  dialog->addColorItem(QuadColor, page, QObject::tr("Quad Color"), d);

  _settings.getData(PhasePlot, d);
  dialog->addComboItem(PhasePlot, page, QObject::tr("Phase Plot"), plotList, d);

  _settings.getData(QuadPlot, d);
  dialog->addComboItem(QuadPlot, page, QObject::tr("Quad Plot"), plotList, d);

  _settings.getData(PhaseLabel, d);
  dialog->addTextItem(PhaseLabel, page, QObject::tr("Phase Label"), d, QString());

  _settings.getData(QuadLabel, d);
  dialog->addTextItem(QuadLabel, page, QObject::tr("Quad Label"), d, QString());

  page++;
  k = QObject::tr("Sine Wave");
  dialog->addPage(page, k);

  _settings.getData(SineColor, d);
  dialog->addColorItem(SineColor, page, QObject::tr("Sine Color"), d);

  _settings.getData(LeadColor, d);
  dialog->addColorItem(LeadColor, page, QObject::tr("Lead Color"), d);

  _settings.getData(SinePlot, d);
  dialog->addComboItem(SinePlot, page, QObject::tr("Sine Plot"), plotList, d);

  _settings.getData(LeadPlot, d);
  dialog->addComboItem(LeadPlot, page, QObject::tr("Lead Plot"), plotList, d);

  _settings.getData(SineLabel, d);
  dialog->addTextItem(SineLabel, page, QObject::tr("Sine Label"), d, QString());

  _settings.getData(LeadLabel, d);
  dialog->addTextItem(LeadLabel, page, QObject::tr("Lead Label"), d, QString());

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


