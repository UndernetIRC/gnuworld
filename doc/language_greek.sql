-- $Id: language_greek.sql,v 1.8 2007/12/26 19:13:00 kewlio Exp $
-- Greek	 language definition.
-- 04/05/02 - Marios Solomou <hawk@undernet.org>.
-- 04/05/02 - deop <d30pma@cytanet.com.cy>.

DELETE FROM languages WHERE language_id=19;

INSERT INTO languages VALUES(19,'GR','Greek',31337);

DELETE FROM translations where language_id=19;
COPY "translations" FROM stdin; 
19	1	Signwmi, Eiste eidi anagnorismenos san %s	31337	0
19	2	ANAGNWRISH EPITIXIMENH san %s	31337	0
19	3	Signwmi, Den exete arketa access gia na xrisimopoieisete auti tin entoli	31337	0
19	4	Signwmi, to kanali %s einai adeio	31337	0
19	5	Den vlepw ton/tin %s pouthena	31337	0
19	6	Den mporw na vrw ton/tin %s sto kanali %s	31337	0
19	7	To kanali %s den fenetai na einai registered	31337	0
19	8	Exete ginei oped apo ton/tin %s (%s) sto %s	31337	0
19	9	Exete ginei voiced apo ton/tin %s (%s) sto %s	31337	0
19	10	%s: Den eisai se auto to kanali	31337	0
19	11	O/H %s einai eidi opped sto %s	31337	0
19	12	O/H %s einai eidi voiced sto %s	31337	0
19	13	Exeis ginei deopped apo ton/tin %s (%s) sto %s	31337	0
19	14	Exeis ginei devoiced apo ton/tin %s (%s) sto %s	31337	0
19	15	O/H %s den einai opped sto %s	31337	0
19	16	O/H %s den einai voiced sto %s	31337	0
19	17	ANAGNWRISI ANEPITIXHS san %s (Lathos Kwdikos)	31337	0
19	18	Den eimai s`ekeino to kanali!	31337	0
19	19	Avasimo banlevel range. Vasiko range einai apo 1-%i.	31337	0
19	20	Avasimi diarkeia gia ban. H diarkeia tou ban den mporei na einai pano apo to anwtato orio twn %d hours.	31337	0
19	21	O logos tou Ban den mporei na Xeperasei tous 128 charaktires	31337	0
19	22	To Sikgekrimeno ban einai eidi stin lista twn bans mou!	31337	0
19	23	To ban %s einai eidi kalimeno apo ton/tin %s	31337	0
19	24	Avasimo katwtato orio.	31337	0
19	25	Avasimo anwtato orio.	31337	0
19	26	Xristis: %s ACCESS: %s %s	31337	0
19	27	Kanali: %s -- AUTOMODE: %s	31337	0
19	28	TELEUTAIO MODIFIED: %s (%s prin)	31337	0
19	29	** SUSPENDED ** - Ligei se %s (Orio %i)	31337	0
19	30	TELEUTAIA SINANTISH: %s prin.	31337	0
19	31	Iparxoun pio polles apo %d teriastes Eisodous i Simetoxes.	31337	0
19	32	Parakalw perioriste to erotima sas.	31337	0
19	33	Telos tis listas twn access	31337	0
19	34	Den eiparxei Teriasma!	31337	0
19	35	Den mporeis na kaneis add ena xristi me isa i psilotera access apo ta dika sou.	31337	0
19	36	Avasimo orio access.	31337	0
19	37	O/H %s einai eidi added sto %s me orio access %i.	31337	0
19	38	Egine added o xristis %s sto %s me orio access %i	31337	0
19	39	Kati pige lathos: %s	31337	0
19	40	%s: Telos tis listas twn bans	31337	0
19	41	Den mporeis na deis tis leptomeries tou xristi (Mi orato)	31337	0
19	42	Plirofories gia ton/tin: %s (%i)	31337	0
19	43	Currently logged on via: %s	31337	0
19	44	URL: %s	31337	0
19	45	Glwssa: %i	31337	0
19	46	Kanalia: %s	31337	0
19	47	Enswmatomenoi deixtes gia Flood : %i	31337	0
19	48	Paragwgi Flood (Bytes): %i	31337	0
19	49	To %s einai registered apo:	31337	0
19	50	%s - teleutaia sinantisi : %s prin	31337	0
19	51	Desc: %s	31337	0
19	52	Me kaneis Flood oste? Den prokeitai na eipakouw se sena pia	31337	0
19	53	Nomizo oti sou esteila ligo polla dedomena, Tha se kano ignore gia ligo.	31337	0
19	54	Asimplirwti entoli	31337	0
19	55	Gia na xrisimopoieiseis %s, prepei na kaneis /msg %s@%s	31337	0
19	56	Signwmi, Prepei na eisai logged ston X gia na xrisimopoieiseis autin tin entoli.	31337	0
19	57	To kanali %s exei ginei suspend apo ena cservice administrator.	31337	0
19	58	Ta access sou sto %s exoun ginei suspend.	31337	0
19	59	H  simaia NOOP exei tethei sto %s	31337	0
19	60	H simaia STRICTOP exei tethei sto %s	31337	0
19	61	Exeis aplos kanei deop pio polla apo %i atoma	31337	0
19	62	SYNTAX: %s	31337	0
19	63	Proswrina exoun auxithei ta access sou sto kanali %s se %i	31337	0
19	64	To %s einai register.	31337	0
19	65	To %s den einai register.	31337	0
19	66	Den nomizo oti O/H %s tha to ektimouse auto.	31337	0
19	67	\002*** Lista twn Bans gia to kanali %s ***\002	31337	0
19	68	%s %s Orio: %i	31337	0
19	69	EXEI GINEI ADD APO: %s (%s)	31337	0
19	70	APO: %s	31337	0
19	71	LIGEI: %s	31337	0
19	72	\002*** TELOS ***\002	31337	0
19	73	Den xero O/H %s pios einai.	31337	0
19	74	Den eisai exousiodoteimenos me emena pia.	31337	0
19	75	O/H %s den fenetai na exei access sto %s.	31337	0
19	76	Den mporeis na kaneis modinfo se ena xristi me isa i psilotera access apo ta dika sou.	31337	0
19	77	Den mporeis na doseis se ena xristi psilotera i isa access me ta dika sou.	31337	0
19	78	Exei ginei modinfo to orio twn access tou xristi %s's sto kanali %s se %i	31337	0
19	79	Exei tethei AUTOMODE gia OP gia ton/tin %s sto kanali %s	31337	0
19	80	Exei tethei AUTOMODE gia VOICE gia ton/tin %s sto kanali %s	31337	0
19	81	Exei tethei AUTOMODE se NONE gia ton/tin %s sto kanali %s	31337	0
19	82	H frasi tou pass den mporei na einai to username i to en xrisi nickname - H sintaxi einai: NEWPASS <nea frasi-pass>	31337	0
19	83	O kwdikos exei epitiximena allaxei.	31337	0
19	84	H simaia NOOP exei tethei sto %s	31337	0
19	85	H simaia STRICTOP exei tethei sto %s (kai O/H %s den einai anagnwrismenos/anagnwrismeni)	31337	0
19	86	H simaia STRICTOP exei tethei sto %s (kai O/H %s den exei arketa access)	31337	0
19	87	To kanali %s exei ginei purge	31337	0
19	88	To %s einai idi register me emena.	31337	0
19	89	Avasimo onoma kanaliou.	31337	0
19	90	To kanali %s exei ginei register	31337	0
19	91	Aferethike O/H %s apo tin lista siopis mou	31337	0
19	92	Den mporo na vro ton/tin %s stin lista siopis mou	31337	0
19	93	Den mporeis na afereseis ena xristi me psilotera i isa access apo auta pou exeis	31337	0
19	94	Den mporeis na afereseis ton eauto sou apo ena kanali tou opiou eisai katoxos	31337	0
19	95	Aferethike o xristis %s apo to %s	31337	0
19	96	H rithmisi INVISIBLE einai twra ON.	31337	0
19	97	H rithmisi INVISIBLE einai twra OFF.	31337	0
19	98	%s gia %s einai %s	31337	0
19	99	H axia tou %s prepei na einai ON i OFF	31337	0
19	100	Avasimi rithmisi gia USERFLAGS. Oi swstes axies einai NONE, OP, VOICE.	31337	0
19	101	USERFLAGS gia to %s einai %s	31337	0
19	102	H axia tou MASSDEOPPRO prepei na einai apo 0-7	31337	0
19	103	MASSDEOPPRO gia to %s exei tethei se %d	31337	0
19	104	H axia tou FLOODPRO prepei na einai apo 0-7	31337	0
19	105	FLOODPRO gia to %s exei tethei se %d	31337	0
19	106	To DESCRIPTION den mporei na ipervenei tous 80 xaraktires!	31337	0
19	107	To DESCRIPTION gia to %s exei katharistei.	31337	0
19	108	To DESCRIPTION gia to %s einai: %s	31337	0
19	109	To URL den mporei na ipervenei tous 75 xaraktires!	31337	0
19	110	To URL gia to  %s exei katharistei.	31337	0
19	111	To URL gia to %s einai: %s	31337	0
19	112	To nima twn keywords den mporei na ipervei tous 80 xaraktires!	31337	0
19	113	Ta KEYWORDS gia to %s einai: %s	31337	0
19	114	H glwssa exei tethei se %s.	31337	0
19	115	BLABH: Avasimi epilogi glwssas.	31337	0
19	116	Den mporei na brethei to %s sto network!	31337	0
19	117	BLABH: Avasimi rithmisi gia to kanali.	31337	0
19	118	lista gia Ignore:	31337	0
19	119	H lista gia Ignore einai adeia	31337	0
19	120	-- Telos tis Listas twn Ignore --	31337	0
19	121	CMaster Channel Services internal status:	31337	0
19	122	[     Channel Record Stats] \002Cached Entries:\002 %i    \002DB Requests:\002 %i    \002Cache Hits:\002 %i    \002Efficiency:\002 %.2f%%	31337	0
19	123	[        User Record Stats] \002Cached Entries:\002 %i    \002DB Requests:\002 %i    \002Cache Hits:\002 %i    \002Efficiency:\002 %.2f%%	31337	0
19	124	[Access Level Record Stats] \002Cached Entries:\002 %i    \002DB Requests:\002 %i    \002Cache Hits:\002 %i    \002Efficiency:\002 %.2f%%	31337	0
19	125	[         Ban Record Stats] \002Cached Entries:\002 %i    \002DB Requests:\002 %i    \002Cache Hits:\002 %i    \002Efficiency:\002 %.2f%%	31337	0
19	126	Last received User NOTIFY: %i	31337	0
19	127	Last received Channel NOTIFY: %i	31337	0
19	128	Last received Level NOTIFY: %i	31337	0
19	129	Last received Ban NOTIFY: %i	31337	0
19	130	Custom data containers allocated: %i	31337	0
19	131	\002Uptime:\002 %s	31337	0
19	132	To kanali %s exei %d xristes (%i operators)	31337	0
19	133	To Mode einai: %s	31337	0
19	134	Simaies pou exoun tethei: %s	31337	0
19	135	Den mporeis na kaneis suspend ena xristi me isa i psilotera access apo ta dika sou.	31337	0
19	136	Aniparktes monades xronou	31337	0
19	137	Avasimi diarkeia gia suspend.	31337	0
19	138	SUSPENSION gia ton/tin %s exei akirwthei	31337	0
19	139	O/H %s einai eidi suspended sto %s	31337	0
19	140	SUSPENSION gia ton/tin %s tha lixei se %s	31337	0
19	141	BLABH: To topic den mporei na ipervei tous 145 xaraktires	31337	0
19	142	Den exeis  arketa access gia na afereseis to ban %s apo tin vasi dedomenwn tou %s's	31337	0
19	143	Aferethikan %i bans pou teriazan sto %s	31337	0
19	144	Aferethikan ta prosorina sou access %i apo to %s	31337	0
19	145	Den fenetai na exeis forced access sto %s, Mipos eleixan?	31337	0
19	146	O/H %s den einai suspended sto %s	31337	0
19	147	O %s einai IRC operator	31337	0
19	148	O/H %s DEN einai logged in.	31337	0
19	149	O/H %s einai logged in san %s%s	31337	0
19	150	%s einai Official CService Representative%s and logged in as %s	31337	0
19	151	%s einai Official CService Administrator%s and logged in as %s	31337	0
19	152	%s einai Official CService Developer%s and logged in as %s	31337	0
19	153	Iparxoun parapano apo %i kataxwriseis [%s]	31337	0
19	154	Parakalw perioriste tin anazitisi sas	31337	0
19	155	Den iparxoun kataxwriseis gia [%s]	31337	0
19	156	%s: Ekkatharisi twn channel modes.	31337	0
19	158	Avasimi epilogi.	31337	0
19	159	%s einai Official Undernet Service Bot.	31337	0
19	160	%s einai Official Coder-Com Representative%s and logged in as %s	31337	0
19	161	%s einai Official Coder-Com Contributer%s and logged in as %s	31337	0
19	162	%s einai Official Coder-Com Developer%s and logged in as %s	31337	0
19	163	%s einai Official Coder-Com Senior%s and logged in as %s	31337	0
19	164	kai IRC operator	31337	0
19	165	Prostethike ban %s sto %s se level %i	31337	0
19	166	%s: I lista twn ban einai adeia.	31337	0
19	167	Eimai idi se auto to kanali!	31337	0
19	168	Auti i entoli einai periorismeni mono gia IRC Operators	31337	0
19	169	Den eimai opped sto %s	31337	0
19	170	%s gia %i lepta	31337	0
19	9998	Na thimase: Kanenas apo tin CService den tha sou zitisei na tou pis to password sou, MIN to doseis se kanena pou ipostirizei oti einai apo tin CService.	31337	0
\.

