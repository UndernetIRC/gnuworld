/**
 * cservice_crypt.cc
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

#include "cservice_crypt.h"
#ifdef HAVE_LIBSSL

#include <array>
#include <cstring>
#include <string>
#include <string_view>
#include <vector>
#include <stdexcept>
#include <regex>
#include <optional>
#include <charconv>
#include <openssl/evp.h>
#include <openssl/hmac.h>
#include <openssl/rand.h>
#include <openssl/crypto.h>
#include <openssl/sha.h>
#include <iomanip>

#include "cservice_config.h"
#include "ELog.h"

namespace gnuworld
{

/**
 * Write an error if `err` is non-null, then return std::nullopt.
 */
template <typename T = void>
static std::optional<T> fail( std::string_view msg, std::string* err )
{
    if( err ) *err = std::string( msg ) ;
    return std::nullopt ;
}

/**
 * Base64-encode a byte array.
 */
std::string b64encode( const unsigned char* data, size_t len )
{
    std::string out ;
    out.resize( 4 * ( ( len + 2 ) / 3 ) ) ;
    int n = EVP_EncodeBlock( reinterpret_cast< unsigned char* >( &out[ 0 ] ), data, (int)len ) ;
    if( n < 0 ) return {} ;
    out.resize( (size_t)n ) ;
    return out ;
}

/**
 * Base64-decode a string to a byte vector.
 */
std::optional< std::vector< unsigned char > >
b64decode( std::string_view b64, std::string* err, bool validatePrintable )
{
    if( b64.size() % 4 != 0 ) return fail< std::vector< unsigned char > >( "bad base64 length", err ) ;

    // Validate that the input contains only valid base64 characters
    for( char c : b64 ) {
        if( !std::isalnum( c ) && c != '+' && c != '/' && c != '=' ) {
            return fail< std::vector< unsigned char > >( "invalid base64 character", err ) ;
        }
    }

    std::vector< unsigned char > out ;
    out.resize( 3 * ( b64.size() / 4 ) ) ;

    int n = EVP_DecodeBlock( out.data(),
                             reinterpret_cast< const unsigned char* >( b64.data() ),
                             (int)b64.size() ) ;
    if( n < 0 ) return fail< std::vector< unsigned char > >( "bad base64", err ) ;

    // Compute real length by subtracting '=' padding (0,1,2)
    size_t pad = 0 ;
    if( !b64.empty() && b64.back() == '=' )
    {
        pad++ ;
        if( b64.size() >= 2 && b64[ b64.size() - 2 ] == '=' ) pad++ ;
    }
    size_t real_len = (size_t)n - pad ;
    if( real_len > out.size() ) return fail< std::vector< unsigned char > >( "base64 overflow", err ) ;
    out.resize( real_len ) ;
    
    // Optionally validate that decoded data contains only printable ASCII characters and null bytes
    if( validatePrintable ) {
        for( size_t i = 0 ; i < real_len ; ++i ) {
            if( out[ i ] != 0 && ( out[ i ] < 32 || out[ i ] > 126 ) ) {
                return fail< std::vector< unsigned char > >( "decoded data contains non-printable characters", err ) ;
            }
        }
    }

    return out ;
}

/**
 * PBKDF2-HMAC-SHA256 key derivation.
 */
static inline std::optional< std::array< unsigned char, 32 > >
pbkdf2_sha256( std::string_view password,
               const unsigned char* salt, size_t salt_len,
               int iterations,
               std::string* err )
{
    std::array< unsigned char, 32 > out{} ;
    if( !PKCS5_PBKDF2_HMAC( password.data(), (int)password.size(),
                            salt, (int)salt_len, iterations,
                            EVP_sha256(), (int)out.size(), out.data() ) )
        return fail< std::array< unsigned char, 32 > >( "PBKDF2 failed", err ) ;
    return out ;
}

/**
 * SHA256 hash.
 */
static inline std::optional< std::array< unsigned char, 32 > >
sha256( const unsigned char* data, size_t len, std::string* err )
{
    std::array< unsigned char, 32 > out{} ;
    unsigned int outlen = 0 ;
    EVP_MD_CTX* ctx = EVP_MD_CTX_new() ;
    if( !ctx ) return fail< std::array< unsigned char, 32 > >( "EVP_MD_CTX_new failed", err ) ;
    if( EVP_DigestInit_ex( ctx, EVP_sha256(), nullptr ) != 1
     || EVP_DigestUpdate( ctx, data, len ) != 1
     || EVP_DigestFinal_ex( ctx, out.data(), &outlen ) != 1 )
    {
        EVP_MD_CTX_free( ctx ) ;
        return fail< std::array< unsigned char, 32 > >( "SHA256 failed", err ) ;
    }
    EVP_MD_CTX_free( ctx ) ;
    return out ;
}

/**
 * HMAC-SHA256.
 */
static inline std::optional< std::array< unsigned char, 32 > >
hmac_sha256( const unsigned char* key, size_t key_len,
             const unsigned char* msg, size_t msg_len,
             std::string* err )
{
    std::array< unsigned char, 32 > out{} ;
    unsigned int outlen = 0 ;
    if( !HMAC( EVP_sha256(), key, (int)key_len, msg, msg_len, out.data(), &outlen ) )
        return fail< std::array< unsigned char, 32 > >( "HMAC failed", err ) ;
    return out ;
}

/**
 * Parse a SCRAM-SHA-256 record string into its components.
 */
std::optional< ScramParsed >
parse_scram_sha256_record( const std::string& rec, std::string* err )
{
    constexpr const char* kPrefix = "SCRAM-SHA-256$" ;
    if( rec.rfind( kPrefix, 0 ) != 0 )
        return fail< ScramParsed >( "bad prefix", err ) ;

    std::string_view rest( rec ) ;
    rest.remove_prefix( std::char_traits< char >::length( kPrefix ) ) ;

    // iterations
    auto pos1 = rest.find( ':' ) ;
    if( pos1 == std::string_view::npos ) return fail< ScramParsed >( "bad record (no ':')", err ) ;
    int iterations = 0 ;
    {
        auto it_str = rest.substr( 0, pos1 ) ;
        auto first = it_str.data() ;
        auto last  = it_str.data() + it_str.size() ;
        auto res = std::from_chars( first, last, iterations ) ;
        if( res.ec != std::errc() || res.ptr != last )
            return fail< ScramParsed >( "bad iterations", err ) ;
    }
    rest.remove_prefix( pos1 + 1 ) ;

    // salt_b64
    auto pos2 = rest.find( '$' ) ;
    if( pos2 == std::string_view::npos ) return fail< ScramParsed >( "bad record (no '$')", err ) ;
    auto salt_b64 = rest.substr( 0, pos2 ) ;
    rest.remove_prefix( pos2 + 1 ) ;

    // stored_b64 : server_b64
    auto pos3 = rest.find( ':' ) ;
    if( pos3 == std::string_view::npos ) return fail< ScramParsed >( "bad record (no second ':')", err ) ;
    auto stored_b64 = rest.substr( 0, pos3 ) ;
    auto server_b64 = rest.substr( pos3 + 1 ) ;

    auto saltOpt   = b64decode( salt_b64, err ) ;
    if( !saltOpt )  return std::nullopt ;
    auto storedOpt = b64decode( stored_b64, err ) ;
    if( !storedOpt ) return std::nullopt ;
    auto serverOpt = b64decode( server_b64, err ) ;
    if( !serverOpt ) return std::nullopt ;

    if( storedOpt->size() != 32 ) return fail< ScramParsed >( "storedKey wrong size", err ) ;
    if( serverOpt->size() != 32 ) return fail< ScramParsed >( "serverKey wrong size", err ) ;

    ScramParsed out {
        iterations,
        std::move( *saltOpt ),
        std::move( *storedOpt ),
        std::move( *serverOpt )
    } ;
    return out ;
}

/**
 * Create a SCRAM-SHA-256 record string from a plaintext password.
 */
std::optional< std::string >
make_scram_sha256_record( std::string_view password, std::string* err )
{
    if( CRYPT_ITERATIONS < 4096 )
        return fail< std::string >( "iterations too low", err ) ;
    if( CRYPT_SALT_LEN < 12 || CRYPT_SALT_LEN > 64 )
        return fail< std::string >( "salt_len out of range", err ) ;

    std::vector< unsigned char > salt( CRYPT_SALT_LEN ) ;
    if( RAND_bytes( salt.data(), (int)salt.size() ) != 1 )
        return fail< std::string >( "RAND_bytes failed", err ) ;

    auto saltedOpt = pbkdf2_sha256( password, salt.data(), salt.size(), CRYPT_ITERATIONS, err ) ;
    if( !saltedOpt ) return std::nullopt ;
    auto& salted = *saltedOpt ;

    static const unsigned char ck[] = "Client Key" ;
    auto clientKeyOpt = hmac_sha256( salted.data(), salted.size(), ck, sizeof( ck ) - 1, err ) ;
    if( !clientKeyOpt ) return std::nullopt ;
    auto& clientKey = *clientKeyOpt ;

    auto storedKeyOpt = sha256( clientKey.data(), clientKey.size(), err ) ;
    if( !storedKeyOpt ) return std::nullopt ;
    auto& storedKey = *storedKeyOpt ;

    static const unsigned char sk[] = "Server Key" ;
    auto serverKeyOpt = hmac_sha256( salted.data(), salted.size(), sk, sizeof( sk ) - 1, err ) ;
    if( !serverKeyOpt ) return std::nullopt ;
    auto& serverKey = *serverKeyOpt ;

    std::string rec ;
    rec.reserve( 64 + CRYPT_SALT_LEN ) ;
    rec += "SCRAM-SHA-256$" ;
    rec += std::to_string( CRYPT_ITERATIONS ) ;
    rec += ":" ;
    rec += b64encode( salt.data(), salt.size() ) ;
    rec += "$" ;
    rec += b64encode( storedKey.data(), storedKey.size() ) ;
    rec += ":" ;
    rec += b64encode( serverKey.data(), serverKey.size() ) ;

    OPENSSL_cleanse( salted.data(), salted.size() ) ;
    OPENSSL_cleanse( clientKey.data(), clientKey.size() ) ;

    return rec ;
}

/**
 * Generate a random nonce for SCRAM authentication.
 */
std::optional< std::string > generateRandomNonce( size_t length, std::string* err )
{
    std::vector< unsigned char > buf( length ) ;
    if( RAND_bytes( buf.data(), (int)length ) != 1 )
        return fail< std::string >( "RAND_bytes failed in generateRandomNonce", err ) ;
    // Use base64 for nonce encoding, remove any trailing '='
    std::string nonce = b64encode( buf.data(), buf.size() ) ;
    nonce.erase( std::remove( nonce.begin(), nonce.end(), '=' ), nonce.end() ) ;
    return nonce ;
}

/**
 * Validate a SCRAM-SHA-256 client proof.
 */
bool validate_scram_sha256_proof(
    const std::vector< unsigned char >& storedKey,
    const std::string& authMessage,
    const std::string& clientProof_b64
)
{
    auto proofOpt = b64decode( clientProof_b64 ) ;
    if( !proofOpt || proofOpt->size() != storedKey.size() )
    {
        elog << "[SCRAM] Proof decode failed or wrong size\n" ;
        return false ;
    }
    const std::vector< unsigned char >& clientProof = *proofOpt ;

    unsigned int siglen = 0 ;
    unsigned char clientSignature[ SHA256_DIGEST_LENGTH ] ;
    HMAC( EVP_sha256(),
          storedKey.data(), storedKey.size(),
          reinterpret_cast< const unsigned char* >( authMessage.data() ), authMessage.size(),
          clientSignature, &siglen ) ;

    std::vector< unsigned char > clientKey( clientProof.size() ) ;
    for( size_t i = 0 ; i < clientProof.size() ; ++i )
        clientKey[ i ] = clientProof[ i ] ^ clientSignature[ i ] ;

    unsigned char computedStoredKey[ SHA256_DIGEST_LENGTH ] ;
    SHA256( clientKey.data(), clientKey.size(), computedStoredKey ) ;

    bool result = std::memcmp( computedStoredKey, storedKey.data(), SHA256_DIGEST_LENGTH ) == 0 ;
    return result ;
}

/**
 * Compute the SCRAM server signature.
 */
std::string compute_server_signature(
    const std::vector< unsigned char >& serverKey,
    const std::string& authMessage
)
{
    unsigned int siglen = 0 ;
    unsigned char serverSignature[ SHA256_DIGEST_LENGTH ] ;
    HMAC( EVP_sha256(),
          serverKey.data(), serverKey.size(),
          reinterpret_cast< const unsigned char* >( authMessage.data() ), authMessage.size(),
          serverSignature, &siglen ) ;
    return b64encode( serverSignature, siglen ) ;
}

} // namespace gnuworld

#endif // HAVE_LIBSSL
