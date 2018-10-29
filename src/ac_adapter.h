/*---------------------------------------------------------------------------*/
/* Program to get/set some ACPI settings                                     */
/* Author : David Leemans   -   http://freeunix.dyndns.org:8000/             */
/* Last update : 22-07-2008                                                  */
/*---------------------------------------------------------------------------*/
/*                                                                           */
/* ac_adapter.h                                                              */
/*                                                                           */		
/* Copyright (C) 2008                                                        */
/* David Leemans <davidleemans AT scarlet DOT be>	                     */
/*                                                                           */
/* This program is free software; you can redistribute it and/or modify      */
/* it under the terms of the GNU General Public License as published by      */
/* the Free Software Foundation; either version 2 of the License, or         */
/* (at your option) any later version.                                       */
/*                                                                           */
/* This program is distributed in the hope that it will be useful,           */
/* but WITHOUT ANY WARRANTY; without even the implied warranty of            */
/* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             */
/* GNU General Public License for more details.                              */
/*                                                                           */
/* You should have received a copy of the GNU General Public License         */
/* along with this program; if not, write to the Free Software               */
/* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA */
/*---------------------------------------------------------------------------*/

#ifndef AC_ADAPTER_H
#define AC_ADAPTER_H

int Do_AC_Info(int verbose);

int Do_AC_Info_Proc(int verbose);

int Do_AC_Info_Sys();

#endif
