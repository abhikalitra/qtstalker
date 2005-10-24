/*
 *  Qtstalker stock charter
 *
 *  Copyright (C) 2001-2005 Stefan S. Stratigakos
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

#include "FormulaEdit.h"
#include "PrefDialog.h"
#include "IndicatorPlugin.h"
#include "SymbolDialog.h"
#include "../pics/delete.xpm"
#include "../pics/edit.xpm"
#include "../pics/insert.xpm"
#include "../pics/openchart.xpm"
#include "../pics/filesave.xpm"
#include <qlayout.h>
#include <qfile.h>
#include <qtextstream.h>
#include <qdir.h>
#include <qmessagebox.h>
#include <qtabwidget.h>
#include <qinputdialog.h>

#define BUTTON_SIZE 24

FormulaEdit::FormulaEdit (QWidget *w, int t) : QWidget(w)
{
  type = (FormulaEditType) t;
  config.getPluginList(Config::IndicatorPluginPath, functionList);

  PlotLine pl;
  pl.getLineTypes(lineTypes);
  
  QHBoxLayout *hbox = new QHBoxLayout(this);
  hbox->setMargin(0);
  hbox->setSpacing(1);

  QTabWidget *tab = new QTabWidget(this);
  hbox->addWidget(tab);

  // create formula page
  QWidget *tw = new QWidget(this);

  QHBoxLayout *thbox = new QHBoxLayout(tw);
  thbox->setMargin(5);
  thbox->setSpacing(0);

  formula = new QTextEdit(tw);
  thbox->addWidget(formula);

  ftoolbar = new Toolbar(tw, 30, 30, TRUE);
  thbox->addWidget(ftoolbar);

  QString s = "open";
  QString s2 = tr("Open Rule");
  ftoolbar->addButton(s, openchart, s2);
  QObject::connect(ftoolbar->getButton(s), SIGNAL(clicked()), this, SLOT(openRule()));

  s = "save";
  s2 = tr("Save Rule");
  ftoolbar->addButton(s, filesave, s2);
  QObject::connect(ftoolbar->getButton(s), SIGNAL(clicked()), this, SLOT(saveRule()));
  
  tab->addTab(tw, tr("Formula"));

  // create plot page
  tw = new QWidget(this);

  thbox = new QHBoxLayout(tw);
  thbox->setMargin(5);
  thbox->setSpacing(0);

  plot = new QListBox(tw);
  connect(plot, SIGNAL(doubleClicked(QListBoxItem *)), this, SLOT(slotDoubleClicked(QListBoxItem *)));
  thbox->addWidget(plot);

  ptoolbar = new Toolbar(tw, 30, 30, TRUE);
  thbox->addWidget(ptoolbar);

  s = "insert";
  s2 = tr("Insert");
  ptoolbar->addButton(s, insert, s2);
  QObject::connect(ptoolbar->getButton(s), SIGNAL(clicked()), this, SLOT(insertPlotItem()));
  
  s = "edit";
  s2 = tr("Edit");
  ptoolbar->addButton(s, edit, s2);
  QObject::connect(ptoolbar->getButton(s), SIGNAL(clicked()), this, SLOT(editPlotItem()));
  
  s = "delete";
  s2 = tr("Delete");
  ptoolbar->addButton(s, deleteitem, s2);
  QObject::connect(ptoolbar->getButton(s), SIGNAL(clicked()), this, SLOT(deletePlotItem()));

  s = "open";
  s2 = tr("Open Rule");
  ptoolbar->addButton(s, openchart, s2);
  QObject::connect(ptoolbar->getButton(s), SIGNAL(clicked()), this, SLOT(openRule()));

  s = "save";
  s2 = tr("Save Rule");
  ptoolbar->addButton(s, filesave, s2);
  QObject::connect(ptoolbar->getButton(s), SIGNAL(clicked()), this, SLOT(saveRule()));

  tab->addTab(tw, tr("Plot"));
}

FormulaEdit::~FormulaEdit ()
{
}

void FormulaEdit::insertPlotItem ()
{
  QString pl = tr("Plot");
  QString cl = tr("Color");
  QString ll = tr("Label");
  QString ltl = tr("Line Type");
  QString vl = tr("Variable");

  PrefDialog *dialog = new PrefDialog(this);
  dialog->setCaption(tr("Insert Plot"));
  dialog->createPage (pl);
  QString s("Var");
  dialog->addTextItem(vl, pl, s);
  QColor c("red");
  dialog->addColorItem(cl, pl, c);
  s = "Label";
  dialog->addTextItem(ll, pl, s);
  dialog->addComboItem(ltl, pl, lineTypes, 0);

  int rc = dialog->exec();
  if (rc != QDialog::Accepted)
  {
    delete dialog;
    return;
  }

  s = "plot (";
  s.append(dialog->getText(vl));
  s.append(",");
  s.append(dialog->getColor(cl).name());
  s.append(",");
  s.append(dialog->getText(ll));
  s.append(",");
  s.append(dialog->getCombo(ltl));
  s.append(")");
  plot->insertItem(s, plot->currentItem() + 1);

  delete dialog;
}

void FormulaEdit::editPlotItem ()
{
  QString s = plot->currentText();
  if (! s.length())
    return;
  s.remove(0, s.find("(", 0, TRUE) + 1);
  s.truncate(s.find(")", -1, TRUE));
  QStringList l = QStringList::split(",", s, FALSE);
  int loop;
  for (loop = 0; loop < (int) l.count(); loop++)
    l[loop] = l[loop].stripWhiteSpace();

  QString pl = tr("Plot");
  QString cl = tr("Color");
  QString ll = tr("Label");
  QString ltl = tr("Line Type");
  QString vl = tr("Variable");

  PrefDialog *dialog = new PrefDialog(this);
  dialog->setCaption(tr("Edit Plot"));
  dialog->createPage (pl);
  dialog->addTextItem(vl, pl, l[0]);
  QColor c(l[1]);
  dialog->addColorItem(cl, pl, c);
  dialog->addTextItem(ll, pl, l[2]);

  l = lineTypes;
  l.append("Bar");
  l.append("Candle");
  dialog->addComboItem(ltl, pl, l, l[3]);

  int rc = dialog->exec();
  if (rc != QDialog::Accepted)
  {
    delete dialog;
    return;
  }

  s = "plot (";
  s.append(dialog->getText(vl));
  s.append(",");
  s.append(dialog->getColor(cl).name());
  s.append(",");
  s.append(dialog->getText(ll));
  s.append(",");
  s.append(dialog->getCombo(ltl));
  s.append(")");
  plot->changeItem(s, plot->currentItem());

  delete dialog;
}

void FormulaEdit::deletePlotItem ()
{
  plot->removeItem(plot->currentItem());
}

void FormulaEdit::setLine (QString &d)
{
  if (d.contains("enable="))
  {
    enableLine = d;
    return;
  }

  if (d.contains("plugin="))
  {
    pluginLine = d;
    return;
  }

  if (d.contains("plotType="))
  {
    plotTypeLine = d;
    return;
  }

  if (d.contains("plot"))
    plot->insertItem(d, -1);
  else
    formula->append(d);
}

QString FormulaEdit::getText ()
{
  QString s = (enableLine + "\n");
  s.append(pluginLine + "\n");
  s.append(plotTypeLine + "\n");
  s.append(formula->text()  + "\n");

  int loop;
  for (loop = 0; loop < (int) plot->count(); loop++)
    s.append(plot->text(loop) + "\n");

  return s;
}

void FormulaEdit::openRule ()
{
  QString s("*");
  QString s2(config.getData(Config::CUSRulePath));
  SymbolDialog *dialog = new SymbolDialog(this, 
  					  s2,
					  s,
					  QFileDialog::ExistingFiles);
  dialog->setCaption(tr("Select rule to open."));

  int rc = dialog->exec();

  if (rc == QDialog::Accepted)
  {
    QFile f(dialog->selectedFile());
    if (! f.open(IO_ReadOnly))
    {
      qDebug("FormulaEdit::openItem:can't read file");
      delete dialog;
      return;
    }
    QTextStream stream(&f);
  
    formula->clear();
    plot->clear();
    
    while(stream.atEnd() == 0)
    {
      s = stream.readLine();
      s = s.stripWhiteSpace();
      if (! s.length())
        continue;
      setLine(s);
    }
  
    f.close();
  }

  delete dialog;
}

void FormulaEdit::saveRule ()
{
  if (! plot->count())
  {
    QMessageBox::information(this,
                             tr("Qtstalker: Error"),
			     tr("Plot missing."));
    return;
  }

  bool ok;  
  QString selection = QInputDialog::getText(tr("Save Rule"),
  					    tr("Enter name for rule."),
					    QLineEdit::Normal,
					    tr("NewRule"),
					    &ok,
					    this);
  if ((! ok) || (selection.isNull()))
    return;

  while (selection.contains(" "))
    selection = selection.remove(selection.find(" ", 0, TRUE), 1);
    
  QString s = config.getData(Config::CUSRulePath);
  s.append("/");
  s.append(selection);
  QDir dir(s);
  if (dir.exists(s, TRUE))
  {
    int rc = QMessageBox::warning(this,
  			          tr("Qtstalker: Warning"),
			          tr("Rule already exists. Do you want to replace it?"),
			          QMessageBox::Yes,
			          QMessageBox::No,
			          QMessageBox::NoButton);

    if (rc == QMessageBox::No)
      return;
  }
  
  QFile f(s);
  if (! f.open(IO_WriteOnly))
  {
    qDebug("FormulaEdit::saveItem:can't open file");
    return;
  }
  QTextStream stream(&f);
  
  stream << getText() << "\n";

  f.close(); 
}

void FormulaEdit::slotDoubleClicked (QListBoxItem *)
{
  editPlotItem();
}

int FormulaEdit::getLines ()
{
  return (int) formula->lines();
}

