#!/bin/bash
# You can change the above to the good path to your "bash" binary if needed
#
# $Id: WipeUser.sh,v 1.7 2003/07/14 10:35:44 nighty Exp $
#

# some default script values
DBNAME="cservice"
DBHOST="127.0.0.1"






















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
echo "* GNUworld/mod.cservice Username Removal Tool                         *"
echo "*                                                                     *"
echo "* With this script, you can totally remove a user's entry in your db  *"
echo "*                                                                     *"
echo "* Warning: This program stops on any input error.                     *"
echo "*                                                                     *"
echo "***********************************************************************"
echo "\$Id: WipeUser.sh,v 1.7 2003/07/14 10:35:44 nighty Exp $"
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
echo -n "<-- User name to totally remove ? "
read -e REP2
if [ "$REP2" == "" ]; then
	echo "--> ERROR: Invalid User name (blank)"
	echo "--> Stopping..."
	echo "--> exit(1)"
	exit 1
fi
FORCEUSERNAME="$REP2"
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

if [ "$USRID" == "1" ]; then
	echo "--> ERROR: Cannot remove master user (ID = 1)"
	echo "--> Stopping..."
	echo "--> exit(1)"
	exit 1
fi

QC001="SELECT COUNT(user_id) FROM userlog WHERE user_id=$USRID"
QC002="SELECT COUNT(user_id) FROM levels WHERE user_id=$USRID"
QC003="SELECT COUNT(user_id) FROM levels WHERE access=500 AND user_id=$USRID"
QC004="SELECT COUNT(user_id) FROM supporters WHERE user_id=$USRID"
QC005="SELECT COUNT(user_id) FROM objections WHERE user_id=$USRID"
QC006="SELECT COUNT(user_id) FROM users_lastseen WHERE user_id=$USRID"
QC007="SELECT COUNT(manager_id) FROM pending WHERE manager_id=$USRID"
QC008="SELECT COUNT(user_id) FROM pending_emailchanges WHERE user_id=$USRID"
QC009="SELECT COUNT(user_id) FROM pending_pwreset WHERE user_id=$USRID"
QC010="SELECT COUNT(user_id) FROM acl WHERE user_id=$USRID"
QC011="SELECT COUNT(manager_id) FROM pending_mgrchange WHERE manager_id=$USRID"
QC012="SELECT COUNT(new_manager_id) FROM pending_mgrchange WHERE new_manager_id=$USRID"
QC013="SELECT COUNT(user_id) FROM mailq WHERE user_id=$USRID"
QC014="SELECT COUNT(user_id) FROM notes WHERE user_id=$USRID or from_user_id=$USRID"
QC015="SELECT COUNT(user_id) FROM fraud_list_data WHERE user_id=$USRID"

echo "--> Counting records..."

CNT01=`echo -n "$QC001" | $PSQLCMD -h $DBHOST $DBNAME | head -n 3 | tail -n 1 | awk '{ print $1 }' | sed -e 's/(//'`
CNT02=`echo -n "$QC002" | $PSQLCMD -h $DBHOST $DBNAME | head -n 3 | tail -n 1 | awk '{ print $1 }' | sed -e 's/(//'`
CNT03=`echo -n "$QC003" | $PSQLCMD -h $DBHOST $DBNAME | head -n 3 | tail -n 1 | awk '{ print $1 }' | sed -e 's/(//'`
CNT04=`echo -n "$QC004" | $PSQLCMD -h $DBHOST $DBNAME | head -n 3 | tail -n 1 | awk '{ print $1 }' | sed -e 's/(//'`
CNT05=`echo -n "$QC005" | $PSQLCMD -h $DBHOST $DBNAME | head -n 3 | tail -n 1 | awk '{ print $1 }' | sed -e 's/(//'`
CNT06=`echo -n "$QC006" | $PSQLCMD -h $DBHOST $DBNAME | head -n 3 | tail -n 1 | awk '{ print $1 }' | sed -e 's/(//'`
CNT07=`echo -n "$QC007" | $PSQLCMD -h $DBHOST $DBNAME | head -n 3 | tail -n 1 | awk '{ print $1 }' | sed -e 's/(//'`
CNT08=`echo -n "$QC008" | $PSQLCMD -h $DBHOST $DBNAME | head -n 3 | tail -n 1 | awk '{ print $1 }' | sed -e 's/(//'`
CNT09=`echo -n "$QC009" | $PSQLCMD -h $DBHOST $DBNAME | head -n 3 | tail -n 1 | awk '{ print $1 }' | sed -e 's/(//'`
CNT10=`echo -n "$QC010" | $PSQLCMD -h $DBHOST $DBNAME | head -n 3 | tail -n 1 | awk '{ print $1 }' | sed -e 's/(//'`
CNT11=`echo -n "$QC011" | $PSQLCMD -h $DBHOST $DBNAME | head -n 3 | tail -n 1 | awk '{ print $1 }' | sed -e 's/(//'`
CNT12=`echo -n "$QC012" | $PSQLCMD -h $DBHOST $DBNAME | head -n 3 | tail -n 1 | awk '{ print $1 }' | sed -e 's/(//'`
CNT13=`echo -n "$QC013" | $PSQLCMD -h $DBHOST $DBNAME | head -n 3 | tail -n 1 | awk '{ print $1 }' | sed -e 's/(//'`
CNT14=`echo -n "$QC014" | $PSQLCMD -h $DBHOST $DBNAME | head -n 3 | tail -n 1 | awk '{ print $1 }' | sed -e 's/(//'`
CNT14=`echo -n "$QC015" | $PSQLCMD -h $DBHOST $DBNAME | head -n 3 | tail -n 1 | awk '{ print $1 }' | sed -e 's/(//'`

echo "--> The following entries would be removed if confirmed :"
echo "-->"
echo "--> 	$CNT01	userlog entries"
echo "--> 	$CNT02	channel accesses ($CNT03 level 500 (channels would be PURGED))"
echo "--> 	$CNT04	channel support entries"
echo "--> 	$CNT05	application objections"
echo "--> 	$CNT06	last_seen entry"
echo "--> 	$CNT07	channel applications (old and present)"
echo "--> 	$CNT08	pending email change entries"
echo "--> 	$CNT09	pending verif q/a reset entries"
echo "--> 	$CNT10	ACL entry"
echo "--> 	$CNT11	pending manager changes (user is old manager, channel will be PURGED)"
echo "--> 	$CNT12	pending manager changes (user is temp manager, channel will be PURGED)"
echo "-->	$CNT13	mailq entries"
echo "-->	$CNT14	notes sent/received"
echo "-->	$CNT15	fraud list entries"
echo "-->	1	user entry"
echo "-->"
echo -n "<-- Are you sure you want to totally and definetively remove those entries from your databse [y/N] ? [N] "
read -e YOUSURE
if [ "$YOUSURE" != "y" ]; then
	if [ "$YOUSURE" != "Y" ]; then
		YOUSURE="N"
	else
		YOUSURE="Y"
	fi
else
	YOUSURE="Y"
fi

if [ "$YOUSURE" == "N" ]; then
	echo "--> You're such a weenie ;P"
	echo "--> Aborting upon user request (nothing was removed)"
	echo "--> Stopping..."
	echo "--> exit(0)"
	exit 0
fi

echo "--> Removing entries..."

QR001="DELETE FROM userlog WHERE user_id=$USRID"
QR002="DELETE FROM levels WHERE user_id=$USRID AND (access!=500 OR channel_id=1)"
QR003="SELECT channel_id FROM levels WHERE access=500 AND user_id=$USRID" # first set of channels to purge
QR004="DELETE FROM supporters WHERE user_id=$USRID"
QR005="DELETE FROM objections WHERE user_id=$USRID"
QR006="DELETE FROM users_lastseen WHERE user_id=$USRID"
QR007="DELETE FROM pending WHERE manager_id=$USRID"
QR008="DELETE FROM pending_emailchanges WHERE user_id=$USRID"
QR009="DELETE FROM pending_pwreset WHERE user_id=$USRID"
QR010="DELETE FROM acl WHERE user_id=$USRID"
QR011="SELECT channel_id FROM pending_mgrchange WHERE manager_id=$USRID" # second set of channels to purge
QR012="DELETE FROM pending_mgrchange WHERE new_manager_id=$USRID"
QR013="DELETE FROM mailq WHERE user_id=$USRID"
QR014="DELETE FROM notes WHERE user_id=$USRID or from_user_id=$USRID"
QR015="DELETE FROM fraud_list_data WHERE user_id=$USRID"
QR099="DELETE FROM users WHERE id=$USRID"

REM01=`echo -n "$QR001" | $PSQLCMD -h $DBHOST $DBNAME`
echo "--> DB: $REM01"
REM02=`echo -n "$QR002" | $PSQLCMD -h $DBHOST $DBNAME`
echo "--> DB: $REM02"
echo -n "$QR003" | $PSQLCMD -h $DBHOST $DBNAME | grep "." | grep -v -- "--" | grep -v "rows" | grep -v "channel_id" | awk '{ print "DELETE FROM levels WHERE channel_id=" $1 }' | $PSQLCMD -h $DBHOST $DBNAME | xargs echo "--> DB:"
echo -n "$QR003" | $PSQLCMD -h $DBHOST $DBNAME | grep "." | grep -v -- "--" | grep -v "rows" | grep -v "channel_id" | awk '{ print "DELETE FROM channels WHERE id=" $1 }' | $PSQLCMD -h $DBHOST $DBNAME | xargs echo "--> DB:"
REM04=`echo -n "$QR004" | $PSQLCMD -h $DBHOST $DBNAME`
echo "--> DB: $REM04"
REM05=`echo -n "$QR005" | $PSQLCMD -h $DBHOST $DBNAME`
echo "--> DB: $REM05"
REM06=`echo -n "$QR006" | $PSQLCMD -h $DBHOST $DBNAME`
echo "--> DB: $REM06"
REM07=`echo -n "$QR007" | $PSQLCMD -h $DBHOST $DBNAME`
echo "--> DB: $REM07"
REM08=`echo -n "$QR008" | $PSQLCMD -h $DBHOST $DBNAME`
echo "--> DB: $REM08"
REM09=`echo -n "$QR009" | $PSQLCMD -h $DBHOST $DBNAME`
echo "--> DB: $REM09"
REM10=`echo -n "$QR010" | $PSQLCMD -h $DBHOST $DBNAME`
echo "--> DB: $REM10"
echo -n "$QR011" | $PSQLCMD -h $DBHOST $DBNAME | grep "." | grep -v -- "--" | grep -v "rows" | grep -v "channel_id" | awk '{ print "DELETE FROM pending_mgrchanges WHERE channel_id=" $1 }' | $PSQLCMD -h $DBHOST $DBNAME | xargs echo "--> DB:"
echo -n "$QR011" | $PSQLCMD -h $DBHOST $DBNAME | grep "." | grep -v -- "--" | grep -v "rows" | grep -v "channel_id" | awk '{ print "DELETE FROM channels WHERE id=" $1 }' | $PSQLCMD -h $DBHOST $DBNAME | xargs echo "--> DB:"
REM12=`echo -n "$QR012" | $PSQLCMD -h $DBHOST $DBNAME`
echo "--> DB: $REM12"
REM13=`echo -n "$QR013" | $PSQLCMD -h $DBHOST $DBNAME`
echo "--> DB: $REM13"
REM14=`echo -n "$QR014" | $PSQLCMD -h $DBHOST $DBNAME`
echo "--> DB: $REM14"
REM15=`echo -n "$QR015" | $PSQLCMD -h $DBHOST $DBNAME`
echo "--> DB: $REM15"

REM99=`echo -n "$QR099" | $PSQLCMD -h $DBHOST $DBNAME`
echo "--> DB: $REM99"

echo "--> Username '$FORCEUSERNAME' has been totally removed."

echo "--> Job finished !"
echo "--> NOTICE: Remember your CService bot updates his cache on a regular basis"
echo "--> NOTICE: depending on your cservice.conf file this can take a few minutes for the changes/addition to be available on IRC."
echo "--> Stopping..."
echo "--> exit(0)"

exit 0

# EOF
