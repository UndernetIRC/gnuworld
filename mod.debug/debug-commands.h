/**
 * debug-commands.h
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

#ifndef DEBUG_COMMANDS_H
#define DEBUG_COMMANDS_H

#include <string>

namespace gnuworld {

class iClient;
class debug;

class Command {
  public:
    Command(debug* _bot, std::string _commandName, std::string _help)
        : bot(_bot), commandName(std::move(_commandName)), help(std::move(_help)) {}

    virtual ~Command() = default;

    virtual void Exec(const iClient*, const std::string&) = 0;

    [[nodiscard]] const std::string& getName() const { return commandName; }
    [[nodiscard]] const std::string& getHelp() const { return help; }

    virtual void Usage(const iClient*);

  protected:
    debug* bot;
    std::string commandName;
    std::string help;
};

#define DECLARE_COMMAND(commandName)                                                               \
    class commandName##Command : public Command {                                                  \
      public:                                                                                      \
        using Command::Command;                                                                    \
        void Exec(const iClient*, const std::string&) override;                                    \
    };

DECLARE_COMMAND(USERINFO)
DECLARE_COMMAND(CHANINFO)
DECLARE_COMMAND(SERVERS)
DECLARE_COMMAND(SERVERINFO)
DECLARE_COMMAND(SHUTDOWN)

} // namespace gnuworld

#endif
