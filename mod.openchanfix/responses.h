/**
 * responses.h
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
 * $Id: responses.h,v 1.6 2006/12/26 14:36:08 buzlip01 Exp $
 */

#ifndef __RESPONSES_H
#define __RESPONSES_H "$Id: responses.h,v 1.6 2006/12/26 14:36:08 buzlip01 Exp $"

namespace gnuworld
	{
namespace cf
	{
	namespace language
		{
		/* ADDFLAG */
		const int one_flag_per_addflag		= 1;
		const int no_such_user			= 2;
		const int owner_add_owner_only		= 3;
		const int user_man_add_owner_only	= 4;
		const int cant_add_flag_diff_group	= 5;
		const int cant_add_block_flag		= 6;
		const int cant_add_serveradmin_flag	= 7;
		const int user_already_has_flag		= 8;
		const int added_flag_to_user		= 9;
		const int norm_user_add_owner_only	= 155;

		/* ADDHOST */
		const int cant_add_host_an_owner	= 186;
		const int cant_add_host_manager		= 187;
		const int cant_add_host_diff_group	= 10;
		const int already_has_hostmask		= 11;
		const int failed_adding_hostmask	= 12;
		const int adding_hostmask_to_user	= 13;
		const int removed_default_hostmask	= 170;

		/* ADDNOTE */
		const int invalid_channel_name		= 14;
		const int note_recorded			= 15;

		/* ADDUSER */
		const int user_already_exists		= 16;
		const int created_user_w_host		= 17;
		const int created_user_wo_host		= 18;
		const int error_creating_user		= 19;

		/* ALERT */
		const int alert_already_set		= 20;
		const int alert_flag_added		= 21;

		/* BLOCK */
		const int channel_blocking_disabled	= 22;
		const int channel_already_blocked	= 23;
		const int channel_has_been_blocked	= 24;
		const int aborting_manual_fix		= 178;
		const int aborting_auto_fix		= 179;

		/* CANFIX */
		const int canfix_op_accounts		= 197;
		const int canfix_rank_score_acc_header	= 198;
		const int can_fix_not_enough_points	= 199;
		const int canfix_manual_fix_disabled	= 204;

		/* CHANFIX */
		const int manual_fix_disabled		= 25;
		const int not_enough_servers		= 26;
		const int no_such_channel		= 27;
		const int registered_channel		= 29;
		const int no_scores_for_chan		= 30;
		const int already_being_man_fixed	= 31;
		const int highscore_channel		= 32;
		const int channel_being_auto_fixed	= 33;
		const int channel_blocked		= 34;
		const int channel_has_notes		= 35;
		const int manual_chanfix_ack		= 36;
		const int temporarily_blocked_override	= 189;
		const int score_not_high_enough		= 196;
		const int not_enough_servers_non_oper	= 200;
		const int channel_being_auto_fixed_noper	= 201;
		const int channel_blocked_non_oper	= 202;
		const int channel_temp_blocked		= 211;

		/* CHECK */
		const int check_results			= 37;

		/* DELFLAG */
		const int one_flag_per_delflag		= 38;
		const int only_owner_del_owner_flag	= 39;
		const int only_owner_del_user_flag	= 40;
		const int cant_delete_flag_diff_group	= 41;
		const int cant_remove_block_flag	= 42;
		const int cant_remove_server_flag	= 43;
		const int user_does_not_have_flag	= 44;
		const int deleted_flag			= 45;
		const int only_owner_del_norm_flag	= 156;

		/* DELHOST */
		const int cant_del_host_an_owner	= 184;
		const int cant_del_host_manager		= 185;
		const int cant_del_host_diff_group	= 46;
		const int user_doesnt_have_host		= 47;
		const int failed_deleting_host		= 48;
		const int deleted_hostmask		= 49;

		/* DELNOTE */
		const int no_entry_in_db		= 50;
		const int chan_has_no_notes		= 51;
		const int error_checking_noteid		= 52;
		const int no_note_with_id		= 53;
		const int no_note_id_for_chan		= 54;
		const int note_not_added_by_you		= 55;
		const int note_not_manually_added	= 56;
		const int note_deleted			= 57;

		/* DELUSER */
		const int cant_delete_an_owner		= 58;
		const int cant_delete_manager		= 59;
		const int cant_delete_from_diff_group	= 60;
		const int deleted_user			= 61;
		const int error_deleting_user		= 62;

		/* HISTORY */
		const int chan_no_manual_fixes		= 63;
		const int error_occured_notes		= 64;
		const int chan_manually_fix		= 65;
		const int end_of_list			= 67;

		/* INFO */
		const int no_info_for_chan		= 68;
		const int information_on		= 69;
		const int info_chan_blocked		= 70;
		const int info_chan_being_fixed		= 71;
		const int info_chan_being_autofixed	= 72;
		const int info_notes_count		= 73;
		const int end_of_information		= 74;
		const int info_chan_alerted		= 75;
		const int info_notes			= 76;
		const int temporarily_blocked		= 188;
		const int info_fix_started		= 192;
		const int info_fix_waiting		= 193;
		const int info_chan_temp_blocked	= 214;

		/* INVITE */
		const int cant_find_channel		= 77;
		const int chanfix_not_in_chan		= 78;
		const int already_in_channel		= 79;

		/* LASTCOM */
		const int lastcom_too_far_back		= 208;
		const int lastcom_listing_messages	= 209;
		const int end_of_log			= 210;
		
		/* LISTBLOCKED */
		const int listblocks_blocked_chans	= 190;
		const int listblocked_total_blocked	= 191;

		/* LISTHOSTS */
		const int cant_view_hosts_diff_group	= 163;
		const int host_list_header		= 164;
		const int host_list_footer		= 165;

		/* OPLIST */
		const int top_unique_op_accounts	= 80;
		const int found_unique_op_accounts	= 81;
		const int rank_score_acc_header		= 82;
		const int end_of_oplist			= 207;

		/* OPNICKS */
		const int opped_clients_on		= 83;
		const int one_opped_client		= 84;
		const int opped_clients			= 85;

		/* REHASH */
		const int reloaded_help			= 86;
		const int reloaded_conf			= 87;

		/* RELOAD */
		const int reloading_client		= 88;

		/* REQUESTOP */
		const int requestop_ack			= 205;
		const int requestop_must_wait		= 206;

		/* SCORE */
		const int score_for_channel_nick	= 89;
		const int score_for_channel_account	= 158;
		const int compact_no_such_nick		= 90;
		const int no_such_nick			= 91;
		const int compact_no_score_for_account	= 156;
		const int no_score_for_account		= 157;
		const int score_none			= 92;
		const int top_scores_for_chan		= 93;
		const int top_scores_for_ops		= 94;
		const int top_scores_for_nonops		= 95;
		const int no_score_for_nick		= 166;
		const int compact_no_score_for_nick	= 167;

		/* SET */
		const int num_server_syntax		= 96;
		const int num_servers_set		= 97;
		const int enabled_autofix		= 98;
		const int disabled_autofix		= 99;
		const int enable_autofix_syntax		= 100;
		const int enabled_manual_chanfix	= 101;
		const int disabled_manual_chanfix	= 102;
		const int enable_chanfix_syntax		= 103;
		const int enabled_chan_blocking		= 104;
		const int disabled_chan_blocking	= 105;
		const int enable_chan_blocking_syntax	= 106;
		const int setting_doesnt_exist		= 107;

		/* SETGROUP */
		const int user_already_in_group		= 108;
		const int set_group_for_user		= 109;

		/* SHUTDOWN */
		const int shutting_down			= 110;
		const int update_in_progress		= 173;

		/* SIMULATE */
		const int manual_simulate_complete	= 221;
		const int manual_simulate_starting	= 224;
		const int manual_simulate_estimate	= 225;

		/* STATUS */
		const int status_uptime			= 111;
		const int status_auto_fixing		= 112;
		const int status_manual_fixing		= 113;
		const int status_chan_blocking		= 114;
		const int status_servers_amount		= 115;
		const int status_splitmode_enabled	= 116;
		const int status_splitmode_disabled	= 117;
		const int status_channel_service_linked	= 171;
		const int status_channel_service_not_linked	= 172;
		const int status_total_channels		= 174;
		const int status_total_channels_with_extra	= 175;

		/* SUSPEND */
		const int cant_suspend_an_owner		= 180;
		const int cant_suspend_manager		= 181;
		const int cant_suspend_diff_group	= 118;
		const int user_already_suspended	= 119;
		const int user_suspended		= 120;
		const int user_cant_suspend_self	= 159;

		/* UNALERT */
		const int no_alert_set			= 121;
		const int alert_removed			= 122;

		/* UNBLOCK */
		const int channel_not_blocked		= 123;
		const int channel_unblocked		= 124;

		/* UNSUSPEND */
		const int cant_unsuspend_an_owner	= 182;
		const int cant_unsuspend_manager	= 183;
		const int cant_unsuspend_diff_group	= 125;
		const int user_not_suspended		= 126;
		const int user_unsuspended		= 127;

		/* USERSCORES */
		const int userscores_header 		= 194;
		const int userscores_noscore		= 195;
		
		/* USET */
		const int lang_set_to			= 128;
		const int send_notices			= 129;
		const int send_privmsgs			= 130;
		const int uset_notice_on_off		= 131;
		
		/* WHOFLAG */
		const int one_flag_per_whoflag		= 226;

		/* WHOGROUP */
		const int whogroup_your_group		= 134;
		const int users_with_group		= 135;
		const int number_of_users		= 136;
		const int whogroup_list_groups		= 168;
		const int number_of_groups		= 169;

		/* WHOIS */
		const int list_of_all_users		= 137;
		const int user_flags_group		= 138;
		const int whois_suspended		= 140;
		const int whois_group			= 142;
		const int whois_flags_none		= 143;
		const int whois_flags			= 144;
		const int whois_lang			= 155;
		const int whois_modif			= 160;
		const int whois_needoper_yes		= 161;
		const int whois_needoper_no		= 162;
		const int whois_lastused		= 176;
		const int whois_lastused_never		= 177;

		/* chanfix.cc */
		const int no_commands_during_burst	= 145;
		const int unknown_command		= 146;
		const int access_suspended		= 147;
		const int host_not_matching		= 148;
		const int need_to_auth			= 149;
		const int requires_flag			= 150;
		const int requires_flags		= 151;
		const int requires_auth_and_flag	= 152;
		const int requires_auth_and_flags	= 153;
		const int syntax			= 154;
		const int no_commands_during_burst_noper	= 203;
		const int sim_opping			= 222;
		const int sim_modes_removed		= 223;

	} // namespace language
} // namespace cf
} // namespace gnuworld

#endif /* __RESPONSES_H */
