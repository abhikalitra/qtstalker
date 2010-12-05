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

#ifndef SYMBOL_DIALOG_DIALOG_HPP
#define SYMBOL_DIALOG_DIALOG_HPP

#include <QDialog>
#include <QStringList>
#include <QTreeWidget>
#include <QPushButton>
#include <QLabel>
#include <QComboBox>
#include <QLineEdit>

class SymbolDialog : public QDialog
{
  Q_OBJECT

  signals:
    void signalDone (QString);

  public:
    SymbolDialog ();
    void createGUI ();
//    void setSymbols (QString exchange, QString symbol);
//    void getSymbols (Group &);
//    void getSymbolSearch (QString &ex, QString &ss);
    void loadExchanges ();
    void loadSettings ();
    void saveSettings ();
    void symbols (QStringList &);
    QString stringData ();

  public slots:
    void done ();
    void cancel ();
    void searchSelectionChanged ();
    void symbolSelectionChanged ();
    void help ();
    void searchButtonPressed ();
    void loadExchanges2 (QStringList);
    void searchRequestDone (QStringList);
    void addButtonPressed ();
    void deleteButtonPressed ();

  private:
    QTreeWidget *_searchList;
    QTreeWidget *_symbolList;
    QComboBox *_exchanges;
    QLineEdit *_search;
    QPushButton *_searchButton;
    QPushButton *_okButton;
    QPushButton *_cancelButton;
    QPushButton *_addButton;
    QPushButton *_deleteButton;
    QString _helpFile;
    QLabel *_message;
    QStringList _groups;
};

#endif
