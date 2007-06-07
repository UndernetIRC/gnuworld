<?
/*
 *
 * Undernet GNUworld Interactive Installation Guide (aka UGIIG)
 * $Id: index.php,v 1.22 2007/06/07 01:52:37 nighty Exp $
 *
 * Released under the GNU Public License : http://www.gnu.org/licenses/gpl.txt
 * by nighty <nighty@undernet.org>
 * Last modified: 17/07/2006
 *
 *
 */


// Originally this was an include, but as its all in ONE file, i can include it right here ;P
/* miniconf 1.06 - <nighty@undernet.org> - 03/04/2002 */

function footer($prev,$next) {
	global $coder_name,$coder_email,$os,$modules,$secure,$r,$subset;
	echo "<hr size=1 noshade width=100%>\n";
	if ($prev!="" || $next!="") {
		$urlroot = "./?s=3&os=" . urlencode($os) . "&modules=" . urlencode($modules) . "&r=" . $r . "&secure=" . $secure;
		echo "<table border=0 width=639 cellspacing=0 cellpadding=0>\n";
		echo "<tr>";
		echo "<td width=213>";
		if ($prev!="") {
			if ($prev!="*") {
				echo "<a href=\"" . $urlroot . "&subset=" . $prev . "\">&lt;&nbsp;Previous</a>";
			} else {
				echo "<a href=\"" . $urlroot . "\">&lt;&nbsp;Previous</a>";
			}
		}
		$tmp = explode("-",$subset);
		echo "</td><td width=213 align=center><a href=\"" . $urlroot . "&subset=" . $tmp[0] . "\">Up Level</a></td>";
		echo "<td align=right width=213>";
		if ($next!="") {
			if ($next!="*") {
				echo "<a href=\"" . $urlroot . "&subset=" . $next . "\">Next&nbsp;&gt;</a>";
			} else {
				echo "<a href=\"" . $urlroot . "\">Next&nbsp;&gt;</a>";
			}
		}
		echo "</td></tr></table>\n";
		echo "<hr size=1 noshade width=100%>\n";
	}
	$offset = date("Z",time());
	if ($offset<0) { $sign = "-"; } else { $sign = "+"; }
	$gmtoffset = $sign . abs(($offset/3600));
	if ($offset==0) { $gmtoffset = ""; }
	//echo "<address>maintained by <b>&lt;</b><a href=\"mailto:" . $coder_email . "\">" . $coder_email . "</a><b>&gt;</b> - Last modified : <b>" . date("M-d-Y H:i:s",   (  filemtime("index.php")+( date("Z",filemtime("index.php"))/3600 )   )   ) . " UTC/GMT</b> - <b>[</b><a href=\"./\">Home</a><b>]</b></address>";
	echo "<address>maintained by <b>&lt;</b><a href=\"mailto:" . $coder_email . "\">" . $coder_email . "</a><b>&gt;</b> - <b>[</b><a href=\"./\">Home</a><b>]</b>";
	echo "&nbsp;<br><font size=-2><b>";
?>$Id: index.php,v 1.22 2007/06/07 01:52:37 nighty Exp $<?
	echo "</b></font></address>\n";
	echo "</body>\n";
	echo "</html>\n";
	echo "<!-- by " . $coder_name . " <" . $coder_email . "> //-->\n";
	die;
}

function ugig_err($err_code) {
	echo "<h1>Error</h1>";
	echo "<br><br><br>";
	switch ($err_code) {
		default:
			echo "&nbsp;";
			break;
	}
	echo "<br><br><a href=\"./\">Click Here, and start over</a>";
	footer();
	die;
}

$coder_email = "nighty@undernet.org";
$coder_name = "nighty";

define(PG_SQL_FTP_SOURCE,"ftp://ftp.nl.postgresql.org:21/pub/mirror/postgresql/source/");
define(PG_SQL_FTP_SOURCE_FILE,"ftp://ftp.nl.postgresql.org:21/pub/mirror/postgresql/source/v8.1.4/postgresql-8.1.4.tar.gz");
define(PG_SQL_RECOM_VERSION,"8.1.4");

define(TCL_83_PORT_SOURCE,"http://www.freebsd.org/cgi/cvsweb.cgi/ports/lang/tcl84/");
define(TCL_83_PORT_SOURCE_FILE,"http://www.freebsd.org/cgi/cvsweb.cgi/ports/lang/tcl84/tcl84.tar.gz?tarball=1");
define(TCL_83_LINUX_SOURCE,"http://sourceforge.net/project/showfiles.php?group_id=10894");
define(TCL_83_LINUX_SOURCE_FILE,"http://belnet.dl.sourceforge.net/sourceforge/tcl/tcl8.4.13-src.tar.gz");
define(TCL_83_LINUX_FILE_NAME,"tcl8.4.13-src.tar.gz");
define(TCL_83_RECOM_VERSION,"8.4.13");

define(APACHE_FTP_SOURCE,"http://mir2.ovh.net/ftp.apache.org/dist/httpd/");
define(APACHE_FTP_SOURCE_FILE,"http://mir2.ovh.net/ftp.apache.org/dist/httpd/apache_1.3.36.tar.gz");
define(APACHE_FILE_NAME,"apache_1.3.36.tar.gz");
define(APACHE_RECOM_VERSION,"1.3.36");

define(PHP_FTP_SOURCE,"http://www.php.net/downloads.php");
define(PHP_FTP_SOURCE_FILE,"http://nl2.php.net:80/distributions/php-4.4.2.tar.gz");
define(PHP_FILE_NAME,"php-4.4.2.tar.gz");
define(PHP_RECOM_VERSION,"4.4.2");

define(LIBPNG_URL,"http://www.libpng.org/");
define(LIBPNG_URL_FILE,"http://belnet.dl.sourceforge.net/sourceforge/libpng/libpng-1.2.12.tar.gz");
define(LIBPNG_VERSION,"1.2.12");
define(LIBPNG_FILE_NAME,"libpng-1.2.12.tar.gz");

define(LIBJPEG_URL,"http://www.ijg.org/");
define(LIBJPEG_URL_FILE,"http://www.ijg.org/files/jpegsrc.v6b.tar.gz");
define(LIBJPEG_VERSION,"6b");
define(LIBJPEG_FILE_NAME,"jpegsrc.v6b.tar.gz");

define(FREETYPE2_URL,"http://freetype.sourceforge.net");
define(FREETYPE2_URL_FILE,"http://download.savannah.gnu.org/releases/freetype/freetype-2.2.1.tar.gz");
define(FREETYPE2_VERSION,"2.2.1");
define(FREETYPE2_FILE_NAME,"freetype-2.2.1.tar.gz");

define(ZLIB_URL,"http://www.gzip.org/zlib/");
define(ZLIB_URL_FILE,"http://www.gzip.org/zlib/zlib-1.2.3.tar.gz");
define(ZLIB_VERSION,"1.2.3");
define(ZLIB_FILE_NAME,"zlib-1.2.3.tar.gz");


?>
<!-- $Id: index.php,v 1.22 2007/06/07 01:52:37 nighty Exp $ //-->
<html>
<head>
<title>Undernet GNUworld Interactive Installation Guide</title>
</head>
<body bgcolor=#ffffff text=#000000>
<h2>Undernet GNUworld Interactive Installation Guide</h2>
<?
	switch($s) {
		case 1:
			echo "<h3>Operating System</h3>";
			echo "<a href=\"./\">Back to introduction</a>";
			break;
		case 2:
			echo "<h3>Modules Options</h3>";
			echo "<a href=\"./?s=1\">Back to operating system choice</a>";
			break;
		case 3:
			echo "<h3>Installation details</h3>";
			if ($subset=="") {
				echo "<a href=\"./?s=2&os=" . $os . "\">Back to modules options</a>";
			} else {
				echo "<a href=\"./?s=3&os=" . $os . "&modules=" . urlencode($modules) . "&r=" . $r . "&secure=" . $secure . "\">Table of content</a>";
				if (strlen($subset)>1) {
					echo "&nbsp;/&nbsp;";
					$tmp = explode("-",$subset);
					$maincat = $tmp[0];
					if ($maincat=="A") { $catname = "Setting up PostgreSQL"; }
					if ($maincat=="B") { $catname = "Setting up GNUworld"; }
					if ($maincat=="C") { $catname = "Setting up Apache and PHP"; }
					echo "<a href=\"./?s=3&os=" . $os . "&modules=" . urlencode($modules) . "&r=" . $r . "&secure=" . $secure . "&subset=" . $maincat . "\">" . $catname . "</a>";
				}
			}
			break;
		case 4:
			echo "<h3>Frequently Asked Questions (and answers)</h3>";
			break;
		case 5:
			echo "<h3>Upgrading GNUworld</h3>";
			break;
		case 6:
			echo "<h3>Credits</h3>";
			break;
		default:
			echo "<h3>Introduction</h3>";
			$s=0;
			break;
	}
?>
<hr size=1 noshade width=100%>
<?
if ($s==0) { // ** INTRODUCTION
?>
Welcome to the <b>Undernet GNUworld Interactive Installation Guide</b>.<br><br>
The purpose of that page is to provide a targeted help, using your own specifications,<br>
on how to install <b>GNUworld</b> properly onto your system.<br><br>
This help covers only the two most common types of Operating Systems, Linux and FreeBSD.<br><br>
If you wish to share your experiences on other systems, like SunOS or HPUX etc.. of working installations<br>
using modified parameters of this HOWTO, please let <a href="mailto:<?=$coder_email?>">me</a> know, this may get included.
<br><br>
<ol>
<li> <a href="./?s=1"><b>Click here to start building your HOWTO</b></a>
<li> <a href="./?s=4">Frequently Asked Questions (F.A.Q.)</a>
<li> <a href="./?s=5">Upgrading GNUworld</a>
<li> <a href="./?s=6">Credits</a>
</ol>
<br><br>
<pre>
GNUworld Homepage			: <a href="http://www.gnuworld.org/" target="_blank">http://www.gnuworld.org/</a>
Undernet Coder-Comittee Homepage	: <a href="http://coder-com.undernet.org/" target="_blank">http://coder-com.undernet.org/</a>
Undernet Homepage			: <a href="http://www.undernet.org/" target="_blank">http://www.undernet.org/</a>
Undernet CService Website Interface	: <a href="http://cservice.undernet.org/live/" target="_blank">http://cservice.undernet.org/live/</a>
Undernet CService Website		: <a href="http://cservice.undernet.org/" target="_blank">http://cservice.undernet.org/</a>

If you have any questions about <b>GNUworld</b>,
or other's Coder-Comittee projects such as <b>ircu2.10.x</b>,

you can either :
	- post your questions to <a href="mailto:coder-com@undernet.org">coder-com@undernet.org</a>,
	- join the <b>Undernet IRC Network</b>
		use either <b>eu.undernet.org</b> or <b>us.undernet.org</b> or <a href="http://www.undernet.org/servers.php" target=_blank>the Undernet servers list</a>,
		depending on your location, as your IRC server (port <b>6667</b>)
		and <b>/join #coder-com</b>.
	- view discussions and post your questions on the <a href="http://www.undernet.org/forum/viewforum.php?forum=7" target=_blank>Undernet Coder-Comittee Forum</a>
</pre>
<br><br>
<?
}
if ($s==1) { // *** OS CHOICE
?>
<form name=ospick method=get>
<input type=hidden name=s value=2>
To start, choose an Operating System : <select name=os>
<option value=freebsd>FreeBSD</option>
<option value=linux>Linux (Slackware, Redhat, Mandrake, SuSe..)</option>
</select> <input type=submit value=" Proceed >> ">
</form>
<?
}
if ($s==2) { // *** CHOOSING INSTALLED MODULES OPTIONS
	if ($os!="linux" && $os!="freebsd") {
		ugig_err(0);
	}
	if ($os=="linux") { $OSNAME="Linux"; }
	if ($os=="freebsd") { $OSNAME="FreeBSD"; }
?>
<form name=modopts method=get>
<input type=hidden name=s value=3>
<input type=hidden name=os value=<?=$os?>>
<ol>
<li> Do you want to setup :<br><br>
<input type=radio name=modules value="cservice"> <b>mod.cservice</b> only (X like module)<br>
<input type=radio name=modules value="ccontrol"> <b>mod.ccontrol</b> only (Uworld like module)<br>
<input type=radio name=modules value="cservice ccontrol" checked> both <b>mod.cservice</b> and <b>mod.ccontrol</b><br><br><br>
<li> Can you access <b>root</b> priviledges on your <?=$OSNAME?> machine ?<br><br>
<input type=radio name=r value=1 checked>Yes<br>
<input type=radio name=r value=0>No<br><br><br>
<li> Do you want help on how to secure your database with a password ?<br>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<i>(advanced users)</i><br>
<input type=radio name=secure value=1>Yes<br>
<input type=radio name=secure value=0 checked>No<br><br><br>
<li> <input type=submit value=" Proceed >> ">
</ol>
</form>
<?
}
if ($s==3) { // *** ADDITIONNAL INFORMATION FOR EXAMPLE CUSTOMIZATION + HELP DISPLAY
	if ($os!="linux" && $os!="freebsd") {
		ugig_err(0);
	}
	if ($os=="linux") { $OSNAME="Linux"; }
	if ($os=="freebsd") { $OSNAME="FreeBSD"; }
	if ($modules!="cservice" && $modules!="ccontrol" && $modules!="cservice ccontrol") {
		ugig_err(0);
	}
	if (!isset($r) || $r<0 || $r>1) { ugig_err(0); }

	if ($r==0 && $subset=="") {
		echo "<h1>Warning</h1>";
		echo "<pre>";
		echo "Installing GNUworld without having <b>root</b> access on the machine\nhas requirements and constraints: \n\n";
		if ($secure) {
			echo "	- In order to have password support for PostgreSQL, ask your System administrator.";
		}
		echo "</pre>";
	}
	$url_root="./?s=3&os=" . urlencode($os) . "&modules=" . urlencode($modules) . "&r=" . $r . "&secure=" . $secure;
	if (!isset($secure) || $secure<0 || $secure>1) { ugig_err(0); }
	if (!isset($subset) || $subset<0 || $subset>50) {
		$MODULESLIST="mod.cservice and mod.ccontrol modules";
		if ($modules=="cservice") { $MODULESLIST="mod.cservice module"; }
		if ($modules=="ccontrol") { $MODULESLIST="mod.ccontrol module"; }
?>
<ol type=A>
<? if ($r) { ?>
	<li> <a href="<?=$url_root?>&subset=A"><b>Setting up PostgreSQL</b></a>
	<ol type=1>
		<li> <a href="<?=$url_root?>&subset=A-1">Getting PostgreSQL</a>
		<li> <a href="<?=$url_root?>&subset=A-2">Requirements</a>
		<ol type=i>
			<li> <a href="<?=$url_root?>&subset=A-2-i">Getting and Installing TCL 8.x+</a>
		</ol>
		<li> <a href="<?=$url_root?>&subset=A-3">Building PostgreSQL</a>
		<li> <a href="<?=$url_root?>&subset=A-4">Setting up Database space</a>
<? if ($secure) { ?>
		<li> <a href="<?=$url_root?>&subset=A-5">Setting up security for the database (passwords)</a>
<? } ?>
	</ol>
<? } else { ?>
	<li> <a href="<?=$url_root?>&subset=R"><b>Requirements</b></a>
<? } ?>
	<li> <a href="<?=$url_root?>&subset=B"><b>Setting up GNUworld</b></a>
	<ol type=1>
		<li> <a href="<?=$url_root?>&subset=B-1">Obtaining CVS distributions</a>
		<li> <a href="<?=$url_root?>&subset=B-2">Building <?=$MODULESLIST?></a>
		<li> <a href="<?=$url_root?>&subset=B-3">Initializing databases</a>
<? if ($modules=="cservice" || $modules=="cservice ccontrol") { ?>
		<li> <a href="<?=$url_root?>&subset=B-4">mod.cservice configuration</a>
<? } ?>
<? if ($modules=="ccontrol" || $modules=="cservice ccontrol") { ?>
		<li> <a href="<?=$url_root?>&subset=B-4b">mod.ccontrol configuration</a>
<? } ?>
		<li> <a href="<?=$url_root?>&subset=B-5">Running GNUworld</a>
<? if (ereg("cservice",$modules)) { ?>
		<li> <a href="<?=$url_root?>&subset=B-6">Configuring website (config.inc)</a>
		<li> <a href="<?=$url_root?>&subset=B-7">Configuring and setting up 'THE JUDGE'</a>
	</ol>
<? if ($r) { ?>
	<li> <a href="<?=$url_root?>&subset=C"><b>Setting up Apache and PHP</b></a>
	<ol type=1>
		<li> <a href="<?=$url_root?>&subset=C-1">Getting Apache</a>
		<li> <a href="<?=$url_root?>&subset=C-2">Getting PHP</a>
		<li> <a href="<?=$url_root?>&subset=C-3">Building Apache with PHP and PostrgreSQL support</a>
		<li> <a href="<?=$url_root?>&subset=C-4">Short HOWTO on apache's &lt;VirtualHost&gt; directive</a>
<? } ?>
	</ol>
<? } else { echo "</ol>\n"; } ?>
</ol>
<?
		footer("","");
	} else {
		if ($subset=="A") {
?>
<h2>A. Setting up PostgreSQL</h2>
This part will help you in setting up PostreSQL properly for a GNUworld usage of it.
<br><br>
<ol type=A start=1>
	<li> <b>Setting up PostgreSQL</b>
	<ol type=1>
		<li> <a href="<?=$url_root?>&subset=A-1">Getting PostgreSQL</a>
		<li> <a href="<?=$url_root?>&subset=A-2">Requirements</a>
		<ol type=i>
			<li> <a href="<?=$url_root?>&subset=A-2-i">Getting and Installing TCL 8.x+</a>
		</ol>
		<li> <a href="<?=$url_root?>&subset=A-3">Building PostgreSQL</a>
		<li> <a href="<?=$url_root?>&subset=A-4">Setting up Database space</a>
<? if ($secure) { ?>
		<li> <a href="<?=$url_root?>&subset=A-5">Setting up security for the database (passwords)</a>
<? } ?>
	</ol>
</ol>
<?
			footer("","");
		}
		if ($subset=="B") {
?>
<h2>B. Setting up GNUworld</h2>
This part will provide the help you need to setup the GNUworld program.
<br><br>
<ol type=A start=2>
	<li> <b>Setting up GNUworld</b>
	<ol type=1>
		<li> <a href="<?=$url_root?>&subset=B-1">Obtaining CVS distributions</a>
		<li> <a href="<?=$url_root?>&subset=B-2">Building <?=$MODULESLIST?></a>
		<li> <a href="<?=$url_root?>&subset=B-3">Initializing databases</a>
<? if ($modules=="cservice" || $modules=="cservice ccontrol") { ?>
		<li> <a href="<?=$url_root?>&subset=B-4">mod.cservice configuration</a>
<? } ?>
<? if ($modules=="ccontrol" || $modules=="cservice ccontrol") { ?>
		<li> <a href="<?=$url_root?>&subset=B-4b">mod.ccontrol configuration</a>
<? } ?>
		<li> <a href="<?=$url_root?>&subset=B-5">Running GNUworld</a>
<? if (ereg("cservice",$modules)) { ?>
		<li> <a href="<?=$url_root?>&subset=B-6">Configuring website (config.inc)</a>
		<li> <a href="<?=$url_root?>&subset=B-7">Configuring and setting up 'THE JUDGE'</a>
<? } ?>
	</ol>
</ol>
<?
			footer("","");
		}
		if ($subset=="C") {
?>
<h2>C. Setting up Apache and PHP</h2>
This part will help you in setting up Apache and PHP with PostgreSQL support and explain how you can add a virtualhost for your website.
<br><br>
<ol type=A start=3>
	<li> <b>Setting up Apache and PHP</b>
	<ol type=1>
		<li> <a href="<?=$url_root?>&subset=C-1">Getting Apache</a>
		<li> <a href="<?=$url_root?>&subset=C-2">Getting PHP</a>
		<li> <a href="<?=$url_root?>&subset=C-3">Building Apache with PHP and PostrgreSQL support</a>
		<li> <a href="<?=$url_root?>&subset=C-4">Short HOWTO on apache's &lt;VirtualHost&gt; directive</a>
	</ol>
</ol>
<?
			footer("","");
		}
		if ($subset=="R") {
?>
<h2>A. Requirements</h2>
This is what you need to ensure before trying to install GNUworld if you are not <b>root</b><br>
on your system and want to set up GNUworld using an already existing <?
	if (ereg("cservice",$modules)) {
		echo "Apache+PHP+";
	}
	echo "PostgreSQL";
?> Installation<br><br>
<pre>
<? if (ereg("cservice",$modules)) { ?>
	<b>-</b> Check that you can access a web browsable directory (web space) on the machine
	  (if you have an URL like http://www.some-site.com/~youruser/ and you place the files for that URL
	   in a directory in your HOME (for example /home/youruser/public_html) then YOU DO HAVE a web space.)

	<b>-</b> Check that the machine is running a web server with PHP4+ and PostgreSQL 7.2+ support :
	  create a file in your web space called <b>test_apache.php</b> and put the following code in it :
	  <font color=#888888>
	  	&lt;?
	  	phpinfo();
	  	?&gt;
	  </font>

	  Then access it to view the PHP options (http://www.some-site.com/~youruser/test_apache.php).
	  If you view the source code, your server probably either doesnt have PHP support, or doesnt support
	  the <b>.php</b> file extension. See your Server Administrator to fix this problem.

	  Check that the PHP version in the first purple block is <b>4.1.2 or above</b>.
	  Otherwise you don't have the required version of <b>PHP</b>, sorry.

	  In the next block, in section <b>Configure Command</b>, check that the command
	  contains <b>--with-pgsql</b>, otherwise your existing PHP does'nt have the PostgreSQL support.

<? } ?>
	<b>-</b> Check that your PostgreSQL installation was done with the following <b>./configure</b> option(s):
		<b>--with-CXX</b>		: building the C Client
		<b>--enable-multibyte</b>	: Needed to solve a createdb problem and allow php to build correctly.
<? if (ereg("cservice",$modules)) { ?>
		<b>--with-tcl</b>		: building with TCL support (<b>pgtclsh</b>) for mod.cservice.
<? } ?>

For the whole HOWTO here, we will assume PostreSQL is configured that way :
(replace with appropriate PATHs if yours are different)

		Binaries directory :	<b>/usr/local/pgsql/bin</b>
		LIB directory :		<b>/usr/local/pgsql/lib</b>
		Database directory :	<b>/usr/local/pgsql/data</b>

		User having DBA privs :	<b>gnuworld</b>

	<b>-</b> Check PostgreSQL version :

		<?=$os?>:/home/gnuworld$ <b>/usr/local/pgsql/bin/psql --version</b>

	  the first lines should return a version number above or equal to <b>7.2.1</b> like this :
	  (preferrably above <b>8.0.0</b>, version 7.2.x while compatible is not supported anymore
	   by PostgreSQL team)

	  	<font color=#888888>psql (PostgreSQL) 7.2.1
	  	contains readline, history support
		Portions Copyright (c) 1996-2000, PostgreSQL, Inc
		Portions Copyright (c) 1996 Regents of the University of California
		Read the file COPYRIGHT or use the command \copyright to see the
		usage and distribution terms.</font>

	<b>-</b> Check that your user (assuming <b>gnuworld</b>) has DBA (DataBase Administrator) privileges :

		<?=$os?>:/home/gnuworld$ <b>/usr/local/pgsql/bin/createdb test_gnu</b>
		<?=$os?>:/home/gnuworld$ <b>/usr/local/pgsql/bin/dropdb test_gnu</b>

	  if you could run this without any error, then your user has those privileges.
	  Otherwise you need your System Administrator to allow you those actions.
</pre>
<br><br>
<?
			footer("*","B-1");
		}

/* *** CAT. A : SETTING UP PGSQL *** */
/*
if ($subset=="") {
?>
<h2>Title</h2>
<pre>

</pre>
<?
	footer("","");
}
*/
if ($subset=="A-1") {
?>
<h2>Getting PostgreSQL</h2>
<pre>
In order to get PostgreSQL you should visit their official website :
	- <a href="http://www.postgresql.org/" target=_blank>http://www.postgresql.org/</a>
	- <a href="<?=PG_SQL_FTP_SOURCE?>" target=_blank>PostgreSQL Public FTP Site</a>

Direct link to download <b>PostgreSQL <?=PG_SQL_RECOM_VERSION?></b> (recommended version) :
	- <a href="<?=PG_SQL_FTP_SOURCE_FILE?>">FTP</a>
</pre>
<?
	footer("*","A-2");
}
if ($subset=="A-2") {
?>
<h2>Requirements</h2>
<pre>
GNUworld will need to use a tool called <b>pgtclsh</b> in PostgreSQL,
this tool is required to make the Registration System work properly,
if you don't build this programm you will not be able to complete the setup of 'THE JUDGE'
and your channel applications will all stag in <b>Pending</b> status.

<b>pgtclsh</b> needs the TCL library to be compiled.

To make sure you have TCL installed, do the following :

<? if ($os=="linux") { ?>
	<?=$os?>:/root# <b>/usr/bin/updatedb</b>
<? } ?>
<? if ($os=="freebsd") { ?>
	<?=$os?>:/root# <b>/usr/libexec/locate.updatedb</b>
<? } ?>
	<?=$os?>:/root# <b>locate tclConfig.sh</b>

<? if ($os=="linux") { ?>
this should return something like <b>/usr/lib/tclConfig.sh</b> if you have TCL installed.
<? } ?>
<? if ($os=="freebsd") { ?>
this should return something like <b>/usr/local/lib/tcl8.x/tclConfig.sh</b> if you have TCL installed.
<? } ?>
If you dont have that file, go to <a href="<?=$url_root?>&subset=A-2-i">Getting and installing TCL 8.x+</a>.

If you have it, check that you have the proper version by doing :

<? if ($os=="linux") { ?>
	<?=$os?>:/root# <b>grep TCL_VERSION /usr/lib/tclConfig.sh</b>
<? } ?>
<? if ($os=="freebsd") { ?>
	<?=$os?>:/root# <b>grep TCL_VERSION /usr/local/lib/tcl8.x/tclConfig.sh</b>
<? } ?>

this should return something above or equal to <b>8.0</b>.
If this number is 8.0 or above, please <a href="<?=$url_root?>&subset=A-3">click here</a> or go to next step,
if you have an older version of TCL please see <a href="<?=$url_root?>&subset=A-2-i">Getting and installing TCL 8.x+</a>.

</pre>
<?
	footer("A-1","A-3");
}
if ($subset=="A-2-i") {
?>
<h2>Getting and installing TCL 8.x+</h2>
<pre>
<? if ($os=="linux") { ?>

Get the latest TCL package in :
	- <a href="<?=TCL_83_LINUX_SOURCE?>" target=_blank><?=TCL_83_LINUX_SOURCE?></a>

Recommended <b>TCL <?=TCL_83_RECOM_VERSION?></b> :
	- <a href="<?=TCL_83_LINUX_SOURCE_FILE?>"><?=TCL_83_LINUX_FILE_NAME?></a>

When you get the file <b><?=TCL_83_LINUX_FILE_NAME?></b>, just unpack it in a place you have some space,
your home directory can be a good place to put that.

	<?=$os?>:/root# <b>tar -xzf <?=TCL_83_LINUX_FILE_NAME?></b>

now <b>cd</b> to the directory just created ...

	<?=$os?>:/root# <b>cd <?=str_replace(".tar.gz","",TCL_83_LINUX_FILE_NAME)?>/unix/</b>
	<?=$os?>:/root/<?=str_replace(".tar.gz","",TCL_83_LINUX_FILE_NAME)?>/unix#

run the following commands to build and install TCL <?=TCL_83_RECOM_VERSION?> on your <?=$OSNAME?> :

	<?=$os?>:/root/<?=str_replace(".tar.gz","",TCL_83_LINUX_FILE_NAME)?>/unix# <b>./configure</b>
	<?=$os?>:/root/<?=str_replace(".tar.gz","",TCL_83_LINUX_FILE_NAME)?>/unix# <b>make</b>
	<?=$os?>:/root/<?=str_replace(".tar.gz","",TCL_83_LINUX_FILE_NAME)?>/unix# <b>make install</b>

if you run into trouble, additional information can be found <a href="http://www.tcl-tk.net/doc/howto/compile.html#unix" target=_blank>here</a>.
<? } ?>
<? if ($os=="freebsd") { ?>

Check if you have the recommended <b>TCL 8.4.x</b> ports for your <?=$OSNAME?>,
if the directory <b>/usr/ports/lang/tcl84</b> doesn't exist on your system,
then download the corresponding FreeBSD ports <a href="<?=TCL_83_PORT_SOURCE_FILE?>" target=_blank>here</a> (needs FreeBSD port installation knowledge).

<b>cd</b> to the directory, and build TCL 8.4.x :

	<?=$os?>:/root# <b>cd /usr/ports/lang/tcl84/</b>
	<?=$os?>:/usr/ports/lang/tcl84# <b>make</b>

FreeBSD should take care of the full installation.

<? } ?>
</pre>
<?
	footer("A-2","A-3");
}
if ($subset=="A-3") {
?>
<h2>Building PostgreSQL</h2>
<pre>
<font color=#ff1111><b>If you have trouble installing, and prior to asking PostgreSQL team about it,
please ensure you have a recent version of PostgreSQL, old 7.x series are not supported anymore !</b></font>

Get the <b>postgresql-<?=PG_SQL_RECOM_VERSION?>.tar.gz</b> file you downloaded into step 1.
Unpack it :

	<?=$os?>:/root# <b>tar -xzf postgresql-<?=PG_SQL_RECOM_VERSION?>.tar.gz</b>
	<?=$os?>:/root# <b>cd postgresql-<?=PG_SQL_RECOM_VERSION?>/</b>
	<?=$os?>:/root/postgresql-<?=PG_SQL_RECOM_VERSION?># <b>./configure --with-CXX \
		--enable-multibyte \
<? if ($os=="freebsd") { ?>
		--with-tcl --without-tk \
		--with-tclconfig=/usr/local/lib/tcl8.4  \
		--with-includes=/usr/local/include/tcl8.4 \
		--with-libraries=/usr/local/lib/tcl8.4</b>
<? } ?>
<? if ($os=="linux") { ?>
		--with-tcl --without-tk</b>

If the configure complains about not finding <b>tclConfig.sh</b>, or if
you experience missing <b>.h</b> files when doing the next steps of compilation,
you may want to re-run the <b>./configure</b> adding the following parameters :

		--with-tclconfig=/path/to/tclConfig.sh_directory
		--with-includes=/path/to/tcl/include
		--with-libraries=/path/to/tcl/lib
<? } ?>


You need <b>gmake 3.79</b> or above to build PostgreSQL (and you will also need it for GNUworld later),
check you have it by running <b>gmake --version</b>, you should get something like this :
Note: You will also need <b>automake 1.6</b> or above in order to build GNUworld.

	<?=$os?>:/root/postgresql-<?=PG_SQL_RECOM_VERSION?># <b>gmake --version</b><font color=#999999>
	GNU Make version 3.79.1, by Richard Stallman and Roland McGrath.
<? if ($os=="linux") { ?>
	Built for i686-pc-linux-gnu
<? } ?>
<? if ($os=="freebsd") { ?>
	Built for i386--freebsd4.4
<? } ?>
	Copyright (C) 1988, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 2000
	        Free Software Foundation, Inc.
	This is free software; see the source for copying conditions.
	There is NO warranty; not even for MERCHANTABILITY or FITNESS FOR A
	PARTICULAR PURPOSE.

	Report bugs to &lt;bug-make@gnu.org&gt;.	</font>

If you don't have <b>gmake</b>, you can get it <a href="ftp://ftp.grolier.fr/mirrors/ftp.gnu.org/gnu/make/make-3.80.tar.gz">here</a>.
If you don't have <b>automake 1.6+</b>, you can get 1.7.9 <a href="ftp://ftp.grolier.fr/mirrors/ftp.gnu.org/gnu/automake/automake-1.7.9.tar.gz">here</a>.
If that's ok, you can go on and build PostgreSQL for real, then install it...

	<?=$os?>:/root/postgresql-<?=PG_SQL_RECOM_VERSION?># <b>gmake</b>


If compilation fails due to an error like ...<font color=#aaaaaa>
	commands/SUBSYS.o(.text+0x4902): In function `DoCopy':
	: undefined reference to `errno'
	collect2: ld returned 1 exit status
	gmake[2]: *** [postgres] Error 1
	gmake[2]: Leaving directory `/root/postgresql-<?=PG_SQL_RECOM_VERSION?>/src/backend'</font>
... then open file '/root/postgresql-<?=PG_SQL_RECOM_VERSION?>/src/backend/commands/copy.c'
and add <b>#include &lt;errno.h&gt;</b> to the includes already present.
You will then be able to do <b>gmake</b> again so it will end without error.


	<?=$os?>:/root/postgresql-<?=PG_SQL_RECOM_VERSION?># <b>gmake install</b>

<? if ($os=="freebsd") { ?>
If you are using <b>bash</b> as your shell programm, add the following line :

        export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/local/pgsql/lib

into <b>/etc/profile</b>.
<b>zsh</b>, <b>sh</b> and <b>ksh</b> use the same command in their respective <b>.shrc .zshrc .kshrc</b> ~ files.


For <b>csh</b> and <b>tcsh</b> you will need to use <b>setenv</b> instead :

        setenv LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/local/pgsql/lib


<? } ?>
<? if ($os=="linux") { ?>
Edit the file <b>/etc/ld.so.conf</b> and add the following line :

                   /usr/local/pgsql/lib


save the file, then run :

	<?=$os?>:/root/postgresql-<?=PG_SQL_RECOM_VERSION?># <b>ldconfig</b>

<? } ?>

To check that your PostreSQL installation includes TCL support, check that the following
file exists onto your system :

	<b>/usr/local/pgsql/bin/pgtclsh</b>

</pre>
<?
	footer("A-2","A-4");
}
if ($subset=="A-4") {
?>
<h2>Setting up Database space</h2>
<pre>

You now need to create a new user on the system that will be the <b>DB Administrator</b>
of your PostgreSQL, the following example will assume the <b>username</b> used by GNUworld
to access the database will be that DB Administrator.

<? if ($secure) { ?>
The details on how to handle multiple users and password over PostgreSQL, and thus
making it possible to choose another user for your GNUworld to use, are described in the next part.
<? } ?>

Creating the user, assuming username <b>gnuworld</b> :

	<?=$os?>:/root# <b>adduser</b><font color=#999999><? if ($os=="freebsd") { ?>
	Use option ``-silent'' if you don't want to see all warnings and questions.

	Check /etc/shells
	Check /etc/master.passwd
	Check /etc/group
	Enter your default shell: bash csh date false no sh tcsh
	[bash]: <font color=#009900><b>[</b>ENTER<b>]</b></font>
	Your default shell is: bash -> /usr/bin/bash
	Enter your default HOME partition: [/home]: <font color=#009900><b>[</b>ENTER<b>]</b></font>
	Copy dotfiles from: /usr/share/skel no [/usr/share/skel]: <font color=#009900><b>[</b>ENTER<b>]</b></font>
	Send message from file: /etc/adduser.message no [no]: <font color=#009900><b>[</b>ENTER<b>]</b></font>
	Do not send message
	Use passwords (y/n) [y]: <font color=#009900><b>[</b>ENTER<b>]</b></font>

	Ok, let's go.
	Don't worry about mistakes. I will give you the chance later to correct any input.
	Enter username [a-z0-9_-]: <font color=#000000><b>gnuworld</b></font>
	Enter full name []: <font color=#000000><b>GNUworld Account</b></font>
	Enter shell bash csh date false no sh tcsh [bash]: <font color=#009900><b>[</b>ENTER<b>]</b></font>
	Enter home directory (full patrld]: <font color=#009900><b>[</b>ENTER<b>]</b></font>
	Uid [1001]: <font color=#009900><b>[</b>ENTER<b>]</b></font>
	Enter login class: default []: <font color=#009900><b>[</b>ENTER<b>]</b></font>
	Login group gnuworld [gnuworld]: <font color=#009900><b>[</b>ENTER<b>]</b></font>
	Login group is ``gnuworld''. Invite gnuworld into other groups: guest no
	[no]: <font color=#009900><b>[</b>ENTER<b>]</b></font>
	Enter password []: <font color=#000000><b>********</b></font>
	Enter password again []: <font color=#000000><b>********</b></font>

	Name:     gnuworld
	Password: ****
	Fullname: GNUworld Account
	Uid:      1001
	Gid:      1001 (gnuworld)
	Class:
	Groups:   gnuworld
	HOME:     /home/gnuworld
	Shell:    /usr/bin/bash
	OK? (y/n) [y]: <font color=#009900><b>[</b>ENTER<b>]</b></font>
	Added user ``gnuworld''
	Copy files from /usr/share/skel to /home/gnuworld
	Add another user? (y/n) [y]: <font color=#000000><b>n</b></font>
	Goodbye!</font>
<? } ?>
<? if ($os=="linux") { ?>


	Login name for new user (8 characters or less) []: <font color=#000000><b>gnuworld</b></font>

	User id for gnuworld [ defaults to next available]: <font color=#009900><b>[</b>ENTER<b>]</b></font>

	Initial group for gnuworld [users]: <font color=#009900><b>[</b>ENTER<b>]</b></font>

	Additional groups for gnuworld (seperated
	with commas, no spaces) []: <font color=#009900><b>[</b>ENTER<b>]</b></font>

	gnuworld's home directory [/home/gnuworld]: <font color=#009900><b>[</b>ENTER<b>]</b></font>

	gnuworld's shell [/bin/bash]: <font color=#009900><b>[</b>ENTER<b>]</b></font>

	gnuworld's account expiry date (YYYY-MM-DD) []: <font color=#009900><b>[</b>ENTER<b>]</b></font>

	OK, I'm about to make a new account. Here's what you entered so far:

	New login name: gnuworld
	New UID: [Next available]
	Initial group: users
	Additional groups: [none]
	Home directory: /home/gnuworld
	Shell: /bin/bash
	Expiry date: [no expiration]

	This is it... if you want to bail out, hit Control-C.  Otherwise, press
	ENTER to go ahead and make the account.
	<font color=#009900><b>[</b>ENTER<b>]</b></font>

	Making new account...

	Changing the user information for gnuworld
	Enter the new value, or press return for the default
	        Full Name []: <font color=#000000><b>GNUworld Account</b></font>
	        Room Number []: <font color=#009900><b>[</b>ENTER<b>]</b></font>
	        Work Phone []: <font color=#009900><b>[</b>ENTER<b>]</b></font>
	        Home Phone []: <font color=#009900><b>[</b>ENTER<b>]</b></font>
	        Other []: <font color=#009900><b>[</b>ENTER<b>]</b></font>

	Changing password for gnuworld
	Enter the new password (minimum of 5, maximum of 127 characters)
	Please use a combination of upper and lower case letters and numbers.
	New password: <font color=#000000><b>********</b></font>
	Re-enter new password: <font color=#000000><b>********</b></font>
	Password changed.
	Done...</font>
<?  } ?>

	<?=$os?>:/root# <b>mkdir /usr/local/pgsql/data</b>
	<?=$os?>:/root# <b>chown gnuworld /usr/local/pgsql/data</b>
	<?=$os?>:/root# <b>su - gnuworld</b>
	<?=$os?>:/home/gnuworld$ <b>/usr/local/pgsql/bin/initdb -D /usr/local/pgsql/data</b>

Then run <b>PostgreSQL</b> :

	<?=$os?>:/home/gnuworld$ <b>/usr/local/pgsql/bin/postmaster -S -B 64 -N 32 -i -D /usr/local/pgsql/data -o -F</b>

Note: if you plan on using this *STRICTLY* locally, and you want to ensure ONLY local connections are possible
therefore not requiring your to lock out port 5432 on your firewall to prevent remote access to your PostgreSQL database,
then you can run the following command instead of the above to run your PostgreSQL :
	<?=$os?>:/home/gnuworld$ <b>/usr/local/pgsql/bin/postmaster -S -B 64 -N 32 -i -D /usr/local/pgsql/data -o -F -h 127.0.0.1</b>
<font color=#ff0000><b>You should run only ONE of those commands, not both !</b></font>


	<?=$os?>:/home/gnuworld$ <b>exit</b>
	<?=$os?>:/root#

</pre>
<?
	if ($secure) {
		footer("A-3","A-5");
	} else {
		footer("A-3","B");
	}
}
if ($subset=="A-5") {
?>
<h2>Setting up security for the database (passwords)</h2>
<pre>

If, for example you want to install a PostgreSQL on a machine where resides some users you
don't trust, or just don't want to be able, just with a .php to edit your PostgreSQL database,
you should protect the access to the database with user passwords.

For this you will need to edit the <b>/usr/local/pgsql/data/pg_hba.conf</b> file.

Quite at the end of that file, you will see the two following lines :
<b>
	local        all                                           trust
	host         all         127.0.0.1     255.255.255.255     trust
</b>
Replace the two words '<b>trust</b>' by the word '<b>crypt</b>', making the lines look like :
<b>
	local        all                                           crypt
	host         all         127.0.0.1     255.255.255.255     crypt
</b>

This will require the user connecting to the database to issue the good password in pg_shadow,
in other words the UNIX password of that local user.

<b>NOTE:</b> You will NEED to use that password in <?
if ($modules=="cservice ccontrol") { echo "mod.cservice,\nmod.ccontrol "; }
if ($modules=="ccontrol") { echo "mod.ccontrol\n"; }
if ($modules=="cservice") { echo "mod.cservice\n"; }
?>and website configuration in the next steps.

After that change, you need to restart your PostreSQL, do the following :

	<?=$os?>:/root# <b>su - gnuworld</b>
	<?=$os?>:/home/gnuworld$ <b>/usr/local/pgsql/bin/pg_ctl -D /usr/local/pgsql/data restart</b>


</pre>
<?
	footer("A-4","B");
}

/* *** CAT. B : SETTING UP GNUWORLD *** */
if ($subset=="B-1") {
?>
<h2>Obtaining CVS distributions</h2>
<pre>
To obtain <b>gnuworld</b> <?
if (ereg("cservice",$modules)) {
	echo "and <b>website</b> ";
}
?>using CVS, issue the following commands :

<? if (!$secure && $r) { ?>
	<?=$os?>:/root# <b>su - gnuworld</b>
<? } ?>
	<?=$os?>:/home/gnuworld$ <b>touch .cvspass</b>
	<?=$os?>:/home/gnuworld$ <b>cvs -d:pserver:anonymous@gnuworld.cvs.sourceforge.net:/cvsroot/gnuworld login</b>

Just hit the <b>[</b>ENTER<b>]</b> key as the password.

	<?=$os?>:/home/gnuworld$ <b>cvs -z3 -d :pserver:anonymous@gnuworld.cvs.sourceforge.net:/cvsroot/gnuworld co gnuworld</b>
<? if (ereg("cservice",$modules)) { ?>
	<?=$os?>:/home/gnuworld$ <b>cvs -z3 -d :pserver:anonymous@gnuworld.cvs.sourceforge.net:/cvsroot/gnuworld co website</b>

You should now have two directories in <b>/home/gnuworld</b> called <b>gnuworld/</b> and <b>website/</b>;
they contain the CVS latest distributions of both c++ program and website.
<? } else { ?>

You should now have one directory in <b>/home/gnuworld</b> called <b>gnuworld/</b>;
it contains the CVS latest distribution of c++ program for GNUworld.
<? } ?>
</pre>
<?
	if ($r) {
		if ($secure) {
			footer("A-5","B-2");
		} else {
			footer("A-4","B-2");
		}
	} else {
		footer("R","B-2");
	}
}
if ($subset=="B-2") {
?>
<h2>Building <?
if ($modules=="cservice ccontrol") {
	echo "mod.cservice and mod.ccontrol modules";
}
if ($modules=="cservice") {
	echo "mod.cservice module";
}
if ($modules=="ccontrol") {
	echo "mod.ccontrol module";
}
?></h2>
<pre>
<b>cd</b> to the <b>gnuworld/</b> directory :

	<?=$os?>:/home/gnuworld$ <b>cd gnuworld/</b>
	<?=$os?>:/home/gnuworld/gnuworld$ <b>./configure --enable-modules=<?=str_replace(" ",",",$modules)?></b>
	<?=$os?>:/home/gnuworld/gnuworld$ <b>gmake</b>

<b>NOTE:</b>	Compilation may take time, up to a couple of hours on slow machines, just get a cofee ;)
	if you run on an SMP (multiple CPUs) machine, you can use <b>gmake -j 2</b> (up to <b>gmake -j 12</b>) instead of <b>gmake</b>,
	to parallelize the compilation thus making it faster.

If you have the compiler complaining about <b>libpq++.h</b>, that means :
<? if ($r) { ?>
	- you didn't added the required stuff to <?

	if ($os=="linux") { echo "<b>/etc/ld.so.conf</b>\n"; }
	if ($os=="freebsd") { echo "<b>LD_LIBRARY_PATH</b>\n"; }

	?>
	  as explained in <a href="<?=$url_root?>&subset=A-3">Building PostgreSQL</a>.

	- your PostgreSQL is NOT setup properly
	  (ie. was not build with the <b>--with-CXX</b> configure option),
	  you should then go to <a href="<?=$url_root?>&subset=A">Setting up PostgreSQL</a>.
<? } else { ?>
	- your PostgreSQL is not properly setup and needs to be rebuilt
	  by the system administrator (<b>root</b>) using the required ./configure options.
	  (see <a href="<?=$url_root?>&subset=R">Requirements</a>).
<? } ?>


	<?=$os?>:/home/gnuworld/gnuworld$ <b>gmake install</b>
	<?=$os?>:/home/gnuworld/gnuworld$ <b>cd ..</b>
	<?=$os?>:/home/gnuworld$

</pre>
<?
	footer("B-1","B-3");
}
if ($subset=="B-3") {
?>
<h2>Initializing databases</h2>
<pre>

	<?=$os?>:/home/gnuworld$ <b>cd gnuworld/doc/</b>
	<?=$os?>:/home/gnuworld/gnuworld/doc$

<? if (ereg("cservice",$modules)) { ?>
<h2>mod.cservice (X like module)</h2>

Create the database :

	<?=$os?>:/home/gnuworld/gnuworld/doc$ <b>/usr/local/pgsql/bin/createdb cservice</b>

Install the <b>PL</b> language :
(if you skip this step or do it AFTER anything has been put in the database,
this will <b>NOT</b> work and you will not be able to login properly to the bot on IRC)

	<?=$os?>:/home/gnuworld/gnuworld/doc$ <b>/usr/local/pgsql/bin/createlang -L /usr/local/pgsql/lib plpgsql cservice</b>

Load the CService schema :

	<?=$os?>:/home/gnuworld/gnuworld/doc$ <b>/usr/local/pgsql/bin/psql cservice < cservice.sql</b>
	<?=$os?>:/home/gnuworld/gnuworld/doc$ <b>/usr/local/pgsql/bin/psql cservice < cservice.config.sql</b>
	<?=$os?>:/home/gnuworld/gnuworld/doc$ <b>/usr/local/pgsql/bin/psql cservice < languages.sql</b>
	<?=$os?>:/home/gnuworld/gnuworld/doc$ <b>/usr/local/pgsql/bin/psql cservice < language_table.sql</b>
	<?=$os?>:/home/gnuworld/gnuworld/doc$ <b>/usr/local/pgsql/bin/psql cservice < greeting.sql</b>
	<?=$os?>:/home/gnuworld/gnuworld/doc$ <b>/usr/local/pgsql/bin/psql cservice < cservice.help.sql</b>
	<?=$os?>:/home/gnuworld/gnuworld/doc$ <b>/usr/local/pgsql/bin/psql cservice < cservice.web.sql</b>

You can safely ignore WARNINGS and NOTICES here.

Now let's create the database that the website will use to handle user logging in on the Web.

	<?=$os?>:/home/gnuworld/gnuworld/doc$ <b>/usr/local/pgsql/bin/createdb local_db</b>
	<?=$os?>:/home/gnuworld/gnuworld/doc$ <b>/usr/local/pgsql/bin/psql local_db < local_db.sql</b>

Now add the CService administrator to the <b>cservice</b> database.
This command will add an account called 'Admin' with level 1000 access on
channel '*'...use this power wisely, The Dark Side is very powerful.

This will also register channel #coder-com to 'Admin' to make the VERIFY
command work on authenticated users.

	<?=$os?>:/home/gnuworld/gnuworld/doc$ <b>/usr/local/pgsql/bin/psql cservice < cservice.addme.sql</b>

<b>NOTE:</b> You will change the password for that user later.
<?
	if (ereg("ccontrol",$modules)) {
		echo "\n\n\n\n";
	}
} ?>

<? if (ereg("ccontrol",$modules)) { ?>

<h2>mod.ccontrol (Uworld like module)</h2>

Create the database :

	<?=$os?>:/home/gnuworld/gnuworld/doc$ <b>/usr/local/pgsql/bin/createdb ccontrol</b>

Load the CControl schema :

	<?=$os?>:/home/gnuworld/gnuworld/doc$ <b>/usr/local/pgsql/bin/psql ccontrol < ccontrol.sql</b>
	<?=$os?>:/home/gnuworld/gnuworld/doc$ <b>/usr/local/pgsql/bin/psql ccontrol < ccontrol.help.sql</b>

Now add the CControl administrator to the <b>ccontrol</b> database.
This command will add an account called 'Admin' with level CODER.

	<?=$os?>:/home/gnuworld/gnuworld/doc$ <b>/usr/local/pgsql/bin/psql ccontrol < ccontrol.addme.sql</b>

<b>NOTE:</b> You will change the password for that user later.

Then finish your CControl database setup with the following command :

	<?=$os?>:/home/gnuworld/gnuworld/doc$ <b>/usr/local/pgsql/bin/psql ccontrol < ccontrol.commands.sql</b>


<? } ?>


</pre>
<?
	if (ereg("cservice",$modules)) {
		footer("B-2","B-4");
	} else {
		footer("B-2","B-4b");
	}
}
if ($subset=="B-4") {
?>
<h2>mod.cservice configuration</h2>
<pre>

Go to the directory where the configuration files resides :

	<?=$os?>:/home/gnuworld/gnuworld/doc$ <b>cd ../bin</b>
	<?=$os?>:/home/gnuworld/gnuworld/bin$

Edit the file <b>cservice.conf</b> and change to whatever fits your local configuration,
mandatory changes should be :

	<b>sql_db</b> = cservice
<? if ($secure) { ?>
	<b>sql_pass</b> = the_password_for_user_gnuworld
<? } ?>

you should edit the following variables in that configuration, and leave the others untouched :

	<b>sql_db</b>, <b>sql_pass</b>, <b>nickname</b>, <b>username</b>, <b>hostname</b>, <b>userdescription</b>,
	<b>pending_page_url</b>, <b>relay_channel</b>, and <b>debug_channel</b>.

<b>NOTE:</b>	If you don't have any password, you probably didn't restricted your PostgreSQL,
	in that case, just leave the default one (<b>foo</b>), it will just be ignored
	but you need to put something, thus not leaving it blank.

</pre>
<?
	if (ereg("ccontrol",$modules)) {
		footer("B-3","B-4b");
	} else {
		footer("B-3","B-5");
	}
}

if ($subset=="B-4b") {
?>
<h2>mod.ccontrol configuration</h2>
<pre>
<? if (!ereg("cservice",$modules)) { ?>

Go to the directory where the configuration files resides :

	<?=$os?>:/home/gnuworld/gnuworld/doc$ <b>cd ../bin</b>
	<?=$os?>:/home/gnuworld/gnuworld/bin$
<? } ?>

Edit the file <b>ccontrol.conf</b> and change to whatever fits your local configuration,
mandatory changes should be :

	<b>sql_db</b> = ccontrol
<? if ($secure) { ?>
	<b>sql_pass</b> = the_password_for_user_gnuworld
<? } ?>

you should edit the following variables in that configuration, and leave the others untouched :

	<b>sql_db</b>, <b>sql_pass</b>, <b>nickname</b>, <b>username</b>, <b>hostname</b>, <b>userdescription</b>,
	<b>operchan</b>, <b>msgchan</b>, <b>abuse_mail</b>, and <b>ccemail</b>.

<b>NOTE:</b>	If you don't have any password, you probably didn't restricted your PostgreSQL,
	in that case, just leave the default one (<b>''</b>), it will just be ignored
	but you need to put something, thus not leaving it blank.



mod.ccontrol is not, by default, enabled in <b>GNUworld.conf</b>,
you need to find the following line in that file :

	<b>#module = libccontrol.la ccontrol.conf</b>

and uncomment it (remove the first '#'), thus giving :

	<b>module = libccontrol.la ccontrol.conf</b>


</pre>
<?
	if (ereg("cservice",$modules)) {
		footer("B-4","B-5");
	} else {
		footer("B-3","B-5");
	}
}

if ($subset=="B-5") {
?>
<h2>Running GNUworld</h2>
<pre>

Running GNUworld needs an IRC (P10 Protocol compliant) server ready to welcome it.
We recommend the use of the latest <b>ircu2.10.x</b> from Undernet's Coder-comittee.

Latest known STABLE version is <b>ircu2.10.12.08</b>, please visit <a href="http://coder-com.undernet.org/" target=_blank>http://coder-com.undernet.org/</a>
to get information on how to download and install it.

We will assume you have set up an <b>ircu2.10.12.08</b> server for the example.

Edit the file <b>GNUworld.conf</b> in the <b>/home/gnuworld/gnuworld/bin/</b> directory,
and define the properties of the GNUWorld Oper Service server here.

<b>*** WARNING ***</b>: The default <b>numeric</b> for the GNUWorld Oper Service server is <b>51</b>,
if you already have a Server linked on your network using that numeric,
GNUworld will not be able to link, and complain about <b>NUMERIC COLLISION</b>...
you need, if so, to pick a new numeric, not already in use onto your IRC Network.

<b>uplink</b>		must be the numerical IP of the IRC server GNUworld will link to
		This server <b>needs</b> to be compiled as a <b>HUB</b> (see ircu installation documentation).
		You can use <b>127.0.0.1</b> if your IRC server is on the same machine as GNUworld.
		We will assume the IP to be <b>192.168.0.1</b>.

<b>port</b>		this should usually be kept at <b>4400</b>. It is the <b>server port</b> to which
		GNUworld will try to connect to when linking to <b>uplink</b>.

<b>password</b>	this can be anything you want, avoid using weird chars,
		it's just the password GNUworld will use to authenticate itself when linking to <b>uplink</b>,
		this will also be the C:Line password on <b>uplink</b>.
		We will assume the password to be <b>testlink</b>.

<b>name</b>		This will be the server name of the GNUworld Oper Service server, keep it in mind,
		we will assume it is <b>services.undernet.org</b>.

<b>description</b>	Put whatever you want here, it will just be the server comment
		in the <b>/whois X</b> or <b>/whois Uworld</b> outputs.

Do not change <b>maxclients</b>.
At this step you should have finished editing <b>GNUworld.conf</b>, save it.

Now, right before trying to run GNUworld, you need to setup your C:Lines, H:Lines and U:Lines on
your IRC server(s) in order for GNUworld to be able to operate normally.

You will need to add the following <b>U:Line/U Block</b> in <b>*ALL*</b> servers on your network
(or Uworld will not be able to do modes, and X won't be able to op itself in channels)

U:Line (for ircd versions up to ircu2.10.11.x series)
	<b>U:services.undernet.org:<?=str_replace(" ",",",str_replace("cservice","X",str_replace("ccontrol","Uworld",$modules)))?>:*</b>

U Block (for ircd versions from ircu2.10.12.x series)
	[N/A]

You need to add the following lines/blocks only to the <b>ircd.conf</b> of the <b>uplink</b> server :

Lines to add (for ircd versions up to ircu2.10.11.x series)
	<b>H:*:*:services.undernet.org</b>
	<b>C:192.168.0.1:testlink:services.undernet.org::90</b>

Block additions (for ircd versions from ircu2.10.12.x series)
	[N/A]

<i>this assumes <b>90</b> is a valid Y:Line in your config and is of type <b>Server Class</b>,
replace, if needed by the apropriate number.</i>


Do not forget to either <b>kill -HUP &lt;your ircd PID&gt;</b> or <b>/rehash</b> as an Oper on it
for the changes in ircd.conf to take effect.


<h1>NOW ...</h1>
You should be able to start GNUworld !

issue the following command to start it :

	<?=$os?>:/home/gnuworld/gnuworld/bin$ <b>./gnuworld -c -f GNUWorld.conf &</b>

If, for some reason, this doesnt work at this point and GNUworld either doesn't link at all
to your network or gets disconnected immediately, take a look at the following files' last lines
before asking your question on <b>#coder-com</b> on <b>Undernet</b> :

		<b>/home/gnuworld/gnuworld/bin/socket.log</b>
		and
		<b>/home/gnuworld/gnuworld/bin/debug.log</b>


<? if ($modules=="cservice ccontrol") { ?>
If GNUworld is linked, you should be able to <b>/whois</b> the two <b>nickname</b>s
of the two clients (X-like and Uworld-like) you defined in <b>cservice.conf</b> and <b>ccontrol.conf</b>.
<? } ?>
<? if ($modules=="cservice") { ?>
If GNUworld is linked, you should be able to <b>/whois</b> the <b>nickname</b>
of the X-like client you defined in <b>cservice.conf</b>.
<? } ?>
<? if ($modules=="ccontrol") { ?>
If GNUworld is linked, you should be able to <b>/whois</b> the <b>nickname</b>
of the Uworld-like client you defined in <b>ccontrol.conf</b>.
<? } ?>

<? if (ereg("cservice",$modules)) { ?>
To login to <b>X</B>, issue the following command on IRC :

		<b>/msg X@services.undernet.org login Admin temPass</b>
		-X- AUTHENTICATION SUCCESSFULL AS Admin!

You will change your password later using the web interface since the 'newpass' command was lately
removed in X and is now outdated.
<? } ?>


<? if (ereg("ccontrol",$modules)) { ?>
To login to <b>Uworld</b>, issue the following command on IRC :

		<b>/msg Uworld login Admin temPass</b>
		-Uworld- Authentication successfull.

Then change your password :
		<b>/msg Uworld newpass <i>your_new_password</i></b>
		-Uworld- Password changed.

The user's default for the Admin account is to allow non Opers to log in, provided
they have the good password, since that user has alot of power, it is probably a good
idea to change this so only Opers could login as <b>Admin</b>.
For this, issue the following command :

		<b>/msg Uworld moduser Admin -op on</b>
		-Uworld- NeedOp set to ON for Admin.
<? } ?>

<? if (ereg("cservice",$modules)) { ?>
<font color=#ff0000><b>
You now need to configure the website in order to use all the features of GNUworld,
if you do not go on, you will not be able to use features as :
		- Channel automatic registration
		- User creation
		- Various forms and password recovery
		- Ability for admins to view and control alot of things over mod.cservice
</b></font>
<? } else { ?>
<font color=#009900><b>
You have finished setting up GNUworld mod.ccontrol module.

Clicking "Next" will bring you to the Table of Contents of this Interactive Guide.

If you need any extra information, please feel free to <b>/join #coder-com</b>
on <b>/server us.undernet.org</b> or <b>/server eu.undernet.org</b>.
</b></font>
<? } ?>
</pre>
<?
	if (ereg("ccontrol",$modules)) {
		if (ereg("cservice",$modules)) {
			footer("B-4b","B-6");
		} else {
			footer("B-4b","*");
		}
	} else {
		footer("B-4","B-6");
	}
}

if ($subset=="B-6") {
?>
<h2>Configuring website (config.inc)</h2>
<pre>

Configuring <b>config.inc</b> is required, but should'nt be too much painfull ;)

	<?=$os?>:/home/gnuworld/gnuworld/bin$ <b>cd ../../website/php_includes/</b>
	<?=$os?>:/home/gnuworld/website/php_includes$

Edit the file <b>config.inc</b> with any decent text editor...
		<b>DO NOT USE wordpad.exe HERE</b> (or anywhere else tho ;P).

if you used wordpad.exe, <a href="http://www.microsoft.com/" target=_blank>click here</a> and chill ;).

<font color=#ff0000><b>
IMPORTANT NOTICE:</b>
	The <b>config.inc</b> file starts with <b>&lt;?</b> and ends with <b>?&gt;</b>.
	This should remain as is, if you insert <b>any</b> spaces or lines at the end or the begining
	of the file, the website will not work, echoing errors about <b>Cannot add header...</b>.
</font>

<font color=#888888>
	&lt;?
	/*

	&lt;!-- <? echo "\$" . "Id" ?>: config.inc,v -.-- ----/--/-- --:--:-- nighty Exp <? echo "\$" ?> //--&gt;

	'config.inc' For GNUworld's mod.cservice's website interface

	Created: <nighty@undernet.org> - 10/29/2001
	Last modif: <nighty@undernet.org> - --/--/----

	This file contains two sections,
	- what you *MUST* edit to ensure your website can work properly.
	- what you *CAN* edit to tweak it more precisely.

	Enjoy!@#

	*/

	/*------------------------------------------------------------------------*/
	/*   WHAT YOU NEED TO EDIT (when finished, look at the end of this file)  */
	/*------------------------------------------------------------------------*/

<font color=#000000>For the example we will assume it stays in a <b>/live</b> subdir of an existing website.</font>
	define(LIVE_LOCATION,"/live"); // define this to "" if you dont put it in a subdirectory like '/live' (default) but in '/' for example.

<font color=#000000>Set this to whatever your site will have as URL and the others for your convenience.</font>
<? if ($r) { ?>
	define(IFACE_URL,"<font color=#000000><b>http://www.your-site.com/live/</b></font>"); // The GUI main URL (the one to access it)
<? } else { ?>
	define(IFACE_URL,"<font color=#000000><b>http://www.some-site.com/~youruser/live/</b></font>"); // The GUI main URL (the one to access it)
<? } ?>
	define(SERVICE_CHANNEL,"#cservice"); // The CService Official Channel

	/* NOTE: to FORGE reply adresses, user 'nobody' needs a 'Tnobody' line added in /etc/sendmail.cf (or equiv.) */

	define(NETWORK_NAME,"Your Network"); // Define this to your network name... better to be a "normal" single word ;)
	define(BOT_NAME,"X"); // You bot's name, defines some strings in the website to fit more your configuration.
	define(IRC_SERVER,"irc.network.net"); // Default server for irc:// option.
	define(NETWORK_EMAIL,"cservice@network.net"); // the email of your network (default).

	define(PURGE_EMAIL,"purge@network.net"); // the email where channel purge request form is sent.
	define(XAT_EMAIL,"x@network.net"); // the email where manager change form is sent.
	define(OBJECT_EMAIL,"object@network.net"); // the email where replies to password changes are sent.
	define(ABUSE_GLOBAL_EMAIL,"abuse@network.net"); // the email where abuse NOT REGARDING Channel Service should go (for complaints)

	define(FROM_NEWUSER,"cmaster@network.net"); // the From: email for a NEWUSER registration email
	define(FROM_NEWUSER_SUBJECT,"Your Channel Service Account"); // the subject of that mail.

	define(FROM_FPASS,"cmaster-recovery@network.net"); // the From: email for a FORGOTTEN PASSWORD email.
	define(FROM_FPASS_SUBJECT,"Your Channel Service Password for "); // the sujbect of that mail. 'username' is added at this string's end.

	/******************************/
	/* LOCAL DATABASE DEFINITIONS */
	/******************************/
<font color=#000000><b>LOCAL DATABASE</b> (local_db)
The default should fit our whole example. <b>gnuworld</b> is the user having access to the <b>PostgreSQL</b>
on the local machine.
<? if ($secure) { ?>
You need to add the password of <b>gnuworld</b> for <b>LOCALDB_PASS</b> if you use password with PostgreSQL.
<? } ?></font>
	define(LOCALDB_NAME,"local_db"); // the local database that contains the gnuworld/doc/local_db.sql information.
	define(LOCALDB_USER,"gnuworld"); // the local user that has full access to LOCALDB_NAME.
	define(LOCALDB_PASS,""); // the password, use blank if you dont use passwords.

	/*******************************/
	/* REMOTE DATABASE DEFINITIONS */
	/*******************************/
<font color=#000000><b>REMOTE DATABASE</b> (cservice)
The default should fit our whole example. <b>gnuworld</b> is the user having access to the <b>PostgreSQL</b>
on the local machine (which *can* be remote, but not forcibly like here..).
<? if ($secure) { ?>
You need to add the password of <b>gnuworld</b> for <b>REMOTEDB_PASS</b> if you use password with PostgreSQL.
<? } ?></font>
	define(REMOTEDB_NAME,"cservice"); // the remote databasr that contains cservice.sql, cservice.web.sql information.
	define(REMOTEDB_USER,"gnuworld"); // the user that has access remotely to that db
	define(REMOTEDB_PASS,""); // the password, use blank if you dont use passwords.
	define(REMOTEDB_HOST,"127.0.0.1"); // define to whatever host/IP of the "remote" db is.

	/*******************************/
	/* LOCAL WEBSITE CONFIGURATION */
	/*******************************/
	define(HOSTING_LOGO,""); // this image should go in website/docs/gnuworld/images and be maximum 250*33 pixels.
	define(HOSTING_URL,"");
	define(HOSTING_STATS,0); // 1: enable, 0: disable. If you enabled this you *NEED* to ensure that a file
				 // called 'hosting_stats' is WRITABLE by the httpd user in /tmp.
				 //
				 // This option enables/disables the page access to 901+ on the web as well.
				 // If the file is not found, or found not writable, it's disabled.

<font color=#000000>There's a link to <b>CService Site</b> in the site's left menu, define here where that link should go,
it will be sent to <b>_top</b> (the whole browser current window).</font>
	define(CSERVICE_SITE_URL,"../");






	/*---------------------*/
	/*  WHAT YOU CAN EDIT  */
	/*---------------------*/

<font color=#000000>You should change all those variables to random salts you pick instead of <b>0123456789</b>.
<u>NOTE:</u> the size of the SALT is not important, but a reasonable value like 10 chars give a certain amount of security.</font>
	define(CRC_SALT_0001,"0123456789");
	define(CRC_SALT_0002,"0123456789");
	define(CRC_SALT_0003,"0123456789");
	define(CRC_SALT_0004,"0123456789");
	define(CRC_SALT_0005,"0123456789");
	define(CRC_SALT_0006,"0123456789");
	define(CRC_SALT_0007,"0123456789");
	define(CRC_SALT_0008,"0123456789");
	define(CRC_SALT_0009,"0123456789");
	define(CRC_SALT_0010,"0123456789");
	define(CRC_SALT_0011,"0123456789");
	define(CRC_SALT_0012,"0123456789");
	define(CRC_SALT_0013,"0123456789");
	define(CRC_SALT_0014,"0123456789");
	define(CRC_SALT_0015,"0123456789");
	define(CRC_SALT_0016,"0123456789");
	define(CRC_SALT_0017,"0123456789");
	define(CRC_SALT_0018,"0123456789");
	define(CRC_SALT_0019,"0123456789");
	define(CRC_SALT_0020,"0123456789");

	/* SALTs for external authentication capabilities (experimental) */
	define(CRC_SALT_EXT1,"0123456789");
	define(CRC_SALT_EXT2,"0123456789");
	define(CRC_SALT_EXT3,"0123456789");
	define(CRC_SALT_EXT4,"0123456789");
	define(CRC_SALT_EXT5,"0123456789");

	/*
	    those options are only used when using multiple mirrors of your CService website
	    if you dont do mirrors at all, just leave those options as they are, this wont affect anything.
	    if you do mirrors, the following options means that if the website is accessed with "ROUNDROBIN",
	    it will automatically redirect accesses to "LOCALMIRROR".
	*/
	define(ADMINONLY_MIRROR,0); // Define this to 1 if you want that only * person can login.
	define(CLIENT_MIRROR_URL,"http://cservice.network.net/live/"); // define this in case ADMINONLY_MIRROR is set to 1.

<font color=#000000>As stated above, <font color=#ff0000><b>do not modify this unless you have the use of it</b></font>.
The purpose is, for example, if you plan to have <b>http://cservice-live.undernet.org/</b>
to point to a DNS Round Robin, with mirrors of the site.
Each mirror also has its <b>cservice-live.xx.undernet.org</b> DNS pointer,
for example depending on the country .. cservice-live.fr ... cservice-live.us....
Configuring the next parameters according to that will make that if you access a page from within
the URL <b>http://cservice-live.undernet.org/</b>, the site will automatically refresh it to
<b>http://cservice-live.xx.undernet.org/</b>, so you dont mess with multiple sites while browsing,
because this would just NOT work according to the current web authentication scheme that reads
a database locally for each mirror.</font>

	define(LOCALMIRROR,"cservice-live.xx.network.net");
	define(ROUNDROBIN,"cservice-live.network.net");


<font color=#000000>Below is a brand new functionnality ... the site only comes with a single theme by default (called 'default')
and if you look in 'gnuworld/doc/update_themes.sql' and in the 'website/docs/gnuworld/themes/data' directory you'll see it is easy
to add other themes by yourself .. just copy the <b>INSERT</b> clause from the .sql file and change the first number '1' to '2', and so on...
Soon an interface will come up to modify and create themes when online as a 901+ level.
The AUTO_SWITCH_THEMES function will  permit for example for your site to automatically pick a different theme for a certain date or period you will decide,
this can be used to christmas, halloween, new year, and other various stuff at your convenience.
Unfortunately this function is not finished either at this time but will also work very shortly.
</font>
	/******************/
	/* THEMES OPTIONS */
	/******************/
<font color=#000000><b>ATTENTION</b>: STD_THEME is case sensitive, check what was in the "INSERT" you eventually did.</font>
	define(STD_THEME,"default");  // define this to your site Theme, this MUST be a valid sub-directory of 'website/docs/gnuworld/themes/data/'
				      // If you want more information on this new feature, please visit http://cservice.undernet.org/gnuworld-setup/
				      // or check the gnuworld/doc/idoc/ directory.
	define(AUTO_SWITCH_THEMES,0); // If you set this .. dated themes will appear when they should (latest 'created_ts' "wins" if periods cover each other)
				      // (not working at the moment... sorry)


	/*************************/
	/* VARIOUS CHECK OPTIONS */
	/*************************/
	define(ENABLE_NOTES,1); // set this to 1 to enable or 0 to disable the "Notes" link on the web
				// you *NEED* to have your X (mod.cservice) bot running with the code
				// supporting it to enable this, check out 'gnuworld/doc/notes.sql' too.
	define(NOTES_ADMIN_ONLY,1); // This applies if 'ENABLE_NOTES' is set to 1, when set to 0, everyone can use the feature
				    // when set to 1, only * persons can.
	define(NOTES_LIM_TOTAL,10); // Total number of notes a user can send to all users. (0 = unlimited)
	define(NOTES_LIM_PERUSR,3); // Total number of notes a user can send to a single user. (0 = unlimited)
	define(NOTES_LIM_INBOX,15); // Total number of notes a user can have in his 'notes box'. (0 = unlimited)
	// of course, if ENABLE_NOTES is defined to '0', above three values have no effect.

	define(COOKIE_DOMAIN,""); // Blank:default. Define this to ".yourdomain.com" or anything else at your own risks ;P

	define(BOFH_PASS_ADMIN,1); // set this to 0 to disable password complexity checking for * people.
	define(BOFH_PASS_USER,0); // set this to 1 to enable password complexity checking for all users (overriding BOFH_PASS_ADMIN=0).
	define(PW_MIN_CHARS,6); // minimum chars a password must contain.
	define(PW_MIN_CAPSL,1); // minimum upper case letters a password must contain.
	define(PW_MIN_MINSL,1); // minimum lower case letters a password must contain.
	define(PW_MIN_DIGIT,1); // minimum digits a password must contain.
	define(PW_MIN_OTHER,1); // minimum other chars a password must contain.

	define(IPCHK_MAXHITS,3); // Number of failed forgotten_pass/login attempts before IP_LOCK

	define(IPCHK_BANTIME,86400); // Time your IP/username is locked (seconds) when caught into IPCHK_MAXHITS+ failed attempts.

	define(ENABLE_COMPLAINTS_MODULE,0); // set this to 0 to disable , 1 to enable, the /complaints reporting module (for example if you dont use it ;P)

<font color=#000000>The next parameter, instead of the <b>0</b>, if you have enabled the above complaint module,
has to be an <b>existing user ID (number)</b> on your database. This will be the user that will post the anonymous
objections (admin comments) to pending channels when a complaint is lodged for that.</font>
	define(COMPLAINTS_ADMINCOMMENT_ID,0); // this is *MUST* be set to a PERMANENTLY VALID USERNAME ID, if COMPLAINTS MODULE is enabled.
					      // this will be the user that will post the anonymous objections under the form of an admin comment
					      // when a complaint of this type is sent.

	define(NEWUSERS_GFXCHECK,0);				// 1 = add a graphical code entry check to "newusers", 0 = disable the feature (or if no "libgd" is present)
	define(FONT_PATH,"/home/gnuworld/website/fonts/");	// ABSOLUTE PATH to the 'website/fonts/' directory (normally "/home/gnuworld/website/fonts/")
	define(SPECIFIC_FONT,"rans.ttf");			// if you let this empty, fonts will be randomly chosen into FONT_PATH (one single font used per code/image).
	define(JPEG_OUT_QUALITY,57);				// %Quality of the JPEG generated (the less you put this, the harder it will be to read/recognize, ranges 1 to 100).

	define(NEWUSERS_IPCHECK,0); // define this to 1, if you want to enable ONE IP = ONE USERNAME per 24h.

<font color=#000000>set the two following parameters to <b>0</b> to disable them.</font>
	define(LREQ_TIME,<font color=#000000><b>600</b></font>); // min time between forgotten password requests to avoid flooding, set this to 0 to disable.

	define(UNETUB_TIME,<font color=#000000><b>14400</b></font>); // 4 hours before being able to create another user from the same IP. set to 0 to disable.

	define(REGPROC_IDLECHECK,1); // If this is set to 1, users that are idle more than 21 days will not be allowed to
				     // register a channel or be a channel supporter until they login to the bot on IRC.


	define(REGPROC_ALLOWMULTIPLE,0); // If this is set to 1, users will be allowed to register an unlimited amount
					 // of channels, but will still be allowed to have only ONE channel being applied at a time.


	define(DEFAULT_REQUIRED_SUPPORTERS,10); // this option defines number of default required supporters (see below).
	/*
	This feature needs you to update the schema as following if you are NOT making a new installation :
		ALTER TABLE variables ADD last_updated INT4;

	Current feature of REQUIRED_SUPPORTERS :
			0	set it to 0 to allow instant registration (no supporters)	[VERY SMALL NETWORKS]
			1-9	requiring 1 to 9 supporters for standard registration		[VARIOUS NETWORKS]
			10	requires 10 supporters to register a channel.			[WIDE NETWORKS]

	>>>>>>> IMPORTANT NOTICE <<<<<<<<
	The number of required supporters will be, if not existing in tables "variables" in the remote DB,
	updated to the number you set above (ie. first run), BUT, if there's already a definition for it in the DB,
	the DB one will be the one used by the interface.
	You can change the amount of required supporters using the website itself in 'ACL Manager'.
	You must be 901+ to view the [Modify] button next to number of required supporters, ensure
	the "New Regs" are LOCKED (at the top of the ACL Manager page) then click on the [Modify] button,
	One button there will allow you to reset to "config.inc"'s setting, or change to another number,
	after validating, re-open "New Regs" and see the changes.
	This requires to login with a user with an admin access of 901 or above (coder level only).
	>>>>>>>>>>>>>>>><<<<<<<<<<<<<<<<<
	*/




	/* To show you read this whole file, please comment or remove the next line. */

<font color=#000000>add <b>//</b> at the begining of the following line :</font>
	<font color=#000000><b>//</b></Font>die("&lt;h2&gt;&lt;b&gt;Error&lt;/b&gt;: Edit website/php_includes/config.inc file !&lt;/h2&gt;");

	?&gt;


</font>

<b>Save the file !</b>

</pre>
<?
	footer("B-5","B-7");
}
if ($subset=="B-7") {
?>
<h2>Configuring and setting up 'THE JUDGE'</h2>
<pre>

<b>appjudge</b> is a <b>pgtclsh</b> script that, ran by <b>crontab</b>, will check
for applications in <b>Pending</b> status to make them to <b>Notification</b> period,
or <b>Reject</b> applications that didnt passed <b>Traffic Check</b>, or put in
<b>Ready for review</b> state other applications that passed and are ready to be
reviewed by CService administrators for registration.

In other words, you need this for your Automatic Registration System to work.

<? if ($r) { ?>
	<?=$os?>:/home/gnuworld/website/php_includes$ <b>exit</b>
	<?=$os?>:/root# <b>cd /home/gnuworld/gnuworld/doc/cmaster/</b>
	<?=$os?>:/home/gnuworld/gnuworld/doc/cmaster# <b>cp -rp regproc/ /usr/local/pgsql/</b>
	<?=$os?>:/home/gnuworld/gnuworld/doc/cmaster# <b>chown -R gnuworld /usr/local/pgsql/regproc/</b>
	<?=$os?>:/home/gnuworld/gnuworld/doc/cmaster# <b>su - gnuworld</b>
	<?=$os?>:/home/gnuworld$
<? } else { ?>
	<?=$os?>:/home/gnuworld/website/php_includes$ <b>cd ../../gnuworld/doc/cmaster/</b>
	<?=$os?>:/home/gnuworld/gnuworld/doc/cmaster$ <b>cp -r regproc/ /home/gnuworld</b>
<? } ?>

<? if ($r) { ?>
Edit the <b>/usr/local/pgsql/regproc/appjudge-config</b> file, and set your preferences,
<? } else { ?>
Edit the <b>/home/gnuworld/regproc/appjudge-config</b> file, and set your preferences,
<? } ?>
the defaults at <b>99</b> are definetly not good.

<font color=#888888>
	# Database name (duh)

<font color=#000000>Set this to the CService database, in our example, <b>cservice</b></font>
	set SQL_DB "<font color=#000000><b>cservice</b></font>"

<font color=#000000><b>DO NOT MODIFY THIS</b> heh!@#</font>
	# Number of seconds in a day, I suggest you do not change this ;)
	set DAY                         86400

<font color=#00000>Tweak the following for your convenience</font>
	# Number of days an application has to pass the activity test
	set MAX_DAYS                    99

	# Number of unique IP's that must visit a channel during the activity test
	set UNIQUE_JOINS                99

	# Number of supporters that must visit the channel during the activity test
	set MIN_SUPPORTERS              99

	# Duration of the notify period
	set NOTIFY_DAYS                 99

	# Number of days supporters have to confirm their support
	set SUPPORT_DAYS                99

<font color=#000000>You definetly need to change this <b>0</b> to an <b>existing user ID (number)</b> on your database,
for example, if you had activated the complaints module earlier (when editing 'config.inc') it is wise to pick,
here the same <b>ID</b> as in <b>COMPLAINTS_ADMINCOMMENT_ID</b> in 'config.inc'...</font>
	# The Judge's Reviewer ID
	set REVIEWER_ID                 0

<font color=#000000><b>DO NOT MODIFY THIS EITHER</b></font>
	# These are better left alone
	set AS_PENDING_SUPPORT          0
	set AS_TRAFFIC_CHECK            1
	set AS_NOTIFICATION             2
	set AS_COMPLETED                3
	set AS_CANCELLED                4
	set AS_PENDREVIEW               8
	set AS_REJECTED                 9
</font>

Save the file !

<? if ($r) { ?>
Ensure <b>/usr/local/pgsql/regproc/appjudge</b> is set executable :

	<?=$os?>:/home/gnuworld$ <b>chmod 755 /usr/local/pgsql/regproc/appjudge</b>


Add the content of <b>/usr/local/pgsql/regproc/cron-judge.in</b> to <b>gnuworld</b>'s crontab :

	<?=$os?>:/home/gnuworld$ <b>crontab /usr/local/pgsql/regproc/cron-judge.in</b>
<? } else { ?>
Ensure <b>/home/gnuworld/regproc/appjudge</b> is set executable :

	<?=$os?>:/home/gnuworld/gnuworld/doc/cmaster$ <b>chmod 755 /home/gnuworld/regproc/appjudge</b>


Add the content of <b>/home/gnuworld/regproc/cron-judge.in</b> to <b>gnuworld</b>'s crontab :

	<?=$os?>:/home/gnuworld/gnuworld/doc/cmaster$ <b>crontab /home/gnuworld/regproc/cron-judge.in</b>
<? } ?>

</pre>
<?
	footer("B-6","C");
}

/* *** CAT. C : SETTING UP APACHE AND PHP *** */
if ($subset=="C-1") {
?>
<h2>Getting Apache</h2>
<pre>

To get the latest Apache server, go to <a href="<?=APACHE_FTP_SOURCE?>" target=_blank><?=APACHE_FTP_SOURCE?></a>,
and take the latest <b>1.3.x</b>.

Recommended version <a href="<?=APACHE_FTP_SOURCE_FILE?>"><?=APACHE_FILE_NAME?></a>.

Put that, as <b>root</b> in your <b>/root</b> home directory.

</pre>
<?
	footer("B-7","C-2");
}
if ($subset=="C-2") {
?>
<h2>Getting PHP</h2>
<pre>

To get the latest version of PHP, go to <a href="<?=PHP_FTP_SOURCE?>" target=_blank><?=PHP_FTP_SOURCE?></a>.

Recommended version <a href="<?=PHP_FTP_SOURCE_FILE?>"><?=PHP_FILE_NAME?></a>.

<b>PHP also requires these programs if you plan to use the Graphical code check on 'New users' :
( compiling with libGD support )</b>
</pre>
<table border=1 cellspacing=0 cellpadding=3>
  <tr>
    <td><b>Program</font></b></td>
    <td><b>Version</font></b></td>
    <td><b>Main Site</font></b></td>
  </tr>
  <tr>
    <td><a href="<?=LIBPNG_URL_FILE?>">libPNG</a></td>
    <td><?=LIBPNG_VERSION?></td>
    <td><a target=_blank href="<?=LIBPNG_URL?>"><?=LIBPNG_URL?></a></td>
  </tr>
  <tr>
    <td><a href="<?=LIBJPEG_URL_FILE?>">libJPEG</a></td>
    <td><?=LIBJPEG_VERSION?></td>
    <td><a target=_blank href="<?=LIBJPEG_URL?>"><?=LIBJPEG_URL?></a></td>
  </tr>
  <tr>
    <td><a href="<?=FREETYPE2_URL_FILE?>">FreeType2</a></td>
    <td><?=FREETYPE2_VERSION?></td>
    <td><a target=_blank href="<?=FREETYPE2_URL?>"><?=FREETYPE2_URL?></a></td>
  </tr>
  <tr>
    <td><a href="<?=ZLIB_URL_FILE?>">zlib</a></td>
    <td><?=ZLIB_VERSION?></td>
    <td><a target=_blank href="<?=ZLIB_URL?>"><?=ZLIB_URL?></a></td>
  </tr>
</table>
<pre>
Put these, as <b>root</b> in your <b>/root</b> home directory.

</pre>
<?
	footer("C-1","C-3");
}
if ($subset=="C-3") {
?>
<h2>Building Apache with PHP and PostgreSQL support</h2>
<pre>

<i>Side note: of course it's always best to be up to date with php and apache version
<?
	$nv_ap = explode(".",APACHE_RECOM_VERSION);
	$nv_ph = explode(".",PHP_RECOM_VERSION);

	$nv_apache_v = $nv_ap[0] . "." . $nv_ap[1] . "." . ($nv_ap[2]+1);
	$nv_php_v = $nv_ph[0] . "." . $nv_ph[1] . "." . ($nv_ph[2]+1);
?>
if you have apache_<?=$nv_apache_v?> or php-<?=$nv_php_v?> .. don't hesitate, take it ;)</i>

I will be a bit short on it because :
	- It should'nt need to be long ;)
	- Apache and PHP documentations are way more complete than that one.

	<?=$os?>:/root# <b>tar -xzf <?=APACHE_FILE_NAME?></b>
	<?=$os?>:/root# <b>tar -xzf <?=PHP_FILE_NAME?></b>

	<u>* EITHER YOU COMPILE IT <b>WITH LIBGD SUPPORT</b> (for the Graphical code for new users)</u>
		<?=$os?>:/root# <b>tar -xzf <?=LIBPNG_FILE_NAME?></b>
		<?=$os?>:/root# <b>tar -xzf <?=LIBJPEG_FILE_NAME?></b>
		<?=$os?>:/root# <b>tar -xzf <?=FREETYPE2_FILE_NAME?></b>
		<?=$os?>:/root# <b>tar -xzf <?=ZLIB_FILE_NAME?></b>
		<?=$os?>:/root# <b>cd apache_<?=APACHE_RECOM_VERSION?>/</b>
		<?=$os?>:/root/apache_<?=APACHE_RECOM_VERSION?># <b>./configure --prefix=/usr/local/apache</b>
		<?=$os?>:/root/apache_<?=APACHE_RECOM_VERSION?># <b>cd ../zlib-<?=ZLIB_VERSION?>/</b>
		<?=$os?>:/root/zlib-<?=ZLIB_VERSION?># <b>./configure --prefix=/usr</b>
		<?=$os?>:/root/zlib-<?=ZLIB_VERSION?># <b>make</b>
		<?=$os?>:/root/zlib-<?=ZLIB_VERSION?># <b>make install</b>
		<?=$os?>:/root/zlib-<?=ZLIB_VERSION?># <b>cd ../freetype-<?=FREETYPE2_VERSION?>/</b>
		<?=$os?>:/root/freetype-<?=FREETYPE2_VERSION?># <b>gmake</b>
		<?=$os?>:/root/freetype-<?=FREETYPE2_VERSION?># <b>gmake</b>		(yes twice)
		<?=$os?>:/root/freetype-<?=FREETYPE2_VERSION?># <b>gmake install</b>
		<?=$os?>:/root/freetype-<?=FREETYPE2_VERSION?># <b>cd ../libpng-<?=LIBPNG_VERSION?>/</b>
		<?=$os?>:/root/libpng-<?=LIBPNG_VERSION?># <b>cd scripts</b>
		<?=$os?>:/root/libpng-<?=LIBPNG_VERSION?># <b>cp makefile.<?=$os?> ../Makefile</b>
		<?=$os?>:/root/libpng-<?=LIBPNG_VERSION?># <b>cd ..</b>
		<?=$os?>:/root/libpng-<?=LIBPNG_VERSION?># <b>make</b>
		<?=$os?>:/root/libpng-<?=LIBPNG_VERSION?># <b>make install</b>
		<?=$os?>:/root/libpng-<?=LIBPNG_VERSION?># <b>cd ../jpeg-<?=LIBJPEG_VERSION?>/</b>
		<?=$os?>:/root/jpeg-<?=LIBJPEG_VERSION?># <b>./configure</b>
		<?=$os?>:/root/jpeg-<?=LIBJPEG_VERSION?># <b>make</b>
		<?=$os?>:/root/jpeg-<?=LIBJPEG_VERSION?># <b>make install</b>
		<?=$os?>:/root/jpeg-<?=LIBJPEG_VERSION?># <b>cd ../php-<?=PHP_RECOM_VERSION?>/</b>
		<?=$os?>:/root/php-<?=PHP_RECOM_VERSION?># <b>./configure --with-pgsql=/usr/local/pgsql --enable-track-vars --with-apache=../apache_<?=APACHE_RECOM_VERSION?> \
					  --with-zlib --with-gd --enable-gd-native-ttf --with-ttf --enable-gd-imgstrttf \
					  --with-jpeg-dir=../jpeg-<?=LIBJPEG_VERSION?> --with-png-dir=../libpng-<?=LIBPNG_VERSION?> \
					  --with-freetype-dir=/usr/include/freetype2/freetype</b>

	<u>* OR YOU COMPILE IT <b>WITHOUT LIBGD SUPPORT</b> (NO Graphical code for new users, simplier)</u>
		<?=$os?>:/root# <b>cd apache_<?=APACHE_RECOM_VERSION?>/</b>
		<?=$os?>:/root/apache_<?=APACHE_RECOM_VERSION?># <b>./configure --prefix=/usr/local/apache</b>
		<?=$os?>:/root/php-<?=PHP_RECOM_VERSION?># <b>./configure --with-pgsql=/usr/local/pgsql --enable-track-vars --with-apache=../apache_<?=APACHE_RECOM_VERSION?></b>

	Then you continue with ...

	<?=$os?>:/root/php-<?=PHP_RECOM_VERSION?># <b>make</b>
	<?=$os?>:/root/php-<?=PHP_RECOM_VERSION?># <b>make install</b>
	<?=$os?>:/root/php-<?=PHP_RECOM_VERSION?># <b>cd ../apache_<?=APACHE_RECOM_VERSION?>/</b>
	<?=$os?>:/root/apache_<?=APACHE_RECOM_VERSION?># <b>./configure --prefix=/usr/local/apache --activate-module=src/modules/php4/libphp4.a</b>
	<?=$os?>:/root/apache_<?=APACHE_RECOM_VERSION?># <b>make</b>
	<?=$os?>:/root/apache_<?=APACHE_RECOM_VERSION?># <b>make install</b>
	<?=$os?>:/root/apache_<?=APACHE_RECOM_VERSION?># <b>cd ../php-<?=PHP_RECOM_VERSION?></b>
	<?=$os?>:/root/php-<?=PHP_RECOM_VERSION?># <b>cp php.ini-dist /usr/local/lib/php.ini</b>

Make sure you edit <b>/usr/local/lib/php.ini</b> to set &quot;<b>register_globals</b>&quot; to &quot;<b>On</b>&quot;
otherwise the interface wont work. (before starting apache or you will have to restart)

	<?=$os?>:/root/php-<?=PHP_RECOM_VERSION?># <b>cd /usr/local/apache/htdocs/</b>
	<?=$os?>:/usr/local/apache/htdocs# <b>chmod 711 ~gnuworld</b>
	<?=$os?>:/usr/local/apache/htdocs# <b>chmod 711 ~gnuworld/website</b>
	<?=$os?>:/usr/local/apache/htdocs# <b>chmod 755 ~gnuworld/website/php_includes</b>
	<?=$os?>:/usr/local/apache/htdocs# <b>chmod 644 ~gnuworld/website/php_includes/config.inc</b>
	<?=$os?>:/usr/local/apache/htdocs# <b>chmod 755 ~gnuworld/website/docs/gnuworld/</b>
	<?=$os?>:/usr/local/apache/htdocs# <b>ln -s /home/gnuworld/website/docs/gnuworld live</b>

Edit the file <b>/usr/local/apache/conf/httpd.conf</b>...
	Find
		<b>&lt;IfModule mod_dir.c&gt;
			DirectoryIndex index.html
		&lt;/IfModule&gt;</b>

	and replace by :
		<b>&lt;IfModule mod_dir.c&gt;
			DirectoryIndex index.php index.html
		&lt;/IfModule&gt;</b>





	then find (look for example for 'AddType') :
		<b># And for PHP 4.x, use:
		#
		#AddType application/x-httpd-php .php
		#AddType application/x-httpd-php-source .phps</b>

	and replace by : (if you haven't found the lines above, add them in the file then)
		<b># And for PHP 4.x, use:
		#
		AddType application/x-httpd-php .php
		AddType application/x-httpd-php-source .phps</b>


Save the <b>httpd.conf</b> !


Now we can START apache :

	<?=$os?>:/usr/local/apache/htdocs# <b>../bin/apachectl start</b>

And you can go to :

	<b>http://www.your-site.com/live/</b>

and see if that works (that should if you dont mess with your URL ;)).


</pre>
<?
	footer("C-2","C-4");
}
if ($subset=="C-4") {
?>
<h2>Short HOWTO on apache's &lt;VirtualHost&gt; directive</h2>
<pre>

This may be not needed for most users if the previous step worked, but in case you wonder
how you could, if someone makes <b>cservice.some-domain.com</b> point to your machine
to put the GNUworld interface on it .. here is how it works :

Edit the <b>/usr/local/apache/conf/httpd.conf</b> file ..

Find the line :

	<b>#NameVirtualHost *</b>

and uncomment it, giving :

	<b>NameVirtualHost *</b>


Then, go to the end of the file and add the following :

	<b>&lt;VirtualHost *&gt;
		ServerAdmin your@email.com
		DocumentRoot /home/gnuworld/website/docs/gnuworld
		ServerName cservice.some-domain.com
		ErrorLog logs/cservice.some-domain.com-error.log
		CustomLog logs/cservice.some-domain.com-access.log common
	&lt;/VirtualHost&gt;</b>


save <b>httpd.conf</b>,
then <b>restart apache</b> :

	<?=$os?>:/root# <b>/usr/local/apache/bin/apachectl restart</b>

Change the following line in <b>/home/gnuworld/website/php_includes/config.inc</b> :

	<b>define(LIVE_LOCATION,"/live");</b>

replace by

	<b>define(LIVE_LOCATION,"");</b>


and go to <b>http://cservice.some-domain.com/</b>.

</pre>
<?
	footer("C-3","!");
}

if ($subset=="!") {
?>
<h2>Installation Successfull</h2>
<pre>

Congratulations, you have set up <b>GNUworld</b> on your machine.

You can go back to <a href="./">Home page</a>,
or visit one of the following links for additionnal information :

	Coder-Comittee			<a href="http://coder-com.undernet.org/" target=_blank>http://coder-com.undernet.org/</a>
	Undernet CService		<a href="http://cservice.undernet.org/" target=_blank>http://cservice.undernet.org/</a>
	Undernet CService Web Interface	<a href="http://cservice.undernet.org/live/" target=_blank>http://cservice.undernet.org/live/</a>

	GNUworld Home			<a href="http://www.gnuworld.org/" target=_blank>http://www.gnuworld.org/</a>


	<a href="./?s=4">Frequently Asked Questions (FAQs)</a>
	<a href="./?s=5">Upgrading GNUworld</a>
	<a href="./?s=6">Credits</a>

</pre>
<?
	footer("C-4","*");
}



	}
}
if ($s==4) { // *** FAQs
?>
<br>
<ul>
<li> <a href=#faq0>How do I give myself admin access to X ?</a>
<li> <a href=#faq1>Whats the full command to start GNUWorld ?</a>
<li> <a href=#faq2>I get this error: PostgreSQL error message: connectDBStart() -- connect() failed: Connection refused</a>
<li> <a href=#faq3>I get this error: checking for libpq++.h... configure: error: Unable to find PostGreSQL</a>
<li> <a href=#faq4>Were can i get the Website module for GNUWorld X ?</a>
<li> <a href=#faq5>How can i upgrade GNUWorld without install it again ?</a>
<li> <a href=#faq6>How can i have that documentation on my server ?</a>
<li> <a href=#faq7>I changed number of required supporters in the config but the page doesnt take it</a>
<li> <a href=#faqu1>My website is doing errors complaining about 'tz_setting' and/or 'acl', how do i fix this ?</a>
<li> <a href=#faqu2>I enabled 'NOTES' in my config.inc file and the page is doing errors about it</a>
</ul>
<hr width=100% noshade size=1><br>
<pre>
<a name=faq0><b>How do I give myself admin access to X ?</b>
First you have to edit the <i>cservice.addme.sql</i> script, you will find it in
the <i>doc/</i> directory of the 'gnuworld' cvs repository.
What you have to do is edit the username who will be added:
e.g.:
	<i>INSERT into users (user_name,password,language_id,last_updated) VALUES ('Admin','07d976a29bdb3ff42e19c102852151b28097bbad',1,now()::abstime::int4);</i>
Change the "Admin" to your preferred username.
After that you run it with this command:
	<i>psql cservice < cservice.addme.sql</i>

<hr width=100% noshade size=1><br>

<a name=faq1><b>Whats the full command to start GNUWorld ?</b>
After installation, the binary is contained in the <i>gnuworld/bin</i> directory.
If you don't want to see debug output, the best way to run gnuworld is:
	<i>cd gnuworld/bin</i>
	<i>./gnuworld -c -f GNUWorld.conf &gt;/dev/null 2&gt;&1 &</i>
The <i>-c</i> option will however log debug output in <i>debug.log</i> and <i>socket.log</i> in the <i>bin/</i> directory,
if you don't want that, just don't use the <i>-c</i> option.

<hr width=100% noshade size=1><br>

<a name=faq2><b>I get this error: PostgreSQL error message: connectDBStart() -- connect() failed: Connection refused</b>
Make sure you are starting the PostgreSQL server with the -i parameter (To listen on TCP sockets).

<hr width=100% noshade size=1><br>

<a name=faq3><b>I get this error: checking for libpq++.h... configure: error: Unable to find PostGreSQL</b>
Are you sure you have installed PostgreSQL ? It can be downloaded from
	<i><a href="http://www.postgresql.org/" target=_blank>http://www.postgresql.org</a></i>
Also, ensure you read the PostgreSQL documentation and add the configuration flag <i>--with-CXX</i>
to install the PostgreSQL C++ API.
We recommend you read <a href="./?s=1">our current documentation</a> about it used with GNUworld.

<hr width=100% noshade size=1><br>

<a name=faq4><b>Were can i get the Website module for GNUWorld X ?</b>
You can obtain it from CVS:
	<i>cvs -d:pserver:anonymous@cvs.sourceforge.net:/cvsroot/gnuworld</i>
login (There is no password, just press enter)
and tand then type:
	<i>cvs -z3 -d:pserver:anonymous@cvs.sourceforge.net:/cvsroot/gnuworld checkout website</i>

<hr width=100% noshade size=1><br>

<a name=faq5><b>How can i upgrade GNUWorld without install it again ?</b>
To upgrade your GNUWorld version, you go to the <i>gnuworld/</i> directory
and use the CVS command:
	<i>cvs update -d</i>

<hr width=100% noshade size=1><br>

<a name=faq6><b>How can i have that documentation on my server ?</b>
You should find it in <i>gnuworld/doc/idoc</i> directory.
Please read the <i>README</i> file first.

<hr width=100% noshade size=1><br>

<a name=faq7><b>I changed number of required supporters in the config but the page doesnt take it</b>

This is because the config.inc value is only a "default" one you can restore when you want.
When you *FIRST* run the website, and when no "number of supporters required" value is yet set
in the database, it reads it from config.inc and stores it as "first default" in the databse,
but then, if it exists and has a value in the database, it will NEVER re-read it from the config.inc file
unless you specifically ask for it in the 'ACL Manager' page, this is also the place where you can,
and should change that value when it has already been set once.

To modify this you need a "coder" level (901 or above), go to 'ACL Manager' page, ensure that
the "New Regs" are <b>LOCKED</b>, then click on the <b>[Modify]</b> button on the right of the
"Required number of supporters", you can here, either reset database value to the current config.inc's one,
or set it to something else between 0 and 10.
When done, have the "New Regs" unlocked and you have it (dont forget to eventually reload the Channel Registration
Page in order to correctly see the changes).

<hr width=100% noshade size=1><br>


<a name=faqu1><b>My website is doing errors complaining about 'tz_setting' and/or 'acl', how do i fix this ?</b>
Latest patch problem user may encouter is the "timezone" setting that has been
moved to the database instead of being stored in a cookie, this implied changes about the database,
added to that a new ACL system came up, replacing the WebAxs page, this also required some database
schema modification ...

For people upgrading from a version that still had the <b>WebAxs</b> link in the left menu,
to the new website version, the one with <b>ACL Manager</b>... you need to :

execute the two <b>update_tzmove_r.sql</b> and <b>update_tzmove_l.sql</b> files in <b>gnuworld/doc</b> directory,
respectively to remote (cservice) and local (local_db) databases,
	<b>psql cservice &lt; update_tzmove_r.sql</b>
	<b>psql local_db &lt; update_tzmove_l.sql</b>

then if you would examine the other <b>cservice.sql</b> or <b>cservice.web.sql</b> files you would notice
the following changes are also required in the remote database (cservice):
	Run <b>psql cservice</b> and type the following at the <b>cservice:=#</b> prompt :
<b>
	CREATE TABLE acl (
	        acl_id SERIAL,
	        user_id INT4 NOT NULL,
	        isstaff INT2 NOT NULL,
	        flags INT4 DEFAULT '0' NOT NULL,
	        xtra INT4 NOT NULL,
	        last_updated INT4 NOT NULL,
	        last_updated_by INT4 NOT NULL,
	        suspend_expire INT4 DEFAULT '0' NOT NULL,
	        suspend_by INT4 DEFAULT '0' NOT NULL,
	        deleted INT2 DEFAULT '0' NOT NULL
	);

	ALTER TABLE pendingusers ADD poster_ip VARCHAR(15) DEFAULT '';

	ALTER TABLE users ADD maxlogins INT4 DEFAULT 1;
</b>

<hr width=100% noshade size=1><br>

<a name=faqu2><b>I enabled 'NOTES' in my config.inc file and the page is doing errors about it</b>

This feature was added recently, in order to handle it properly you need
the latest CVS version of GNUworld (mod.cservice) and of the website.

If the errors are complaining about a "notes" thing missing somewhere, it's probably because
that new table is not created in your remote database structure (cservice):
	Run <b>psql cservice</b> and type the following at the <b>cservice:=#</b> prompt :
<b>
	CREATE TABLE notes (
	        message_id SERIAL,
	        user_id INT4 CONSTRAINT users_notes_ref REFERENCES users( id ),
	        from_user_id INT4 CONSTRAINT users_notes_ref REFERENCES users( id ),
	        message VARCHAR( 300 ),
	        last_updated INT4 NOT NULL,

	        PRIMARY KEY(message_id, user_id)
	);
</b>

<hr width=100% noshade size=1><br>


</pre>
<?
}
if ($s==5) { // *** UPGRADING
?>
<pre>
Upgrading GNUworld or the website can be done easily if a few rules are followed, sometimes,
just in case you get a weird error, have a look at this page first, you may find very usefull
information here.

This page assumes that you have installed GNUworld using <a href="./?s=1">our guide</a>.

Quite all the times, there's a problem after upgrading GNUworld and/or website,
the problem comes from the database schema that is outdated because you haven't updated it aside.

If any db schema occur, you will see that the files in <b>gnuworld/doc</b> directory, will have
some change (when you <b>cvs update -d</b>), if there's is any then you need to ensure your
database structure is correct by :
	- watching changed/added files in <a href="http://cvs.sourceforge.net/cgi-bin/viewcvs.cgi/gnuworld/gnuworld/doc/" target=_blank><b>gnuworld/doc</b></a>
	- watching <a href="http://cvs.sourceforge.net/cgi-bin/viewcvs.cgi/gnuworld/gnuworld/mod.cservice/RELEASE.NOTES" target=_blank><b>gnuworld/mod.cservice/RELEASE.NOTES</b></a>

and apply eventual patches or sql script you would need to.

Here are some links to FAQs regarding latest problems you may encounter in upgrading GNUworld :
</pre>
<ul>
<li> <a href=./?s=4#faqu1>My website is doing errors complaining about 'tz_setting' and/or 'acl', how do i fix this ?</a>
<li> <a href=./?s=4#faqu2>I enabled 'NOTES' in my config.inc file and the page is doing errors about it</a>
</ul>
<pre>

<font color=#ff0000>CERT Security Advisory: you should upgrade to <b>php-4.1.2</b> for security reasons :</font>
	<a href="http://www.cert.org/advisories/CA-2002-05.html" target=_blank>http://www.cert.org/advisories/CA-2002-05.html</a>


If you have any questions please see <a href="./">home page</a> and links to help URLs, emails or IRC Channels.

</pre>
<?
}
if ($s==6) { // *** CREDITS
?>
<pre>

This help was built thanks to everyone in #coder-com on Undernet that contributed,
either in having problems thus making us think about how to explain better so these don't occur,
or in pointing bugs or mistakes.

Special thanks to A1kmm for pushing me in putting this "non-root setup guidelines".
Thanks to MrIron for providing support with the FAQs...
Thanks to d[a]aave for pointing out how to ensure PostgreSQL runs strictly locally secured.

This documentation is in no way perfect and will be modified from time to time according
to GNUworld evolutions and users problems.

Thanks to whoever pushed me to do it.


								nighty.
</pre>
<?
}
footer("","");
?>
