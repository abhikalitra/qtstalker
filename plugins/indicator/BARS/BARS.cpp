/*
 *  Qtstalker stock charter
 *
 *  Copyright (C) 2001-2006 Stefan S. Stratigakos
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
#include "DBBase.h"
#include "Config.h"
#include <qdict.h>
#include <qobject.h>
#include <qinputdialog.h>
#include <math.h>
#include <qfileinfo.h>


BARS::BARS ()
{
  pluginName = "BARS";
  helpFile = "bars.html";

  methodList.append("Bar");
  methodList.append("Candle");
  methodList.append("PF");

  Config config;
  config.getData(Config::Home, dbPath);
  dbPath.append("/index/pf.db");

  barUpColorLabel = "barUpColor";
  barDownColorLabel = "barDownColor";
  barNeutralColorLabel = "barNeutralColor";
  candleColorLabel = "candleColor";
  labelLabel = "label";
  methodLabel = "method";
  lineTypeLabel = "lineType";
  pluginLabel = "plugin";

  pfXColorLabel = "pfXColor";
  pfOColorLabel = "pfOColor";
  pfReversalLabel = "pfReversal";
  pfMethodLabel = "pfMethod";

  maColorLabel = "maColor";
  maLineTypeLabel = "maLineType";
  maPeriodLabel = "maPeriod";
  maLabelLabel = "maLabel";
  maTypeLabel = "maType";
  maInputLabel = "maInput";

  maColor2Label = "maColor2";
  maLineType2Label = "maLineType2";
  maPeriod2Label = "maPeriod2";
  maLabel2Label = "maLabel2";
  maType2Label = "maType2";
  maInput2Label = "maInput2";

  maColor3Label = "maColor3";
  maLineType3Label = "maLineType3";
  maPeriod3Label = "maPeriod3";
  maLabel3Label = "maLabel3";
  maType3Label = "maType3";
  maInput3Label = "maInput3";

  formatList.append(FormatString);

  setDefaults();
}

BARS::~BARS ()
{
}

void BARS::setDefaults ()
{
  barUpColor.setNamedColor("green");
  barDownColor.setNamedColor("red");
  barNeutralColor.setNamedColor("blue");
  candleColor.setNamedColor("green");
  label = pluginName;

  pfXColor.setNamedColor("green");
  pfOColor.setNamedColor("red");
  pfReversal = 3;
  pfBoxSize = 1;
  pfMethod = QObject::tr("Default");

  maColor.setNamedColor("red");
  maColor2.setNamedColor("red");
  maColor3.setNamedColor("yellow");
  maLineType = PlotLine::Line;
  maLineType2 = PlotLine::Line;
  maLineType3 = PlotLine::Line;
  maLabel = "10MA";
  maLabel2 = "50MA";
  maLabel3 = "200MA";
  maPeriod = 10;
  maPeriod2 = 50;
  maPeriod3 = 200;
  maType = 0;
  maType2 = 0;
  maType3 = 0;
  maInput = BarData::Close;
  maInput2 = BarData::Close;
  maInput3 = BarData::Close;
}

void BARS::calculate ()
{
  if (! method.compare("Bar"))
  {
    calculateBar();
    return;
  }

  if (! method.compare("Candle"))
  {
    calculateCandle();
    return;
  }

  if (! method.compare("PF"))
  {
    calculatePF();
    return;
  }
}

void BARS::calculateBar ()
{
  PlotLine *line = new PlotLine;
  int loop;
  QColor color;

  for (loop = 0; loop < (int) data->count(); loop++)
  {
    if (loop > 0)
    {  
      if (data->getClose(loop) > data->getClose(loop - 1))
        color = barUpColor;
      else
      {
        if (data->getClose(loop) < data->getClose(loop - 1))
          color = barDownColor;
        else
          color = barNeutralColor;
      }
    }
    else
      color = barNeutralColor;

    line->append(color, data->getOpen(loop), data->getHigh(loop), data->getLow(loop),
                 data->getClose(loop), FALSE);

    QDateTime dt;
    data->getDate(loop, dt);
    line->append(dt);
  }

  line->setType(PlotLine::Bar);
  line->setLabel(label);
  output->addLine(line);

  calculateMA();
}

void BARS::calculateCandle ()
{
  PlotLine *line = new PlotLine;
  int loop;
  for (loop = 0; loop < (int) data->count(); loop++)
  {
    double c = data->getClose(loop);
    double o = data->getOpen(loop);
    bool fillFlag = FALSE;

    if (o != 0)
    {
      if (c < o)
        fillFlag = TRUE;
    }

    line->append(candleColor, o, data->getHigh(loop), data->getLow(loop), c, fillFlag);

    QDateTime dt;
    data->getDate(loop, dt);
    line->append(dt);
  }

  line->setType(PlotLine::Candle);
  line->setLabel(label);
  output->addLine(line);

  calculateMA();
}

void BARS::calculatePF ()
{
  // determine start either x or o
  if (data->count() < 2)
    return;

  getPFSettings();

  bool XOFlag = FALSE;
  if (data->getHigh(1) > data->getHigh(0))
    XOFlag = TRUE; // prices rising, we start with x's

  double high = 0;
  double d = data->getHigh(0) /  pfBoxSize;
  int t = (int) d;
  if (t * pfBoxSize <= data->getHigh(0))
    high = (t + 1) * pfBoxSize;
  else
    high = t * pfBoxSize;

  double low = 0;
  t = (int) (data->getLow(0) / pfBoxSize);
  low = t * pfBoxSize;

  PlotLine *line = new PlotLine;
  int loop;
  for (loop = 1; loop < (int) data->count(); loop++)
  {
    if (XOFlag)
    {
      if (data->getHigh(loop) > high)
      {
        // new high
        d = data->getHigh(loop) /  pfBoxSize;
        t = (int) d;
        if (t * pfBoxSize <= data->getHigh(loop))
          high = (t + 1) * pfBoxSize;
        else
          high = t * pfBoxSize;
      }

      double reversal = high - (pfBoxSize * pfReversal);
      if (data->getLow(loop) < reversal)
      {
        // reversal to O's
        line->append(pfXColor, pfBoxSize, high, low, low, XOFlag);

        high = high - pfBoxSize; // lower high 1 box

        t = (int) (data->getLow(loop) / pfBoxSize);
        low = t * pfBoxSize;

        XOFlag = FALSE;
      }
    }
    else
    {
      if (data->getLow(loop) < low)
      {
        // new low
        t = (int) (data->getLow(loop) / pfBoxSize);
        low = t * pfBoxSize;
      }

      double reversal = low + (pfBoxSize * pfReversal);
      if (data->getHigh(loop) > reversal)
      {
        // reversal to X's
        line->append(pfOColor, pfBoxSize, high, low, low, XOFlag);

        low = low + pfBoxSize; // raise low 1 box

        d = data->getHigh(loop) /  pfBoxSize;
        t = (int) d;
        if (t * pfBoxSize <= data->getHigh(loop))
          high = (t + 1) * pfBoxSize;
        else
          high = t * pfBoxSize;

        XOFlag = TRUE;
      }
    }
  }

  if (XOFlag)
    line->append(pfXColor, pfBoxSize, high, low, low, XOFlag);
  else
    line->append(pfOColor, pfBoxSize, high, low, low, XOFlag);

  line->setType(PlotLine::PF);
  line->setLabel(label);
  output->addLine(line);
}

void BARS::getPFSettings ()
{
  // set default traditional
  pfBoxSize = (((data->getMax() - data->getMin()) / 2) + data->getMin()) * 0.02;

  if (! pfMethod.compare(QObject::tr("Default")))
    return;

  QString s;
  Config config;
  config.getData(Config::CurrentChart, s);
  QFileInfo fi(s);
  s = fi.fileName();

  DBBase db;
  if (db.open(dbPath))
  {
    qDebug("BARS::getPFBoxSize: error opening %s", dbPath.latin1());
    return;
  }

  QString s2;
  db.getData(s, s2);
  if (! s2.length())
    return;

  QStringList l = QStringList::split(",", s2, FALSE);
  pfBoxSize = l[0].toDouble();
  pfReversal = l[1].toInt();
  db.close();
}

void BARS::calculateMA ()
{
  if (maPeriod > 1)
  {
    PlotLine *in = data->getInput(maInput);
    if (in)
    {
      PlotLine *ma = getMA(in, maType, maPeriod);
      ma->setColor(maColor);
      ma->setType(maLineType);
      ma->setLabel(maLabel);
      output->addLine(ma);
      delete in;
    }
  }

  if (maPeriod2 > 1)
  {
    PlotLine *in = data->getInput(maInput2);
    if (in)
    {
      PlotLine *ma = getMA(in, maType2, maPeriod2);
      ma->setColor(maColor2);
      ma->setType(maLineType2);
      ma->setLabel(maLabel2);
      output->addLine(ma);
      delete in;
    }
  }

  if (maPeriod3 > 1)
  {
    PlotLine *in = data->getInput(maInput3);
    if (in)
    {
      PlotLine *ma = getMA(in, maType3, maPeriod3);
      ma->setColor(maColor3);
      ma->setType(maLineType3);
      ma->setLabel(maLabel3);
      output->addLine(ma);
      delete in;
    }
  }
}

int BARS::indicatorPrefDialog (QWidget *w)
{
  bool newFlag = FALSE;
  if (! method.length())
    newFlag = TRUE;

  if (newFlag)
  {
    bool ok;
    QString s = QInputDialog::getItem(QObject::tr("BARS type Selection"),
                                      QObject::tr("Select a bar type:"),
                                      methodList,
                                      0,
                                      TRUE,
                                      &ok,
                                      w);
    if (ok)
    {
      method = s;
      label = method;
    }
    else
      return FALSE;
  }

  QString pl = QObject::tr("Parms");
  QString ucl = QObject::tr("Up Color");
  QString dcl = QObject::tr("Down Color");
  QString ncl = QObject::tr("Neutral Color");
  QString ccl = QObject::tr("Candle Color");
  QString ll = QObject::tr("Label");

  QString pfxcl = QObject::tr("X Color");
  QString pfocl = QObject::tr("O Color");
  QString pfrl = QObject::tr("Reversal");
  QString pfbsl = QObject::tr("Box Size");
  QString pfml = QObject::tr("Method");

  QString pl2 = QObject::tr("MA");
  QString macl = QObject::tr("MA Color");
  QString mall = QObject::tr("MA Label");
  QString maltl = QObject::tr("MA Line Type");
  QString mapl = QObject::tr("MA Period");
  QString matl = QObject::tr("MA Type");
  QString mail = QObject::tr("MA Input");

  QString pl3 = QObject::tr("MA2");
  QString ma2cl = QObject::tr("MA2 Color");
  QString ma2ll = QObject::tr("MA2 Label");
  QString ma2ltl = QObject::tr("MA2 Line Type");
  QString ma2pl = QObject::tr("MA2 Period");
  QString ma2tl = QObject::tr("MA2 Type");
  QString ma2il = QObject::tr("MA2 Input");

  QString pl4 = QObject::tr("MA3");
  QString ma3cl = QObject::tr("MA3 Color");
  QString ma3ll = QObject::tr("MA3 Label");
  QString ma3ltl = QObject::tr("MA3 Line Type");
  QString ma3pl = QObject::tr("MA3 Period");
  QString ma3tl = QObject::tr("MA3 Type");
  QString ma3il = QObject::tr("MA3 Input");

  PrefDialog *dialog = new PrefDialog(w);
  dialog->setCaption(QObject::tr("BARS Indicator"));
  dialog->createPage (pl);
  dialog->setHelpFile(helpFile);

  if (! method.compare("Bar"))
  {
    dialog->setCaption(QObject::tr("Bar Indicator"));
    dialog->addColorItem(ucl, pl, barUpColor);
    dialog->addColorItem(dcl, pl, barDownColor);
    dialog->addColorItem(ncl, pl, barNeutralColor);
  }

  if (! method.compare("Candle"))
  {
    dialog->setCaption(QObject::tr("Candle Indicator"));
    dialog->addColorItem(ccl, pl, candleColor);
  }

  if (! method.compare("PF"))
  {
    dialog->setCaption(QObject::tr("P&F Indicator"));

    QStringList l;
    l.append(QObject::tr("Default"));
    l.append(QObject::tr("Custom"));
    dialog->addComboItem(pfml, pl, l, pfMethod);

    dialog->addColorItem(pfxcl, pl, pfXColor);
    dialog->addColorItem(pfocl, pl, pfOColor);

    double bs = pfBoxSize;
    int rv = pfReversal;

    if (! pfMethod.compare(QObject::tr("Custom")))
    {
      QString s;
      Config config;
      config.getData(Config::CurrentChart, s);
      QFileInfo fi(s);
      s = fi.fileName();

      DBBase db;
      if (! db.open(dbPath))
      {
        QString s2;
        db.getData(s, s2);
        if (s2.length())
        {
          QStringList l = QStringList::split(",", s2, FALSE);
          bs = l[0].toDouble();
          rv = l[1].toInt();
        }
        db.close();
      }
    }

    dialog->addDoubleItem(pfbsl, pl, bs, 0.0001, 999999.0);
    dialog->addIntItem(pfrl, pl, rv, 1, 99);
  }

  dialog->addTextItem(ll, pl, label);

  QStringList mal;
  getMATypes(mal);

  if (! method.compare("Bar") || ! method.compare("Candle"))
  { 
    dialog->createPage (pl2);
    dialog->addColorItem(macl, pl2, maColor);
    dialog->addTextItem(mall, pl2, maLabel);
    dialog->addComboItem(maltl, pl2, lineTypes, maLineType);
    dialog->addComboItem(matl, pl2, mal, maType);
    dialog->addIntItem(mapl, pl2, maPeriod, 1, 999999);
    dialog->addComboItem(mail, pl2, inputTypeList, maInput);

    dialog->createPage (pl3);
    dialog->addColorItem(ma2cl, pl3, maColor2);
    dialog->addTextItem(ma2ll, pl3, maLabel2);
    dialog->addComboItem(ma2ltl, pl3, lineTypes, maLineType2);
    dialog->addComboItem(ma2tl, pl3, mal, maType2);
    dialog->addIntItem(ma2pl, pl3, maPeriod2, 1, 999999);
    dialog->addComboItem(ma2il, pl3, inputTypeList, maInput2);

    dialog->createPage (pl4);
    dialog->addColorItem(ma3cl, pl4, maColor3);
    dialog->addTextItem(ma3ll, pl4, maLabel3);
    dialog->addComboItem(ma3ltl, pl4, lineTypes, maLineType3);
    dialog->addComboItem(ma3tl, pl4, mal, maType3);
    dialog->addIntItem(ma3pl, pl4, maPeriod3, 1, 999999);
    dialog->addComboItem(ma3il, pl4, inputTypeList, maInput3);
  }
 
  int rc = dialog->exec();
  
  if (rc == QDialog::Accepted)
  {
    if (! method.compare("Bar"))
    {
      dialog->getColor(ucl, barUpColor);
      dialog->getColor(dcl, barDownColor);
      dialog->getColor(ncl, barNeutralColor);
      lineType = PlotLine::Bar;
    }

    if (! method.compare("Candle"))
    {
      dialog->getColor(ccl, candleColor);
      lineType = PlotLine::Candle;
    }

    if (! method.compare("PF"))
    {
      dialog->getCombo(pfml, pfMethod);
      dialog->getColor(pfxcl, pfXColor);
      dialog->getColor(pfocl, pfOColor);
      pfReversal = dialog->getInt(pfrl);

      if (! pfMethod.compare(QObject::tr("Custom")) && data)
      {
        double d = dialog->getDouble(pfbsl);
        QString s;
        Config config;
        config.getData(Config::CurrentChart, s);
        QFileInfo fi(s);
        s = fi.fileName();
        DBBase db;
        if (! db.open(dbPath))
        {
          QString s2 = QString::number(d) + "," + QString::number(pfReversal);
          db.setData(s, s2);
          db.close();
        }
      }

      lineType = PlotLine::PF;
    }

    dialog->getText(ll, label);

    if (! method.compare("Bar") || ! method.compare("Candle"))
    { 
      dialog->getColor(macl, maColor);
      maLineType = (PlotLine::LineType) dialog->getComboIndex(maltl);
      maPeriod = dialog->getInt(mapl);
      dialog->getText(mall, maLabel);
      maType = dialog->getComboIndex(matl);
      maInput = (BarData::InputType) dialog->getComboIndex(mail);

      dialog->getColor(ma2cl, maColor2);
      maLineType2 = (PlotLine::LineType) dialog->getComboIndex(ma2ltl);
      maPeriod2 = dialog->getInt(ma2pl);
      dialog->getText(ma2ll, maLabel2);
      maType2 = dialog->getComboIndex(ma2tl);
      maInput2 = (BarData::InputType) dialog->getComboIndex(ma2il);

      dialog->getColor(ma3cl, maColor3);
      maLineType3 = (PlotLine::LineType) dialog->getComboIndex(ma3ltl);
      maPeriod3 = dialog->getInt(ma3pl);
      dialog->getText(ma3ll, maLabel3);
      maType3 = dialog->getComboIndex(ma3tl);
      maInput3 = (BarData::InputType) dialog->getComboIndex(ma3il);
    }

    rc = TRUE;
  }
  else
    rc = FALSE;
  
  delete dialog;
  return rc;
}

void BARS::setIndicatorSettings (Setting &dict)
{
  setDefaults();
  
  if (! dict.count())
    return;

  QString s;
  dict.getData(labelLabel, s);
  if (s.length())
    label = s;
  
  dict.getData(lineTypeLabel, s);
  if (s.length())
    lineType = (PlotLine::LineType) s.toInt();

  dict.getData(methodLabel, s);
  if (s.length())
    method = s;

  dict.getData(barUpColorLabel, s);
  if (s.length())
    barUpColor.setNamedColor(s);
    
  dict.getData(barDownColorLabel, s);
  if (s.length())
    barDownColor.setNamedColor(s);
    
  dict.getData(barNeutralColorLabel, s);
  if (s.length())
    barNeutralColor.setNamedColor(s);

  dict.getData(candleColorLabel, s);
  if (s.length())
    candleColor.setNamedColor(s);

  dict.getData(maColorLabel, s);
  if (s.length())
    maColor.setNamedColor(s);
    
  dict.getData(maLineTypeLabel, s);
  if (s.length())
    maLineType = (PlotLine::LineType) s.toInt();

  dict.getData(maPeriodLabel, s);
  if (s.length())
    maPeriod = s.toInt();

  dict.getData(maLabelLabel, s);
  if (s.length())
    maLabel = s;
      
  dict.getData(maTypeLabel, s);
  if (s.length())
    maType = s.toInt();
    
  dict.getData(maInputLabel, s);
  if (s.length())
    maInput = (BarData::InputType) s.toInt();

  dict.getData(maColor2Label, s);
  if (s.length())
    maColor2.setNamedColor(s);
    
  dict.getData(maLineType2Label, s);
  if (s.length())
    maLineType2 = (PlotLine::LineType) s.toInt();

  dict.getData(maPeriod2Label, s);
  if (s.length())
    maPeriod2 = s.toInt();

  dict.getData(maLabel2Label, s);
  if (s.length())
    maLabel2 = s;
      
  dict.getData(maType2Label, s);
  if (s.length())
    maType2 = s.toInt();
    
  dict.getData(maInput2Label, s);
  if (s.length())
    maInput2 = (BarData::InputType) s.toInt();

  dict.getData(maColor3Label, s);
  if (s.length())
    maColor3.setNamedColor(s);
    
  dict.getData(maLineType3Label, s);
  if (s.length())
    maLineType3 = (PlotLine::LineType) s.toInt();

  dict.getData(maPeriod3Label, s);
  if (s.length())
    maPeriod3 = s.toInt();

  dict.getData(maLabel3Label, s);
  if (s.length())
    maLabel3 = s;
      
  dict.getData(maType3Label, s);
  if (s.length())
    maType3 = s.toInt();
    
  dict.getData(maInput3Label, s);
  if (s.length())
    maInput3 = (BarData::InputType) s.toInt();

  dict.getData(pfXColorLabel, s);
  if (s.length())
    pfXColor.setNamedColor(s);
    
  dict.getData(pfOColorLabel, s);
  if (s.length())
    pfOColor.setNamedColor(s);
    
  dict.getData(pfReversalLabel, s);
  if (s.length())
    pfReversal = s.toInt();

  dict.getData(pfMethodLabel, s);
  if (s.length())
    pfMethod = s;
}

void BARS::getIndicatorSettings (Setting &dict)
{
  QString ts = barUpColor.name();
  dict.setData(barUpColorLabel, ts);
  ts = barDownColor.name();
  dict.setData(barDownColorLabel, ts);
  ts = barNeutralColor.name();
  dict.setData(barNeutralColorLabel, ts);
  ts = candleColor.name();
  dict.setData(candleColorLabel, ts);
  dict.setData(labelLabel, label);
  dict.setData(methodLabel, method);
  ts = QString::number(lineType);
  dict.setData(lineTypeLabel, ts);
  dict.setData(pluginLabel, pluginName);

  ts = pfXColor.name();
  dict.setData(pfXColorLabel, ts);
  ts = pfOColor.name();
  dict.setData(pfOColorLabel, ts);
  ts = QString::number(pfReversal);
  dict.setData(pfReversalLabel, ts);
  dict.setData(pfMethodLabel, pfMethod);

  ts = maColor.name();
  dict.setData(maColorLabel, ts);
  ts = QString::number(maLineType);
  dict.setData(maLineTypeLabel, ts);
  ts = QString::number(maPeriod);
  dict.setData(maPeriodLabel, ts);
  dict.setData(maLabelLabel, maLabel);
  ts = QString::number(maType);
  dict.setData(maTypeLabel, ts);
  ts = QString::number(maInput);
  dict.setData(maInputLabel, ts);

  ts = maColor2.name();
  dict.setData(maColor2Label, ts);
  ts = QString::number(maLineType2);
  dict.setData(maLineType2Label, ts);
  ts = QString::number(maPeriod2);
  dict.setData(maPeriod2Label, ts);
  dict.setData(maLabel2Label, maLabel2);
  ts = QString::number(maType2);
  dict.setData(maType2Label, ts);
  ts = QString::number(maInput2);
  dict.setData(maInput2Label, ts);

  ts = maColor3.name();
  dict.setData(maColor3Label, ts);
  ts = QString::number(maLineType3);
  dict.setData(maLineType3Label, ts);
  ts = QString::number(maPeriod3);
  dict.setData(maPeriod3Label, ts);
  dict.setData(maLabel3Label, maLabel3);
  ts = QString::number(maType3);
  dict.setData(maType3Label, ts);
  ts = QString::number(maInput3);
  dict.setData(maInput3Label, ts);
}

PlotLine * BARS::calculateCustom (QString &p, QPtrList<PlotLine> &d)
{
  // format1 (BARS): TYPE
  // format2 (BARS): TYPE, COLOR
  // format3 (BARS): TYPE, REVERSAL
  // TODO: format4 (BARS): TYPE, REVERSAL, BOXSIZE

  int type = 0; // 0 == bars, 1 == candle, 2 == pf
  formatList.clear();
  QStringList l = QStringList::split(",", p, FALSE);
  if (l.count() == 1)
    formatList.append(FormatString); // OHLC bars
  else
  {
    if (l.count() == 2)
    {
      bool ok;
      formatStringList[1].toInt(&ok);
      if (! ok)
      {
        // candle bars
        formatList.append(FormatString);
        formatList.append(FormatString);
        type = 1;
      }
      else
      {
        // PF bars
        formatList.append(FormatString);
        formatList.append(FormatInteger);
        type = 2;
      }
    }
    else
    {
      qDebug("BARS::calculateCustom: invalid parm count");
      return 0;
    }
  }

  if (checkFormat(p, d, 2, 1))
    return 0;

  method = formatStringList[0];

  if (type == 1)
    candleColor.setNamedColor(formatStringList[1]);

  if (type == 2)
    pfReversal = formatStringList[1].toInt();

  clearOutput();
  calculate();
  return output->getLine(0);
}

void BARS::formatDialog (QStringList &, QString &rv, QString &rs)
{
  rs.truncate(0);
  rv.truncate(0);

  bool ok;
  method = QInputDialog::getItem(QObject::tr("BARS Method Selection"),
                                 QObject::tr("Select a method:"),
                                 methodList,
                                 0,
                                 TRUE,
                                 &ok,
                                 0);
  if (! ok)
    return;

  QString pl = QObject::tr("Parms");
  QString vnl = QObject::tr("Variable Name");
  QString cl = QObject::tr("Color");
  QString rl = QObject::tr("Reversal");
  PrefDialog *dialog = new PrefDialog(0);
  dialog->setCaption(QObject::tr("BARS Format"));
  dialog->createPage (pl);
  dialog->setHelpFile(helpFile);

  QString s;
  dialog->addTextItem(vnl, pl, s);

  if (! method.compare("Candle"))
    dialog->addColorItem(cl, pl, candleColor);

  if (! method.compare("PF"))
    dialog->addIntItem(rl, pl, pfReversal);

  int rc = dialog->exec();

  if (rc == QDialog::Accepted)
  {
    dialog->getText(vnl, rv);
    rs = method;

    if (! method.compare("Candle"))
    {
      dialog->getColor(cl, candleColor);
      rs.append("," + candleColor.name());
    }

    if (! method.compare("PF"))
    {
      pfReversal = dialog->getInt(rl);
      rs.append("," + QString::number(pfReversal));
    }
  }

  delete dialog;
}

//*******************************************************
//*******************************************************
//*******************************************************

IndicatorPlugin * createIndicatorPlugin ()
{
  BARS *o = new BARS;
  return ((IndicatorPlugin *) o);
}




