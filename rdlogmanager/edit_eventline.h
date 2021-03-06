// edit_clockline.h
//
// Edit A Rivendell Log Clock Event
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

#ifndef EDIT_CLOCKLINE_H
#define EDIT_CLOCKLINE_H

#include <qlineedit.h>

#include <rdclockmodel.h>
#include <rddialog.h>
#include <rdtimeedit.h>

class EditEventLine : public RDDialog
{
 Q_OBJECT
 public:
  EditEventLine(RDEventLine *eventline,RDClockModel *clock,int line,
		QWidget *parent=0);
  QSize sizeHint() const;
  QSizePolicy sizePolicy() const;

 private slots:
  void selectData();
  void okData();
  void cancelData();

 protected:
  void closeEvent(QCloseEvent *e);

 private:
  RDEventLine *edit_eventline;
  QLineEdit *edit_eventname_edit;
  RDTimeEdit *edit_starttime_edit;
  RDTimeEdit *edit_endtime_edit;
  int edit_line;
  RDClockModel *edit_clock_model;
};


#endif  // EDIT_EVENTLINE_H
