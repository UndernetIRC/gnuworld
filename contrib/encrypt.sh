
function encrypt() {
  pass=$1
  COOKIE=`mcookie|cut -b-8`
  export COOKIE pass
  echo ${COOKIE}`echo ${COOKIE}${pass} | md5sum | cut -f1 -d' '`
}

if [ $# -eq 0 ]; then
	while read pass; do
		encrypt $pass
	done
	exit 0
fi

if [ "$1" = "--help" ]; then
	echo $0 -- generate passwords for gnuworld
	echo
	echo usage:
	echo $0 password
	echo   or
	echo cat passwords \| $0
	exit 0
fi

encrypt $1

