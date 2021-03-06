// rdapplication.cpp
//
// Base Application Class
//
//   (C) Copyright 2018-2021 Fred Gleason <fredg@paravelsystems.com>
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
#include <syslog.h>

#include <qapplication.h>
#include <qobject.h>
#include <qprocess.h>

#include "rdescape_string.h"

#include "dbversion.h"
#include "rdapplication.h"
#include "rdcmd_switch.h"

RDCoreApplication *rdc=NULL;
QStringList __rdapplication_temp_files;

void __RDCoreApplication_ExitCallback()
{
  for(int i=0;i<__rdapplication_temp_files.size();i++) {
    unlink(__rdapplication_temp_files.at(i).toUtf8());
  }
}


RDCoreApplication::RDCoreApplication(const QString &module_name,
				     const QString &cmdname,
				     const QString &usage,QObject *parent)
  : QObject(parent)
{
  app_module_name=module_name;
  app_command_name=cmdname;
  app_usage=usage;

  //
  // Maintainer's Note
  //
  // This for the 'ident' value passed to openlog(1). It *must*
  // an actual (const char *) string stored on the heap. *Don't* pass
  // a QByteArray!
  //
  strncpy(app_syslog_name,cmdname.toUtf8(),PATH_MAX-1);

  app_heartbeat=NULL;
  app_airplay_conf=NULL;
  app_cae=NULL;
  app_cmd_switch=NULL;
  app_config=NULL;
  app_library_conf=NULL;
  app_logedit_conf=NULL;
  app_panel_conf=NULL;
  app_ripc=NULL;
  app_station=NULL;
  app_system=NULL;
  app_user=NULL;

  atexit(__RDCoreApplication_ExitCallback);
}


RDCoreApplication::~RDCoreApplication()
{
  if(app_heartbeat!=NULL) {
    delete app_heartbeat;
  }
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
  if(app_logedit_conf!=NULL) {
    delete app_logedit_conf;
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
  if(app_cmd_switch!=NULL) {
    delete app_cmd_switch;
  }
  if(app_ripc!=NULL) {
    delete app_ripc;
  }
}


bool RDCoreApplication::open(QString *err_msg,RDCoreApplication::ErrorType *err_type,
			 bool check_svc)
{
  int schema=0;
  QString db_err;
  bool skip_db_check=false;
  int persistent_dropbox_id=-1;
  bool ok=false;

  if(err_type!=NULL) {
    *err_type=RDCoreApplication::ErrorOk;
  }

  //
  // Read command switches
  //
  app_cmd_switch=new RDCmdSwitch(app_command_name,app_usage);
  for(unsigned i=0;i<app_cmd_switch->keys();i++) {
    if(app_cmd_switch->key(i)=="--skip-db-check") {
      skip_db_check=true;
      app_cmd_switch->setProcessed(i,true);
    }
    if(app_cmd_switch->key(i)=="--ticket") {
      app_ticket=app_cmd_switch->value(i);
      app_cmd_switch->setProcessed(i,true);
    }
    if(app_cmd_switch->key(i)=="--persistent-dropbox-id") {
      persistent_dropbox_id=app_cmd_switch->value(i).toUInt(&ok);
      if(ok) {
	app_command_name=QString().sprintf("dropbox[%u]",persistent_dropbox_id);
      }
      app_cmd_switch->setProcessed(i,true);
    }
  }

  //
  // Open rd.conf(5)
  //
  app_config=new RDConfig();
  app_config->load();
  app_config->setModuleName(app_module_name);

  //
  // Initialize Logging
  //
  if(app_cmd_switch->debugActive()) {
    openlog(app_syslog_name,LOG_PERROR,app_config->syslogFacility());
  }
  else {
    openlog(app_syslog_name,0,app_config->syslogFacility());
  }

  //
  // Check Rivendell Service Status
  //
  if(check_svc) {
    if(!CheckService(err_msg)) {
      if(err_type!=NULL) {
	*err_type=RDCoreApplication::ErrorNoService;
      }
      return false;
    }
  }

  //
  // Open Database
  //
  if(!RDOpenDb(&schema,&db_err,app_config)) {
    *err_msg=QObject::tr("Unable to open database")+" ["+db_err+"]";
    return false;
  }
  if((RD_VERSION_DATABASE!=schema)&&(!skip_db_check)) {
    if(err_type!=NULL) {
      *err_type=RDCoreApplication::ErrorDbVersionSkew;
    }
    *err_msg=QObject::tr("Database version mismatch, should be")+
      QString().sprintf(" %u, ",RD_VERSION_DATABASE)+
      QObject::tr("is")+
      QString().sprintf(" %u",schema);
    return false;
  }
  app_heartbeat=new RDDbHeartbeat(app_config->mysqlHeartbeatInterval(),this);

  //
  // Open Accessors
  //
  app_station=new RDStation(app_config->stationName());
  app_system=new RDSystem();
  app_schemas=new RDRssSchemas();
  app_library_conf=new RDLibraryConf(app_config->stationName());
  app_logedit_conf=new RDLogeditConf(app_config->stationName());
  app_airplay_conf=new RDAirPlayConf(app_config->stationName(),"RDAIRPLAY");
  app_panel_conf=new RDAirPlayConf(app_config->stationName(),"RDPANEL");
  app_user=new RDUser();
  app_cae=new RDCae(app_station,app_config,this);
  app_ripc=new RDRipc(app_station,app_config,this);
  connect(app_ripc,SIGNAL(userChanged()),this,SLOT(userChangedData()));

  if(!app_station->exists()) {
    if(err_type!=NULL) {
      *err_type=RDCoreApplication::ErrorNoHostEntry;
    }
    *err_msg=QObject::tr("This host")+" (\""+app_config->stationName()+"\") "+
      QObject::tr("does not have a Hosts entry in the database.")+"\n"+
      QObject::tr("Open RDAdmin->ManageHosts->Add to create one.");
    return false;
  }

  return true; 
}


RDAirPlayConf *RDCoreApplication::airplayConf()
{
  return app_airplay_conf;
}


RDCae *RDCoreApplication::cae()
{
  return app_cae;
}


RDCmdSwitch *RDCoreApplication::cmdSwitch()
{
  return app_cmd_switch;
}


RDConfig *RDCoreApplication::config()
{
  return app_config;
}


RDLibraryConf *RDCoreApplication::libraryConf()
{
  return app_library_conf;
}


RDLogeditConf *RDCoreApplication::logeditConf()
{
  return app_logedit_conf;
}


RDAirPlayConf *RDCoreApplication::panelConf()
{
  return app_panel_conf;
}


RDRipc *RDCoreApplication::ripc()
{
  return app_ripc;
}


RDRssSchemas *RDCoreApplication::rssSchemas()
{
  return app_schemas;
}


RDStation *RDCoreApplication::station()
{
  return app_station;
}


RDSystem *RDCoreApplication::system()
{
  return app_system;
}


RDUser *RDCoreApplication::user()
{
  return app_user;
}


bool RDCoreApplication::dropTable(const QString &tbl_name)
{
  bool ret=false;

  QString sql=QString("show tables where ")+
    "Tables_in_"+config()->mysqlDbname()+"=\""+tbl_name+"\"";
  RDSqlQuery *q=new RDSqlQuery(sql);
  if(q->first()) {
    sql=QString("drop table `")+tbl_name+"`";
    RDSqlQuery *q1=new RDSqlQuery(sql);
    delete q1;
    ret=true;
  }
  delete q;

  return ret;
}


void RDCoreApplication::addTempFile(const QString &pathname)
{
  __rdapplication_temp_files.push_back(pathname);
}


void RDCoreApplication::logAuthenticationFailure(const QHostAddress &orig_addr,
					     const QString &login_name)
{
  if(login_name.isEmpty()) {
    syslog(LOG_NOTICE,"failed WebAPI login from %s",
	   orig_addr.toString().toUtf8().constData());
  }
  else {
    syslog(LOG_NOTICE,"failed WebAPI login from %s for user \"%s\"",
	   orig_addr.toString().toUtf8().constData(),
	   login_name.toUtf8().constData());
  }
}


void RDCoreApplication::syslog(int priority,const char *fmt,...) const
{
  va_list args;

  va_start(args,fmt);
  if((priority&248)==0) {  // So custom one-off facility numbers still work
    priority=priority|(app_config->syslogFacility()<<3);
  }
  vsyslog(priority,fmt,args);
  va_end(args);
}


void RDCoreApplication::syslog(RDConfig *config,int priority,const char *fmt,...)
{
  va_list args;

  va_start(args,fmt);
  if((priority&248)==0) {  // So custom one-off facility numbers still work
    priority=priority|(config->syslogFacility()<<3);
  }
  vsyslog(priority,fmt,args);
  va_end(args);
}


QString RDCoreApplication::exitCodeText(RDCoreApplication::ExitCode code)
{
  QString ret=tr("unknown")+QString().sprintf(" [%u]",code);

  switch(code) {
  case RDCoreApplication::ExitOk:
    ret=tr("ok");
    break;

  case RDCoreApplication::ExitPriorInstance:
    ret=tr("prior instance already running");
    break;

  case RDCoreApplication::ExitNoDb:
    ret=tr("unable to open database");
    break;

  case RDCoreApplication::ExitSvcFailed:
    ret=tr("unable to start a service component");
    break;

  case RDCoreApplication::ExitInvalidOption:
    ret=tr("unknown/invalid command option");
    break;

  case RDCoreApplication::ExitOutputProtected:
    ret=tr("unable to overwrite output [-P given]");
    break;

  case RDCoreApplication::ExitNoSvc:
    ret=tr("no such service");
    break;

  case RDCoreApplication::ExitNoLog:
    ret=tr("no such log");
    break;

  case RDCoreApplication::ExitNoReport:
    ret=tr("no such report");
    break;

  case RDCoreApplication::ExitLogGenFailed:
    ret=tr("log generation failed");
    break;

  case RDCoreApplication::ExitLogLinkFailed:
    ret=tr("schedule import failed");
    break;

  case RDCoreApplication::ExitNoPerms:
    ret=tr("insufficient permissions");
    break;

  case RDCoreApplication::ExitReportFailed:
    ret=tr("report generation failed");
    break;

  case RDCoreApplication::ExitImportFailed:
    ret=tr("one or more audio imports failed");
    break;

  case RDCoreApplication::ExitNoDropbox:
    ret=tr("unknown dropbox id");
    break;

  case RDCoreApplication::ExitNoGroup:
    ret=tr("no such group");
    break;

  case RDCoreApplication::ExitInvalidCart:
    ret=tr("invalid cart number");
    break;

  case RDCoreApplication::ExitNoSchedCode:
    ret=tr("no such scheduler code");
    break;

  case RDCoreApplication::ExitBadTicket:
    ret=tr("bad ticket");
    break;

  case RDCoreApplication::ExitLast:
    break;
  }

  return ret;
}


void RDCoreApplication::userChangedData()
{
  QString sql;
  RDSqlQuery *q=NULL;

  if(app_ticket.isEmpty()) {
    app_user->setName(app_ripc->user());
    emit userChanged();
    return;
  }
  QStringList f0=app_ticket.split(":");
  if(f0.size()==2) {
    sql=QString("select ")+
      "LOGIN_NAME "+  // 00
      "from WEBAPI_AUTHS where "+
      "TICKET=\""+RDEscapeString(f0.at(0))+"\" && "+
      "IPV4_ADDRESS=\""+RDEscapeString(f0.at(1))+"\" && "+
      "EXPIRATION_DATETIME>now()";
    q=new RDSqlQuery(sql);
    if(q->first()) {
      app_user->setName(q->value(0).toString());
      emit userChanged();
      delete q;
      return;
    }
    delete q;
  }
  fprintf(stderr,"%s: %s\n",
	  QString(qApp->arguments().at(0)).split("/",QString::SkipEmptyParts).last().toUtf8().constData(),
	  RDCoreApplication::exitCodeText(RDCoreApplication::ExitBadTicket).
	  toUtf8().constData());
  exit(RDCoreApplication::ExitBadTicket);
}


 bool RDCoreApplication::CheckService(QString *err_msg)
{
  bool ret=false;
  int trial=config()->serviceTimeout();

  if(trial<=0) {
    trial=1;
  }
  while((!ret)&&(trial>0)) {
    QStringList args;
    QProcess *proc=new QProcess(this);

    args.push_back("--property");
    args.push_back("ActiveState");
    args.push_back("show");
    args.push_back("rivendell");
    proc->start("systemctl",args);
    proc->waitForFinished();
    if(proc->exitStatus()!=QProcess::NormalExit) {
      *err_msg=tr("systemctl(1) crashed.");
    }
    else {
      if(proc->exitCode()!=0) {
	*err_msg=tr("systemctl(1) returned exit code")+
	  QString().sprintf(" %d:\n",proc->exitCode())+
	  proc->readAllStandardError();
      }
      else {
	*err_msg=tr("Rivendell service is not active.");
	QStringList f0=QString(proc->readAllStandardOutput()).
	  split("\n",QString::SkipEmptyParts);
	for(int i=0;i<f0.size();i++) {
	  QStringList f1=f0.at(i).trimmed().split("=");
	  if((f1.size()==2)&&(f1.at(0)=="ActiveState")) {
	    ret=f1.at(1).toLower()=="active";
	    if(ret) {
	      *err_msg=tr("OK");
	    }
	  }
	}
      }
    }
    delete proc;

    trial--;
    sleep(1);
  }

  return ret;
}
