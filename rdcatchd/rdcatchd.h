// rdcatchd.h
//
// The Rivendell Netcatcher.
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

#ifndef RDCATCHD_H
#define RDCATCHD_H

#define XLOAD_UPDATE_INTERVAL 1000
#define RDCATCHD_USAGE "[-d][--event-id=<id>]\n\nOptions:\n\n-d\n     Set 'debug' mode, causing rdcatchd(8) to stay in the foreground\n     and print debugging info on standard output.\n\n--event-id=<id>\n     Execute event <id> and then exit.\n\n" 

#include <vector>
#include <list>

#include <qlist.h>
#include <qobject.h>
#include <qstring.h>
#include <qtcpserver.h>
#include <qsignalmapper.h>
#include <qtimer.h>
#include <qhostaddress.h>
#include <qsignalmapper.h>

#include <rd.h>
#include <rdcart.h>
#include <rdcatch_conf.h>
#include <rdcatch_connect.h>
#include <rdcmd_switch.h>
#include <rddeck.h>
#include <rdmacro_event.h>
#include <rdsocket.h>
#include <rdsettings.h>
#include <rdtimeengine.h>
#include <rdtty.h>

#include "catch_event.h"
#include "event_player.h"

//
// Global RDCATCHD Definitions
//
#define RDCATCHD_GPO_INTERVAL 333
#define RDCATCHD_MAX_MACROS 64
#define RDCATCHD_FREE_EVENTS_INTERVAL 1000
#define RDCATCHD_HEARTBEAT_INTERVAL 10000
#define RDCATCHD_ERROR_ID_OFFSET 1000000

class ServerConnection
{
 public:
  ServerConnection(int id,QTcpSocket *sock);
  ~ServerConnection();
  int id() const;
  bool isAuthenticated() const;
  void setAuthenticated(bool state);
  bool meterEnabled() const;
  void setMeterEnabled(bool state);
  QTcpSocket *socket();
  bool isClosing() const;
  void close();
  QString accum;

 private:
  int conn_id;
  bool conn_authenticated;
  bool conn_meter_enabled;
  QTcpSocket *conn_socket;
  bool conn_is_closing;
};




class MainObject : public QObject
{
  Q_OBJECT
 public:
  MainObject(QObject *parent=0);

 private slots:
  //
  // rdcatchd.cpp
  //
  void newConnectionData();
  void rmlReceivedData(RDMacro *rml);
  void gpiStateChangedData(int matrix,int line,bool state);
  void startTimerData(int id);
  void offsetTimerData(int id);
  void engineData(int);
  void socketReadyReadData(int conn_id);
  void socketKillData(int conn_id);
  void garbageData();
  void isConnectedData(bool state);
  void recordLoadedData(int card,int stream);
  void recordingData(int card,int stream);
  void recordStoppedData(int card,int stream);
  void recordUnloadedData(int card,int stream,unsigned msecs);
  void playLoadedData(int handle);
  void playingData(int handle);
  void playStoppedData(int handle);
  void playUnloadedData(int handle);
  void runCartData(int chan,int number,unsigned cartnum);
  void meterData();
  void eventFinishedData(int id);
  void freeEventsData();
  void heartbeatData();
  void sysHeartbeatData();
  void updateXloadsData();
  void startupCartData();
  void notificationReceivedData(RDNotification *notify);

  //
  // batch.cpp
  //
  void catchConnectedData(int serial,bool state);
  void userChangedData();
  void exitData();

 private:
  //
  // batch.cpp
  //
  void RunBatch(RDCmdSwitch *cmd);
  void RunImport(CatchEvent *evt);
  void RunDownload(CatchEvent *evt);
  void RunUpload(CatchEvent *evt);
  CatchEvent *batch_event;
  bool Export(CatchEvent *evt);
  QString GetExportCmd(CatchEvent *evt,QString *tempname);
  bool Import(CatchEvent *evt);
  QString GetImportCmd(CatchEvent *evt,QString *tempname);

  //
  // rdcatchd.cpp
  //
  bool StartRecording(int event);
  void StartPlayout(int event);
  void StartMacroEvent(int event);
  void StartSwitchEvent(int event);
  void StartDownloadEvent(int event);
  void StartUploadEvent(int event);
  bool ExecuteMacroCart(RDCart *cart,int id=-1,int event=-1);
  void SendFullStatus(int ch);
  void SendMeterLevel(int deck,short levels[2]);
  void SendDeckEvent(int deck,int number);
  void ParseCommand(int);
  void DispatchCommand(ServerConnection *conn);
  void EchoCommand(int,const QString &cmd);
  void BroadcastCommand(const QString &cmd,int except_ch=-1);
  void EchoArgs(int,const char);
  void LoadEngine(bool adv_day=false);
  QString LoadEventSql();
  void LoadEvent(RDSqlQuery *q,CatchEvent *e,bool add);
  void LoadDeckList();
  int GetRecordDeck(int card,int stream);
  int GetPlayoutDeck(int handle);
  int GetFreeEvent();
  bool AddEvent(int id);
  void RemoveEvent(int id);
  bool UpdateEvent(int id);
  int GetEvent(int id);
  void PurgeEvent(int event);
  void LoadHeartbeat();
  void CheckInRecording(QString cutname,CatchEvent *evt,unsigned msecs,
			unsigned threshold);
  void CheckInPodcast(CatchEvent *e) const;
  RDRecording::ExitCode ReadExitCode(int event);
  void WriteExitCode(int event,RDRecording::ExitCode code,
		     const QString &err_text="");
  void WriteExitCodeById(int id,RDRecording::ExitCode code,
			 const QString &err_text="");
  QString BuildTempName(int event,QString str);
  QString BuildTempName(CatchEvent *evt,QString str);
  QString GetFileExtension(QString filename);
  bool SendErrorMessage(CatchEvent *event,const QString &err_desc,QString rml);
  void ResolveErrorWildcards(CatchEvent *event,const QString &err_desc,
			     QString *rml);
  void RunLocalMacros(RDMacro *rml);
  unsigned GetNextDynamicId();
  void RunRmlRecordingCache(int chan);
  void StartRmlRecording(int chan,int cartnum,int cutnum,int maxlen);
  void StartBatch(int id);
  void SendNotification(RDNotification::Type type,RDNotification::Action,
			const QVariant &id);
  QString GetTempRecordingName(int id) const;
  QString catch_default_user;
  QString catch_host;
  bool debug;
  RDTimeEngine *catch_engine;
  int16_t tcp_port;
  QTcpServer *server;
  RDCatchConnect *catch_connect;

  QList<ServerConnection *> catch_connections;
  QSignalMapper *catch_ready_mapper;
  QSignalMapper *catch_kill_mapper;
  QTimer *catch_garbage_timer;
  bool catch_record_status[MAX_DECKS];
  int catch_record_card[MAX_DECKS];
  int catch_record_stream[MAX_DECKS];
  RDDeck::Status catch_record_deck_status[MAX_DECKS];
  int catch_record_id[MAX_DECKS];
  QString catch_record_name[MAX_DECKS];
  bool catch_record_aborting[MAX_DECKS];

  unsigned catch_record_pending_cartnum[MAX_DECKS];
  unsigned catch_record_pending_cutnum[MAX_DECKS];
  unsigned catch_record_pending_maxlen[MAX_DECKS];

  bool catch_playout_status[MAX_DECKS];
  int catch_playout_card[MAX_DECKS];
  int catch_playout_stream[MAX_DECKS];
  int catch_playout_port[MAX_DECKS];
  int catch_playout_handle[MAX_DECKS];
  RDDeck::Status catch_playout_deck_status[MAX_DECKS];
  int catch_playout_event_id[MAX_DECKS];
  int catch_playout_id[MAX_DECKS];
  QString catch_playout_name[MAX_DECKS];
  EventPlayer *catch_playout_event_player[MAX_DECKS];

  int catch_monitor_port[MAX_DECKS];
  bool catch_monitor_state[MAX_DECKS];
  
  unsigned catch_record_threshold[MAX_DECKS];
  QHostAddress catch_swaddress[MAX_DECKS];
  int catch_swmatrix[MAX_DECKS];
  int catch_swoutput[MAX_DECKS];
  int catch_swdelay[MAX_DECKS];
  QSignalMapper *catch_gpi_start_mapper;
  QSignalMapper *catch_gpi_offset_mapper;
  uid_t catch_uid;
  gid_t catch_gid;
  bool catch_event_free[RDCATCHD_MAX_MACROS];
  RDMacroEvent *catch_event_pool[RDCATCHD_MAX_MACROS];
  int catch_macro_event_id[RDCATCHD_MAX_MACROS];
  QSignalMapper *catch_event_mapper;
  std::vector<CatchEvent> catch_events;
  QTimer *catch_heartbeat_timer;
  unsigned catch_heartbeat_cart;

  int catch_default_format;
  int catch_default_channels;
  int catch_default_layer;
  int catch_default_bitrate;
  int catch_ripper_level;
  std::vector<int> catch_active_xloads;
  QTimer *catch_xload_timer;
  QString catch_temp_dir;
  RDCatchConf *catch_conf;
};


#endif  // RDCATCHD_H
