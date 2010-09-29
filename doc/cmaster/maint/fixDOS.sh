#!/bin/bash

####################################################################################################
#
# "$Id: fixDOS.sh,v 1.7 2003/07/14 10:35:44 nighty Exp $"
#
# fixDOS.sh : a small bash script to convert all .php, .inc etc.. files in your current
#             directory from DOS (\r\n) to UNIX (\n) CRLF/LF Format.
#
# Use 'fixDOS.sh --help' for more informations.
#
#
# 2002-01-09 : <nighty@undernet.org>, first release.
#
#
####################################################################################################

if [ "$1" == "--help" ]; then
	echo "HELP for '`basename $0`' :"
	echo "-"
	echo "- Usage '`basename $0` [xxx] [-nobackup]'"
	echo "- [xxx] would be another file format you want to fix (e.g. *.xxx)"
	echo "- [-nobackup] must be used either alone, or as second parameter,"
	echo "-             it prevents the program from keeping trace of 'old' files."
	echo "- Default will fix following extensions :"
	echo "-   PHP, INC, HTML, HTM, C, CC, H, PHP3, TXT."
	exit 0
fi

#
# Check files
echo -n "Checking files... "

SEDPROGRAM=`which sed | head -n 1 | cut -d\  -f1 `

if [ "$SEDPROGRAM" == "which:" ]; then
	echo "(failed, could'nt find 'sed' in the path)"
	exit 1
fi

echo "(ok)"

#
# Fix every .php, .inc, and any passer <.xxx> argument
# thus removing ^M's from files (DOS->Unix format)

FILE="0"

echo "Fixing files in `pwd` :"
echo -n "Fixing .PHP files ["
for fichphp in ./*.php; do
	if `sed -e 's/\r//g' $fichphp > /tmp/$fichphp-TMP.$$ 2>/dev/null`; then
		mv $fichphp $fichphp.bakFD
		mv /tmp/$fichphp-TMP.$$ $fichphp
		echo -n "."
		FILE="1"
	else
		echo -n "no file"
	fi
done
echo "] (ok)"
echo -n "Fixing .INC files ["
for fichinc in ./*.inc; do
        if `sed -e 's/\r//g' $fichinc > /tmp/$fichinc-TMP.$$ 2>/dev/null`; then
        	mv $fichinc $fichinc.bakFD
        	mv /tmp/$fichinc-TMP.$$ $fichinc
        	echo -n "."
		FILE="1"
	else
		echo -n "no file"
	fi
done
echo "] (ok)"
echo -n "Fixing .HTML files ["
for fichinc in ./*.html; do
        if `sed -e 's/\r//g' $fichinc > /tmp/$fichinc-TMP.$$ 2>/dev/null`; then
                mv $fichinc $fichinc.bakFD
                mv /tmp/$fichinc-TMP.$$ $fichinc
                echo -n "."
                FILE="1"
        else
                echo -n "no file"
        fi
done
echo "] (ok)"
echo -n "Fixing .HTM files ["
for fichinc in ./*.htm; do
        if `sed -e 's/\r//g' $fichinc > /tmp/$fichinc-TMP.$$ 2>/dev/null`; then
                mv $fichinc $fichinc.bakFD
                mv /tmp/$fichinc-TMP.$$ $fichinc
                echo -n "."
                FILE="1"
        else
                echo -n "no file"
        fi
done
echo "] (ok)"
echo -n "Fixing .C files ["
for fichinc in ./*.c; do
        if `sed -e 's/\r//g' $fichinc > /tmp/$fichinc-TMP.$$ 2>/dev/null`; then
                mv $fichinc $fichinc.bakFD
                mv /tmp/$fichinc-TMP.$$ $fichinc
                echo -n "."
                FILE="1"
        else
                echo -n "no file"
        fi
done
echo "] (ok)"
echo -n "Fixing .CC files ["
for fichinc in ./*.cc; do
        if `sed -e 's/\r//g' $fichinc > /tmp/$fichinc-TMP.$$ 2>/dev/null`; then
                mv $fichinc $fichinc.bakFD
                mv /tmp/$fichinc-TMP.$$ $fichinc
                echo -n "."
                FILE="1"
        else
                echo -n "no file"
        fi
done
echo "] (ok)"
echo -n "Fixing .H files ["
for fichinc in ./*.h; do
        if `sed -e 's/\r//g' $fichinc > /tmp/$fichinc-TMP.$$ 2>/dev/null`; then
                mv $fichinc $fichinc.bakFD
                mv /tmp/$fichinc-TMP.$$ $fichinc
                echo -n "."
                FILE="1"
        else
                echo -n "no file"
        fi
done
echo "] (ok)"
echo -n "Fixing .PHP3 files ["
for fichinc in ./*.php3; do
        if `sed -e 's/\r//g' $fichinc > /tmp/$fichinc-TMP.$$ 2>/dev/null`; then
                mv $fichinc $fichinc.bakFD
                mv /tmp/$fichinc-TMP.$$ $fichinc
                echo -n "."
                FILE="1"
        else
                echo -n "no file"
        fi
done
echo "] (ok)"
echo -n "Fixing .TXT files ["
for fichinc in ./*.txt; do
        if `sed -e 's/\r//g' $fichinc > /tmp/$fichinc-TMP.$$ 2>/dev/null`; then
                mv $fichinc $fichinc.bakFD
                mv /tmp/$fichinc-TMP.$$ $fichinc
                echo -n "."
                FILE="1"
        else
                echo -n "no file"
        fi
done
echo "] (ok)"
if [ "$1" != "-nobackup" ]; then
	if [ "$1" != "" ]; then
		echo -n "Fixing .$1 files ["
		for ficharg in ./*.$1; do
	        	if `sed -e 's/\r//g' $ficharg > /tmp/$ficharg-TMP.$$ 2>/dev/null`; then
	        		mv $ficharg $ficharg.bakFD
	        		mv /tmp/$ficharg-TMP.$$ $ficharg
	        		echo -n "."
				FILE="1"
			else
				echo -n "no file"
			fi
		done
		echo "] (ok)"
	fi
	if [ "$2" != "-nobackup" ]; then
		if [ "$FILE" == "1" ]; then
			echo -n "Backing up files in '`pwd`/backup-fixDOS-$$.tar.gz'... "
			tar -cOpf backup-fixDOS-$$.tar *.bakFD
			gzip -9 backup-fixDOS-$$.tar
			rm -f *.bakFD
			echo "(ok)"
		fi
	else
		rm -f *.bakFD
	fi
else
	rm -f *.bakFD
fi

echo "Cleanup finished."
echo "-"

exit 0



