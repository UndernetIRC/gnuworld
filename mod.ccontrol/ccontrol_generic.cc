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
 * $Id: ccontrol_generic.cc,v 1.5 2008/01/03 04:26:22 kewlio Exp $
 */

#include <time.h>
#include <stdio.h>
#include <string.h>
#include "ccontrol_generic.h"

char ago[250];

char *Duration(long ts)
{
	/* express duration in human readable format */
	long duration;
	int days, hours, mins = 0;
	char tmp[16];

	ago[0] = '\0';

	duration = ts;

	days = (duration / 86400);
	duration %= 86400;
	hours = (duration / 3600);
	duration %= 3600;
	mins = (duration / 60);
	duration %= 60;

	if (days > 0)
	{
		sprintf(tmp, "%dd", days);
		strcat(ago, tmp);
	}
	if (hours > 0)
	{
		sprintf(tmp, "%dh", hours);
		strcat(ago, tmp);
	}
	if (mins > 0)
	{
		sprintf(tmp, "%dm", mins);
		strcat(ago, tmp);
	}
	/* only show seconds if we have any (or no other units */
	if ((duration > 0) || (strlen(ago) == 0))
	{
		sprintf(tmp,"%ds", (int) duration);
		strcat(ago,tmp);
	}

	return ago;
}

char *Ago(long ts)
{
        /* express a a timestamp in human readable format */
        long duration;

	duration = (time(NULL) - ts);

        return (Duration(duration));
}

