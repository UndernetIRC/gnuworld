/**
 * ccontrol_generic.cc
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307,
 * USA.
 *
 * $Id: ccontrol_generic.cc,v 1.1 2005/06/19 01:07:45 kewlio Exp $
 */

#include <time.h>
#include <stdio.h>
#include <string.h>
#include "ccontrol_generic.h"

char ago[250];

char *Ago(long ts)
{
	/* express (now - ts) in human readable format */
	long duration;
	int days, hours, mins = 0;
	char tmp[16];

	ago[0] = '\0';

	duration = (time(NULL) - ts);
	if (duration >= 86400)
	{
		/* magnitude of days */
		days = (duration / 86400);
		duration = (duration % 86400);
		sprintf(tmp, "%dd", days);
		strcat(ago,tmp);
	}
	if (duration >= 3600)
	{
		/* magnitude of hours */
		hours = (duration / 3600);
		duration = (duration % 3600);
		sprintf(tmp,"%dh", hours);
		strcat(ago,tmp);
	}
	if (duration >= 60)
	{
		/* magnitude of minutes */
		mins = (duration / 60);
		duration = (duration % 60);
		sprintf(tmp,"%dm", mins);
		strcat(ago,tmp);
	}
	sprintf(tmp,"%ds", (int) duration);
	strcat(ago,tmp);

	return ago;
}

