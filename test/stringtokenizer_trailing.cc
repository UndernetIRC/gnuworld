/**
 * stringtokenizer_trailing.cc
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
 * Exercises StringTokenizer::trailing() — the IRC trailing parameter
 * (first token beginning with ':', leading ':' stripped).
 */

#include <iostream>
#include <optional>
#include <string>

#include "StringTokenizer.h"

using namespace std;
using namespace gnuworld;

static int failures = 0;

static void check(bool condition, const string& description) {
    if (!condition) {
        cout << "FAILED: " << description << endl;
        ++failures;
    }
}

static void testTrailingBasic() {
    StringTokenizer st("CMD arg :hello");
    auto t = st.trailing();
    check(t.has_value() && *t == "hello", "trailing returns text after the leading ':'");
}

static void testTrailingMultiWord() {
    StringTokenizer st("CMD arg :hello world there");
    auto t = st.trailing();
    check(t.has_value() && *t == "hello world there",
          "trailing reassembles multi-word realname after the colon");
}

static void testTrailingColonOnly() {
    StringTokenizer st("CMD arg :");
    auto t = st.trailing();
    // Tokenize drops empty tokens, so a lone trailing ':' may not appear.
    // If present as ":", substr(1) is empty; if absent, nullopt.
    // Space-delimited ":" alone is a token ":".
    check(t.has_value() && t->empty(), "trailing of a lone ':' token is an empty string");
}

static void testTrailingAbsent() {
    StringTokenizer st("CMD nick user ip host");
    check(!st.trailing().has_value(), "trailing returns nullopt when no ':'-prefixed token exists");

    StringTokenizer empty;
    check(!empty.trailing().has_value(), "trailing returns nullopt for an empty tokenizer");
}

static void testTrailingCheckXqShapes() {
    // Legacy iauth CHECK (no account field)
    StringTokenizer legacy("CHECK nick user 1.2.3.4 host :Some Real Name");
    check(legacy.trailing().has_value() && *legacy.trailing() == "Some Real Name",
          "CHECK without account: trailing is the realname");

    // Optional account before trailing
    StringTokenizer withAcct("CHECK nick user 1.2.3.4 host myaccount :Some Real Name");
    check(withAcct.trailing().has_value() && *withAcct.trailing() == "Some Real Name",
          "CHECK with account: trailing is still the realname");

    // Explicit no-account marker
    StringTokenizer starAcct("CHECK nick user 1.2.3.4 host * :Some Real Name");
    check(starAcct.trailing().has_value() && *starAcct.trailing() == "Some Real Name",
          "CHECK with '*': trailing is still the realname");

    // Same pattern used by iauthXQCheck callers
    string fullname = withAcct.trailing().value_or(withAcct.assemble(5));
    check(fullname == "Some Real Name", "value_or(assemble(5)) yields trailing when present");

    StringTokenizer noColon("CHECK nick user 1.2.3.4 host leftover");
    fullname = noColon.trailing().value_or(noColon.assemble(5));
    check(fullname == "leftover", "value_or(assemble(5)) falls back when trailing is absent");
}

int main() {
    testTrailingBasic();
    testTrailingMultiWord();
    testTrailingColonOnly();
    testTrailingAbsent();
    testTrailingCheckXqShapes();

    if (0 == failures) {
        cout << "All StringTokenizer trailing tests passed." << endl;
        return 0;
    }

    cout << failures << " test(s) failed." << endl;
    return 1;
}
