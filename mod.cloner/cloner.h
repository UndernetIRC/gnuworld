/**
 * cloner.h
 * Load fake clones for testing or fun.
 *
 * Copyright (C) 2002 Daniel Karrels <dan@karrels.com>
 *		      Reed Loden <reed@reedloden.com>
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
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307,
 * USA.
 *
 * $Id: cloner.h,v 1.10 2004/06/04 20:17:23 jeekay Exp $
 */

#ifndef __CLONER_H
#define __CLONER_H "$Id: cloner.h,v 1.10 2004/06/04 20:17:23 jeekay Exp $"

#include	<string>
#include	<vector>
#include	<list>

#include	<ctime>

#include	"client.h"
#include	"iClient.h"
#include	"iServer.h"

namespace gnuworld
{

class cloner : public xClient
{

typedef std::list< std::string > channelListType ;

public:
  cloner( const std::string& configFileName ) ;
  virtual ~cloner() ;

  virtual void                OnAttach() ;
  virtual void                OnConnect() ;
  virtual void                OnDetach( const string& ) ;
  virtual void                OnTimer( const xServer::timerID&, void* ) ;
  virtual void                OnPrivateMessage( iClient*, const std::string&,
                                bool secure = false ) ;
  virtual void                OnNetworkKick( Channel*, iClient*,
                                iClient*, const string&, bool ) ;
private:

  virtual void                addClone() ;
  virtual size_t              joinClone( iClient*, Channel* ) ;
  virtual size_t              joinClone( const size_t, Channel* ) ;
  virtual size_t              partClone( iClient*, Channel*, const string ) ;
  virtual size_t              partClone( const size_t, Channel*, const string ) ;
  virtual size_t              quitClone( const size_t, const string ) ;
  virtual size_t              quitClone( iClient*, const string ) ;
  virtual bool                banMatch( const Channel*, const iClient* ) ;

  gnuworld::xServer::timerID	loadCloneTimer ;
  gnuworld::xServer::timerID	cycleCloneTimer = 0 ;
  gnuworld::xServer::timerID	delayJoinTimer = 0 ;

  virtual bool                hasAccess( const std::string& ) const ;

  virtual std::string         randomNick( int minLength = 5,
                                int maxLength = 9 ) ;
  virtual std::string         randomUser() ;
  virtual std::string         randomHost() ;
  virtual std::string         randomAccount() ;
  virtual char                randomChar() ;
  virtual iClient*            randomClone() ;

  struct cloneSettings {
    bool ident = false ;
    bool modeO = false ;
    bool modeK = false ;
    bool modeR = false ;
    bool modeX = false ;
    channelListType channels = {} ;
  } ;

  std::vector< struct cloneSettings > cloneQueue ;

  bool                        cycleRun = false ;
  float                       cyclePercentage = 0 ;
  unsigned int                cycleTime = 0 ;

  std::list< std::string >    allowAccess ;
  std::vector< std::string >  userNames ;
  std::vector< std::string >  hostNames ;
  std::vector< std::pair
    < std::string, int > >    accountNames ;

  typedef std::vector
            < iClient* >      cloneVectorType ;
  cloneVectorType             clones ;

  iServer*                    fakeServer = nullptr ;

  bool                        allowOpers ;

  size_t                      cloneBurstCount ;
  size_t                      minNickLength ;
  size_t                      maxNickLength ;
  size_t                      delayCount = 0 ;
  size_t                      joinCloneCount = 0 ;

  std::string                 cloneDescription ;
  std::string                 cloneMode ;
  std::string                 fakeServerName ;
  std::string                 fakeServerDescription ;

  Channel*                    delayChan = nullptr ;
} ;

} // namespace gnuworld

#endif // __CLONER_H
