// list_groups.h
//
// List Rivendell Groups
//
//   (C) Copyright 2002-2021 Fred Gleason <fredg@paravelsystems.com>
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

#ifndef LIST_GROUPS_H
#define LIST_GROUPS_H

#include <QPushButton>

#include <rddb.h>
#include <rddialog.h>
#include <rdgrouplistmodel.h>
#include <rdtableview.h>

class ListGroups : public RDDialog
{
  Q_OBJECT
 public:
  ListGroups(QWidget *parent=0);
  ~ListGroups();
  QSize sizeHint() const;
  QSizePolicy sizePolicy() const;
  
 private slots:
  void addData();
  void editData();
  void renameData();
  void deleteData();
  void reportData();
  void doubleClickedData(const QModelIndex &index);
  void modelResetData();
  void closeData();

 protected:
  void resizeEvent(QResizeEvent *e);

 private:
  RDTableView *list_groups_view;
  RDGroupListModel *list_groups_model;
  QPushButton *list_add_button;
  QPushButton *list_edit_button;
  QPushButton *list_rename_button;
  QPushButton *list_delete_button;
  QPushButton *list_report_button;
  QPushButton *list_close_button;
};


#endif  // LIST_GROUPS_H
