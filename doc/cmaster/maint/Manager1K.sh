#!/bin/bash
# You can change the above to the good path to your "bash" binary if needed
#
# $Id: Manager1K.sh,v 1.4 2003/07/14 10:35:44 nighty Exp $
#

# some default script values
DBNAME="cservice"
DBHOST="127.0.0.1"

# default information to put in the db for those .., DO NOT USE a BLANK 'LASTMODIF', use "0".
LASTMODIFBY=`basename $0`
LASTMODIF="now()::abstime::int4"



















# I suggest you dont modify stuff below this point unless you know what you are doing ;P





























PSQLCMD=""
echo -n "Checking for 'psql' binary... "
if [ -x /usr/local/pgsql/bin/psql ]; then
	PSQL="1"
	PSQLCMD="/usr/local/pgsql/bin/psql"
else
	PSQL="0"
	if [ -x `which psql | head -n 1` ]; then
		PSQL="1"
		PSQLCMD="psql"
	else
		PSQL="0"
	fi
fi

if [ "$PSQL" == "0" ]; then
	echo "not found."
	echo "--> Stopping..."
	echo "--> exit(1)"
	exit 1
else
	echo "Ok."
fi

echo "***********************************************************************"
echo "*                                                                     *"
echo "* GNUworld/mod.cservice level 1000 management script                  *"
echo "*                                                                     *"
echo "* With this script, you can make an existing user become a level 1000 *"
echo "* or create a new user that will be level 1000.                       *"
echo "* if you Modify a user already level 1000, he will be turned in 999.  *"
echo "*                                                                     *"
echo "*                                                                     *"
echo "* Warning: This program stops on any input error.                     *"
echo "*                                                                     *"
echo "***********************************************************************"
echo "\$Id: Manager1K.sh,v 1.4 2003/07/14 10:35:44 nighty Exp $"
echo "***********************************************************************"
echo "* Released under the GNU Public License                               *"
echo "***********************************************************************"
echo -n "<-- Database host/IP ? [$DBHOST] "
read -e NEWDBHOST
if [ "$NEWDBHOST" != "" ]; then
	echo "--> Using custom: $NEWDBHOST"
	DBHOST="$NEWDBHOST"
else
	echo "--> Using default: $DBHOST"
fi
echo -n "<-- Database name ? [$DBNAME] "
read -e NEWDBNAME
if [ "$NEWDBNAME" != "" ]; then
	echo "--> Using custom: $NEWDBNAME"
	DBNAME="$NEWDBNAME"
else
	echo "--> Using default: $DBNAME"
fi
echo -n "<-- (M)odify existing user or (C)reate new user ? [M] "
read -e REP1
if [ "$REP1" == "c" ]; then
	REP1="C"
fi
if [ "$REP1" == "m" ]; then
	REP1="M"
fi
if [ "$REP1" != "C" ]; then
	if [ "$REP1" != "M" ]; then
		REP1="M"
	fi
fi
FROMC="0"
FORCEUSERNAME=""
if [ "$REP1" == "C" ]; then
	echo "--> CREATE"
	echo -n "<-- User name ? "
	read -e REP2
	if [ "$REP2" == "" ]; then
		echo "--> ERROR: Invalid User name (blank)"
		echo "--> Stopping..."
		echo "--> exit(1)"
		exit 1
	fi
	NUMCHARU=`echo -n $REP2 | wc -c`
	NOTOKU0=`expr $NUMCHARU \> 12`
	NOTOKU1=`expr $NUMCHARU \< 2`
	if [ "$NOTOKU0" == "1" ]; then
		echo "--> ERROR: Too many chars in username ($NUMCHARU). Valid range: 2 to 12."
		echo "--> Stopping..."
		echo "--> exit(1)"
		exit 1
	fi
	if [ "$NOTOKU1" == "1" ]; then
		echo "--> ERROR: Too few chars in username ($NUMCHARU). Valid range: 2 to 12."
		echo "--> Stopping..."
		echo "--> exit(1)"
		exit 1
	fi

	FORCEUSERNAME="$REP2"
	if `echo -n "SELECT id FROM users WHERE lower(user_name)='$REP2'" | $PSQLCMD -h $DBHOST $DBNAME >/dev/null 2>&1`; then
		USRID=`echo -n "SELECT id FROM users WHERE lower(user_name)='$REP2'" | $PSQLCMD -h $DBHOST $DBNAME | head -n 3 | tail -n 1 | awk '{ print $1 }'`
		if [ "$USRID" != "(0" ]; then
			echo "--> ERROR: Invalid User name (already taken)"
			echo "--> Stopping..."
			echo "--> exit(1)"
			exit 1
		fi
	else
		echo "--> ERROR: Credentials provided for database connection are either invalid or database setup isn't correct."
		echo "--> ERROR: Connection to database '$DBNAME' at '$DBHOST' failed."
		echo "--> Stopping..."
		echo "--> exit(2)"
		exit 2
	fi


	echo -n "<-- E-Mail address ? "
	read -e REP2
	if [ "$REP2" == "" ]; then
		echo "--> ERROR: Invalid E-Mail address (blank)"
		echo "--> Stopping..."
		echo "--> exit(1)"
		exit 1
	fi
	USRID=`echo -n "SELECT id FROM users WHERE lower(email)='$REP2'" | $PSQLCMD -h $DBHOST $DBNAME | head -n 3 | tail -n 1 | awk '{ print $1 }'`
	if [ "$USRID" != "(0" ]; then
		echo "--> ERROR: Invalid E-Mail address (already taken)"
		echo "--> Stopping..."
		echo "--> exit(1)"
		exit 1
	else
		UEMAIL="$REP2"
		echo "!!! NOTICE: E-Mail is OK (not in use), but not checked against SYNTAX/NOREG/DOMAINLOCK. Assuming you know what you do ;P"
	fi

	echo "--> (1) What's your mother's maiden name ?"
	echo "--> (2) What's your dog's (or cat's) name ?"
	echo "--> (3) What's your father's birth date ?"
	echo -n "<--  What verification question (1, 2 or 3) ? [2] "
	read -e REP2
	REPOK="0"
	if [ "$REP2" == "" ]; then
		REP2="2"
	fi
	if [ "$REP2" == "1" ]; then
		REPOK="1"
	fi
	if [ "$REP2" == "2" ]; then
		REPOK="1"
	fi
	if [ "$REP2" == "3" ]; then
		REPOK="1"
	fi
	if [ "$REPOK" == "0" ]; then
		echo "--> ERROR: Invalid verification question choice ($REP2)"
		echo "--> Stopping..."
		echo "--> exit(1)"
		exit 1
	fi
	UQID="$REP2"

	echo -n "<-- Answer to the verification question (4 chars minimum) ? "
	read -e REP2
	if [ "$REP2" == "" ]; then
		echo "--> ERROR: Invalid answer (blank)"
		echo "--> Stopping..."
		echo "--> exit(1)"
		exit 1
	fi
	NUMCHAR=`echo -n $REP2 | wc -c`
	NOTOK=`expr $NUMCHAR \< 4`
	if [ "$NOTOK" == "1" ]; then
		echo "--> ERROR: You need at least 4 chars !"
		echo "--> Stopping..."
		echo "--> exit(1)"
		exit 1
	fi
	UVA="$REP2"

	echo -n "<-- Please specify the user password (no thing will be displayed on screen, 6 chars minimum) : "
	read -es PASS1
	echo ""
	NUMCHARP=`echo -n $PASS1 | wc -c`
	NOTOKP=`expr $NUMCHARP \< 6`
	if [ "$NOTOKP" == "1" ]; then
		echo "--> ERROR: You need at least 6 chars !"
		echo "--> Stopping..."
		echo "--> exit(1)"
		exit 1
	fi

	echo -n "<-- Please, this password again : "
	read -es PASS2
	echo ""
	if [ "$PASS1" != "$PASS2" ]; then
		echo "--> ERROR: Password mismatch"
		echo "--> Stopping..."
		echo "--> exit(1)"
		exit 1
	fi

	GENSALT=`mcookie | cut -b-8`

	ENCPART=`echo -n $GENSALT$PASS1 | md5sum | cut -f1 -d' '`
	ENCPASS="$GENSALT$ENCPART"

	echo "--> Crypted password    :  $ENCPASS"
	echo "-->             ENCODED :         [$ENCPART]"
	echo "-->             SALT    : [$GENSALT]"

	echo "--> Adding user '$FORCEUSERNAME'..."
	DAQRY="INSERT INTO users (user_name,password,email,url,question_id,verificationdata,language_id,public_key,last_updated_by,last_updated,signup_cookie,signup_ip,tz_setting) VALUES ('$FORCEUSERNAME','$ENCPASS','$UEMAIL','',$UQID,'$UVA',1,'','$LASTMODIFBY',now()::abstime::int4,'Added from the console','127.0.0.1','')"
	echo "--> DB: `echo -n \"$DAQRY\" | $PSQLCMD -h $DBHOST $DBNAME`"
	FROMC="1"
	REP1="M"
fi
if [ "$REP1" == "M" ]; then
	if [ "$FORCEUSERNAME" == "" ]; then
		echo "--> MODIFY"
		echo -n "<-- User name ? "
		read -e REP2
		if [ "$REP2" == "" ]; then
			echo "--> ERROR: Invalid User name (blank)"
			echo "--> Stopping..."
			echo "--> exit(1)"
			exit 1
		fi
	else
		REP2="$FORCEUSERNAME"
	fi
	if `echo -n "SELECT id FROM users WHERE lower(user_name)='$REP2'" | $PSQLCMD -h $DBHOST $DBNAME >/dev/null 2>&1`; then
		USRID=`echo -n "SELECT id FROM users WHERE lower(user_name)='$REP2'" | $PSQLCMD -h $DBHOST $DBNAME | head -n 3 | tail -n 1 | awk '{ print $1 }'`
		if [ "$USRID" == "(0" ]; then
			echo "--> ERROR: Invalid User name (non existant)"
			echo "--> Stopping..."
			echo "--> exit(1)"
			exit 1

		fi
	else
		echo "--> ERROR: Credentials provided for database connection are either invalid or database setup isn't correct."
		echo "--> ERROR: Connection to database '$DBNAME' at '$DBHOST' failed."
		echo "--> Stopping..."
		echo "--> exit(2)"
		exit 2
	fi

	if [ "$FROMC" == "1" ]; then
		if [ "$USRID" == "1" ]; then
			echo "--> NOTICE: It's the first user inserted.. which means you probably never used 'cservice.addme.sql' on that database ($DBNAME)"
			echo "--> NOTICE: We will be running what should be at this moment just like if you ran 'cservice.addme.sql'"
			echo "--> NOTICE: You will NOT need to run 'cservice.addme.sql' anymore, so don't run it ;P"

			ADDADMCHAN="INSERT into channels (name,flags,channel_ts,registered_ts,last_updated) VALUES ('*',1,now()::abstime::int4,31337,now()::abstime::int4)"
			ADDCODERCHAN="INSERT into channels (name,flags,channel_ts,registered_ts,last_updated) VALUES ('#coder-com',1,now()::abstime::int4,31337,now()::abstime::int4)"
			ADDCODERACCESS="INSERT into levels (channel_id,user_id,access,added_by,added,last_modif_by,last_modif,last_updated) VALUES (2,1,500,'$LASTMODIFBY',$LASTMODIF,'$LASTMODIFBY',$LASTMODIF,now()::abstime::int4)"

			echo "--> Inserting ADMIN (*) channel..."
			echo "--> DB: `echo -n \"$ADDADMCHAN\" | $PSQLCMD -h $DBHOST $DBNAME`"
			echo "--> Inserting CODER (#coder-com) channel..."
			echo "--> DB: `echo -n \"$ADDCODERCHAN\" | $PSQLCMD -h $DBHOST $DBNAME`"
			echo "--> Inserting level 500 access for '$FORCEUSERNAME' on '#coder-com'..."
			echo "--> DB: `echo -n \"$ADDCODERACCESS\" | $PSQLCMD -h $DBHOST $DBNAME`"
		fi
	fi


	ADMINLVL=`echo -n "SELECT access FROM levels WHERE channel_id=1 AND user_id=$USRID" | $PSQLCMD -h $DBHOST $DBNAME | head -n 3 | tail -n 1 | awk '{ print $1 }'`

	echo -n "--> users.id = $USRID          current admin level = "
	if [ "$ADMINLVL" == "(0" ]; then
		echo "0"
	else
		echo $ADMINLVL
	fi

	if [ "$ADMINLVL" == "1000" ]; then
		echo "--> Modifying ADMIN access from $ADMINLVL to 999..."
		DAQRY="UPDATE levels SET access=999,last_modif_by='$LASTMODIFBY',last_modif=$LASTMODIF,last_updated=now()::abstime::int4 WHERE channel_id=1 AND user_id=$USRID AND access=$ADMINLVL"
		echo "--> DB: `echo -n \"$DAQRY\" | $PSQLCMD -h $DBHOST $DBNAME`"
	else
		if [ "$ADMINLVL" == "(0" ]; then
			echo "--> Adding ADMIN access at level 1000..."
			DAQRY="INSERT INTO levels (channel_id,user_id,access,added_by,added,last_modif_by,last_modif,last_updated) VALUES (1,$USRID,1000,'$LASTMODIFBY',$LASTMODIF,'$LASTMODIFBY',$LASTMODIF,now()::abstime::int4)"
			echo "--> DB: `echo -n \"$DAQRY\" | $PSQLCMD -h $DBHOST $DBNAME`"
		else
			echo "--> Modifying ADMIN access from $ADMINLVL to 1000..."
			DAQRY="UPDATE levels SET access=1000,last_modif_by='$LASTMODIFBY',last_modif=$LASTMODIF,last_updated=now()::abstime::int4 WHERE channel_id=1 AND user_id=$USRID AND access=$ADMINLVL"
			echo "--> DB: `echo -n \"$DAQRY\" | $PSQLCMD -h $DBHOST $DBNAME`"
		fi
	fi


fi

echo "--> Job finished !"
echo "--> NOTICE: Remember your CService bot updates his cache on a regular basis"
echo "--> NOTICE: depending on your cservice.conf file this can take a few minutes for the changes/addition to be available on IRC."
echo "--> Stopping..."
echo "--> exit(0)"

exit 0

# EOF
