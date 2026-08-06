"""P10 base64 and numnick helpers for the fake hub."""

_B64 = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789[]"
_B64_VAL = {c: i for i, c in enumerate(_B64)}


def int_to_b64(value: int, width: int) -> str:
    """Encode an integer as a P10 base64 string of fixed width."""
    chars = []
    for _ in range(width):
        chars.append(_B64[value & 63])
        value >>= 6
    return "".join(reversed(chars))


def b64_to_int(s: str) -> int:
    """Decode a P10 base64 string to an integer."""
    result = 0
    for c in s:
        result = (result << 6) | _B64_VAL[c]
    return result


def server_numeric(num: int) -> str:
    """Encode a server numeric as 2-char P10 base64."""
    return int_to_b64(num, 2)


def client_numnick(server_num: int, client_num: int) -> str:
    """Encode a full SSCCC numnick (2-char server + 3-char client)."""
    return server_numeric(server_num) + int_to_b64(client_num, 3)


def ipv4_to_b64(ip: str = "127.0.0.1") -> str:
    """Encode an IPv4 address as a 6-character P10 base64 string."""
    parts = [int(x) for x in ip.split(".")]
    value = (parts[0] << 24) | (parts[1] << 16) | (parts[2] << 8) | parts[3]
    return int_to_b64(value, 6)


def parse_numnick(numnick: str) -> tuple[int, int]:
    """Parse a 5-char numnick into (server_numeric, client_numeric)."""
    return b64_to_int(numnick[:2]), b64_to_int(numnick[2:])


def strip_msg_tags(line: str) -> str:
    """Remove a leading IRCv3 @tag-section from an S2S line if present."""
    if not line.startswith("@"):
        return line
    sp = line.find(" ")
    return line[sp + 1 :] if sp != -1 else line


def p10_token(line: str) -> str | None:
    """Extract the P10 command token (second word), or first word if unprefixed."""
    parts = strip_msg_tags(line).split()
    if not parts:
        return None
    # Unprefixed commands (PASS, SERVER, EB, EA during handshake)
    if parts[0] in {"PASS", "SERVER", "ERROR", "EB", "EA"}:
        return parts[0]
    if len(parts) >= 2:
        return parts[1]
    return None
