/* FileSocket.h
 */

#ifndef __FILESOCKET_H
#define __FILESOCKET_H "$Id: FileSocket.h,v 1.3 2001/02/02 18:10:29 dan_karrels Exp $"

#include	<string>

#include	"ClientSocket.h"

using std::string ;

/**
 * This class subclasses from ClientSocket so that it may be seemlessly
 * used by clients of ClientSocket.  This class mimics the functionality
 * of a ClientSocket, but reads from a file.  The purpose of this class
 * is for debugging purposes: clients of ClientSocket may read information
 * from a log file while offline instead of an actual network connection.
 * Because a socket (file) descriptor to the input file is given here,
 * and the caller may use select() on that descriptor, we must open
 * the input file for RW.  The send() method will not write to the file,
 * but select() must function properly for write tests.
 */
class FileSocket : public ClientSocket
{

public:

	/**
	 * Construct a FileSocket object that reads from a file.
	 */
	FileSocket( const string& fileName ) ;

	/**
	 * Destroy this FileSocket, closing the input file.
	 */
	virtual ~FileSocket() ;

	/**
	 * Attempt to connect to a given host.
	 * This method is overloaded here, and is currently a NOOP.
	 * Later, for more reliable testing of clients of this class,
	 * this method may randomly return bad descriptors,
	 * representing connection errors.
	 */
	int connect( const string& = string(),
		unsigned short int portNo = 0 ) ;

	/**
	 * Close the input stream (a file in this case).
	 */
	virtual int close() ;

	/**
	 * Return the size of the TCP receive window,
	 * or -1 on error.
	 */
	virtual int recvBufSize() const
		{ return 50000 ; }

	/**
	 * Return the size of the TCP send window,
	 * or -1 on error.
	 */
	virtual int sendBufSize() const
		{ return 50000 ; }

	/**
	 * Write a line of text to the FileSocket.
	 * Return the number of bytes written, 0 if no bytes
	 * written, or -1 on error.
	 * Currently, this method always returns immediately
	 * with the size of the string to be written.
	 */
	virtual int send( const string&, const size_t ) ;

	/**
	 * Read into the given unsigned character array
	 * not more than the number of bytes specified by the
	 * second method argument.
	 * Return number of bytes read, 0 if none read (EOF),
	 * or -1 on error.
	 */
	virtual int recv( unsigned char*, size_t ) ;

protected:

	/**
	 * Disable the copy constructor.
	 * This method is declared but NOT defined.
	 */
	FileSocket( const FileSocket& ) ;

	/**
	 * Disable copy assignment.
	 * This method is declared but NOT defined.
	 */
	FileSocket operator=( const FileSocket& ) ;

} ;

#endif // __FILESOCKET_H
