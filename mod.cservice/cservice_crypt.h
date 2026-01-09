/**
 * cservice_crypt.h
 * Author: MrIron <mriron@undernet.org>
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
 */

#include "defs.h"
#ifdef HAVE_LIBSSL

#pragma once

#include <vector>
#include <string>
#include <optional>

namespace gnuworld {

/**
 * Parsed SCRAM-SHA-256 record.
 */
struct ScramParsed {
    int iterations ;
    std::vector< unsigned char > salt ;      // variable length, typically 16
    std::vector< unsigned char > storedKey ; // 32 bytes
    std::vector< unsigned char > serverKey ; // 32 bytes
} ;

/**
 * Create a SCRAM-SHA-256 record string from a plaintext password.
 * Format: "SCRAM-SHA-256$<iter>:<b64(salt)>$<b64(storedKey)>:<b64(serverKey)>"
 * Returns std::nullopt on failure; writes error text into err if provided.
 */
std::optional< std::string > make_scram_sha256_record( std::string_view password, std::string* err = nullptr ) ;

/**
 * Parse a SCRAM-SHA-256 record string into its components.
 * Returns std::nullopt on parse error; writes error text into err if provided.
 */
std::optional< ScramParsed > parse_scram_sha256_record( const std::string& record, std::string* err = nullptr ) ;

/**
 * Generate a random nonce for SCRAM authentication.
 * Returns std::nullopt on failure; writes error text into err if provided.
 */
std::optional< std::string > generateRandomNonce( size_t length = 18, std::string* err = nullptr ) ;

/**
 * Base64-encode a byte array.
 */
std::string b64encode( const unsigned char* data, size_t len ) ;

/**
 * Base64-decode a byte array.
 */
std::optional< std::vector< unsigned char > >
b64decode( std::string_view b64, std::string* err = nullptr, bool validatePrintable = false ) ;

/**
 * Validate a SCRAM-SHA-256 client proof.
 */
bool validate_scram_sha256_proof(
    const std::vector< unsigned char >& storedKey,
    const std::string& authMessage,
    const std::string& clientProof_b64
) ;

/**
 * Compute the SCRAM server signature.
 */
std::string compute_server_signature(
    const std::vector< unsigned char >& serverKey,
    const std::string& authMessage
) ;

} // namespace

#endif // HAVE_LIBSSL
