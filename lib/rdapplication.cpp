// rdapplication.cpp
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

#include <stdlib.h>

#include <qapplication.h>
#include <qobject.h>

#include "dbversion.h"
#include "rdapplication.h"
#include "rdcmd_switch.h"

RDApplication *rda=NULL;

RDApplication::RDApplication(const QString &module_name,QObject *parent)
  : QObject(parent)
{
  app_module_name=module_name;

  app_airplay_conf=NULL;
  app_cae=NULL;
  app_config=NULL;
  app_library_conf=NULL;
  app_panel_conf=NULL;
  app_ripc=NULL;
  app_station=NULL;
  app_system=NULL;
  app_user=NULL;
}


RDApplication::~RDApplication()
{
  if(app_config!=NULL) {
    delete app_config;
  }
  if(app_system!=NULL) {
    delete app_system;
  }
  if(app_station!=NULL) {
    delete app_station;
  }
  if(app_library_conf!=NULL) {
    delete app_library_conf;
  }
  if(app_airplay_conf!=NULL) {
    delete app_airplay_conf;
  }
  if(app_panel_conf!=NULL) {
    delete app_panel_conf;
  }
  if(app_user!=NULL) {
    delete app_user;
  }
  if(app_cae!=NULL) {
    delete app_cae;
  }
  if(app_ripc!=NULL) {
    delete app_ripc;
  }
}


bool RDApplication::open(QString *err_msg)
{
  unsigned schema=0;
  QString db_err;
  bool skip_db_check=false;

  //
  // Read command switches
  //
  RDCmdSwitch *cmd=new RDCmdSwitch(qApp->argc(),qApp->argv(),"","");
  for(unsigned i=0;i<cmd->keys();i++) {
    if(cmd->key(i)=="--skip-db-check") {
      skip_db_check=true;
      cmd->setProcessed(i,true);
    }
  }
  delete cmd;

  //
  // Open rd.conf(5)
  //
  app_config=new RDConfig();
  app_config->load();
  app_config->setModuleName(app_module_name);

  //
  // Open Database
  //
  QSqlDatabase *db=RDInitDb(&schema,&db_err);
  if(!db) {
    *err_msg=QObject::tr("Unable to open database")+" ["+db_err+"]";
    return false;
  }
  if((RD_VERSION_DATABASE!=schema)&&(!skip_db_check)) {
    *err_msg=QObject::tr("Database version mismatch, should be")+
      QString().sprintf(" %u, ",RD_VERSION_DATABASE)+
      QObject::tr("is")+
      QString().sprintf(" %u",schema);
    return false;
  }

  //
  // Open Accessors
  //
  app_system=new RDSystem();
  app_station=new RDStation(app_config->stationName());
  app_library_conf=new RDLibraryConf(app_config->stationName());
  app_airplay_conf=new RDAirPlayConf(app_config->stationName(),"RDAIRPLAY");
  app_panel_conf=new RDAirPlayConf(app_config->stationName(),"RDPANEL");
  app_user=new RDUser();
  app_cae=new RDCae(app_station,app_config,this);
  app_ripc=new RDRipc(app_station,app_config,this);

  return true; 
}


RDAirPlayConf *RDApplication::airplayConf()
{
  return app_airplay_conf;
}


RDCae *RDApplication::cae()
{
  return app_cae;
}


RDConfig *RDApplication::config()
{
  return app_config;
}


RDLibraryConf *RDApplication::libraryConf()
{
  return app_library_conf;
}


RDAirPlayConf *RDApplication::panelConf()
{
  return app_panel_conf;
}


RDRipc *RDApplication::ripc()
{
  return app_ripc;
}


RDStation *RDApplication::station()
{
  return app_station;
}


RDSystem *RDApplication::system()
{
  return app_system;
}


RDUser *RDApplication::user()
{
  return app_user;
}
