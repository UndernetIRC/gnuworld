--
-- Building PostgreSQL for GNUworld
--
--
-- 2001-10-14 : nighty <nighty@undernet.org>
--

in order to build postgres you need :
	- a recent version of gmake (GNU make 3.79+)
	- a recent version of TCL (TCL 8+)
	- 'cd' to the directory of pgsql your just tar -xzvf'd
	- use the following configure options :


		--with-CXX		: needed for several things like GNUworld or PHP to have pgsql support.
		--with-perl		: needed for plpsql language.
		--with-tcl --without-tk	: needed to compile pgtclsh (TCL interpreter)
					  which is needed for the script gnuworld/doc/cmaster/regproc/appjudge
					  and this script is the one making the applications advance in registration process.
		

					  To install this properly do the following :
						- copy regproc/ and its subdirs to /usr/local/pgsql
						- make it owned by your gnuworld user (the one running the postgres) :
							chown -R gnuworld /usr/local/pgsql/regproc
						- edit the file /usr/local/pgsql/regproc/appjudge-config and set your preferences.
						- crontab the content of /usr/local/pgsql/regproc/cron-judge.in :
							as root :  crontab -u gnuworld /usr/local/pgsql/regproc/cron-judge.in
							or, as gnuworld : crontab /usr/local/pgsql/regproc/cron-judge.in

						- you're done!

	- build pgsql
		root:# ./configure --with-CXX --with-tcl --without-tk --with-perl
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

