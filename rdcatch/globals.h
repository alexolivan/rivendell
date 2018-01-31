// globals.h
//
// Global Variable Declarations for RDCatch
//
//   (C) Copyright 2002-2005,2016-2018 Fred Gleason <fredg@paravelsystems.com>
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

#ifndef GLOBALS_H
#define GLOBALS_H

#include <rdaudio_port.h>
#include <rdcart_dialog.h>

//
// Global Resources
//
extern RDAudioPort *rdaudioport_conf;
extern RDCartDialog *catch_cart_dialog;
extern int catch_audition_card;
extern int catch_audition_port;


#endif  // GLOBALS_H
