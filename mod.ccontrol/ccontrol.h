/* ccontrol.h
 * Author: Daniel Karrels dan@karrels.com
 */

#ifndef __CCONTROL_H
#define __CCONTROL_H "$Id: ccontrol.h,v 1.45 2001/11/03 01:13:06 mrbean_ Exp $"


#include	<string>
#include	<vector>
#include	<map>
#include        <iomanip>

#include	<cstdio>

#include	"client.h"
#include	"iClient.h"
#include	"server.h"
#include	"CControlCommands.h"
#include	"Channel.h"
#include	"libpq++.h"
#include	"libpq-int.h"
#include        "match.h"
#include	"md5hash.h" 
#include        "ccUser.h"
#include	"AuthInfo.h"
#include        "ccGline.h"
#include        "ccServer.h"
#include 	"ccLogin.h"
#include 	"ccException.h"
#include        "server.h"
#include	"CommandsDec.h"
#include	"ccGate.h"
#include	<pthread.h>
namespace gnuworld
{
 
using std::string ;
using std::vector ;

namespace uworld
{

using gnuworld::xServer;

/*
 *  Sublcass the postgres API to create our own accessor
 *  to get at the PID information.
 */

class cmDatabase : public PgDatabase
{
public:
	cmDatabase(const string& conninfo)
	 : PgDatabase(conninfo.c_str()) {}
	virtual ~cmDatabase() {}

	inline int getPID() const
		{ return pgConn->be_pid; }
};

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
	 * The type used to store the authenticated users.
	 */
	typedef list< AuthInfo* >	authListType ;

	/**
	 * Holds the authenticated user list
	 */
	authListType			authList ;


	typedef list< ccGline* >        glineListType ;

	/**
	 * Holds the glines
	 */

	glineListType			glineList ;

	typedef list< ccLogin* >	loginListType;
	
	
	loginListType			loginList ;
	
	loginListType			ignoreList;
	
	typedef list< ccException* >    exceptionListType;
	
	exceptionListType		exceptionList;
	
	typedef list< string >		clonesQueueType;
	
	clonesQueueType			clonesQueue;
	
	typedef clonesQueueType::iterator clonesIterator;
				
	typedef list< ccGate* >		gateQueueType;
	
	gateQueueType			gatesWaitingQueue;
	
	gateQueueType			gatesCheckingQueue;
	
	typedef gateQueueType::iterator gateIterator;
	
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
	virtual int BurstChannels() ;

	/**
	 * This method is invoked each time the client is sent a
	 * PRIVMSG.
	 */
	virtual int OnPrivateMessage( iClient*, const string&,
			bool secure = false ) ;

	virtual int OnCTCP( iClient* ,
                const string& ,
                const string&,
		bool Secure = false ) ;

	/**
	 * This method is invoked each time a network event occurs.
	 */
	virtual int OnEvent( const eventType&,
		void* = 0, void* = 0,
		void* = 0, void* = 0 ) ;


	/**
	 * This method is invoked each time a channel event occurs
	 * for one of the channels for which this client has registered
	 * to receive channel events.
	 */
	virtual int OnChannelEvent( const channelEventType&,
		Channel*,
		void* = 0, void* = 0,
		void* = 0, void* = 0 ) ;

	virtual int OnTimer(xServer::timerID, void*);

	/**
	 * This method is called once this client has been attached
	 * to an xServer.  This method is overloaded because the
	 * command handlers each require a reference to the xServer
	 * for efficiency.
	 */
	virtual void ImplementServer( xServer* ) ;

	/**
	 * Return true if the given channel name corresponds to a
	 * channel which is an IRCoperator only channel.
	 */
	inline bool isOperChan( const string& chanName ) const ;

	/**
	 * Return true if the given channel corresponds to a
	 * channel which is an IRCoperator only channel.
	 */
	inline bool isOperChan( const Channel* theChan ) const ;

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
	virtual bool Kick( Channel*, iClient*, const string& ) ;

	/**
	 * This method will cause the bot to join the channel with
	 * the given name, if the client is not already on that
	 * channel.
	 */
	virtual bool Join( const string&, const string& = string(),
		time_t = 0, bool = false) ;

	/**
	 * This method will cause this client to part the given channel,
	 * if it is already on that channel.
	 */
	virtual bool Part( const string& ) ;

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

	/**
	 * This method will check if a client is authenticated 
	 * based on the iClient structure
	 */
	 
	AuthInfo* IsAuth( const iClient* theClient ) const ;

	/**
	 * This method will check if a client is authenticated 
	 * based on a nick
	 */

        AuthInfo *IsAuth( const string& ) const ;

	/**
	 * This method will check if a client is authenticated 
	 * based on the ccUser structure
	 */

	AuthInfo *IsAuth( const ccUser* ) const ;
	
	/**
	 * This method will check if a client is authenticated 
	 * based on the full numeric
	 */

	AuthInfo *IsAuth( const unsigned int ) const ;
       
        
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
	
	bool AuthUser( ccUser* );
	
	/**
	 * This method will deauthenticate a client with the bot
	 */

	bool deAuthUser( const string& );

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
	 * This method will check if a mask is valid
	 */

	virtual bool validUserMask(const string& userMask) const ;

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


	ccGline* findMatchingGline( const string& );

	ccGline* findGline( const string& );

	/**
	 * This method logs the bot commands to the message channel
	 */

	bool MsgChanLog( const char * , ... );

	/**
	 * This method logs the commands to the database 
	 * for lastcom report
	 */

	bool DailyLog( AuthInfo * , const char *, ... );

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

	bool MailReport(const char *, char *);

	/**
	 * This method checks the gline paramerters for valid time/host
	 */

//	int CheckGline(const char *, unsigned int);
	
	int checkGline(const string ,unsigned int ,unsigned int &);

	bool isSuspended(AuthInfo *);

	bool isSuspended(ccUser *);
	
	bool refreshSuspention();
	
	bool refreshGlines();
	
	bool burstGlines();
	
	bool loadGlines();

	void wallopsAsServer(const char * , ... );

	int getExceptions( const string & );
	
	bool isException( const string & );
	
	bool listExceptions( iClient * );
	
	bool insertException( iClient * , const string & , int );

	bool delException( iClient * , const string & );
	
	ccLogin *findLogin( const string & );

	void removeLogin( ccLogin * );

	void addLogin( const string & );
	
	int removeIgnore( const string & );
	
	int removeIgnore( iClient * );

	void ignoreUser( ccLogin * );

	bool listIgnores( iClient * );
	
	bool refreshIgnores();
	
	bool loadExceptions();
	
	void listGlines( iClient * );

	void listSuspended( iClient * );
	
	void listServers( iClient * );
	
	
	void loadCommands();
	
	bool updateCommand ( Command* );
	
	Command* findRealCommand( const string& );
	
	Command* findCommandInMem( const string& );

	bool UpdateCommandFromDb ( Command* Comm );

	bool CleanServers();
	
	const string expandDbServer(const string);
	
	void addClone(const string);
	
	void delClone(const string);
	
	void checkClones(const int);
	
	void GatesCheck();
	
//	void *initGate(void *);
	
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
		
	typedef loginListType::iterator loginIterator;
	
	loginIterator login_begin()
		{ return loginList.begin() ; }

	loginIterator login_end()
		{ return loginList.end() ; }

	loginIterator ignore_begin()
		{ return ignoreList.begin() ; }

	loginIterator ignore_end()
		{ return ignoreList.end() ; }
	
	typedef exceptionListType::iterator exceptionIterator;
	
	exceptionIterator exception_begin()
		{ return exceptionList.begin(); }

	exceptionIterator exception_end()
		{ return exceptionList.end(); }
				
	/**
	 * Retrieve the default length of time for glines.
	 */
	inline const time_t& getDefaultGlineLength() const
		{ return gLength ; }

	/**
	 * PostgreSQL Database
	 */
	cmDatabase* SQLDb;

	/* TimerID - Posts the daily log to the abuse team  */
	xServer::timerID postDailyLog;

	/* TimerID = Expired glines interval timer */
	xServer::timerID expiredGlines;

	/* TimerID = Expired ignores interval timer */
	xServer::timerID expiredIgnores;

	xServer::timerID expiredSuspends;

	xServer::timerID clonesCheck;
		
	xServer::timerID gatesStatusCheck;
	
protected:

	/**
	 * The name of the channel to which to send network events.
	 * This is also an oper chan.
	 */
	string			msgChan ;

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

	 int 			GLInterval;

	 int			userMaxConnection;
	 
	 int			maxGlineLen;
	
	 int			maxThreads;

	 bool			checkGates;	 
	
} ; 

void* initGate( void * );
 
} //namespace uworld

} // namespace gnuworld
 
#endif // __CCONTROL_H
