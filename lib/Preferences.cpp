/*
 *  Qtstalker stock charter
 *
 *  Copyright (C) 2001-2008 Stefan S. Stratigakos
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

#include "Preferences.h"
#include "Config.h"

#include <QLabel>
#include <QLayout>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QtDebug>
#include <QSize>
#include <QColor>
#include <QFont>
#include <QMessageBox>





Preferences::Preferences (QWidget *w) : QDialog (w, 0)
{
  setWindowTitle(tr("Edit Preferences"));

  QVBoxLayout *vbox = new QVBoxLayout;
  setLayout(vbox);

  tabs = new QTabWidget;
  vbox->addWidget(tabs);

  buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel | QDialogButtonBox::Help);
  connect(buttonBox, SIGNAL(accepted()), this, SLOT(slotSave()));
  connect(buttonBox, SIGNAL(rejected()), this, SLOT(cancelPressed()));
  vbox->addWidget(buttonBox);
  
  createGeneralPage();
  createDatabasePage();
  createColorPage();
  createFontPage();
  createMTPage();
  createCTPage();

  Config config;
  QSize sz;
  config.getData(Config::PrefDlgWindowSize, sz);
  resize(sz);

  modified = 0;
}

void Preferences::createGeneralPage ()
{
  // general parms page

  QWidget *w = new QWidget;

  QVBoxLayout *vbox = new QVBoxLayout;
  w->setLayout(vbox);
  
  QGridLayout *grid = new QGridLayout;
  grid->setMargin(5);
  grid->setSpacing(5);
  grid->setColumnStretch(1, 2);
  vbox->addLayout(grid);
  
  vbox->insertStretch(-1, 1);
  
  int row = 0;
  int col = 0;
  QString s;
  Config config;
  
  // bar spacing 1
  QLabel *label = new QLabel(tr("Bar Spacing 1"));
  grid->addWidget(label, row, col++);
  
  bs1Spinner = new QSpinBox;
  bs1Spinner->setRange(2, 99);
  config.getData(Config::PSButton1, s);
  if (! s.isEmpty())
    bs1Spinner->setValue(s.toInt());
  connect(bs1Spinner, SIGNAL(valueChanged(int)), this, SLOT(slotModified()));
  grid->addWidget(bs1Spinner, row++, col--);

  // bar spacing 2
  label = new QLabel(tr("Bar Spacing 2"));
  grid->addWidget(label, row, col++);
  
  bs2Spinner = new QSpinBox;
  bs2Spinner->setRange(2, 99);
  config.getData(Config::PSButton2, s);
  if (! s.isEmpty())
    bs2Spinner->setValue(s.toInt());
  connect(bs2Spinner, SIGNAL(valueChanged(int)), this, SLOT(slotModified()));
  grid->addWidget(bs2Spinner, row++, col--);

  // bar spacing 3
  label = new QLabel(tr("Bar Spacing 3"));
  grid->addWidget(label, row, col++);
  
  bs3Spinner = new QSpinBox;
  bs3Spinner->setRange(2, 99);
  config.getData(Config::PSButton3, s);
  if (! s.isEmpty())
    bs3Spinner->setValue(s.toInt());
  connect(bs3Spinner, SIGNAL(valueChanged(int)), this, SLOT(slotModified()));
  grid->addWidget(bs3Spinner, row++, col--);

  // indicator tab rows
  label = new QLabel(tr("Indicator Tab Rows"));
  grid->addWidget(label, row, col++);
  
  tabRows = new QSpinBox;
  tabRows->setRange(1, 3);
  tabRows->setValue(2);
  config.getData(Config::IndicatorTabRows, s);
  if (! s.isEmpty())
    tabRows->setValue(s.toInt());
  connect(tabRows, SIGNAL(valueChanged(int)), this, SLOT(slotModified()));
  grid->addWidget(tabRows, row++, col--);

  // indicator script default
  label = new QLabel(tr("Indicator Script Default"));
  grid->addWidget(label, row, col++);
  
  indicatorScriptDefault = new QLineEdit;
  config.getData(Config::IndicatorScriptDefault, s);
  indicatorScriptDefault->setText(s);
  connect(indicatorScriptDefault, SIGNAL(textChanged(const QString &)), this, SLOT(slotModified()));
  grid->addWidget(indicatorScriptDefault, row++, col--);

  tabs->addTab(w, tr("General"));
}

void Preferences::createDatabasePage ()
{
  // database parms page

  QWidget *w = new QWidget;

  QHBoxLayout *hbox = new QHBoxLayout;
  w->setLayout(hbox);
  
  QGridLayout *grid = new QGridLayout;
  grid->setMargin(5);
  grid->setSpacing(5);
  grid->setColumnStretch(1, 2);
  hbox->addLayout(grid);
  
  int row = 0;
  int col = 0;
  Config config;

  // setup the db sql driver
  QLabel *label = new QLabel(tr("Qt DB Driver"));
  grid->addWidget(label, row, col++);

  QStringList l;
  l << "QSQLITE" << "QSQLITE2" << "QMYSQL" << "QOCI" << "QODBC" << "QPSQL" << "QTDS" << "QDB2" << "QIBASE";
  dbDriver = new QComboBox;
  dbDriver->addItems(l);
  grid->addWidget(dbDriver, row++, col--);
  QString s;
  config.getData(Config::DbPlugin, s);
  if (s.length())
    dbDriver->setCurrentIndex(dbDriver->findText(s));
  

  // setup the db sql host name
  label = new QLabel(tr("Db Host Name"));
  grid->addWidget(label, row, col++);

  dbHostName = new QLineEdit;
  grid->addWidget(dbHostName, row++, col--);
  config.getData(Config::DbHostName, s);
  if (s.length())
    dbHostName->setText(s);
  
  
  // setup the db name
  label = new QLabel(tr("Db Name"));
  grid->addWidget(label, row, col++);

  dbName = new QLineEdit;
  grid->addWidget(dbName, row++, col--);
  config.getData(Config::DbName, s);
  if (s.length())
    dbName->setText(s);
  
  
  // setup the db user name
  label = new QLabel(tr("Db User Name"));
  grid->addWidget(label, row, col++);

  dbUserName = new QLineEdit;
  grid->addWidget(dbUserName, row++, col--);
  config.getData(Config::DbUserName, s);
  if (s.length())
    dbUserName->setText(s);
  
  
  // setup the db password
  label = new QLabel(tr("Db Password"));
  grid->addWidget(label, row, col++);

  dbPassword = new QLineEdit;
  dbPassword->setEchoMode(QLineEdit::Password);
  grid->addWidget(dbPassword, row++, col--);
  config.getData(Config::DbPassword, s);
  if (s.length())
    dbPassword->setText(s);
  
  
  // setup the date column
  label = new QLabel(tr("Date Column Name"));
  grid->addWidget(label, row, col++);

  dateColumn = new QLineEdit;
  grid->addWidget(dateColumn, row++, col--);
  config.getData(Config::DbDateColumn, s);
  if (s.length())
    dateColumn->setText(s);
  
  // setup the open column
  label = new QLabel(tr("Open Column Name"));
  grid->addWidget(label, row, col++);

  openColumn = new QLineEdit;
  grid->addWidget(openColumn, row++, col--);
  config.getData(Config::DbOpenColumn, s);
  if (s.length())
    openColumn->setText(s);
  
  // setup the high column
  label = new QLabel(tr("High Column Name"));
  grid->addWidget(label, row, col++);

  highColumn = new QLineEdit;
  grid->addWidget(highColumn, row++, col--);
  config.getData(Config::DbHighColumn, s);
  if (s.length())
    highColumn->setText(s);
  

  // start a new col list of items in the page
  grid->setRowStretch(row, 2);
  
  row = 0;
  col = 0;
  
  grid = new QGridLayout;
  grid->setMargin(5);
  grid->setSpacing(5);
  grid->setColumnStretch(1, 2);
  hbox->addLayout(grid);

  
  // setup the low column
  label = new QLabel(tr("Low Column Name"));
  grid->addWidget(label, row, col++);

  lowColumn = new QLineEdit;
  grid->addWidget(lowColumn, row++, col--);
  config.getData(Config::DbLowColumn, s);
  if (s.length())
    lowColumn->setText(s);
  
  // setup the close column
  label = new QLabel(tr("Close Column Name"));
  grid->addWidget(label, row, col++);

  closeColumn = new QLineEdit;
  grid->addWidget(closeColumn, row++, col--);
  config.getData(Config::DbCloseColumn, s);
  if (s.length())
    closeColumn->setText(s);
  
  // setup the date column
  label = new QLabel(tr("Volume Column Name"));
  grid->addWidget(label, row, col++);

  volumeColumn = new QLineEdit;
  grid->addWidget(volumeColumn, row++, col--);
  config.getData(Config::DbVolumeColumn, s);
  if (s.length())
    volumeColumn->setText(s);
  
  // setup the oi column
  label = new QLabel(tr("OI Column Name"));
  grid->addWidget(label, row, col++);

  oiColumn = new QLineEdit;
  grid->addWidget(oiColumn, row++, col--);
  config.getData(Config::DbOIColumn, s);
  if (s.length())
    oiColumn->setText(s);
  

  // setup the symbol search table 
  label = new QLabel(tr("Symbol Index Table"));
  grid->addWidget(label, row, col++);

  indexTable = new QLineEdit;
  grid->addWidget(indexTable, row++, col--);
  config.getData(Config::DbIndexTable, s);
  if (s.length())
    indexTable->setText(s);

  
  // setup the symbol search column
  label = new QLabel(tr("Symbol Column"));
  grid->addWidget(label, row, col++);

  symbolColumn = new QLineEdit;
  grid->addWidget(symbolColumn, row++, col--);
  config.getData(Config::DbSymbolColumn, s);
  if (s.length())
    symbolColumn->setText(s);
  
  
  // setup the symbol name column
  label = new QLabel(tr("Symbol Name Column"));
  grid->addWidget(label, row, col++);

  nameColumn = new QLineEdit;
  grid->addWidget(nameColumn, row++, col--);
  config.getData(Config::DbNameColumn, s);
  if (s.length())
    nameColumn->setText(s);
  
  
  // setup the symbol exchange column
  label = new QLabel(tr("Symbol Exchange Column"));
  grid->addWidget(label, row, col++);

  exchangeColumn = new QLineEdit;
  grid->addWidget(exchangeColumn, row++, col--);
  config.getData(Config::DbExchangeColumn, s);
  if (s.length())
    exchangeColumn->setText(s);
  
  grid->setRowStretch(row, 2);

  tabs->addTab(w, tr("Quotes"));
}

void Preferences::createColorPage ()
{
  // colors parms page

  QWidget *w = new QWidget;

  QVBoxLayout *vbox = new QVBoxLayout;
  w->setLayout(vbox);
  
  QGridLayout *grid = new QGridLayout;
  grid->setMargin(5);
  grid->setSpacing(5);
  grid->setColumnStretch(1, 2);
  vbox->addLayout(grid);
  
  vbox->insertStretch(-1, 1);

  int row = 0;
  int col = 0;
  Config config;
  
  // background color
  QLabel *label = new QLabel(tr("Chart Background"));
  grid->addWidget(label, row, col++);
  
  QColor color;
  backgroundColorButton = new ColorButton(w, color);
  grid->addWidget(backgroundColorButton, row++, col--);
  config.getData(Config::BackgroundColor, color);
  backgroundColorButton->setColor(color);
  connect(backgroundColorButton, SIGNAL(valueChanged()), this, SLOT(slotModified()));

  // border color
  label = new QLabel(tr("Chart Border"));
  grid->addWidget(label, row, col++);
  
  borderColorButton = new ColorButton(w, color);
  grid->addWidget(borderColorButton, row++, col--);
  config.getData(Config::BorderColor, color);
  borderColorButton->setColor(color);
  connect(borderColorButton, SIGNAL(valueChanged()), this, SLOT(slotModified()));

  // grid color
  label = new QLabel(tr("Chart Grid"));
  grid->addWidget(label, row, col++);
  
  gridColorButton = new ColorButton(w, color);
  grid->addWidget(gridColorButton, row++, col--);
  config.getData(Config::GridColor, color);
  gridColorButton->setColor(color);
  connect(gridColorButton, SIGNAL(valueChanged()), this, SLOT(slotModified()));

  //FIXME: add adjustment possibility for prefered CO-colors.
  // in this way to add a spinbox to set the amount of colors too
  
  tabs->addTab(w, tr("Colors"));
}

void Preferences::createFontPage ()
{
  // font parms page

  QWidget *w = new QWidget;

  QVBoxLayout *vbox = new QVBoxLayout;
  w->setLayout(vbox);
  
  QGridLayout *grid = new QGridLayout;
  grid->setMargin(5);
  grid->setSpacing(5);
  grid->setColumnStretch(1, 2);
  vbox->addLayout(grid);
  
  vbox->insertStretch(-1, 1);
  
  int row = 0;
  int col = 0;
  Config config;

  // plot font
  QLabel *label = new QLabel(tr("Plot Font"));
  grid->addWidget(label, row, col++);

  QFont font;  
  plotFontButton = new FontButton(w, font);
  grid->addWidget(plotFontButton, row++, col--);
  config.getData(Config::PlotFont, font);
  plotFontButton->setFont(font);
  connect(plotFontButton, SIGNAL(valueChanged()), this, SLOT(slotModified()));

  // app font
  label = new QLabel(tr("App Font"));
  grid->addWidget(label, row, col++);
  
  appFontButton = new FontButton(w, font);
  grid->addWidget(appFontButton, row++, col--);
  config.getData(Config::AppFont, font);
  appFontButton->setFont(font);
  connect(appFontButton, SIGNAL(valueChanged()), this, SLOT(slotModified()));

  tabs->addTab(w, tr("Fonts"));
}

void  Preferences::createMTPage()
{
  // main tool bar page

  QWidget *w = new QWidget;

  QVBoxLayout *vbox = new QVBoxLayout;
  w->setLayout(vbox);
  
  QGridLayout *grid = new QGridLayout; // two more cols as needed
  grid->setMargin(5);
  grid->setSpacing(5);
  grid->setColumnStretch(2, 1); // middle spacing col
  grid->setColumnStretch(5, 2); // outer right col
  vbox->addLayout(grid);
  
  vbox->insertStretch(-1, 1);
  
  int row = 0;
  int col = 0;
  QString s;
  Config config;
  
  QLabel *label = new QLabel(tr("Quit"));
  grid->addWidget(label, row, col++);

  quitBtnCheck = new QCheckBox;
  config.getData(Config::ShowQuitBtn, s);
  quitBtnCheck->setChecked(s.toInt());
  connect(quitBtnCheck, SIGNAL(stateChanged(int)), this, SLOT(slotModified()));
  grid->addWidget(quitBtnCheck, row++, col--);

  label = new QLabel(tr("Preferences"));
  grid->addWidget(label, row, col++);

  prefBtnCheck = new QCheckBox;
  config.getData(Config::ShowPrefBtn, s);
  prefBtnCheck->setChecked(s.toInt());
  connect(prefBtnCheck, SIGNAL(stateChanged(int)), this, SLOT(slotModified()));
  grid->addWidget(prefBtnCheck, row++, col--);

  label = new QLabel(tr("Side Panel"));
  grid->addWidget(label, row, col++);

  sidePanelBtnCheck = new QCheckBox;
  config.getData(Config::ShowSidePanelBtn, s);
  sidePanelBtnCheck->setChecked(s.toInt());
  connect(sidePanelBtnCheck, SIGNAL(stateChanged(int)), this, SLOT(slotModified()));
  grid->addWidget(sidePanelBtnCheck, row++, col--);

  label = new QLabel(tr("Grid"));
  grid->addWidget(label, row, col++);

  gridBtnCheck = new QCheckBox;
  config.getData(Config::ShowGridBtn, s);
  gridBtnCheck->setChecked(s.toInt());
  connect(gridBtnCheck, SIGNAL(stateChanged(int)), this, SLOT(slotModified()));
  grid->addWidget(gridBtnCheck, row++, col--);

  label = new QLabel(tr("Scale to Screen"));
  grid->addWidget(label, row, col++);

  scaleToScreenBtnCheck = new QCheckBox;
  config.getData(Config::ShowScaleToScreenBtn, s);
  scaleToScreenBtnCheck->setChecked(s.toInt());
  connect(scaleToScreenBtnCheck, SIGNAL(stateChanged(int)), this, SLOT(slotModified()));
  grid->addWidget(scaleToScreenBtnCheck, row++, col--);

  label = new QLabel(tr("CrossHair"));
  grid->addWidget(label, row, col++);

  crosshairBtnCheck = new QCheckBox;
  config.getData(Config::ShowCrosshairBtn, s);
  crosshairBtnCheck->setChecked(s.toInt());
  connect(crosshairBtnCheck, SIGNAL(stateChanged(int)), this, SLOT(slotModified()));
  grid->addWidget(crosshairBtnCheck, row++, col--);

  // now fill a second col
  row = 0;
  col = 2;
  
  label = new QLabel(tr("Draw Mode"));
  grid->addWidget(label, row, col++);

  drawModeBtnCheck= new QCheckBox;
  config.getData(Config::ShowDrawModeBtn, s);
  drawModeBtnCheck->setChecked(s.toInt());
  connect(drawModeBtnCheck, SIGNAL(stateChanged(int)), this, SLOT(slotModified()));
  grid->addWidget(drawModeBtnCheck, row++, col--);

  label = new QLabel(tr("New Indicator"));
  grid->addWidget(label, row, col++);

  newIndicatorBtnCheck = new QCheckBox;
  config.getData(Config::ShowNewIndicatorBtn, s);
  newIndicatorBtnCheck->setChecked(s.toInt());
  connect(newIndicatorBtnCheck, SIGNAL(stateChanged(int)), this, SLOT(slotModified()));
  grid->addWidget(newIndicatorBtnCheck, row++, col--);

  label = new QLabel(tr("Data Window"));
  grid->addWidget(label, row, col++);

  dataWindowBtnCheck = new QCheckBox;
  config.getData(Config::ShowDataWindowBtn, s);
  dataWindowBtnCheck->setChecked(s.toInt());
  connect(dataWindowBtnCheck, SIGNAL(stateChanged(int)), this, SLOT(slotModified()));
  grid->addWidget(dataWindowBtnCheck, row++, col--);

  label = new QLabel(tr("Help"));
  grid->addWidget(label, row, col++);

  helpButtonCheck = new QCheckBox;
  config.getData(Config::ShowHelpButton, s);
  helpButtonCheck->setChecked(s.toInt());
  connect(helpButtonCheck, SIGNAL(stateChanged(int)), this, SLOT(slotModified()));
  grid->addWidget(helpButtonCheck, row++, col--);

  tabs->addTab(w, tr("MainToolbar"));
}

void  Preferences::createCTPage() 
{
  // chart tool bar page

  QWidget *w = new QWidget;

  QVBoxLayout *vbox = new QVBoxLayout;
  w->setLayout(vbox);
  
  QGridLayout *grid = new QGridLayout;
  grid->setMargin(5);
  grid->setSpacing(5);
  grid->setColumnStretch(1, 2);
  vbox->addLayout(grid);
  
  vbox->insertStretch(-1, 1);

  int row = 0;
  int col = 0;
  QString s;
  Config config;

  QLabel *label = new QLabel(tr("Bar Length List"));
  grid->addWidget(label, row, col++);

  cmpsComboBoxCheck = new QCheckBox;
  config.getData(Config::ShowCmpsComboBox, s);
  cmpsComboBoxCheck->setChecked(s.toInt());
  connect(cmpsComboBoxCheck, SIGNAL(stateChanged(int)), this, SLOT(slotModified()));
  grid->addWidget(cmpsComboBoxCheck, row++, col--);  

  label = new QLabel(tr("Monthly Bars"));
  grid->addWidget(label, row, col++);

  cmpsMtyBtnCheck = new QCheckBox;
  config.getData(Config::ShowCmpsMtyBtn, s);
  cmpsMtyBtnCheck->setChecked(s.toInt());
  connect(cmpsMtyBtnCheck, SIGNAL(stateChanged(int)), this, SLOT(slotModified()));
  grid->addWidget(cmpsMtyBtnCheck, row++, col--);

  label = new QLabel(tr("Weekly Bars"));
  grid->addWidget(label, row, col++);

  cmpsWkyBtnCheck = new QCheckBox;
  config.getData(Config::ShowCmpsWkyBtn, s);
  cmpsWkyBtnCheck->setChecked(s.toInt());
  connect(cmpsWkyBtnCheck, SIGNAL(stateChanged(int)), this, SLOT(slotModified()));
  grid->addWidget(cmpsWkyBtnCheck, row++, col--);

  label = new QLabel(tr("Daily Bars"));
  grid->addWidget(label, row, col++);

  cmpsDayBtnCheck = new QCheckBox;
  config.getData(Config::ShowCmpsDayBtn, s);
  cmpsDayBtnCheck->setChecked(s.toInt());
  connect(cmpsDayBtnCheck, SIGNAL(stateChanged(int)), this, SLOT(slotModified()));
  grid->addWidget(cmpsDayBtnCheck, row++, col--);

  label = new QLabel(tr("60 Minute Bars"));
  grid->addWidget(label, row, col++);

  cmps60BtnCheck = new QCheckBox;
  config.getData(Config::ShowCmps60Btn, s);
  cmps60BtnCheck->setChecked(s.toInt());
  connect(cmps60BtnCheck, SIGNAL(stateChanged(int)), this, SLOT(slotModified()));
  grid->addWidget(cmps60BtnCheck, row++, col--);

  label = new QLabel(tr("15 Minute Bars"));
  grid->addWidget(label, row, col++);

  cmps15BtnCheck = new QCheckBox;
  config.getData(Config::ShowCmps15Btn, s);
  cmps15BtnCheck->setChecked(s.toInt());
  connect(cmps15BtnCheck, SIGNAL(stateChanged(int)), this, SLOT(slotModified()));
  grid->addWidget(cmps15BtnCheck, row++, col--);

  label = new QLabel(tr("5 Minute Bars"));
  grid->addWidget(label, row, col++);

  cmps5BtnCheck = new QCheckBox;
  config.getData(Config::ShowCmps5Btn, s);
  cmps5BtnCheck->setChecked(s.toInt());
  connect(cmps5BtnCheck, SIGNAL(stateChanged(int)), this, SLOT(slotModified()));
  grid->addWidget(cmps5BtnCheck, row++, col--);

  label = new QLabel(tr("Bar Spacing"));
  grid->addWidget(label, row, col++);

  barSpSpinboxCheck = new QCheckBox;
  config.getData(Config::ShowBarSpSpinbox, s);
  barSpSpinboxCheck->setChecked(s.toInt());
  connect(barSpSpinboxCheck, SIGNAL(stateChanged(int)), this, SLOT(slotModified()));
  grid->addWidget(barSpSpinboxCheck, row++, col--);

  label = new QLabel(tr("Bars To Load"));
  grid->addWidget(label, row, col++);

  barsToLoadFieldCheck = new QCheckBox;
  config.getData(Config::ShowBarsToLoadField, s);
  barsToLoadFieldCheck->setChecked(s.toInt());
  connect(barsToLoadFieldCheck, SIGNAL(stateChanged(int)), this, SLOT(slotModified()));
  grid->addWidget(barsToLoadFieldCheck, row++, col--);

  label = new QLabel(tr("Chart Slider"));
  grid->addWidget(label, row, col++);

  sliderCheck = new QCheckBox;
  config.getData(Config::ShowSlider, s);
  sliderCheck->setChecked(s.toInt());
  connect(sliderCheck, SIGNAL(stateChanged(int)), this, SLOT(slotModified()));
  grid->addWidget(sliderCheck, row++, col--);

  label = new QLabel(tr("Recent charts"));
  grid->addWidget(label, row, col++);

  recentComboBoxCheck = new QCheckBox;
  config.getData(Config::ShowRecentCharts, s);
  recentComboBoxCheck->setChecked(s.toInt());
  connect(recentComboBoxCheck, SIGNAL(stateChanged(int)), this, SLOT(slotModified()));
  grid->addWidget(recentComboBoxCheck, row++, col--);  

  tabs->addTab(w, tr("ChartToolbar"));
}

void Preferences::slotSave ()
{
  Config config;
  QString s;
  
  config.setData(Config::PSButton1, bs1Spinner->value());
  config.setData(Config::PSButton2, bs2Spinner->value());
  config.setData(Config::PSButton3, bs3Spinner->value());
  config.setData(Config::IndicatorTabRows, tabRows->value());
  s = indicatorScriptDefault->text();
  config.setData(Config::IndicatorScriptDefault, s);

  QColor c, c2;
  backgroundColorButton->getColor(c);
  config.getData(Config::BackgroundColor,c2);
  if (c != c2)
  {
    config.setData(Config::BackgroundColor,c);
    emit signalBackgroundColor(c);
  }

  borderColorButton->getColor(c);
  config.getData(Config::BorderColor,c2);
  if (c != c2)
  {
    config.setData(Config::BorderColor,c);
    emit signalBorderColor(c);
  }

  gridColorButton->getColor(c);
  config.getData(Config::GridColor,c2);
  if (c != c2)
  {
    config.setData(Config::GridColor,c);
    emit signalGridColor(c);
  }

  QFont f, f2;
  plotFontButton->getFont(f);
  config.getData(Config::PlotFont, f2);
  if (f != f2)
  {
    config.setData(Config::PlotFont, f);
    emit signalPlotFont(f);
  }

  appFontButton->getFont(f);
  config.getData(Config::AppFont, f2);
  if (f != f2)
  {
    config.setData(Config::AppFont, f);
    emit signalAppFont(f);
  }
  
  // main tool bar settings
  // save all, anyway if changed or not, who cares?
  config.setData(Config::ShowQuitBtn, quitBtnCheck->isChecked());
  config.setData(Config::ShowPrefBtn, prefBtnCheck->isChecked());
  config.setData(Config::ShowSidePanelBtn, sidePanelBtnCheck->isChecked());
  config.setData(Config::ShowGridBtn, gridBtnCheck->isChecked());
  config.setData(Config::ShowScaleToScreenBtn, scaleToScreenBtnCheck->isChecked());
  config.setData(Config::ShowCrosshairBtn, crosshairBtnCheck->isChecked());
  config.setData(Config::ShowDrawModeBtn, drawModeBtnCheck->isChecked());
  config.setData(Config::ShowNewIndicatorBtn, newIndicatorBtnCheck->isChecked());
  config.setData(Config::ShowDataWindowBtn, dataWindowBtnCheck->isChecked());
  config.setData(Config::ShowHelpButton, helpButtonCheck->isChecked());

  // chart tool bar settings
  config.setData(Config::ShowSlider, sliderCheck->isChecked());
  config.setData(Config::ShowBarsToLoadField, barsToLoadFieldCheck->isChecked());
  config.setData(Config::ShowBarSpSpinbox, barSpSpinboxCheck->isChecked());
  config.setData(Config::ShowCmps60Btn, cmps60BtnCheck->isChecked());
  config.setData(Config::ShowCmps15Btn, cmps15BtnCheck->isChecked());
  config.setData(Config::ShowCmps5Btn, cmps5BtnCheck->isChecked());
  config.setData(Config::ShowCmpsDayBtn, cmpsDayBtnCheck->isChecked());
  config.setData(Config::ShowCmpsWkyBtn, cmpsWkyBtnCheck->isChecked());
  config.setData(Config::ShowCmpsMtyBtn, cmpsMtyBtnCheck->isChecked());
  config.setData(Config::ShowCmpsComboBox, cmpsComboBoxCheck->isChecked());
  config.setData(Config::ShowRecentCharts, recentComboBoxCheck->isChecked());

  // save database parms
  s = dbDriver->currentText();
  config.setData(Config::DbPlugin, s);
  
  s = dbHostName->text();
  config.setData(Config::DbHostName, s);
  
  s = dbName->text();
  config.setData(Config::DbName, s);
  
  s = dbUserName->text();
  config.setData(Config::DbUserName, s);
  
  s = dbPassword->text();
  config.setData(Config::DbPassword, s);
  
  s = dateColumn->text();
  config.setData(Config::DbDateColumn, s);
  
  s = openColumn->text();
  config.setData(Config::DbOpenColumn, s);
  
  s = highColumn->text();
  config.setData(Config::DbHighColumn, s);
  
  s = lowColumn->text();
  config.setData(Config::DbLowColumn, s);
  
  s = closeColumn->text();
  config.setData(Config::DbCloseColumn, s);
  
  s = volumeColumn->text();
  config.setData(Config::DbVolumeColumn, s);
  
  s = oiColumn->text();
  config.setData(Config::DbOIColumn, s);
  
  s = indexTable->text();
  config.setData(Config::DbIndexTable, s);
  
  s = symbolColumn->text();
  config.setData(Config::DbSymbolColumn, s);
  
  s = nameColumn->text();
  config.setData(Config::DbNameColumn, s);
  
  s = exchangeColumn->text();
  config.setData(Config::DbExchangeColumn, s);
  
  emit signalReloadToolBars();
  
  emit signalLoadChart();

  QSize sz = size();
  config.setData(Config::PrefDlgWindowSize, sz);

  accept();    
}

void Preferences::slotModified()
{
  modified = TRUE;
}

void Preferences::cancelPressed()
{
  if (modified)
  {
    int rc = QMessageBox::warning(this,
    			        tr("Qtstalker: Warning"),
			        tr("Items modified. Are you sure you want to discard changes?"),
			        QMessageBox::Yes,
			        QMessageBox::No,
			        QMessageBox::NoButton);
    if (rc == QMessageBox::No)
      return;
  }

  Config config;
  QSize sz = size();
  config.setData(Config::PrefDlgWindowSize, sz);

  reject();
}




