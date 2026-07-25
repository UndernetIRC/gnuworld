/**
 * xparameters.h
 * This class originally written by Orlando Bassotto.
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
 * $Id: xparameters.h,v 1.4 2003/12/29 23:59:36 dan_karrels Exp $
 */

#ifndef __XPARAMETERS_H
#define __XPARAMETERS_H "$Id: xparameters.h,v 1.4 2003/12/29 23:59:36 dan_karrels Exp $"

#include <algorithm>
#include <optional>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include <cassert>

#include "ELog.h"

namespace gnuworld {

/**
 * A single IRCv3 message-tag (key[=value]).
 * Value is empty when the tag was sent without '='.
 */
struct MessageTag {
    std::string key;
    std::string value;
};

/**
 * This class is similar to StringTokenizer, except that it accepts
 * pointers to already tokenized C character arrays.  This class
 * will not attempt to copy the arrays.  This is done for efficiency.
 * The tokens kept by instances of this class are indexed starting
 * at zero.
 * This class is mutable.
 */
class xParameters {

  private:
    /**
     * The structure type for storing the tokens internally.
     */
    typedef std::vector<char*> vectorType;

  public:
    /**
     * The variable type used to represent this class's
     * size (number of tokens in the object).
     */
    typedef vectorType::size_type size_type;

    /**
     * IRCv3 message-tags attached to this message, if any.
     */
    using tagListType = std::vector<MessageTag>;

    /**
     * Construct an xParameters object.
     */
    xParameters() {}

    /**
     * Destroy an xParameters object.
     * No streams have been opened, and no memory explicitly
     * dynamically allocated, so this method is a NOOP.
     */
    ~xParameters() {}

    /**
     * This type is used to perform a read only iteration
     * of this objects array of strings.
     */
    typedef vectorType::const_iterator const_iterator;

    /**
     * Return a const_iterator to the beginning of the array
     * of strings.
     */
    inline const_iterator begin() const { return myVector.begin(); }

    /**
     * Return a const_iterator to the end of the array
     * of strings.
     */
    inline const_iterator end() const { return myVector.end(); }

    /**
     * Insert a pointer to a character array (token) into the
     * xParameters instance.
     */
    inline xParameters& operator<<(char* Parameter) {
        myVector.push_back(Parameter);
        return *this;
    }

    /**
     * Return a pointer to a mutable character array token.
     * The tokens are indexed beginning at zero.
     * This method will assert false if the requested index
     * is out of bounds, according to validSubscript.
     */
    inline char* operator[](const size_type& pos) const {
        assert(validSubscript(pos));
        return myVector[pos];
    }

    /**
     * Set a particular element to a new value, NULL is
     * permitted.
     */
    void setValue(const size_t pos, char* newVal) {
        assert(validSubscript(pos));
        myVector[pos] = newVal;
    }

    /**
     * Remove all tokens, if any, from this xParameters object.
     */
    inline void Clear() {
        myVector.clear();
        myTags.clear();
    }

    /**
     * Return the number of tokens held in this xParameters object.
     */
    inline size_type Count() const { return myVector.size(); }

    /**
     * Return the number of tokens held in this xParameters object.
     */
    inline size_type size() const { return myVector.size(); }

    /**
     * Return true if there exist no tokens represented by this
     * class.
     */
    inline bool empty() const { return myVector.empty(); }

    /**
     * Return true if the given variable (i) is a valid index
     * into the table of tokens.
     * Return false otherwise.
     */
    inline bool validSubscript(const size_type& i) const { return (i < myVector.size()); }

    /**
     * Return a string containing all tokens beginning
     * with the zero based index beginIndex.  A ' ' will
     * be placed between token in the string returned.
     */
    inline std::string assemble(const size_type& beginIndex) const {
        assert(validSubscript(beginIndex));
        if (myVector.empty()) {
            return std::string();
        }
        std::string retMe("");
        for (vectorType::size_type i = beginIndex; i < myVector.size(); ++i) {
            retMe += myVector[i];
            if ((i + 1) < myVector.size()) {
                retMe += ' ';
            }
        }
        return retMe;
    }

    /**
     * A simple operator to output an xParameters object to
     * an ELog stream.
     */
    friend ELog& operator<<(ELog& out, const xParameters& param) {
        // Iterate through to each element
        for (size_type i = 0; i < param.size(); ++i) {
            // Place this element into the ELog stream
            out << param[i];

            // If there is at least one more token left,
            // place a space character into the stream
            if ((i + 1) < param.size()) {
                out << ' ';
            }
        }
        // Return the ELog stream so that it may be used
        // for further pipelining of output
        return out;
    }

    /**
     * Replace the message-tag list for this parameter bag.
     */
    inline void setTags(tagListType tags) { myTags = std::move(tags); }

    /**
     * Return the IRCv3 message-tags for this message (may be empty).
     */
    [[nodiscard]] inline const tagListType& getTags() const { return myTags; }

    /**
     * Return true if a tag with the given key is present.
     */
    [[nodiscard]] inline bool hasTag(std::string_view key) const {
        return findTag(key) != myTags.end();
    }

    /**
     * Return the value of the named tag, or nullopt if absent.
     * Tags without a value yield an empty string_view.
     */
    [[nodiscard]] inline std::optional<std::string_view> getTag(std::string_view key) const {
        auto it = findTag(key);
        if (it == myTags.end()) {
            return std::nullopt;
        }
        return it->value;
    }

    /**
     * Unescape an IRCv3 message-tag <escaped value>.
     *
     * Wire sequence -> character:
     *   \: -> ';'   \s -> SPACE   \\ -> '\'
     *   \r -> CR    \n -> LF      (all others unchanged)
     *
     * A lone trailing '\' produces no output character.
     * An invalid escape (e.g. \b) drops the backslash and keeps
     * the following character (e.g. "b").
     */
    [[nodiscard]] static inline std::string unescapeTagValue(std::string_view value) {
        std::string out;
        out.reserve(value.size());
        for (size_t i = 0; i < value.size(); ++i) {
            if (value[i] != '\\') {
                out += value[i];
                continue;
            }
            ++i;
            if (i >= value.size()) {
                // Lone trailing '\': no output character
                break;
            }
            switch (value[i]) {
            case ':': // \:
                out += ';';
                break;
            case 's': // \s
                out += ' ';
                break;
            case '\\': // backslash
                out += '\\';
                break;
            case 'r': // \r
                out += '\r';
                break;
            case 'n': // \n
                out += '\n';
                break;
            default:
                // Invalid escape: drop '\', keep the character
                out += value[i];
                break;
            }
        }
        return out;
    }

    /**
     * Escape an IRCv3 message-tag value for the wire.
     */
    [[nodiscard]] static inline std::string escapeTagValue(std::string_view value) {
        std::string out;
        out.reserve(value.size());
        for (unsigned char c : value) {
            switch (c) {
            case ';':
                out += "\\:";
                break;
            case ' ':
                out += "\\s";
                break;
            case '\\':
                out += "\\\\";
                break;
            case '\r':
                out += "\\r";
                break;
            case '\n':
                out += "\\n";
                break;
            default:
                out += static_cast<char>(c);
                break;
            }
        }
        return out;
    }

    /**
     * Build the wire prefix for a tag list: "@key=value;key2=value2 "
     * (including leading '@' and trailing space). Empty tags yield "".
     */
    [[nodiscard]] static inline std::string formatTagPrefix(const tagListType& tags) {
        if (tags.empty()) {
            return {};
        }
        std::string out("@");
        for (size_t i = 0; i < tags.size(); ++i) {
            if (i > 0) {
                out += ';';
            }
            out += tags[i].key;
            if (!tags[i].value.empty()) {
                out += '=';
                out += escapeTagValue(tags[i].value);
            }
        }
        out += ' ';
        return out;
    }

    /**
     * Parse a semicolon-separated IRCv3 tag list (without the
     * leading '@') into out.  Existing entries in out are cleared.
     */
    static inline void parseTags(std::string_view tagString, tagListType& out) {
        out.clear();
        if (tagString.empty()) {
            return;
        }

        size_t pos = 0;
        while (pos < tagString.size()) {
            // Skip empty segments (e.g. trailing ';')
            if (tagString[pos] == ';') {
                ++pos;
                continue;
            }

            const size_t keyStart = pos;
            while (pos < tagString.size() && tagString[pos] != '=' && tagString[pos] != ';') {
                ++pos;
            }
            std::string_view key = tagString.substr(keyStart, pos - keyStart);
            std::string value;

            if (pos < tagString.size() && tagString[pos] == '=') {
                ++pos;
                const size_t valueStart = pos;
                while (pos < tagString.size() && tagString[pos] != ';') {
                    ++pos;
                }
                value = unescapeTagValue(tagString.substr(valueStart, pos - valueStart));
            }

            if (!key.empty()) {
                out.push_back(MessageTag{std::string(key), std::move(value)});
            }

            if (pos < tagString.size() && tagString[pos] == ';') {
                ++pos;
            }
        }
    }

  protected:
    /**
     * Disable copy constructing.
     * This method is declared, but NOT defined.
     */
    xParameters(const xParameters&);

    /**
     * Disable default assignment.
     * This method is declared but NOT defined.
     */
    xParameters operator=(const xParameters&);

    /**
     * Locate a tag by key.
     */
    [[nodiscard]] inline tagListType::const_iterator findTag(std::string_view key) const {
        return std::ranges::find(myTags, key, &MessageTag::key);
    }

    /**
     * This is the structure used to store tokens internally.
     */
    vectorType myVector;

    /**
     * IRCv3 message-tags for this message (owned strings).
     */
    tagListType myTags;
};

} // namespace gnuworld

#endif // __XPARAMETERS_H
