/**
 * UserFilter.h
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
= */
#include "ccUser.h"

#include <vector>
using std::vector;

#ifndef USERFILTER_H
#define USERFILTER_H

namespace gnuworld {

namespace uworld {

class UserFilter {
public:
        virtual bool filter(ccUser*) = 0;
};

class UserFilterComposite : public UserFilter{

private:
        vector<UserFilter*> filters;
public:
        void addFilter(UserFilter* filter) {
                filters.push_back(filter);
        }

        virtual bool filter(ccUser* user) {
                for(unsigned int i=0; i < filters.size();++i) {
                        if(!filters[i]->filter(user)) {
                                return false;
                        }
                }
                return true;
        }
};

class ByLevelsUserFilter : public UserFilter {
private:
        unsigned int levels;
public:
        ByLevelsUserFilter() {
                this->levels = 0;
        }

        void addLevel(unsigned int userLevel) {
                levels |= (1 << userLevel);
        }

        virtual bool filter(ccUser* user) {
                return (levels & (1 << user->getType())) > 0 ;
        }
};
}
}
#endif  /* USERFILTER_H */
