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
//  createDatabasePage();
//  createSQLPage();
  createColorPage();
  createFontPage();

  Config config;
  QSize sz;
  config.getData((int) Config::PrefDlgWindowSize, sz);
  resize(sz);

  modified = 0;
  refreshModified = 0;
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
  bs1Spinner->setToolTip(tr("Number of screen pixels between bars.\n6 is good for bar charts."));
  bs1Spinner->setRange(2, 99);
  bs1Spinner->setValue(config.getInt(Config::PSButton1));
  connect(bs1Spinner, SIGNAL(valueChanged(int)), this, SLOT(slotModified()));
  grid->addWidget(bs1Spinner, row++, col--);

  // bar spacing 2
  label = new QLabel(tr("Bar Spacing 2"));
  grid->addWidget(label, row, col++);

  bs2Spinner = new QSpinBox;
  bs2Spinner->setToolTip(tr("Number of screen pixels between bars.\n8 is good for candle charts"));
  bs2Spinner->setRange(2, 99);
  bs2Spinner->setValue(config.getInt(Config::PSButton2));
  connect(bs2Spinner, SIGNAL(valueChanged(int)), this, SLOT(slotModified()));
  grid->addWidget(bs2Spinner, row++, col--);

  // indicator tab rows
  label = new QLabel(tr("Indicator Tab Rows"));
  grid->addWidget(label, row, col++);

  tabRows = new QSpinBox;
  tabRows->setToolTip(tr("Maximum number of tabbed chart rows."));
  tabRows->setRange(1, 3);
  tabRows->setValue(2);
  tabRows->setValue(config.getInt(Config::IndicatorTabRows));
  connect(tabRows, SIGNAL(valueChanged(int)), this, SLOT(slotModified()));
  grid->addWidget(tabRows, row++, col--);

  // refresh chart
  label = new QLabel(tr("Refresh Chart"));
  grid->addWidget(label, row, col++);

  refreshSpinner = new QSpinBox;
  refreshSpinner->setToolTip(tr("Refresh chart every x minutes"));
  refreshSpinner->setRange(1, 99);
  refreshSpinner->setValue(config.getInt(Config::Refresh));
  connect(refreshSpinner, SIGNAL(valueChanged(int)), this, SLOT(slotRefreshModified()));
  grid->addWidget(refreshSpinner, row++, col--);

  tabs->addTab(w, tr("General"));
}

/*
void Preferences::createDatabasePage ()
{
  // database parms page

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

  // setup the db sql driver
  QLabel *label = new QLabel(tr("Qt DB Driver"));
  grid->addWidget(label, row, col++);

  QStringList l;
  l << "QSQLITE" << "QSQLITE2" << "QMYSQL" << "QOCI" << "QODBC" << "QPSQL" << "QTDS" << "QDB2" << "QIBASE";
  dbDriver = new QComboBox;
  dbDriver->setToolTip(tr("Qt4 SQL driver required for database."));
  dbDriver->addItems(l);
  grid->addWidget(dbDriver, row++, col--);
  QString s;
  config.getData(Config::DbPlugin, s);
  if (s.length())
    dbDriver->setCurrentIndex(dbDriver->findText(s));
  connect(dbDriver, SIGNAL(currentIndexChanged(const QString &)), this, SLOT(slotModified()));

  // setup the db sql host name
  label = new QLabel(tr("Db Host Name"));
  grid->addWidget(label, row, col++);

  dbHostName = new QLineEdit;
  dbHostName->setToolTip(tr("Network hostname of database. If local, use localhost"));
  grid->addWidget(dbHostName, row++, col--);
  config.getData(Config::DbHostName, s);
  if (s.length())
    dbHostName->setText(s);
  connect(dbHostName, SIGNAL(textChanged(const QString &)), this, SLOT(slotModified()));

  // setup the db name
  label = new QLabel(tr("Db Name"));
  grid->addWidget(label, row, col++);

  dbName = new QLineEdit;
  dbName->setToolTip(tr("The database name. If using the sqlite driver use the file path to the database."));
  grid->addWidget(dbName, row++, col--);
  config.getData(Config::DbName, s);
  if (s.length())
    dbName->setText(s);
  connect(dbName, SIGNAL(textChanged(const QString &)), this, SLOT(slotModified()));

  // setup the db user name
  label = new QLabel(tr("Db User Name"));
  grid->addWidget(label, row, col++);

  dbUserName = new QLineEdit;
  dbUserName->setToolTip(tr("The database user name."));
  grid->addWidget(dbUserName, row++, col--);
  config.getData(Config::DbUserName, s);
  if (s.length())
    dbUserName->setText(s);
  connect(dbUserName, SIGNAL(textChanged(const QString &)), this, SLOT(slotModified()));

  // setup the db password
  label = new QLabel(tr("Db Password"));
  grid->addWidget(label, row, col++);

  dbPassword = new QLineEdit;
  dbPassword->setToolTip(tr("The database user password."));
  dbPassword->setEchoMode(QLineEdit::Password);
  grid->addWidget(dbPassword, row++, col--);
  config.getData(Config::DbPassword, s);
  if (s.length())
    dbPassword->setText(s);
  connect(dbPassword, SIGNAL(textChanged(const QString &)), this, SLOT(slotModified()));

  tabs->addTab(w, tr("DB"));
}
*/

/*
void Preferences::createSQLPage ()
{
  // database quotes sql page

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
  QString s;

  // enter the get all charts sql string
  QLabel *label = new QLabel(tr("All Symbols"));
  grid->addWidget(label, row, col++);

  dbAllSymbols = new QLineEdit;
  dbAllSymbols->setToolTip(tr("The SQL command to list all symbols in the database."));
  grid->addWidget(dbAllSymbols, row++, col--);
  config.getData(Config::DbAllSymbols, s);
  if (s.length())
    dbAllSymbols->setText(s);
  connect(dbAllSymbols, SIGNAL(textChanged(const QString &)), this, SLOT(slotModified()));

  // enter the search symbols sql string
  label = new QLabel(tr("Search Symbols"));
  grid->addWidget(label, row, col++);

  dbSearchSymbols = new QLineEdit;
  dbSearchSymbols->setToolTip(tr("The SQL command to search for specific symbol(s) in the database.\n% is used for globbing."));
  grid->addWidget(dbSearchSymbols, row++, col--);
  config.getData(Config::DbSearchSymbols, s);
  if (s.length())
    dbSearchSymbols->setText(s);
  connect(dbSearchSymbols, SIGNAL(textChanged(const QString &)), this, SLOT(slotModified()));

  // enter the first date sql string
  label = new QLabel(tr("First Date"));
  grid->addWidget(label, row, col++);

  dbFirstDate = new QLineEdit;
  dbFirstDate->setToolTip(tr("The SQL command to return the earliest date for $symbol.\n$symbol is the symbol name."));
  grid->addWidget(dbFirstDate, row++, col--);
  config.getData(Config::DbFirstDate, s);
  if (s.length())
    dbFirstDate->setText(s);
  connect(dbFirstDate, SIGNAL(textChanged(const QString &)), this, SLOT(slotModified()));

  // enter the last date sql string
  label = new QLabel(tr("Last Date"));
  grid->addWidget(label, row, col++);

  dbLastDate = new QLineEdit;
  dbLastDate->setToolTip(tr("The SQL command to return the most recent date for $symbol.\n$symbol is the symbol name."));
  grid->addWidget(dbLastDate, row++, col--);
  config.getData(Config::DbLastDate, s);
  if (s.length())
    dbLastDate->setText(s);
  connect(dbLastDate, SIGNAL(textChanged(const QString &)), this, SLOT(slotModified()));

  // enter the get chart sql string
  label = new QLabel(tr("Get Symbol"));
  grid->addWidget(label, row, col++);

  dbGetSymbol = new QLineEdit;
  dbGetSymbol->setToolTip(tr("The SQL command to return quotes.\n$symbol is the symbol name.\n$sd is the first date.\n$ed is the last date."));
  grid->addWidget(dbGetSymbol, row++, col--);
  config.getData(Config::DbGetSymbol, s);
  if (s.length())
    dbGetSymbol->setText(s);
  connect(dbGetSymbol, SIGNAL(textChanged(const QString &)), this, SLOT(slotModified()));

  tabs->addTab(w, tr("SQL"));
}
*/

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
  backgroundColorButton->setToolTip(tr("Background color for charts."));
  grid->addWidget(backgroundColorButton, row++, col--);
  config.getData((int) Config::BackgroundColor, color);
  backgroundColorButton->setColor(color);
  connect(backgroundColorButton, SIGNAL(valueChanged()), this, SLOT(slotModified()));

  // border color
  label = new QLabel(tr("Chart Border"));
  grid->addWidget(label, row, col++);

  borderColorButton = new ColorButton(w, color);
  borderColorButton->setToolTip(tr("Border line color for charts. Used for seperators, ticks etc."));
  grid->addWidget(borderColorButton, row++, col--);
  config.getData((int) Config::BorderColor, color);
  borderColorButton->setColor(color);
  connect(borderColorButton, SIGNAL(valueChanged()), this, SLOT(slotModified()));

  // grid color
  label = new QLabel(tr("Chart Grid"));
  grid->addWidget(label, row, col++);

  gridColorButton = new ColorButton(w, color);
  gridColorButton->setToolTip(tr("Grid color for charts."));
  grid->addWidget(gridColorButton, row++, col--);
  config.getData((int) Config::GridColor, color);
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
  config.getData((int) Config::PlotFont, font);
  plotFontButton = new FontButton(w, font);
  plotFontButton->setToolTip(tr("Font used for text that appears on the chart."));
  grid->addWidget(plotFontButton, row++, col--);
  plotFontButton->setFont(font);
  connect(plotFontButton, SIGNAL(valueChanged()), this, SLOT(slotModified()));

  // app font
  label = new QLabel(tr("App Font"));
  grid->addWidget(label, row, col++);

  config.getData((int) Config::AppFont, font);
  appFontButton = new FontButton(w, font);
  appFontButton->setToolTip(tr("Font used for everything but charts."));
  grid->addWidget(appFontButton, row++, col--);
  appFontButton->setFont(font);
  connect(appFontButton, SIGNAL(valueChanged()), this, SLOT(slotModified()));

  tabs->addTab(w, tr("Fonts"));
}

void Preferences::slotSave ()
{
  Config config;
  QString s;

  if (! modified)
  {
    QSize sz = size();
    config.setData((int) Config::PrefDlgWindowSize, sz);
    accept();
  }

  config.transaction();

  config.setData((int) Config::PSButton1, bs1Spinner->value());
  config.setData((int) Config::PSButton2, bs2Spinner->value());
  config.setData((int) Config::IndicatorTabRows, tabRows->value());

  config.setData((int) Config::Refresh, refreshSpinner->value());
  if (refreshModified)
    emit signalRefreshChanged(refreshSpinner->value());

  QColor c, c2;
  backgroundColorButton->getColor(c);
  config.getData((int) Config::BackgroundColor, c2);
  if (c != c2)
  {
    config.setData((int) Config::BackgroundColor, c);
    emit signalBackgroundColor(c);
  }

  borderColorButton->getColor(c);
  config.getData((int) Config::BorderColor, c2);
  if (c != c2)
  {
    config.setData((int) Config::BorderColor, c);
    emit signalBorderColor(c);
  }

  gridColorButton->getColor(c);
  config.getData((int) Config::GridColor, c2);
  if (c != c2)
  {
    config.setData((int) Config::GridColor, c);
    emit signalGridColor(c);
  }

  QFont f, f2;
  plotFontButton->getFont(f);
  config.getData((int) Config::PlotFont, f2);
  if (f != f2)
  {
    config.setData((int) Config::PlotFont, f);
  }

  appFontButton->getFont(f);
  config.getData((int) Config::AppFont, f2);
  if (f != f2)
  {
    config.setData((int) Config::AppFont, f);
    emit signalAppFont(f);
  }

  // save database parms
/*
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
*/

  // save sql commands
/*
  s = dbAllSymbols->text();
  config.setData(Config::DbAllSymbols, s);

  s = dbSearchSymbols->text();
  config.setData(Config::DbSearchSymbols, s);

  s = dbFirstDate->text();
  config.setData(Config::DbFirstDate, s);

  s = dbLastDate->text();
  config.setData(Config::DbLastDate, s);

  s = dbGetSymbol->text();
  config.setData(Config::DbGetSymbol, s);
*/

  emit signalLoadChart();

  QSize sz = size();
  config.setData((int) Config::PrefDlgWindowSize, sz);

  config.commit();

  accept();
}

void Preferences::slotModified()
{
  modified = TRUE;
}

void Preferences::slotRefreshModified()
{
  refreshModified = TRUE;
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
  config.setData((int) Config::PrefDlgWindowSize, sz);

  reject();
}




