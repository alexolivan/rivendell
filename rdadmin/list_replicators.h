// list_replicators.h
//
// List Rivendell Replication Configurations
//
//   (C) Copyright 2010-2021 Fred Gleason <fredg@paravelsystems.com>
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License version 2 as
//   published by the Free Software Foundation.
//
//   This program is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.
//
//   You should have received a copy of the GNU General Public
//   License along with this program; if not, write to the Free Software
//   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
//

#ifndef LIST_REPLICATORS_H
#define LIST_REPLICATORS_H

#include <QPushButton>

#include <rddialog.h>
#include <rdreplicatorlistmodel.h>
#include <rdtableview.h>

class ListReplicators : public RDDialog
{
  Q_OBJECT
 public:
  ListReplicators(QWidget *parent=0);
  ~ListReplicators();
  QSize sizeHint() const;
  QSizePolicy sizePolicy() const;
  
 private slots:
  void addData();
  void editData();
  void deleteData();
  void listData();
  void doubleClickedData(const QModelIndex &index);
  void closeData();

 protected:
  void resizeEvent(QResizeEvent *e);

 private:
  QLabel *list_replicators_label;
  RDTableView *list_replicators_view;
  RDReplicatorListModel *list_replicators_model;
  QPushButton *list_add_button;
  QPushButton *list_edit_button;
  QPushButton *list_delete_button;
  QPushButton *list_list_button;
  QPushButton *list_close_button;
};


#endif  // LIST_REPLICATORS_H
