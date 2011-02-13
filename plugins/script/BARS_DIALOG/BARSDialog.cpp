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

#include "BARSDialog.h"
#include "FunctionMA.h"
#include "Curve.h"
#include "Globals.h"
#include "Doc.h"
#include "IndicatorScriptFile.h"

#include <QtDebug>
#include <QDialogButtonBox>
#include <QLayout>
#include <QStringList>
#include <QSettings>
#include <QLabel>
#include <QFormLayout>
#include <QFile>
#include <QTextStream>

BARSDialog::BARSDialog (Command *c)
{
  _command = c;
  _helpFile = "main.html";

  _name = _command->parm(1);

  QStringList l;
  l << "QtStalker" << g_session << ":" << _name << tr("Indicator");
  setWindowTitle(l.join(" "));

  createGUI();
  createBARSPage();
  createMA1Page();
  createMA2Page();
  createMA3Page();

  loadSettings();

  setDialogSettings();

  connect(this, SIGNAL(finished(int)), this, SLOT(deleteLater()));
}

void BARSDialog::createGUI ()
{
  QVBoxLayout *vbox = new QVBoxLayout;
  vbox->setSpacing(2);
  setLayout(vbox);

  _tabs = new QTabWidget;
  vbox->addWidget(_tabs);

  // buttonbox
  QDialogButtonBox *bbox = new QDialogButtonBox(QDialogButtonBox::Help);
  connect(bbox, SIGNAL(accepted()), this, SLOT(done()));
  connect(bbox, SIGNAL(rejected()), this, SLOT(cancel()));
  vbox->addWidget(bbox);

  // ok button
  _okButton = bbox->addButton(QDialogButtonBox::Ok);
  _okButton->setDefault(TRUE);

  // cancel button
  _cancelButton = bbox->addButton(QDialogButtonBox::Cancel);
  _cancelButton->setDefault(TRUE);

  // help button
  QPushButton *b = bbox->button(QDialogButtonBox::Help);
  connect(b, SIGNAL(clicked()), this, SLOT(help()));
}

void BARSDialog::createBARSPage ()
{
  QWidget *w = new QWidget;
  
  QFormLayout *form = new QFormLayout;
  form->setSpacing(2);
  form->setMargin(5);
  w->setLayout(form);

  // up color
  _upColor = new ColorButton(this, QColor(Qt::green));
  form->addRow(tr("Up Color"), _upColor);

  // down color
  _downColor = new ColorButton(this, QColor(Qt::red));
  form->addRow(tr("Down Color"), _downColor);

  // neutral color
  _neutralColor = new ColorButton(this, QColor(Qt::blue));
  form->addRow(tr("Neutral Color"), _neutralColor);

  // label
  _barsLabel = new LineEdit;
  _barsLabel->setText("BARS");
  form->addRow(tr("Label"), _barsLabel);

  _tabs->addTab(w, tr("BARS"));
}

void BARSDialog::createMA1Page ()
{
  QWidget *w = new QWidget;

  QFormLayout *form = new QFormLayout;
  form->setSpacing(2);
  form->setMargin(5);
  w->setLayout(form);

  // color
  _ma1Color = new ColorButton(this, QColor(Qt::red));
  form->addRow(tr("Color"), _ma1Color);

  // plot style
  Curve curve;
  QStringList l;
  curve.list(l, 1);

  _ma1PlotStyle = new QComboBox;
  _ma1PlotStyle->addItems(l);
  _ma1PlotStyle->setCurrentIndex(_ma1PlotStyle->findText("Line", Qt::MatchExactly));
  form->addRow(tr("Plot"), _ma1PlotStyle);

  // label
  _ma1Label = new LineEdit;
  _ma1Label->setText("BARS_MA1");
  form->addRow(tr("Label"), _ma1Label);

  // period
  _ma1Period = new QSpinBox;
  _ma1Period->setRange(1, 100000);
  _ma1Period->setValue(10);
  form->addRow(tr("Period"), _ma1Period);

  // ma type
  FunctionMA mau;
  l = mau.list();

  _ma1Type = new QComboBox;
  _ma1Type->addItems(l);
  _ma1Type->setCurrentIndex(_ma1Type->findText("EMA", Qt::MatchExactly));
  form->addRow(tr("Type"), _ma1Type);

  _tabs->addTab(w, "MA1");
}

void BARSDialog::createMA2Page ()
{
  QWidget *w = new QWidget;

  QFormLayout *form = new QFormLayout;
  form->setSpacing(2);
  form->setMargin(5);
  w->setLayout(form);

  // color
  _ma2Color = new ColorButton(this, QColor(Qt::yellow));
  form->addRow(tr("Color"), _ma2Color);

  // plot style
  Curve curve;
  QStringList l;
  curve.list(l, 1);

  _ma2PlotStyle = new QComboBox;
  _ma2PlotStyle->addItems(l);
  _ma2PlotStyle->setCurrentIndex(_ma2PlotStyle->findText("Line", Qt::MatchExactly));
  form->addRow(tr("Plot"), _ma2PlotStyle);

  // label
  _ma2Label = new LineEdit;
  _ma2Label->setText("BARS_MA2");
  form->addRow(tr("Label"), _ma2Label);

  // period
  _ma2Period = new QSpinBox;
  _ma2Period->setRange(1, 100000);
  _ma2Period->setValue(50);
  form->addRow(tr("Period"), _ma2Period);

  // ma type
  FunctionMA mau;
  l = mau.list();

  _ma2Type = new QComboBox;
  _ma2Type->addItems(l);
  _ma2Type->setCurrentIndex(_ma2Type->findText("EMA", Qt::MatchExactly));
  form->addRow(tr("Type"), _ma2Type);

  _tabs->addTab(w, "MA2");
}

void BARSDialog::createMA3Page ()
{
  QWidget *w = new QWidget;

  QFormLayout *form = new QFormLayout;
  form->setSpacing(2);
  form->setMargin(5);
  w->setLayout(form);

  // color
  _ma3Color = new ColorButton(this, QColor(Qt::blue));
  form->addRow(tr("Color"), _ma3Color);

  // plot style
  Curve curve;
  QStringList l;
  curve.list(l, 1);

  _ma3PlotStyle = new QComboBox;
  _ma3PlotStyle->addItems(l);
  _ma3PlotStyle->setCurrentIndex(_ma3PlotStyle->findText("Line", Qt::MatchExactly));
  form->addRow(tr("Plot"), _ma3PlotStyle);

  // label
  _ma3Label = new LineEdit;
  _ma3Label->setText("BARS_MA3");
  form->addRow(tr("Label"), _ma3Label);

  // period
  _ma3Period = new QSpinBox;
  _ma3Period->setRange(1, 100000);
  _ma3Period->setValue(200);
  form->addRow(tr("Period"), _ma3Period);

  // ma type
  FunctionMA mau;
  l = mau.list();

  _ma3Type = new QComboBox;
  _ma3Type->addItems(l);
  _ma3Type->setCurrentIndex(_ma3Type->findText("EMA", Qt::MatchExactly));
  form->addRow(tr("Type"), _ma3Type);

  _tabs->addTab(w, "MA3");
}

void BARSDialog::help ()
{
  Doc *doc = new Doc;
  doc->showDocumentation(_helpFile);
}

void BARSDialog::done ()
{
  QSettings settings(g_settingsFile);
  QString path = settings.value("indicator_script_directory").toString() + _name;

  IndicatorScriptFile f;
  if (f.open(path))
    return;

  QString name = _barsLabel->text();

  QStringList l;
  l << "BARS" << "BARS" << name;
  f.append(l.join(","));

  l.clear();
  l << "INDICATOR_PLOT_COLOR" << "COMPARE_INDEX_ALL" << name + ".0" << "<" << name + ".1" << name + ".0" << _downColor->color().name();
  f.append(l.join(","));
  
  l.clear();
  l << "INDICATOR_PLOT_COLOR" << "COMPARE_INDEX_ALL" << name + ".0" << "=" << name + ".1" << name + ".0" << _neutralColor->color().name();
  f.append(l.join(","));

  l.clear();
  l << "INDICATOR_PLOT_COLOR" << "COMPARE_INDEX_ALL" << name + ".0" << ">" << name + ".1" << name + ".0" << _upColor->color().name();
  f.append(l.join(","));

  l.clear();
  l << "MA" << _ma1Type->currentText() << _ma1Label->text() << name << _ma1Period->text();
  f.append(l.join(","));

  l.clear();
  l << "MA" << _ma2Type->currentText() << _ma2Label->text() << name << _ma2Period->text();
  f.append(l.join(","));

  l.clear();
  l << "MA" << _ma3Type->currentText() << _ma3Label->text() << name << _ma3Period->text();
  f.append(l.join(","));

  l.clear();
  l << "INDICATOR_PLOT" << name << "0";
  f.append(l.join(","));

  l.clear();
  l << "INDICATOR_PLOT_STYLE" << _ma1Label->text() << _ma1PlotStyle->currentText();
  f.append(l.join(","));

  l.clear();
  l << "INDICATOR_PLOT_COLOR" << "ALL" << _ma1Label->text() << _ma1Color->color().name();
  f.append(l.join(","));

  l.clear();
  l << "INDICATOR_PLOT" << _ma1Label->text() << "1";
  f.append(l.join(","));

  l.clear();
  l << "INDICATOR_PLOT_STYLE" << _ma2Label->text() << _ma2PlotStyle->currentText();
  f.append(l.join(","));

  l.clear();
  l << "INDICATOR_PLOT_COLOR" << "ALL" << _ma2Label->text() << _ma2Color->color().name();
  f.append(l.join(","));

  l.clear();
  l << "INDICATOR_PLOT" << _ma2Label->text() << "2";
  f.append(l.join(","));

  l.clear();
  l << "INDICATOR_PLOT_STYLE" << _ma3Label->text() << _ma3PlotStyle->currentText();
  f.append(l.join(","));

  l.clear();
  l << "INDICATOR_PLOT_COLOR" << "ALL" << _ma3Label->text() << _ma3Color->color().name();
  f.append(l.join(","));

  l.clear();
  l << "INDICATOR_PLOT" << _ma3Label->text() << "3";
  f.append(l.join(","));

  f.close();
  
  Indicator i;
  i.setName(_name);
  i.load();
  i.setCommand("perl");
  i.setScript(path);
  i.setDialog("BARS");
  QString s;
  dialogSettings(s);
  i.setDialogSettings(s);
  i.save();
  
  _command->setReturnCode("0");
  
  saveSettings();
  
  accept();
}

void BARSDialog::cancel ()
{
  saveSettings();
  reject();
}

void BARSDialog::loadSettings ()
{
  QSettings settings(g_settingsFile);

  QSize sz = settings.value("BARS_dialog_window_size", QSize(200,150)).toSize();
  resize(sz);

  // restore the position of the app
  QPoint p = settings.value("BARS_dialog_window_position", QPoint(0,0)).toPoint();
  move(p);
}

void BARSDialog::saveSettings ()
{
  QSettings settings(g_settingsFile);
  settings.setValue("BARS_dialog_window_size", size());
  settings.setValue("BARS_dialog_window_position", pos());
  settings.sync();
}

void BARSDialog::setDialogSettings ()
{
  Indicator i;
  i.setName(_name);
  i.load();

  QStringList l = i.dialogSettings().split(",", QString::SkipEmptyParts);
  if (! l.count())
    return;

  int pos = 0;
  _upColor->setColor(l.at(pos++));
  _downColor->setColor(l.at(pos++));
  _neutralColor->setColor(l.at(pos++));
  _ma1Color->setColor(l.at(pos++));
  _ma2Color->setColor(l.at(pos++));
  _ma3Color->setColor(l.at(pos++));
  _ma1Type->setCurrentIndex(_ma1Type->findText(l.at(pos++)));
  _ma2Type->setCurrentIndex(_ma2Type->findText(l.at(pos++)));
  _ma3Type->setCurrentIndex(_ma3Type->findText(l.at(pos++)));
  _ma1PlotStyle->setCurrentIndex(_ma1PlotStyle->findText(l.at(pos++)));
  _ma2PlotStyle->setCurrentIndex(_ma2PlotStyle->findText(l.at(pos++)));
  _ma3PlotStyle->setCurrentIndex(_ma3PlotStyle->findText(l.at(pos++)));
  _ma1Period->setValue(l.at(pos++).toInt());
  _ma2Period->setValue(l.at(pos++).toInt());
  _ma3Period->setValue(l.at(pos++).toInt());
  _barsLabel->setText(l.at(pos++));
  _ma1Label->setText(l.at(pos++));
  _ma2Label->setText(l.at(pos++));
  _ma3Label->setText(l.at(pos++));
}

void BARSDialog::dialogSettings (QString &d)
{
  QStringList l;
  l << _upColor->color().name() << _downColor->color().name() << _neutralColor->color().name();
  l << _ma1Color->color().name() << _ma2Color->color().name() << _ma3Color->color().name();
  l << _ma1Type->currentText() << _ma2Type->currentText() << _ma3Type->currentText();
  l << _ma1PlotStyle->currentText() << _ma2PlotStyle->currentText() << _ma3PlotStyle->currentText();
  l << _ma1Period->text() << _ma2Period->text() << _ma3Period->text();
  l << _barsLabel->text() << _ma1Label->text() << _ma2Label->text() << _ma3Label->text();
  d = l.join(",");
}
