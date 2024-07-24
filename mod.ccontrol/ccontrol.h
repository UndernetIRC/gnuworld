/**
 * ccontrol.h
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
 * $Id: ccontrol.h,v 1.118 2009/12/24 20:19:14 hidden1 Exp $
 */

#ifndef __CCONTROL_H
#define __CCONTROL_H "$Id: ccontrol.h,v 1.118 2009/12/24 20:19:14 hidden1 Exp $"

//Undef this if you want to log to the database
#define LOGTOHD

// If a server is >MAX_LAG_TIME ms lagged, report it to opers who have GetLag=ON set
#define MAX_LAG_TIME 45
// Minimum Interval in seconds between lag reports for those with GetLag=ON
#define LAG_REPORT_INTERVAL 300
// Maximum time diff tolerated for a server's clock (for reporting in MsgChanLog)
#define MAX_TIME_DIFF 120
// Minimum Interval in seconds between Time diff reports
#define TIMEDIFF_REPORT_INTERVAL 43200
// Maximum time diff tolerated for a server's clock (for issuing a SETTIME)
#define MAX_TIME_DIFF_FOR_SETTIME 4
// Minimum Interval in seconds between SETTIMES for bad clocks
#define TIMEDIFF_SETTIME_INTERVAL 1800
// If a server is >MAX_LAG_TIME_FOR_SETTIME ms lagged, do NOT send SETTIME
#define MAX_LAG_TIME_FOR_SETTIME 3000



#include	<pthread.h>

#include	<string>
#include	<vector>
#include	<map>
#include        <iomanip>
#include	<queue>

#include	<cstdio>
#include	<cstdarg>

#include	"client.h"
#include	"iClient.h"
#include	"server.h"
#include	"CControlCommands.h"
#include	"Channel.h"
#include	"dbHandle.h"
#include        "match.h"
#include	"md5hash.h" 
#include        "ccUser.h"
#include        "ccGline.h"
#include        "ccServer.h"
#include 	"ccFloodData.h"
#include	"ccException.h"
#include        "server.h"
#include	"CommandsDec.h"
#include 	"ccBadChannel.h"
#include	"defs.h"

#ifdef LOGTOHD
    #include "ccLog.h"
#endif

namespace gnuworld
{

using std::map ;
using std::string ;
using std::vector ;

namespace uworld
{

using namespace std;
/*
 *  Sublcass the postgres API to create our own accessor
 *  to get at the PID information.
 */
/*
class cmDatabase : public dbHandle
{
public:
	cmDatabase(const string& conninfo)
	 : dbHandle(conninfo.c_str()) {}
	virtual ~cmDatabase() {}

};
*/

/// Forward declaration of command handler class
class Command ;

/**
 * The ccontrol client class.  This is an operator service for network
 * operations.
 */
class ccontrol : public xClient
{

protected:

	/**
	 * The type used to store the client's command handlers.
	 */
	typedef map< string, Command*, noCaseCompare > commandMapType ;

	/**
	 * The value_type of the command table type.
	 * This type will be used to model key/value pairs
	 * for the command handler table.
	 */
	typedef commandMapType::value_type pairType ;

	/**
	 * The db clients map
	 */
	typedef map<string ,ccUser* ,noCaseCompare> usersMapType;

	/**
	 * Holds the authenticated user list
	 */
	usersMapType			usersMap ;

	/**
	 * A mutable iterator to the usersMap.
	 */
	typedef usersMapType::iterator     usersIterator;
	
	/**
	 * The db accounts map
	 */
	typedef map<string ,ccUser* ,noCaseCompare> accountsMapType;

	/**
	 * Holds the authenticated user list
	 */
	accountsMapType			accountsMap ;

	/**
	 * A mutable iterator to the accountsMap.
	 */
	typedef accountsMapType::iterator     accountsIterator;
	
	/**
	 * The db servers
	 */
	typedef map<string, ccServer*, noCaseCompare> serversMapType;

	/**
	 * Holds the servers map
	 */
	serversMapType			serversMap ;

	/**
	 * A mutable iterator to the serversMap.
	 */
	typedef serversMapType::iterator     serversIterator;

	/**
	 * Type used to hold the gline list.
	 */
	typedef map< string, ccGline* >        glineListType ;

	typedef map< string, string >		opersIPMapType;
	
	/**
	 * Holds the glines
	 */
	glineListType			glineList ;

	glineListType			rnGlineList;
	
	// This is the list of glines sent that match an exception
	// The oper has to send the gline command twice in order to set the G:
	typedef list< pair< string,int > >	glinedExceptionListType ;
	glinedExceptionListType glinedExceptionList;

	typedef pair< ccGline* , bool > glineQueueDataType;
	
	typedef list< glineQueueDataType > glineQueueType;
	 
	glineQueueType			glineQueue;
	
	typedef list< ccFloodData* >	ignoreListType;
	
	ignoreListType			ignoreList;
	
	typedef std::list< string >    stringListType;
	
	typedef vector< ccIpLisp* >    ipLispVectorType;
	
	ipLispVectorType		ipLispVector;

	typedef vector< pair<int, ccIpLnb*> >    ipLnbVectorType;
	
	ipLnbVectorType		ipLnbVector;
		
	//typedef map<ccIpLisp*, int>	ipLispMapType;

	//ipLispMapType		ipLispMap;

	//typedef map<ccIpLnb*, int>	ipLnbMapType;

	//ipLnbMapType		ipLnbMap;

	typedef map<string, int>	ipLclonesMapType;

	//ipLclonesMapType		ipLclonesMap;

	typedef list< ccIpLnb* >    ipLnbListType;

	typedef map<string, ipLnbListType>	ipLnumericMapType;

	ipLnumericMapType		ipLnumericMap;

	typedef pair<iClient*, time_t>	ipLRecentIauthPairType;

	typedef list<ipLRecentIauthPairType>	ipLRecentIauthListType;

	ipLRecentIauthListType		ipLRecentIauthList;

	typedef pair<ccIpLnb*, int>	ipLretPairType;

	typedef list<ipLretPairType>	ipLretPairListType;

	struct ipLretStruct {
		ccIpLnb *nb;
		int count;
		int limit;
		char type;
		string mask;
	};
	
	typedef struct ipLretStruct ipLretStructType;

	typedef list<ipLretStructType>	ipLretStructListType;

	typedef map<string,long> 	clientsIpMapType;
	
	typedef clientsIpMapType::iterator clientsIpIterator;
	
	typedef list<string>		allowedVersionsType;
	
#ifdef LOGTOHD
	typedef list<ccLog*>		ccLogList;

	typedef ccLogList::iterator 	ccLogIterator;
#endif

	typedef map<string , ccBadChannel* , noCaseCompare> badChannelsMapType;
	
	typedef badChannelsMapType::iterator badChannelsIterator;

	typedef map<string,int>	timediffServersMapType;
	
	timediffServersMapType timediffServersMap;

	iServer* myHub;

	ccServer* ccHub;

	typedef pair< unsigned int, string> levelMapperType;
	vector<levelMapperType> levelMapper;

public:

	/**
	 * Default, and only constructor, receives the name
	 * of the configuration file for this client.
	 */
	ccontrol( const string& configFileName ) ;

	/**
	 * Destructor cleans up any allocated memory, etc.
	 */
	virtual ~ccontrol() ;

	/**
	 * This method is called by the xServer when it wants information
	 * about the channels this client will be on.
	 */
	virtual void BurstChannels() ;

	/**
	 * This method is called by the xServer when it wants information
	 * about the glines this client has in its database;
	 */
	virtual bool BurstGlines() ;
	
	/**
	 * This method is invoked each time the client is sent a
	 * PRIVMSG.
	 */
	virtual void OnPrivateMessage( iClient*, const string&,
			bool secure = false ) ;

	virtual void OnServerMessage( iServer*, const string&,
			bool secure = false ) ;

	bool Notice( const iClient* Target, const string& Message );
         
	bool Notice( const iClient* Target, const char* Message, ... );

	virtual void OnCTCP( iClient* ,
                const string& ,
                const string&,
		bool Secure = false ) ;

	/**
	 * This method is invoked each time a network event occurs.
	 */
	virtual void OnEvent( const eventType&,
		void* = 0, void* = 0,
		void* = 0, void* = 0 ) ;

	/**
	 * This method is invoked each time a channel event occurs
	 * for one of the channels for which this client has registered
	 * to receive channel events.
	 */
	virtual void OnChannelEvent( const channelEventType&,
		Channel*,
		void* = 0, void* = 0,
		void* = 0, void* = 0 ) ;

	virtual void OnTimer(const gnuworld::xServer::timerID&, void*);
	
	virtual void OnConnect();
	
	/**
	 * This method is called once this client has been attached
	 * to an xServer.  This method is overloaded because the
	 * command handlers each require a reference to the xServer
	 * for efficiency.
	 */
	virtual void OnAttach() ;

	/**
	 * This method is called duringg shutdown.  It unloads the
	 * client correctly during a shutdown.
	 */
	virtual void OnShutdown(const std::string& reason);

	/**
	 * Return true if the given channel name corresponds to a
	 * channel which is an IRCoperator only channel.
	 */
	bool isOperChan( const string& chanName ) const ;

	/**
	 * Return true if the given channel corresponds to a
	 * channel which is an IRCoperator only channel.
	 */
	bool isOperChan( const Channel* theChan ) const ;

	/**
	 * Return true if this client is on the given channel.
	 */
	virtual bool isOnChannel( const string& chanName ) const
		{ return isOperChan( chanName ) ; }

	/**
	 * Return true if this client is on the given channel.
	 */
	virtual bool isOnChannel( const Channel* theChan ) const
		{ return isOperChan( theChan->getName() ) ; }

	/**
	 * This method will kick the given user from the given channel
	 * for the given reason (arg 3).
	 */
//	virtual bool Kick( Channel*, iClient*, const string& ) ;

	/**
	 * This method will cause the bot to join the channel with
	 * the given name, if the client is not already on that
	 * channel.
	 */
	virtual bool Join( const string&, const string& = string(),
			const time_t& = 0, bool = false) ;

	/**
	 * This method will cause this client to part the given channel,
	 * if it is already on that channel.
	 */
	virtual bool Part( const string& , const string& = string()) ;

	/**
	 * This method will register a given command handler, removing
	 * (and deallocating) the existing handler for this command,
	 * should one exist.
	 */
	virtual bool RegisterCommand( Command* ) ;

	/**
	 * This method will unregister the command handler for the command
	 * of the given command name, deallocating the object from the
	 * heap as well.
	 */
	virtual bool UnRegisterCommand( const string& ) ;

	/**
	 * This method will add the given channel name as an IRC operator
	 * only channel.
	 * This method will cause the client to join the channel and
	 * kick all non-IRCops if it is not already on the channel.
	 */
	virtual bool addOperChan( const string& chanName ) ;

	/**
	 * This method will add the channel with the given name as an IRC
	 * operator only channel for the given reason.
	 * This method will cause the client to join the channel and
	 * kick all non-IRCops if it is not already on the channel.
	 */
	virtual bool addOperChan( const string& chanName, const string& reason ) ;

	/**
	 * This method will remove the channel with the given name from the
	 * list of IRCoperator only channels.
	 * This method will cause the client to part from the channel, if it
	 * is already in the channel.
	 */
	virtual bool removeOperChan( const string& ) ;

	void handleNewClient( iClient* );

	void handleAC( iClient* );

	void OkAuthUser(iClient* , ccUser* );
	
	void isNowAnOper ( iClient* );

	bool isIpOfOper ( const string& );

	void refreshOpersIPMap();

	void announce ( iClient* , const string& );

	string getLastNUHOfOperFromIP ( const string& );
	
	void addGlineToUplink( ccGline* );
	
	int strToLevel(const string&);
	
	const string& levelToStr(unsigned int level);
	/**
	 * This method will check if a client is authenticated 
	 * based on the iClient structure
	 */
	ccUser* IsAuth( const iClient* theClient ) ;

	/**
	 * This method will check if a client is authenticated 
	 * based on a nick
	 */
        ccUser *IsAuth( const string& );

       	/**
	 * This method will add a new oper to the database
	 */
	bool AddOper( ccUser* );
	
	/**
	 * This method will delete an oper from the database
	 * based on the a handle
	 */
	bool DeleteOper( const string& );
	
	/**
	 * This method will mark the client as authenticated
	 */
	bool AuthUser( ccUser* ,iClient* );
	
	/**
	 * This method will deauthenticate all clients with the bot for a user
	 */
	bool deAuthUser( ccUser* );

	/**
	 * This method will deauthenticate a specific client with the bot for a user
	 */
	bool deAuthUser( ccUser*, iClient* );

	/**
	 * This method will return the access flag needed for a command
	 */
	int getCommandLevel( const string& );

	/**
	 * This method will check if a user got a corrisponding
	 * host mask in the databse
	 */
	bool UserGotMask( ccUser* , const string& );

	/**
	 * This method will check if a user got a corrisponding
	 * host in the databse
	 */
	bool UserGotHost( ccUser* , const string& );

	/**
	 * This method will crypt a string based on the md5 hash
	 */
	static string CryptPass( const string& );

	/**
	 * This method will add a host to a client
	 */
        bool AddHost( ccUser* , const string& host );

	/**
	 * This method will delete a host from  aclient
	 */
        bool DelHost( ccUser* , const string& host );
	
	/**
	 * This method lists all the hosts a client got
	 */
	bool listHosts( ccUser* , iClient* );
	
	/**
	 * This method will update a client authenticate info
	 * based on the ccUser structure (called after update)
	 */
	void UpdateAuth( ccUser* );

	/**
	 * This method will get a help entry for a command from the database
	 */
	bool GetHelp( iClient* , const string& );

	/**
	 * This method will get a help entry for a command and its subcommand
	 * from the database
	 */
	bool GetHelp( iClient* , const string& , const string&);
	
	/**
	 * This method will post help for a client
	 */
	void DoHelp( iClient* );

	/**
	 * This method will replace a word in a string with another one
	 */
	static string replace( const string&,
				const string&,
				const string& ) ;
	
	/**
	 * This method will attempt to load an oper info from the database
	 * based on the handle
	 */
	ccUser *GetOper( const string );

	/**
	 * This method will attempt to load an oper info from the database
	 * based on the user account
	 */
	ccUser *GetOperByAC( const string& );

	/**
	 * This method will add an account to accountsMap
	 */
	bool accountsMapAdd (ccUser*, const string&);

	/**
	 * This method will remove an account from accountsMap
	 */
	bool accountsMapDel (const string&);

	/**
	 * This method will attempt to load an oper info from the database
	 * based on the user id
	 */
	ccUser *GetOper( unsigned int );

	/**
	 * This method add a gline to the database
	 */
	bool addGline( ccGline* );

	/**
	 * This method deletes a gline from the database
	 */
	bool remGline( ccGline* );

	ccGline* findMatchingGline( const iClient* );

	ccGline* findMatchingRNGline( const iClient* );

	//TODO Eventually merge these three functions, update it's dependencies
	ccGline* findGline( const string& );

	ccGline* findRealGline( const string& );

	ccGline* findGlineAndRealGline( const string& );

	vector< ccGline* > findAllMatchingGlines(const string& );

	/*
	 * Remove matching glines from mod.ccontrol's glines list, from 'gnuworld server' gline list
	 * and also send gline remove messages to the network.
	 */
	bool removeAllMatchingGlines( const string& , bool );

	/**
	 * This method logs the bot commands to the message channel
	 */
	bool MsgChanLog( const char * , ... ) ;

	bool MsgChanLag( const char * , ... ) ;

	bool MsgChanLimits( const char * , ... ) ;

	/**
	 * This method logs the commands to the database 
	 * for lastcom report
	 */
#ifndef LOGTOHD
	bool DailyLog( ccUser * , const char *, ... );

	bool DailyLog( iClient * , const char *, ... );
#else
	bool DailyLog(ccLog*);
#endif
	
	/**
	 * This method convers a unix time to ascii time
	 */
	char *convertToAscTime(time_t);	

	/**
	 * This method converts a unix time to tm structure
	 */
	struct tm convertToTmTime(time_t );

	/**
	 * This method creates a lastcom report between two dates
	 */
	bool CreateReport(time_t , time_t);	

	/**
	 * This method emails the lastcom report
	 */
	bool MailReport(const char *, const char *);

	/*
	 * checkGline functions for IPv4 and hostnames
	 */
	int checkGline4(string &,unsigned int ,unsigned int &);

	int checkSGline4(string &,unsigned int ,unsigned int &);

	//Checkgline functions only for IPv6
	int checkGline6(string &,unsigned int ,unsigned int &);

	int checkSGline6(string &,unsigned int ,unsigned int &);
	/**
	 * This method checks the gline paramerters for valid time/host
	 */
	int checkGline(string &,unsigned int ,unsigned int &);

	int checkSGline(string &,unsigned int ,unsigned int &);

	bool isSuspended(ccUser *);
	
	bool refreshVersions();
	
	bool refreshSuspention();
	
	bool refreshGlines();

	bool refreshIauthEntries();
	
	void queueGline(ccGline* , bool = true);
	
	bool processGlineQueue();
	
	bool loadGlines();

	bool loadUsers();
	
	bool loadServers();
	
	bool loadMaxUsers();
	
	bool loadVersions();
	
	bool loadBadChannels();
	
	void loadCommands();
	
	bool loadMisc();
	
	void wallopsAsServer(const char * , ... );

	int isGlinedException( const string & );

	void addGlinedException( const string & );
	
	bool isValidCidr( const string & );

	bool getValidCidr( const string &, string &  );
	
	bool isCidrMatch( const string & , const string & );
	
	ccIpLisp* getIpLisp( const string & );

	ccIpLisp* getIpLispbyID( const int & );

	ccIpLnb* getIpLnb( const string &, const string & );
        
	bool listIpLExceptionsOld( iClient *, const string &, bool );
	bool listIpLExceptions( iClient * );

	//bool isIpLClientAllowed( iClient *, ipLretPairListType &, bool);
	bool isIpLClientAllowed( iClient *, ipLretStructListType &, bool);

	bool ipLDropClient( iClient * );

	bool reloadIpLisp( iClient *, ccIpLisp* );

	bool insertIpLisp( iClient * , const string& , int, int, const string &, int, int );

	bool insertIpLnb( iClient * , const string & , int, bool );

	bool delIpLnb( iClient * , const string &, const string&, bool );

	bool delIpLisp( iClient * , const string & );

	bool clearIsps( iClient * );

	bool ipLcidrChangeCheck(iClient *, ccIpLisp *, int);

	bool ipLuserInfo( iClient *, iClient * );

	size_t iauthXQCheck(iServer*, const string&, const string&);

	ccFloodData *findLogin( const string & );

	void removeLogin( ccFloodData * );

	void addLogin( iClient* );
	
	void addFloodData(iClient*, unsigned int);
	
	int removeIgnore( const string & );
	
	int removeIgnore( iClient * );

	void ignoreUser( ccFloodData * );

	bool listIgnores( iClient * );
	
	bool refreshIgnores();
	
	bool loadExceptions();
	
	void listGlines( iClient *, string Mask = "*" );

	void listSuspended( iClient * );
	
	bool updateCommand ( Command* );
	
	Command* findRealCommand( const string& );
	
	Command* findCommandInMem( const string& );

	bool UpdateCommandFromDb ( Command* Comm );

	const string expandDbServer(const string&);
	
	static const string removeSqlChars(const string&);
	
	void checkDbConnection();
	
	void updateSqldb(dbHandle*);
	
	void showStatus(iClient*);
	
	unsigned int checkPassword(string,ccUser*);
	
	/**
	    Servers Functions
	 */
	 
	ccServer* getServer(const string& );
	
	void	addServer(ccServer*);

	void	remServer(ccServer*);

	bool CleanServers();

	void listServers( iClient * );

#ifdef LOGTOHD

	void	initLogs();

	void	addLog(ccLog*);

	void    showLogs(iClient* , unsigned int = 20);

#endif

	/**
	    Signals Commands
	 */
	void OnSignal(int sig); 
	
	void saveServersInfo();
	
	void saveChannelsInfo();
	
	/**
	 MaxUsers Functions
	 */
	 
	void checkMaxUsers();
	
	const unsigned int& getMaxUsers() const
	{ return maxUsers; }
	
	const unsigned long& getDateMax() const
	{ return dateMax; }
	
	const unsigned long& getCurUsers() const
	{ return curUsers; }
	
	/**
	    Server Versions Functions
	 */
	 
	bool addVersion(const string&);
	
	bool remVersion(const string&);
	
	bool isValidVersion(const string&);
	
	bool updateCheckVer(const bool);
	
	void listVersions(iClient*);
	
	/**
	    BadChannel Functions
	 */
	
	ccBadChannel* isBadChannel(const string&);
	
	void addBadChannel(ccBadChannel*);
	
	void remBadChannel(ccBadChannel*);
	
	void listBadChannels( iClient* );
	
	bool updateMisc(const string&, const unsigned int);
	
	bool glineChannelUsers(iClient* , Channel* , const string& , unsigned int , const string& , bool, bool);

	/**
	 * This is a constant iterator type used to perform a read-only
	 * iteration of the operchan structure.
	 */
	typedef vector< string >::const_iterator const_operChanIterator ;

	/**
	 * This it an iterator type used to perform a mutating
	 * iteration of the operchan structure.
	 */
	typedef vector< string >::size_type operChanSizeType ;

	/**
	 * Retrieve a constant iterator to the beginning of the oper
	 * chan structure.
	 */
	inline const_operChanIterator operChan_begin() const
		{ return operChans.begin() ; }

	/**
	 * Retrieve a constant iterator to the end of the oper chan
	 * structure.
	 */
	inline const_operChanIterator operChan_end() const
		{ return operChans.end() ; }

	/**
	 * Retrieve the size of the operchan structure.
	 */
	inline operChanSizeType operChan_size() const
		{ return operChans.size() ; }

	/**
	 * Return true if the oper chan structure is empty.
	 */
	inline bool operChan_empty() const
		{ return operChans.empty() ; }

	/**
	 * The type of a constant iterator to the command map.
	 */
	typedef commandMapType::const_iterator constCommandIterator ;

	/**
	 * Retrieve a constant iterator to the beginning of the command
	 * table.
	 */
	constCommandIterator command_begin() const
		{ return commandMap.begin() ; }

	/**
	 * Retrieve a constant iterator to the end of the command table.
	 */
	constCommandIterator command_end() const
		{ return commandMap.end() ; }

	/**
	 * Retrieve a constant iterator to a command handler for the
	 * given command token.
	 */
	constCommandIterator findCommand( const string& theComm ) const
		{ return commandMap.find( theComm ) ; }

	/**
	 * The type of a mutable iterator to the command map.
	 */
	typedef commandMapType::iterator commandIterator ;

	/**
	 * Retrieve a mutable iterator to the beginning of the command
	 * table.
	 */
	commandIterator command_begin()
		{ return commandMap.begin() ; }

	/**
	 * Retrieve a mutable iterator to the end of the command table.
	 */
	commandIterator command_end()
		{ return commandMap.end() ; }

	/**
	 * Retrieve a mutable iterator to a command handler for the
	 * given command token.
	 */
	commandIterator findCommand( const string& theComm )
		{ return commandMap.find( theComm ) ; }

	typedef glineListType::iterator  glineIterator;
	
	glineIterator gline_begin()
		{ return glineList.begin() ; }
	
	glineIterator gline_end()
		{ return glineList.end() ; }
		

	glineIterator rngline_begin()
		{ return rnGlineList.begin() ; }
	
	glineIterator rngline_end()
		{ return rnGlineList.end() ; }

	typedef ignoreListType::iterator ignoreIterator;
	
	ignoreIterator ignore_begin()
		{ return ignoreList.begin() ; }

	ignoreIterator ignore_end()
		{ return ignoreList.end() ; }
	
	typedef ipLispVectorType::iterator ipLispIterator;
	typedef ipLnbVectorType::iterator ipLnbIterator;
	typedef ipLnumericMapType::iterator ipLnumericIterator;
	typedef ipLclonesMapType::iterator ipLclonesMapIterator;
	
	clientsIpMapType		clientsIpMap;

	opersIPMapType			opersIPMap;
	
	clientsIpIterator clientsIp_begin()
		{ return clientsIpMap.begin(); }
					
	clientsIpIterator clientsIp_end()
		{ return clientsIpMap.end(); }

	typedef  usersMapType::const_iterator	usersconstiterator;
	
	typedef  accountsMapType::const_iterator	accountsconstiterator;
	
	usersconstiterator		usersmap_begin() const
		{ return usersMap.begin(); }

	usersconstiterator		usersmap_end() const
		{ return usersMap.end(); }

	accountsconstiterator		accountsMap_begin() const
		{ return accountsMap.begin(); }

	accountsconstiterator		accountsMap_end() const
		{ return accountsMap.end(); }

	typedef  serversMapType::const_iterator	serversconstiterator;
	
	serversconstiterator		serversMap_begin() const
		{ return serversMap.begin(); }

	serversconstiterator		serversMap_end() const
		{ return serversMap.end(); }

	allowedVersionsType		VersionsList;
	
	typedef list<string>::iterator 	versionsIterator;
	
	badChannelsIterator badChannels_begin()
		{ return badChannelsMap.begin(); }
		
	badChannelsIterator badChannels_end()
		{ return badChannelsMap.end(); }

	/**
	 * Retrieve the default length of time for glines.
	 */
	inline const time_t& getDefaultGlineLength() const
		{ return gLength ; }
	
	/**
	 * PostgreSQL Database
	 */
	dbHandle* SQLDb;

	/* TimerID - Posts the daily log to the abuse team  */
	gnuworld::xServer::timerID postDailyLog;

	/* TimerID = Expired glines/ignores/suspends/... interval timer */
	gnuworld::xServer::timerID expiredTimer;

	gnuworld::xServer::timerID dbConnectionCheck;
	
	gnuworld::xServer::timerID glineQueueCheck;

	gnuworld::xServer::timerID rpingCheck;

	gnuworld::xServer::timerID timeCheck;

	struct sort_pred {
		bool operator()(const std::pair<string,int> &left, const std::pair<string,int> &right) {
			return left.second < right.second;
		}
	};


protected:

	/**
	 * The name of the channel to which to send network events.
	 * This is also an oper chan.
	 */
	string			msgChan ;

	/**
	 * The name of the channel to which to send network events
	 * for limits debugging purposes (rejected clients)
	 * This is also an oper chan.
	 */
	string			limitsChan ;

	/**
	 * The modes to enforce in an oper channel.
	 */
	string			operChanModes ;

	/**
 	 * The kick reason for kicking (-o) users from oper
	 * only channel.
	 */
	string			operChanReason ;

	/**
	 * The default length of time for glines.
	 */
	time_t			gLength ;

	/**
	 * The table of oper only channels.
	 */
	vector< string >	operChans ;

	/**
	 * The command handler table.
	 */
	commandMapType		commandMap ;
	
	/**
	 * URLs to append at the end of G-line messages
	 */
	string			url_excessive_conn ;
	string			url_install_identd ;

#ifdef LOGTOHD
	ccLogList		LogList;
	
	unsigned long		NumOfLogs;
	
	string			LogFileName;

	unsigned int 		LogsToSave;
		
	fstream			LogFile;
#endif

	badChannelsMapType	badChannelsMap;
	
	/**
	 * The email address that ccontrol will send the email from
	 */
	string 			CCEmail;
	
	/**
	 * The email that the lastcom report should be sent to
	 */
	string			AbuseMail;	 

	/**
	 * The full path to sendmail
	 */
	string 			Sendmail_Path;
	
	/**
	 * Send report trigger
	 */
	int			SendReport;
	
	/**
	 * Burst flag
	 */
	 bool 			inBurst;
	
	/**
	 * Refresh gline flag
	 */
	bool			inRefresh;
	/**
	 * Refresh gline interval
	 */

	int 			ExpiredInterval;

	int			CClonesTime;

	int			CClonesGTime;

	int			maxIClones;

	int			maxGlineLen;
	
	int			maxThreads;

	bool			checkClones;

	bool			showCGIpsInLogs;

	time_t			dbConnectionTimer;

	time_t			iauthTimeout;

	string			AnnounceNick;

	string			sqlHost;
	
	string			sqlPort;
	
	string			sqlUser;
	
	string			sqlPass;

	string			sqlDb;		 	
	
	unsigned int		connectCount;
	
	unsigned int		connectRetry;

	unsigned int		maxUsers;
	
	unsigned long int	dateMax;
	
	bool			checkVer;
			
	unsigned long int	curUsers;
	
	unsigned int 		glineBurstInterval;
	
	unsigned int 		glineBurstCount; 
	
	bool			saveGlines;


} ; 

extern bool dbConnected;
} //namespace uworld

} // namespace gnuworld
 
#endif // __CCONTROL_H

