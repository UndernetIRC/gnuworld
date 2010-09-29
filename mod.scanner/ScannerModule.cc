/**
 * ScannerModule.cc
 * Copyright (C) 2002 Daniel Karrels (dan@karrels.com)
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License     
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of   
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307,
 * USA.
 * 
 * $Id: ScannerModule.cc,v 1.1 2002/08/08 21:31:45 dan_karrels Exp $
 */

#include	"ConnectionHandler.h"
#include	"ConnectionManager.h"
#include	"ScannerModule.h"


namespace gnuworld
{

ScannerModule::ScannerModule( ConnectionManager* cm, scanner* theScanner )
	: cm( cm ), theScanner( theScanner )
{}

ScannerModule::~ScannerModule()
{}

} // namespace gnuworld
