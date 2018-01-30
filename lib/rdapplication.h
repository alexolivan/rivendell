// rdapplication.h
//
// Base Application Class
//
//   (C) Copyright 2018 Fred Gleason <fredg@paravelsystems.com>
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

#ifndef RDAPPLICATION_H
#define RDAPPLICATION_H

#include <qobject.h>

#include <rdairplay_conf.h>
#include <rdcae.h>
#include <rdconfig.h>
#include <rddb.h>
#include <rdlibrary_conf.h>
#include <rdripc.h>
#include <rdstation.h>
#include <rdsystem.h>
#include <rduser.h>

class RDApplication : public QObject
{
  Q_OBJECT;
 public:
  RDApplication(const QString &module_name,QObject *parent=0);
  ~RDApplication();
  bool open(QString *err_msg);
  RDAirPlayConf *airplayConf();
  RDCae *cae();
  RDConfig *config();
  RDLibraryConf *libraryConf();
  RDAirPlayConf *panelConf();
  RDRipc *ripc();
  RDStation *station();
  RDSystem *system();
  RDUser *user();

 private:
  RDAirPlayConf *app_airplay_conf;
  RDAirPlayConf *app_panel_conf;
  RDCae *app_cae;
  RDConfig  *app_config;
  RDLibraryConf *app_library_conf;
  RDRipc *app_ripc;
  RDStation *app_station;
  RDSystem *app_system;
  RDUser *app_user;
  QString app_command_name;
  QString app_module_name;
};

extern RDApplication *rda;

#endif  // RDAPPLICATION_H
