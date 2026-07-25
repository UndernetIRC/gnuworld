/**
 * xparameters_tags.cc
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
 * Exercises the IRCv3 message-tag helpers added to xParameters:
 * escapeTagValue/unescapeTagValue, parseTags, formatTagPrefix,
 * hasTag/getTag.
 */

#include <iostream>
#include <string>

#include "xparameters.h"

using namespace std;
using namespace gnuworld;

static int failures = 0;

static void check(bool condition, const string& description) {
    if (!condition) {
        cout << "FAILED: " << description << endl;
        ++failures;
    }
}

static void testEscapeRoundTrip() {
    // Every special character should survive escape -> unescape.
    const string raw = "a;b c\\d\re\nf";
    const string escaped = xParameters::escapeTagValue(raw);
    check(escaped == "a\\:b\\sc\\\\d\\re\\nf",
          "escapeTagValue produces the expected wire form");
    check(xParameters::unescapeTagValue(escaped) == raw,
          "unescapeTagValue reverses escapeTagValue");
}

static void testUnescapeEdgeCases() {
    // Lone trailing backslash: produces no output character for it.
    check(xParameters::unescapeTagValue("abc\\") == "abc",
          "trailing lone backslash is dropped");

    // Invalid escape sequence: drop the backslash, keep the character.
    check(xParameters::unescapeTagValue("a\\bc") == "abc",
          "invalid escape drops backslash and keeps following character");

    // Empty value.
    check(xParameters::unescapeTagValue("") == "", "empty value unescapes to empty");
}

static void testParseTagsBasic() {
    xParameters::tagListType tags;

    xParameters::parseTags("key=value", tags);
    check(tags.size() == 1, "single key=value tag parses to one entry");
    check(tags.size() == 1 && tags[0].key == "key" && tags[0].value == "value",
          "single key=value tag has expected key/value");

    xParameters::parseTags("flag", tags);
    check(tags.size() == 1 && tags[0].key == "flag" && tags[0].value.empty(),
          "key-only tag (no '=') parses with empty value");

    xParameters::parseTags("a=1;b=2;c", tags);
    check(tags.size() == 3, "multiple tags separated by ';' all parse");
    check(tags.size() == 3 && tags[0].key == "a" && tags[0].value == "1" && tags[1].key == "b" &&
              tags[1].value == "2" && tags[2].key == "c" && tags[2].value.empty(),
          "multiple tags parse in order with correct key/value pairs");
}

static void testParseTagsMalformed() {
    xParameters::tagListType tags;

    // Stray/trailing ';' should not produce empty-key entries.
    xParameters::parseTags(";a=1;;b=2;", tags);
    check(tags.size() == 2 && tags[0].key == "a" && tags[1].key == "b",
          "empty segments from stray/leading/trailing ';' are skipped");

    xParameters::parseTags("", tags);
    check(tags.empty(), "empty tag string parses to no tags");
}

static void testEscapedValueRoundTripThroughFormat() {
    xParameters::tagListType tags;
    tags.push_back(MessageTag{"msg", "hello; world \\ done"});
    tags.push_back(MessageTag{"empty", ""});

    const string prefix = xParameters::formatTagPrefix(tags);
    check(!prefix.empty() && prefix.front() == '@' && prefix.back() == ' ',
          "formatTagPrefix wraps tags with leading '@' and trailing space");

    // Strip the leading '@' and trailing ' ' before re-parsing, mirroring
    // what xServer::Process does on incoming lines.
    const string tagBody = prefix.substr(1, prefix.size() - 2);

    xParameters::tagListType roundTripped;
    xParameters::parseTags(tagBody, roundTripped);

    check(roundTripped.size() == 2, "round-tripped tag list has the same number of tags");
    check(roundTripped.size() == 2 && roundTripped[0].key == "msg" &&
              roundTripped[0].value == "hello; world \\ done",
          "escaped value round-trips through formatTagPrefix/parseTags unchanged");
    check(roundTripped.size() == 2 && roundTripped[1].key == "empty" &&
              roundTripped[1].value.empty(),
          "empty-value tag round-trips as a key with no '='");
}

static void testFormatTagPrefixEmpty() {
    xParameters::tagListType tags;
    check(xParameters::formatTagPrefix(tags).empty(),
          "formatTagPrefix returns empty string for an empty tag list");
}

static void testHasTagGetTag() {
    xParameters params;
    xParameters::tagListType tags;
    tags.push_back(MessageTag{"time", "2026-07-25T00:00:00.000Z"});
    tags.push_back(MessageTag{"flag", ""});
    params.setTags(tags);

    check(params.hasTag("time"), "hasTag finds a present tag");
    check(!params.hasTag("missing"), "hasTag returns false for an absent tag");

    auto timeValue = params.getTag("time");
    check(timeValue.has_value() && *timeValue == "2026-07-25T00:00:00.000Z",
          "getTag returns the value of a present tag");

    auto flagValue = params.getTag("flag");
    check(flagValue.has_value() && flagValue->empty(),
          "getTag returns an empty string_view for a value-less tag");

    check(!params.getTag("missing").has_value(), "getTag returns nullopt for an absent tag");

    params.Clear();
    check(params.getTags().empty(), "Clear() also clears the tag list");
}

int main() {
    testEscapeRoundTrip();
    testUnescapeEdgeCases();
    testParseTagsBasic();
    testParseTagsMalformed();
    testEscapedValueRoundTripThroughFormat();
    testFormatTagPrefixEmpty();
    testHasTagGetTag();

    if (0 == failures) {
        cout << "All xparameters tag tests passed." << endl;
        return 0;
    }

    cout << failures << " test(s) failed." << endl;
    return 1;
}
