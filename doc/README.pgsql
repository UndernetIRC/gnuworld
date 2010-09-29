-- "$Id: README.pgsql,v 1.10 2002/08/30 10:38:15 nighty Exp $"
--
-- Building PostgreSQL for GNUworld
--
-- For more detailed information about setting up GNUworld, visit :
--	http://coder-com.undernet.org/gnuworld-setup/
--
--
-- 2001-10-14 : nighty <nighty@undernet.org>
--

in order to build postgres you need :
	- a recent version of gmake (GNU make 3.79 ftp://gnu.wwc.edu/make/make-3.79.tar.gz)
	- a recent version of TCL (TCL 8+ http://dev.scriptics.com/ftp/tcl/tcl8_3/tcl8.3.3.tar.gz)
		NOTE: for FreeBSD users *only* :

			root:# cd /usr/ports/lang/tcl83
			root:# make

				it will auto-download it.

			root:# make install

	- unpack the postgresql tarball (latest is version 7.1.3 : ftp://ftp.us.postgresql.org/v7.1.3/postgresql-7.1.3.tar.gz) :
		tar -xzvf postgresql-7.1.3.tar.gz
	- 'cd' to the directory :
		cd postgresql-7.1.3/
	- use the following configure options :


		--with-CXX		: needed for several things like GNUworld or PHP to have pgsql support.
		--with-tcl --without-tk	: needed to compile pgtclsh (TCL interpreter)
					  which is needed for the script gnuworld/doc/cmaster/regproc/appjudge
					  and this script is the one making the applications advance in registration process.
					  To install this properly, see the included README.appjudge file. 

	- build pgsql
		root:# ./configure --with-CXX --with-tcl --without-tk
		root:# gmake
		root:# gmake install

	- add the pgsl library directories to the lib PATH, this varies from system to another
	  (if you don't do this, GNUworld wille *NOT* be able to compile)

		on Linux type OS's you will need to edit :
			/etc/ld.so.conf
			and add the line :
				/usr/local/pgsql/lib

			save, then run 'ldconfig'.

		on FreeBSD or other systems you may need to set the LD_LIBRARY_PATH environment variable
			you can add the line :

				export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/local/pgsql/lib

			into /etc/profile for bash.

			zsh, sh and ksh use 'export' too in their respective .shrc .zshrc .kshrc ~ files.

			for csh and tcsh you will need to use 'setenv' instead :

				setenv LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/local/pgsql/lib


	- create a data directory for PGSL :
		root:# mkdir /usr/local/pgsql/data
	
		change its permissions to the gnuworld user (the one that run postgres) :
		root:# chown gnuworld /usr/local/pgsql/data

	- do the final process to set the database UP :
			root:# su - gnuworld
			gnuworld:$ /usr/local/pgsql/bin/initdb -D /usr/local/pgsql/data
		start postgresql with default values :
			gnuworld:$ /usr/local/pgsql/bin/postmaster -S -B 64 -N 32 -i -D /usr/local/pgsql/data -o -F

		NOTE: this will only allow 32 threads simultaneously .. this should be large enough for small networks
		if you want to increase this .. please view the 'README.sharedmemory' in this directory.






now go look README.ccontrol and README.cservice ;)

