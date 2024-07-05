Remember: If you have an existing 'old' gnuworld installation, follow doc/inet-migration/README to upgrade

- IPv6 Support
- Two-factor authentication support for users (TOTP)
- Improved channel registration traffic checking (potentially speeding up channel registrations)
- New IPR system (IPR, TOTP, TOTP+IPR) -- including temporary entries and CIDR support
- New users are now automatically registered with INVISIBLE=ON
- Welcome messages and/or topics from X upon new channel registration
- X sets AUTOJOIN (and joins) channel automatically on new registration
- X immediately auto-ops itself when joining channel
- X now allows permanent bans with duration '0' (and the parser allows up to 365d in length too)
- X now processes channel flags (NOOP, STRICTOP) and user access suspensions when it rejoins a channel and acts accordingly
- X now has a fun 'StarWars' (SW) language
- Allow established users to register multiple channels (same logic as MAXLOGINS for up to 3 channels for now)
- LoC (login-on-connect) -- authenticate during server connection and optionally set umode +x automatically  ***
- Drone scan clients during server connection rather than afterwards (disallows fast drones to connect, join channels, and flood before being G-Lined)

+ GNUWorld now handles TheJudge internally (optionally with #define USE_INTERNAL_THE_JUDGE)
+ GNUWorld now handles username expiries internally
+ X now allows 'VARIABLES' to be reloaded from config file via 'REHASH' command
+ Added NOTAKE channel setting (lvl 500+) to prevent users from setting bans which hit all channel users.  TAKEREVENGE options include 'NONE', 'BAN', and 'SUSPEND'
+ Channel managers now get notified when userlist access is changed in their channels
+ Users now get notified when they have access added, modified, or removed from channels
+ Applicants now get notifications online when their application has changed state
+ Supporters now get notifications on incoming channel application
+ Maximum ban reason length increased to 300 chars (from 128)
+ Maximum ban duration parsing changed to 365 days (from 100) and new value of 0 allowed for permanent bans
+ Maximum URL length increased to 128 chars (from 75)
+ Maximum channel description length increased to 380 chars (from 80)
+ Added specific VERIFY response for those users who have the ALUMNI user flag set
+ X now tracking login history for use with SCANHOST command
+ SCANHOST command now supports advanced sorting with maximum 50 results
+ Added NOVOICE channel setting (same behaviour as NOOP but prevents users being voiced)
+ Channel user suspensions can now have an optional reason applied
+ Added AUTOMODE protection on users who do not have sufficient access for the automatic mode (OP or VOICE)
+ Added MODE command to set channel modes via X
+ Added support for mulitiple kicks delimited by ','
+ Added optional channel flood protection (tracked by IP address) for lvl 500+ to set:

    /msg X set <channel> FLOODPRO <ON|OFF>

    /msg X set <channel> <option> <value>

        MSGFLOOD    0-255    (default: 15)
        NOTICEFLOOD    0-15    (default: 5)
        CTCPFLOOD    0-15    (default: 5)
        REPEATFLOOD    0-15    (default: 5)

        FLOODPERIOD    0-15    (default: 15)
    Note:
    	/msg X set <channel> FLOODPRO DEFAULTS -- sets the defult values

    /msg X set <channel> FLOODNET <option>

        ON/BAN            (3 hour ban @ level 75)
        WARN/KICK
        GLINE            (3 hours)
        OFF/NONE

        ... Channel ops will get opnotices when this level is changed
