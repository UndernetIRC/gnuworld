/**
 * debug.h
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
 */

#ifndef DEBUG_H
#define DEBUG_H

#include <format>
#include <memory>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include "client.h"

namespace gnuworld {

class Command;

class debug : public xClient {
  public:
    explicit debug(const std::string&);
    ~debug() override;

    void OnPrivateMessage(iClient*, const std::string&, bool) override;

    bool RegisterCommand(std::unique_ptr<Command>);

    void readConfigFile(const std::string&);

    [[nodiscard]] bool hasAccess(const std::string&) const;

    // xClient overrides (see debug-xclient.cc). Stealth routes NOTICEs
    // via the uplink; format overload hides printf-style Notice.
    bool Notice(const iClient* Target, const std::string&) override;

    template <typename... Args>
    bool Notice(const iClient* Target, std::format_string<Args...> fmt, Args&&... args) {
        return Notice(Target, std::format(fmt, std::forward<Args>(args)...));
    }

  protected:
    using commandMapType = std::unordered_map<std::string, std::unique_ptr<Command>>;

    commandMapType commandMap;
    std::vector<std::string> allowAccess;
};

} // namespace gnuworld

#endif
