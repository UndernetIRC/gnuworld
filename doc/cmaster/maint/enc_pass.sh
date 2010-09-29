#!/bin/bash
#
# $Id: enc_pass.sh,v 1.5 2003/07/14 10:35:44 nighty Exp $
#
# Just for the fun of it ;)
#

GENSALT=`mcookie | cut -b-8`

if [ "$1" != "" ]; then
	NUMCHAR=`echo -n $1 | wc -c`
	if [ `expr $NUMCHAR \= 8` == 1 ]; then
		echo "*** Using custom SALT : $1"
		GENSALT="$1"
	else
		echo "ERROR: Custom SALT has an invalid number of chars (needs to be 8)"
		echo "*** Using SALT : $GENSALT"
	fi
else
	echo "*** Using SALT : $GENSALT"
fi

echo -n "Cleartext password : "
read -se CTPASS
echo ""
ENCPART=`echo -n $GENSALT$CTPASS | md5sum | cut -f1 -d' '`
echo "----> SALT       : $GENSALT"
echo "----> ENCODED    : $ENCPART"
echo "----> CRYPT PASS : $GENSALT$ENCPART"

exit 0
