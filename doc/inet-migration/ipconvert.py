#!/usr/local/bin/python

# Use this script to convert the mod.cservice tables to use 'inet' datatype:
#	whitelist
#	pending_traffic
#	ip_restrict
#
# In the case of ip_restrict (IPR) entries the script will attempt to convert
# hostnames to IPs, and wildcards or ranges to CIDR notation. DNS and masks 
# are no longer permitted.
#
# Usage: ipconvert.py -h <host> -d <database> -u <user> >> converted.sql
#
# Analyze the output file before passing to pgsql appropriately. ie:
#
#	psql -h <host> -d <database> -U <user> -f converted.sql
#
# - Empus <empus@undernet.org>
#

import sys, re, socket, getopt, struct, psycopg2
from netaddr import *
from datetime import datetime
 
d = datetime.now().strftime('%Y-%m-%d')

def ip2long(ip):
    # -- convert IP to long decimal
    packedIP = socket.inet_aton(ip)
    return struct.unpack("!l", packedIP)[0]

def long2ip(dec):
    # -- convert long decimal to IP
    return socket.inet_ntoa(struct.pack('!l', dec))
    
con = None
host = None
db = None
user = None
pw = None

def usage():
	print "ipconvert.py -h <host> -d <database> -u <user>"
	print ""

try:
	opts, args = getopt.getopt(sys.argv[1:], "h:d:u:", ["host=", "database=", "user="])
except getopt.GetoptError as err:
	usage()
	print(err)
	sys.exit(2)

for opt, arg in opts:
	if opt in ("-h", "--host"):
		host = arg
	elif opt in ("-d", "--database"):
		db = arg
	elif opt in ("-u", "--user"):
		user = arg
	elif opt in ("-p", "--pass"):
		pw = arg
	else:
		assert False, "unhandled option"

if host is None:
	host = '127.0.0.1'
	
if db is None:
	usage()
	sys.exit(2)
	

		
try: 
	con = psycopg2.connect(host=host, database=db, user=user)
	cur = con.cursor()

except psycopg2.Error as e:
	print "connection failed!"
	print ""
	sys.exit(1)

else:
	# -- alter whitelist table datatype:
	print "ALTER TABLE whitelist ALTER COLUMN ip TYPE inet USING ip::inet;"
	
	# -- alter the table first to add the new columns
	print ""
	query = "ALTER TABLE ip_restrict"
	query += " ADD COLUMN value inet,"
	query += " ADD COLUMN last_updated int4 NOT NULL DEFAULT now()::abstime::int4,"
	query += " ADD COLUMN last_used int4,"
	query += " ADD COLUMN expiry int4 NOT NULL DEFAULT 0,"
	query += " ADD COLUMN description VARCHAR(255);"
	print "%s" % query
	print ""
	# -- allow null entries so we can do new and easier row inserts
	print "ALTER TABLE ip_restrict ALTER COLUMN added DROP NOT NULL;"
	print "ALTER TABLE ip_restrict ALTER COLUMN added SET DEFAULT now()::abstime::int4;"
	print "ALTER TABLE ip_restrict ALTER COLUMN allowmask DROP NOT NULL;"
	print "ALTER TABLE ip_restrict ALTER COLUMN allowrange1 DROP NOT NULL;"
	print "ALTER TABLE ip_restrict ALTER COLUMN allowrange2 DROP NOT NULL;"
	print ""

	# -- grab the current list of IPR entries
	query = "SELECT ip_restrict.id, user_id, users.user_name, allowmask, allowrange1, allowrange2, added, to_timestamp(added)"
	query += " AS added_date, added_by,"
	query += " (SELECT users.user_name FROM users WHERE id=added_by)"
	query += " AS added_user, type"
	query += " FROM ip_restrict"
	query += " INNER JOIN users ON (ip_restrict.user_id=users.id);"
	cur.execute(query)

	ERROR_IPMASK = 0
	ERROR_HOSTMASK = 0
	ERROR_DNS = 0
	ERROR_IPRANGE = 0
	TOTAL = 0
	IPMASK = 0
	DNS = 0
	IPRANGE = 0
	IPRANGE_EXP = 0

	while True:
	  
		row = cur.fetchone()
		
		TOTAL += 1
		
		if row == None:
			break
		
		# id  | user_id | user_name  |               allowmask                | allowrange1 | allowrange2 |   added    |       added_date       | added_by | added_user | type
		#------+---------+------------+----------------------------------------+-------------+-------------+------------+------------------------+----------+------------+------

		id = row[0]
		user_id = row[1]
		user_name = row[2]
		allowmask = row[3]
		allowrange1 = row[4]
		allowrange2 = row[5]
		added = row[6]
		added_date = row[7]
		added_by = row[8]
		added_user = row[9]
		type = row[10]
		
		# print "id: %s -- user_id: %s -- user_name: %s -- allowmask: %s -- allowrange1: %s -- allowrange2: %s -- added: %s -- added_date: %s -- added_by: %s -- added_user: %s -- type: %s" % (id, user_id, user_name, allowmask, allowrange1, allowrange2, added, added_date, added_by, added_user, type)
		
		if allowmask != '':
			# -- check if it is a mask
			if ":" in allowmask:
				# -- ipv6: this shouldn't ever be seen here
				continue
			elif "*" in allowmask:
				# -- some sort of wildcard mask
				if re.match('^[\d\.\*\?]+$', allowmask):
					# -- IP mask
					# -- see if we can convert easily to the easy /8 /16 or /24 CIDR subnets
					# -- this is crude matching!
					match8 = re.match('^([^\.\*]+)\.\*', allowmask)
					match16 = re.match('^([^\.*]+)\.([^\.*]+)\.\*', allowmask)
					match24 = re.match('^([^\.*]+)\.([^\.*]+)\.([^\.*]+).\*', allowmask)
					
					if match8:
						# -- /8 subnet
						ip1 = match8.group(1)
						cidr = '%s.0.0.0/8' % ip1
						IPMASK += 1
						print "UPDATE ip_restrict SET value='%s',description='[auto] converted from %s on %s' WHERE id=%s;" % (cidr, allowmask, d, id)
					
					  
					elif match16:
						# -- /16 subnet
						ip1 = match16.group(1)
						ip2 = match16.group(2)
						cidr = '%s.%s.0.0/16' % (ip1, ip2)
						IPMASK += 1
						print "UPDATE ip_restrict SET value='%s',description='[auto] converted from %s on %s' WHERE id=%s;" % (cidr, allowmask, d, id)
					
										   
					elif match24:
						# -- /24 subnet
						ip1 = match24.group(1)
						ip2 = match24.group(2)
						ip3 = match24.group(3)
						cidr = '%s.%s.%s.0/24' % (ip1, ip2, ip3)
						IPMASK += 1
						print "UPDATE ip_restrict SET value='%s',description='[auto] converted from %s on %s' WHERE id=%s;" % (cidr, allowmask, d, id)
					
					else:
						# -- manual handling
						ERROR_IPMASK += 1
						print "-- !ERROR_IPMASK! cannot convert IP mask %s to CIDR (id: %s -- user: %s -- added_by: %s -- added_date: %s)" % (allowmask, id, user_name, added_user, added_date)
						# -- disable entry
						print "UPDATE ip_restrict SET type=0,description='[error] old IP mask: %s.' WHERE id=%s;" % (allowmask, id)

				else:
					# -- hostmask -- we cannot convert these
					ERROR_HOSTMASK += 1
					print "-- !ERROR_HOSTMASK! cannot convert HOST mask %s to new format (id: %s -- user: %s -- added_by: %s -- added_date: %s)" % (allowmask, id, user_name, added_user, added_date)
					# -- disable entry
					print "UPDATE ip_restrict SET type=0,description='[error] old hostmask: %s' WHERE id=%s;" % (allowmask, id)

			else:
				# -- check for hostname
				if re.match('^[\d\.]+$', allowmask):
					# -- single IP
					IPMASK += 1
					print "UPDATE ip_restrict SET value='%s' WHERE id=%s;" % (allowmask, id)
					
				else:
					# -- hostname -- do a DNS lookup
					try:
						ip = socket.gethostbyname(allowmask)
						IPMASK += 1
						print "UPDATE ip_restrict SET value='%s',description='[auto] converted from %s on %s' WHERE id=%s;" % (ip, allowmask, d, id)
						
					except socket.gaierror:
						ERROR_DNS += 1
						print "-- !ERROR_DNS! unresolved host: %s (id: %s -- user: %s -- added_by: %s -- added_date: %s)" % (allowmask, id, user_name, added_user, added_date)
						# -- disable entry
						print "UPDATE ip_restrict SET type=0,description='[error] dns lookup failed: %s' WHERE id=%s;" % (allowmask, id)
						continue


		elif allowrange1 != '' and allowrange1 != 0:
			# -- we're working with a range
			if allowrange2 == 0 or allowrange2 == '':
				# -- single IP (long decimal format)
				long = long2ip(allowrange1)
				IPRANGE += 1
				print "UPDATE ip_restrict SET value='%s' WHERE id=%s;" % (long, id)
				
			else:
				# -- we must be dealing with an IP range
				long1 = long2ip(allowrange1)
				long2 = long2ip(allowrange2)
				
				PRINT_ERROR = 1

				# -- see if we can convert easily to the easy /8 /16 CIDR subnets
				# -- this is crude matching but much faster than the netaddr module
				# -- not worth doing for a /24
				match8a = re.match('^([^\.\*]+)\.0\.0\.0', long1)
				match16a = re.match('^([^\.\*]+)\.([^\.\*]+)\.0\.0', long1)
				
				match8b = re.match('^([^\.\*]+)\.255\.255\.255', long2)
				match16b = re.match('^([^\.\*]+)\.([^\.\*]+)\.255\.255', long2)
				
				PRINT_ERROR = 1
				
				if match8a and match8b:
					# -- basic /8 subnet match
					ip1a = match8a.group(1)
					ip1b = match8b.group(1)
					if ip1a == ip1b:
						cidr = '%s.0.0.0/8' % ip1a
						IPRANGE += 1
						PRINT_ERROR = 0
						print "UPDATE ip_restrict SET value='%s',description='[auto] converted from %s - %s on %s' WHERE id=%s;" % (cidr, long1, long2, d, id)
						
				
				elif match16a and match16b:
					# -- basic /16 subnet match
					ip1a = match16a.group(1)
					ip1b = match16a.group(2)
					
					ip2a = match16b.group(1)
					ip2b = match16b.group(2)
					ip1 = "%s %s" % (ip1a, ip1b)
					ip2 = "%s %s" % (ip2a, ip2b)
					if ip1 == ip2:
						cidr = '%s.%s.0.0/16' % (ip1a, ip1b)
						IPRANGE += 1
						PRINT_ERROR = 0
						print "UPDATE ip_restrict SET value='%s',description='[auto] converted from %s - %s on %s' WHERE id=%s;" % (cidr, long1, long2, d, id)
				
				
				else:
					# -- use netaddr module
					# -- this is very slow with really wide ranges but is however, effective
					ip_list = list(iter_iprange(long1, long2))
					ip_length = len(ip_list)
					#print "!DEBUG! IP List Length: %s (range %s -> %s) -- (id: %s -- user: %s -- added_by: %s -- added_date: %s)" % (ip_length, long1, long2, id, user_name, added_user, added_date)
										
					cidr_list = cidr_merge(ip_list)
					length = len(cidr_list)
					if length > 1:
					
						IPRANGE += 1
						PRINT_ERROR = 0
						print "DELETE FROM ip_restrict WHERE id=%s;" % id
						for net in cidr_list:
							IPRANGE_EXP += 1
							desc = '[auto] converted from %s - %s on %s' % (long1, long2, d)
							print "INSERT INTO ip_restrict (user_id,value,added,added_by,type,last_updated,description) VALUES (%s, '%s', now()::abstime::int4, %s, 1, now()::abstime::int4, %s);" % (user_id, net, added_by, desc)
						
					elif length == 1:
						IPRANGE += 1
						PRINT_ERROR = 0
						for net in cidr_list:
							desc = '[auto] converted from %s - %s on %s' % (long1, long2, d)
							print "UPDATE ip_restrict SET value='%s',description='[auto] converted from %s - %s on %s' WHERE id=%s;" % (net, long1, long2, d, id)
					
					elif length == 0 or PRINT_ERROR:          
						ERROR_IPRANGE += 1
										
				
				if PRINT_ERROR:
					print "-- !ERROR_IPRANGE! IP range: %s -> %s (id: %s -- user: %s -- added_by: %s -- added_date: %s)" % (long1, long2, id, user_name, added_user, added_date)
					print "UPDATE ip_restrict SET type=0,description='[error] IP range error: %s-%s' WHERE id=%s;" % (long1, long2, id)

	print ""
	# -- alter the table to drop the old columns
	query = "ALTER TABLE ip_restrict"
	query += " DROP COLUMN allowmask,"
	query += " DROP COLUMN allowrange1,"
	query += " DROP COLUMN allowrange2;"
	print "%s" % query
	print ""
		
	# -- remove any null value rows (ie. those that couldn't be converted)
	print "DELETE FROM ip_restrict WHERE value IS null;"
	# -- prevent null entries for 'value'
	print "ALTER TABLE ip_restrict ALTER COLUMN value SET NOT NULL;"

	ERRORS = ERROR_DNS + ERROR_IPMASK + ERROR_IPRANGE + ERROR_HOSTMASK
	EXPANDED = IPRANGE + IPRANGE_EXP
	print ""
	print "-- TOTAL: %s -- IPmask (%s) -- DNS (%s) -- IPrange (%s -- expanded to %s)" % (TOTAL, IPMASK, ERROR_DNS, IPRANGE, EXPANDED) 
	print "-- ERRORS: %s -- IPmask (%s) -- hostmask (%s) -- DNS (%s) -- IPrange (%s)" % (ERRORS, ERROR_IPMASK, ERROR_HOSTMASK, ERROR_DNS, ERROR_IPRANGE)    
	print ""
	
	
	print ""
	# END OF ip_restrict
	
	# BEGIN pending_traffic
	print ""
	
	#  channel_id |  ip_number  | join_count
	# ------------+-------------+------------
	
	# -- alter the table first to add the temporary column
	print ""

	print "ALTER TABLE pending_traffic ADD COLUMN ip_inet inet;"
	print ""

	# -- grab the current list of supporter traffic entries
	query = "SELECT channel_id, ip_number FROM pending_traffic;"
	cur.execute(query)

	ERROR = 0
	TOTAL = 0
	
	while True:
	  
		row = cur.fetchone()
		
		TOTAL += 1
		
		if row == None:
			break
		
		#  channel_id |  ip_number  | join_count
		# ------------+-------------+------------

		channel_id = row[0]
		ip_number = row[1]
		
		# print "channel_id: %s -- ip_number: %s" % (channel_id, ip_number) 
		
		long = long2ip(ip_number)
		IPRANGE += 1
		print "UPDATE pending_traffic SET ip_inet='%s' WHERE ip_number='%s' AND channel_id='%s';" % (long, ip_number, channel_id)

	print ""
	# -- alter the table to drop the old columns
	print "ALTER TABLE pending_traffic DROP COLUMN ip_number;"
	print "ALTER TABLE pending_traffic RENAME COLUMN ip_inet TO ip_number;"
	print ""
	print "-- TOTAL: %s" % TOTAL
	print ""
	
	con.close()

