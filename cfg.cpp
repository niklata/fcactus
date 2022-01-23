#line 1 "cfg.rl"
#include "fcactus.hpp"
#include <cstring>
#include <climits>
#include <cassert>
#include <nk/string_replace_all.hpp>
#include <nk/from_string.hpp>
#include <nk/scopeguard.hpp>
extern "C" {
#include "nk/privs.h"
}

#define MAX_LINE 2048

struct ParseCfgState {
	ParseCfgState(inotify &inot) : wm(nullptr), inw(inot),
	intv_st(nullptr), intv2_st(nullptr), strv_st(nullptr),
	v_strlen(0), linenum(0), v_int(0), v_int2(0), cs(0), cmdret(0),
	intv2_exist(false)
	{
		create_job();
		memset(v_str, 0, sizeof v_str);
	}
	
	char v_str[1024];
	
	std::unique_ptr<watch_meta> wm;
	
	inotify &inw;
	
	const char *intv_st;
	const char *intv2_st;
	const char *strv_st;
	
	size_t v_strlen;
	size_t linenum;
	
	int v_int;
	int v_int2;
	
	int cs;
	int cmdret;
	
	bool intv2_exist;
	
	void create_job()
	{
		assert(!wm);
		wm = std::make_unique<watch_meta>();
		cmdret = 0;
	}
	
	void finish_job()
	{
		if (!wm)
			return;
		if (!wm->valid()) {
			log_line("ignoring invalid job at line %zu", linenum);
			wm.reset();
			return;
		}
		inw.add(std::move(wm));
		wm.reset();
	}
	
	void setgroupv()
	{
		if (nk_gidbyname(v_str, &wm->group_)) {
			log_line("%s: nonexistent group specified at line %zu", __func__, linenum);
			std::exit(EXIT_FAILURE);
		}
	}
	
	void setuserv()
	{
		if (nk_uidgidbyname(v_str, &wm->user_, &wm->group_)) {
			log_line("%s: nonexistent user specified at line %zu", __func__, linenum);
			std::exit(EXIT_FAILURE);
		}
	}
	
	void setlim(int type)
	{
		struct rlimit rli;
		rli.rlim_cur = v_int == 0 ? RLIM_INFINITY : v_int;
		rli.rlim_max = v_int2 == 0 ? RLIM_INFINITY : v_int2;
		wm->limits_.add(type, rli);
	}
	
};

struct pckm {
	pckm() : st(nullptr), cs(0) {}
	char *st;
	int cs;
};


#line 114 "cfg.rl"



#line 104 "cfg.cpp"
static const int parse_cmd_key_m_start = 1;
static const int parse_cmd_key_m_first_final = 2;
static const int parse_cmd_key_m_error = 0;

static const int parse_cmd_key_m_en_main = 1;


#line 116 "cfg.rl"


// cmdret = 0: Not parsed a command key yet.
// cmdret = 1: Success.  Got a command key.
// cmdret = -1: Error: malformed command key.
// cmdret = -2: Error: incomplete command key.
// cmdret = -3: Error: duplicate command key.
static void parse_command_key(ParseCfgState &fas)
{
	char *p = fas.v_str;
	const char *pe = fas.v_str + fas.v_strlen;
	const char *eof = pe;
	
	struct pckm pckm;
	
	if (fas.cmdret != 0) {
		fas.cmdret = -3;
		log_line("Duplicate 'command' value at line %zu", fas.linenum);
		std::exit(EXIT_FAILURE);
	}
	
	
#line 135 "cfg.cpp"
	{
		pckm.cs = (int)parse_cmd_key_m_start;
	}
	
#line 137 "cfg.rl"
	
	
#line 143 "cfg.cpp"
	{
		switch ( pckm.cs ) {
			case 1:
			goto st_case_1;
			case 2:
			goto st_case_2;
			case 0:
			goto st_case_0;
			case 3:
			goto st_case_3;
			case 4:
			goto st_case_4;
			case 5:
			goto st_case_5;
			case 6:
			goto st_case_6;
			case 7:
			goto st_case_7;
			case 8:
			goto st_case_8;
		}
		_st1:
		if ( p == eof )
			goto _out1;
		p+= 1;
		st_case_1:
		if ( p == pe && p != eof )
			goto _out1;
		if ( p == eof ) {
			goto _st1;}
		else {
			switch( ( (*( p))) ) {
				case 0: {
					goto _st0;
				}
				case 9: {
					goto _st1;
				}
				case 32: {
					goto _st1;
				}
				case 92: {
					goto _ctr3;
				}
			}
			goto _ctr2;
		}
		_ctr2:
		{
#line 101 "cfg.rl"
			pckm.st = p; }
		
#line 196 "cfg.cpp"
		
		goto _st2;
		_ctr4:
		{
#line 102 "cfg.rl"
			
			fas.wm->cmd_ = std::string(pckm.st, p - pckm.st);
			string_replace_all(fas.wm->cmd_, "\\ ", 2, " ");
			string_replace_all(fas.wm->cmd_, "\\\\", 2, "\\");
		}
		
#line 208 "cfg.cpp"
		
		goto _st2;
		_st2:
		if ( p == eof )
			goto _out2;
		p+= 1;
		st_case_2:
		if ( p == pe && p != eof )
			goto _out2;
		if ( p == eof ) {
			goto _ctr4;}
		else {
			switch( ( (*( p))) ) {
				case 0: {
					goto _st0;
				}
				case 9: {
					goto _ctr6;
				}
				case 32: {
					goto _ctr6;
				}
				case 92: {
					goto _st5;
				}
			}
			goto _st2;
		}
		_st0:
		if ( p == eof )
			goto _out0;
		st_case_0:
		goto _out0;
		_ctr10:
		{
#line 101 "cfg.rl"
			pckm.st = p; }
		
#line 247 "cfg.cpp"
		
		goto _st3;
		_ctr6:
		{
#line 102 "cfg.rl"
			
			fas.wm->cmd_ = std::string(pckm.st, p - pckm.st);
			string_replace_all(fas.wm->cmd_, "\\ ", 2, " ");
			string_replace_all(fas.wm->cmd_, "\\\\", 2, "\\");
		}
		
#line 259 "cfg.cpp"
		
		goto _st3;
		_ctr8:
		{
#line 101 "cfg.rl"
			pckm.st = p; }
		
#line 267 "cfg.cpp"
		
		{
#line 107 "cfg.rl"
			fas.wm->args_ = std::string(pckm.st, p - pckm.st); }
		
#line 273 "cfg.cpp"
		
		goto _st3;
		_ctr17:
		{
#line 102 "cfg.rl"
			
			fas.wm->cmd_ = std::string(pckm.st, p - pckm.st);
			string_replace_all(fas.wm->cmd_, "\\ ", 2, " ");
			string_replace_all(fas.wm->cmd_, "\\\\", 2, "\\");
		}
		
#line 285 "cfg.cpp"
		
		{
#line 101 "cfg.rl"
			pckm.st = p; }
		
#line 291 "cfg.cpp"
		
		goto _st3;
		_st3:
		if ( p == eof )
			goto _out3;
		p+= 1;
		st_case_3:
		if ( p == pe && p != eof )
			goto _out3;
		if ( p == eof ) {
			goto _ctr8;}
		else {
			switch( ( (*( p))) ) {
				case 0: {
					goto _st0;
				}
				case 9: {
					goto _ctr10;
				}
				case 32: {
					goto _ctr10;
				}
			}
			goto _ctr9;
		}
		_ctr9:
		{
#line 101 "cfg.rl"
			pckm.st = p; }
		
#line 322 "cfg.cpp"
		
		goto _st4;
		_ctr11:
		{
#line 107 "cfg.rl"
			fas.wm->args_ = std::string(pckm.st, p - pckm.st); }
		
#line 330 "cfg.cpp"
		
		goto _st4;
		_st4:
		if ( p == eof )
			goto _out4;
		p+= 1;
		st_case_4:
		if ( p == pe && p != eof )
			goto _out4;
		if ( p == eof ) {
			goto _ctr11;}
		else {
			if ( ( (*( p))) == 0 ) {
				goto _st0;
			}
			goto _st4;
		}
		_ctr3:
		{
#line 101 "cfg.rl"
			pckm.st = p; }
		
#line 353 "cfg.cpp"
		
		goto _st5;
		_ctr13:
		{
#line 102 "cfg.rl"
			
			fas.wm->cmd_ = std::string(pckm.st, p - pckm.st);
			string_replace_all(fas.wm->cmd_, "\\ ", 2, " ");
			string_replace_all(fas.wm->cmd_, "\\\\", 2, "\\");
		}
		
#line 365 "cfg.cpp"
		
		goto _st5;
		_st5:
		if ( p == eof )
			goto _out5;
		p+= 1;
		st_case_5:
		if ( p == pe && p != eof )
			goto _out5;
		if ( p == eof ) {
			goto _ctr13;}
		else {
			switch( ( (*( p))) ) {
				case 0: {
					goto _st0;
				}
				case 9: {
					goto _ctr6;
				}
				case 32: {
					goto _ctr14;
				}
				case 92: {
					goto _st5;
				}
			}
			goto _st2;
		}
		_ctr14:
		{
#line 102 "cfg.rl"
			
			fas.wm->cmd_ = std::string(pckm.st, p - pckm.st);
			string_replace_all(fas.wm->cmd_, "\\ ", 2, " ");
			string_replace_all(fas.wm->cmd_, "\\\\", 2, "\\");
		}
		
#line 403 "cfg.cpp"
		
		goto _st6;
		_ctr15:
		{
#line 102 "cfg.rl"
			
			fas.wm->cmd_ = std::string(pckm.st, p - pckm.st);
			string_replace_all(fas.wm->cmd_, "\\ ", 2, " ");
			string_replace_all(fas.wm->cmd_, "\\\\", 2, "\\");
		}
		
#line 415 "cfg.cpp"
		
		{
#line 101 "cfg.rl"
			pckm.st = p; }
		
#line 421 "cfg.cpp"
		
		{
#line 107 "cfg.rl"
			fas.wm->args_ = std::string(pckm.st, p - pckm.st); }
		
#line 427 "cfg.cpp"
		
		goto _st6;
		_st6:
		if ( p == eof )
			goto _out6;
		p+= 1;
		st_case_6:
		if ( p == pe && p != eof )
			goto _out6;
		if ( p == eof ) {
			goto _ctr15;}
		else {
			switch( ( (*( p))) ) {
				case 0: {
					goto _st0;
				}
				case 9: {
					goto _ctr17;
				}
				case 32: {
					goto _ctr17;
				}
				case 92: {
					goto _ctr18;
				}
			}
			goto _ctr16;
		}
		_ctr16:
		{
#line 101 "cfg.rl"
			pckm.st = p; }
		
#line 461 "cfg.cpp"
		
		goto _st7;
		_ctr19:
		{
#line 102 "cfg.rl"
			
			fas.wm->cmd_ = std::string(pckm.st, p - pckm.st);
			string_replace_all(fas.wm->cmd_, "\\ ", 2, " ");
			string_replace_all(fas.wm->cmd_, "\\\\", 2, "\\");
		}
		
#line 473 "cfg.cpp"
		
		{
#line 107 "cfg.rl"
			fas.wm->args_ = std::string(pckm.st, p - pckm.st); }
		
#line 479 "cfg.cpp"
		
		goto _st7;
		_st7:
		if ( p == eof )
			goto _out7;
		p+= 1;
		st_case_7:
		if ( p == pe && p != eof )
			goto _out7;
		if ( p == eof ) {
			goto _ctr19;}
		else {
			switch( ( (*( p))) ) {
				case 0: {
					goto _st0;
				}
				case 9: {
					goto _ctr6;
				}
				case 32: {
					goto _ctr6;
				}
				case 92: {
					goto _st8;
				}
			}
			goto _st7;
		}
		_ctr18:
		{
#line 101 "cfg.rl"
			pckm.st = p; }
		
#line 513 "cfg.cpp"
		
		goto _st8;
		_ctr22:
		{
#line 102 "cfg.rl"
			
			fas.wm->cmd_ = std::string(pckm.st, p - pckm.st);
			string_replace_all(fas.wm->cmd_, "\\ ", 2, " ");
			string_replace_all(fas.wm->cmd_, "\\\\", 2, "\\");
		}
		
#line 525 "cfg.cpp"
		
		{
#line 107 "cfg.rl"
			fas.wm->args_ = std::string(pckm.st, p - pckm.st); }
		
#line 531 "cfg.cpp"
		
		goto _st8;
		_st8:
		if ( p == eof )
			goto _out8;
		p+= 1;
		st_case_8:
		if ( p == pe && p != eof )
			goto _out8;
		if ( p == eof ) {
			goto _ctr22;}
		else {
			switch( ( (*( p))) ) {
				case 0: {
					goto _st0;
				}
				case 9: {
					goto _ctr6;
				}
				case 32: {
					goto _ctr14;
				}
				case 92: {
					goto _st8;
				}
			}
			goto _st7;
		}
		_out1: pckm.cs = 1; goto _out; 
		_out2: pckm.cs = 2; goto _out; 
		_out0: pckm.cs = 0; goto _out; 
		_out3: pckm.cs = 3; goto _out; 
		_out4: pckm.cs = 4; goto _out; 
		_out5: pckm.cs = 5; goto _out; 
		_out6: pckm.cs = 6; goto _out; 
		_out7: pckm.cs = 7; goto _out; 
		_out8: pckm.cs = 8; goto _out; 
		_out: {}
	}
	
#line 138 "cfg.rl"
	
	
	if (pckm.cs == parse_cmd_key_m_error) {
		fas.cmdret = -1;
		log_line("Malformed 'command' value at line %zu", fas.linenum);
		std::exit(EXIT_FAILURE);
	} else if (pckm.cs >= parse_cmd_key_m_first_final)
	fas.cmdret = 1;
	else {
		fas.cmdret = -2;
		log_line("Incomplete 'command' value at line %zu", fas.linenum);
		std::exit(EXIT_FAILURE);
	}
}


#line 298 "cfg.rl"



#line 593 "cfg.cpp"
static const int factions_start = 1;
static const int factions_first_final = 212;
static const int factions_error = 0;

static const int factions_en_main = 1;


#line 300 "cfg.rl"


static int do_parse_config(ParseCfgState &fas, const char *p, size_t plen)
{
	const char *pe = p + plen;
	const char *eof = pe;
	
	
#line 610 "cfg.cpp"
	{
		fas.cs = (int)factions_start;
	}
	
#line 307 "cfg.rl"
	
	
#line 618 "cfg.cpp"
	{
		switch ( fas.cs ) {
			case 1:
			goto st_case_1;
			case 0:
			goto st_case_0;
			case 2:
			goto st_case_2;
			case 212:
			goto st_case_212;
			case 3:
			goto st_case_3;
			case 4:
			goto st_case_4;
			case 5:
			goto st_case_5;
			case 6:
			goto st_case_6;
			case 7:
			goto st_case_7;
			case 8:
			goto st_case_8;
			case 9:
			goto st_case_9;
			case 213:
			goto st_case_213;
			case 214:
			goto st_case_214;
			case 10:
			goto st_case_10;
			case 11:
			goto st_case_11;
			case 12:
			goto st_case_12;
			case 13:
			goto st_case_13;
			case 14:
			goto st_case_14;
			case 15:
			goto st_case_15;
			case 16:
			goto st_case_16;
			case 215:
			goto st_case_215;
			case 216:
			goto st_case_216;
			case 17:
			goto st_case_17;
			case 18:
			goto st_case_18;
			case 19:
			goto st_case_19;
			case 20:
			goto st_case_20;
			case 21:
			goto st_case_21;
			case 22:
			goto st_case_22;
			case 23:
			goto st_case_23;
			case 24:
			goto st_case_24;
			case 25:
			goto st_case_25;
			case 26:
			goto st_case_26;
			case 27:
			goto st_case_27;
			case 28:
			goto st_case_28;
			case 29:
			goto st_case_29;
			case 30:
			goto st_case_30;
			case 217:
			goto st_case_217;
			case 31:
			goto st_case_31;
			case 32:
			goto st_case_32;
			case 33:
			goto st_case_33;
			case 34:
			goto st_case_34;
			case 35:
			goto st_case_35;
			case 218:
			goto st_case_218;
			case 36:
			goto st_case_36;
			case 37:
			goto st_case_37;
			case 38:
			goto st_case_38;
			case 39:
			goto st_case_39;
			case 40:
			goto st_case_40;
			case 41:
			goto st_case_41;
			case 219:
			goto st_case_219;
			case 42:
			goto st_case_42;
			case 43:
			goto st_case_43;
			case 44:
			goto st_case_44;
			case 45:
			goto st_case_45;
			case 220:
			goto st_case_220;
			case 46:
			goto st_case_46;
			case 47:
			goto st_case_47;
			case 48:
			goto st_case_48;
			case 49:
			goto st_case_49;
			case 221:
			goto st_case_221;
			case 50:
			goto st_case_50;
			case 51:
			goto st_case_51;
			case 52:
			goto st_case_52;
			case 53:
			goto st_case_53;
			case 54:
			goto st_case_54;
			case 55:
			goto st_case_55;
			case 222:
			goto st_case_222;
			case 56:
			goto st_case_56;
			case 57:
			goto st_case_57;
			case 58:
			goto st_case_58;
			case 59:
			goto st_case_59;
			case 223:
			goto st_case_223;
			case 60:
			goto st_case_60;
			case 61:
			goto st_case_61;
			case 62:
			goto st_case_62;
			case 63:
			goto st_case_63;
			case 224:
			goto st_case_224;
			case 64:
			goto st_case_64;
			case 65:
			goto st_case_65;
			case 66:
			goto st_case_66;
			case 67:
			goto st_case_67;
			case 68:
			goto st_case_68;
			case 225:
			goto st_case_225;
			case 69:
			goto st_case_69;
			case 70:
			goto st_case_70;
			case 71:
			goto st_case_71;
			case 226:
			goto st_case_226;
			case 72:
			goto st_case_72;
			case 73:
			goto st_case_73;
			case 74:
			goto st_case_74;
			case 75:
			goto st_case_75;
			case 76:
			goto st_case_76;
			case 227:
			goto st_case_227;
			case 77:
			goto st_case_77;
			case 228:
			goto st_case_228;
			case 78:
			goto st_case_78;
			case 79:
			goto st_case_79;
			case 80:
			goto st_case_80;
			case 229:
			goto st_case_229;
			case 81:
			goto st_case_81;
			case 82:
			goto st_case_82;
			case 83:
			goto st_case_83;
			case 230:
			goto st_case_230;
			case 84:
			goto st_case_84;
			case 231:
			goto st_case_231;
			case 85:
			goto st_case_85;
			case 86:
			goto st_case_86;
			case 87:
			goto st_case_87;
			case 232:
			goto st_case_232;
			case 88:
			goto st_case_88;
			case 89:
			goto st_case_89;
			case 90:
			goto st_case_90;
			case 91:
			goto st_case_91;
			case 92:
			goto st_case_92;
			case 93:
			goto st_case_93;
			case 233:
			goto st_case_233;
			case 234:
			goto st_case_234;
			case 94:
			goto st_case_94;
			case 95:
			goto st_case_95;
			case 96:
			goto st_case_96;
			case 97:
			goto st_case_97;
			case 98:
			goto st_case_98;
			case 235:
			goto st_case_235;
			case 99:
			goto st_case_99;
			case 236:
			goto st_case_236;
			case 100:
			goto st_case_100;
			case 101:
			goto st_case_101;
			case 102:
			goto st_case_102;
			case 103:
			goto st_case_103;
			case 104:
			goto st_case_104;
			case 237:
			goto st_case_237;
			case 105:
			goto st_case_105;
			case 238:
			goto st_case_238;
			case 106:
			goto st_case_106;
			case 107:
			goto st_case_107;
			case 108:
			goto st_case_108;
			case 239:
			goto st_case_239;
			case 109:
			goto st_case_109;
			case 240:
			goto st_case_240;
			case 110:
			goto st_case_110;
			case 111:
			goto st_case_111;
			case 112:
			goto st_case_112;
			case 113:
			goto st_case_113;
			case 114:
			goto st_case_114;
			case 241:
			goto st_case_241;
			case 115:
			goto st_case_115;
			case 242:
			goto st_case_242;
			case 116:
			goto st_case_116;
			case 117:
			goto st_case_117;
			case 118:
			goto st_case_118;
			case 119:
			goto st_case_119;
			case 120:
			goto st_case_120;
			case 121:
			goto st_case_121;
			case 243:
			goto st_case_243;
			case 122:
			goto st_case_122;
			case 244:
			goto st_case_244;
			case 123:
			goto st_case_123;
			case 124:
			goto st_case_124;
			case 125:
			goto st_case_125;
			case 126:
			goto st_case_126;
			case 127:
			goto st_case_127;
			case 128:
			goto st_case_128;
			case 129:
			goto st_case_129;
			case 130:
			goto st_case_130;
			case 245:
			goto st_case_245;
			case 131:
			goto st_case_131;
			case 246:
			goto st_case_246;
			case 132:
			goto st_case_132;
			case 133:
			goto st_case_133;
			case 134:
			goto st_case_134;
			case 135:
			goto st_case_135;
			case 136:
			goto st_case_136;
			case 137:
			goto st_case_137;
			case 138:
			goto st_case_138;
			case 139:
			goto st_case_139;
			case 247:
			goto st_case_247;
			case 140:
			goto st_case_140;
			case 248:
			goto st_case_248;
			case 141:
			goto st_case_141;
			case 142:
			goto st_case_142;
			case 143:
			goto st_case_143;
			case 144:
			goto st_case_144;
			case 145:
			goto st_case_145;
			case 249:
			goto st_case_249;
			case 146:
			goto st_case_146;
			case 250:
			goto st_case_250;
			case 147:
			goto st_case_147;
			case 148:
			goto st_case_148;
			case 149:
			goto st_case_149;
			case 150:
			goto st_case_150;
			case 151:
			goto st_case_151;
			case 152:
			goto st_case_152;
			case 251:
			goto st_case_251;
			case 153:
			goto st_case_153;
			case 252:
			goto st_case_252;
			case 154:
			goto st_case_154;
			case 155:
			goto st_case_155;
			case 156:
			goto st_case_156;
			case 157:
			goto st_case_157;
			case 158:
			goto st_case_158;
			case 253:
			goto st_case_253;
			case 159:
			goto st_case_159;
			case 254:
			goto st_case_254;
			case 160:
			goto st_case_160;
			case 161:
			goto st_case_161;
			case 162:
			goto st_case_162;
			case 163:
			goto st_case_163;
			case 255:
			goto st_case_255;
			case 164:
			goto st_case_164;
			case 256:
			goto st_case_256;
			case 165:
			goto st_case_165;
			case 166:
			goto st_case_166;
			case 167:
			goto st_case_167;
			case 168:
			goto st_case_168;
			case 169:
			goto st_case_169;
			case 170:
			goto st_case_170;
			case 257:
			goto st_case_257;
			case 171:
			goto st_case_171;
			case 258:
			goto st_case_258;
			case 172:
			goto st_case_172;
			case 173:
			goto st_case_173;
			case 174:
			goto st_case_174;
			case 175:
			goto st_case_175;
			case 176:
			goto st_case_176;
			case 259:
			goto st_case_259;
			case 177:
			goto st_case_177;
			case 260:
			goto st_case_260;
			case 178:
			goto st_case_178;
			case 179:
			goto st_case_179;
			case 180:
			goto st_case_180;
			case 181:
			goto st_case_181;
			case 182:
			goto st_case_182;
			case 183:
			goto st_case_183;
			case 184:
			goto st_case_184;
			case 185:
			goto st_case_185;
			case 186:
			goto st_case_186;
			case 187:
			goto st_case_187;
			case 188:
			goto st_case_188;
			case 261:
			goto st_case_261;
			case 189:
			goto st_case_189;
			case 262:
			goto st_case_262;
			case 190:
			goto st_case_190;
			case 191:
			goto st_case_191;
			case 192:
			goto st_case_192;
			case 193:
			goto st_case_193;
			case 194:
			goto st_case_194;
			case 263:
			goto st_case_263;
			case 195:
			goto st_case_195;
			case 264:
			goto st_case_264;
			case 196:
			goto st_case_196;
			case 197:
			goto st_case_197;
			case 198:
			goto st_case_198;
			case 199:
			goto st_case_199;
			case 200:
			goto st_case_200;
			case 265:
			goto st_case_265;
			case 266:
			goto st_case_266;
			case 201:
			goto st_case_201;
			case 202:
			goto st_case_202;
			case 203:
			goto st_case_203;
			case 204:
			goto st_case_204;
			case 205:
			goto st_case_205;
			case 267:
			goto st_case_267;
			case 268:
			goto st_case_268;
			case 206:
			goto st_case_206;
			case 207:
			goto st_case_207;
			case 208:
			goto st_case_208;
			case 209:
			goto st_case_209;
			case 210:
			goto st_case_210;
			case 211:
			goto st_case_211;
			case 269:
			goto st_case_269;
			case 270:
			goto st_case_270;
		}
		_st1:
		if ( p == eof )
			goto _out1;
		p+= 1;
		st_case_1:
		if ( p == pe && p != eof )
			goto _out1;
		if ( p == eof ) {
			goto _st1;}
		else {
			switch( ( (*( p))) ) {
				case 59: {
					goto _ctr2;
				}
				case 67: {
					goto _st3;
				}
				case 68: {
					goto _st17;
				}
				case 70: {
					goto _st36;
				}
				case 71: {
					goto _st88;
				}
				case 76: {
					goto _st94;
				}
				case 80: {
					goto _st196;
				}
				case 85: {
					goto _st201;
				}
				case 87: {
					goto _st206;
				}
				case 99: {
					goto _st3;
				}
				case 100: {
					goto _st17;
				}
				case 102: {
					goto _st36;
				}
				case 103: {
					goto _st88;
				}
				case 108: {
					goto _st94;
				}
				case 112: {
					goto _st196;
				}
				case 117: {
					goto _st201;
				}
				case 119: {
					goto _st206;
				}
			}
			goto _st0;
		}
		_st0:
		if ( p == eof )
			goto _out0;
		st_case_0:
		goto _out0;
		_ctr2:
		{
#line 293 "cfg.rl"
			fas.finish_job(); fas.create_job(); }
		
#line 1239 "cfg.cpp"
		
		goto _st2;
		_st2:
		if ( p == eof )
			goto _out2;
		p+= 1;
		st_case_2:
		if ( p == pe && p != eof )
			goto _out2;
		if ( p == eof ) {
			goto _st2;}
		else {
			if ( ( (*( p))) == 59 ) {
				goto _st212;
			}
			goto _st0;
		}
		_st212:
		if ( p == eof )
			goto _out212;
		p+= 1;
		st_case_212:
		if ( p == pe && p != eof )
			goto _out212;
		if ( p == eof ) {
			goto _st212;}
		else {
			goto _st0;
		}
		_st3:
		if ( p == eof )
			goto _out3;
		p+= 1;
		st_case_3:
		if ( p == pe && p != eof )
			goto _out3;
		if ( p == eof ) {
			goto _st3;}
		else {
			switch( ( (*( p))) ) {
				case 72: {
					goto _st4;
				}
				case 79: {
					goto _st10;
				}
				case 104: {
					goto _st4;
				}
				case 111: {
					goto _st10;
				}
			}
			goto _st0;
		}
		_st4:
		if ( p == eof )
			goto _out4;
		p+= 1;
		st_case_4:
		if ( p == pe && p != eof )
			goto _out4;
		if ( p == eof ) {
			goto _st4;}
		else {
			switch( ( (*( p))) ) {
				case 82: {
					goto _st5;
				}
				case 114: {
					goto _st5;
				}
			}
			goto _st0;
		}
		_st5:
		if ( p == eof )
			goto _out5;
		p+= 1;
		st_case_5:
		if ( p == pe && p != eof )
			goto _out5;
		if ( p == eof ) {
			goto _st5;}
		else {
			switch( ( (*( p))) ) {
				case 79: {
					goto _st6;
				}
				case 111: {
					goto _st6;
				}
			}
			goto _st0;
		}
		_st6:
		if ( p == eof )
			goto _out6;
		p+= 1;
		st_case_6:
		if ( p == pe && p != eof )
			goto _out6;
		if ( p == eof ) {
			goto _st6;}
		else {
			switch( ( (*( p))) ) {
				case 79: {
					goto _st7;
				}
				case 111: {
					goto _st7;
				}
			}
			goto _st0;
		}
		_st7:
		if ( p == eof )
			goto _out7;
		p+= 1;
		st_case_7:
		if ( p == pe && p != eof )
			goto _out7;
		if ( p == eof ) {
			goto _st7;}
		else {
			switch( ( (*( p))) ) {
				case 84: {
					goto _st8;
				}
				case 116: {
					goto _st8;
				}
			}
			goto _st0;
		}
		_st8:
		if ( p == eof )
			goto _out8;
		p+= 1;
		st_case_8:
		if ( p == pe && p != eof )
			goto _out8;
		if ( p == eof ) {
			goto _st8;}
		else {
			switch( ( (*( p))) ) {
				case 9: {
					goto _st8;
				}
				case 32: {
					goto _st8;
				}
				case 61: {
					goto _st9;
				}
			}
			goto _st0;
		}
		_st9:
		if ( p == eof )
			goto _out9;
		p+= 1;
		st_case_9:
		if ( p == pe && p != eof )
			goto _out9;
		if ( p == eof ) {
			goto _st9;}
		else {
			switch( ( (*( p))) ) {
				case 0: {
					goto _st0;
				}
				case 9: {
					goto _ctr21;
				}
				case 10: {
					goto _st0;
				}
				case 32: {
					goto _ctr21;
				}
			}
			goto _ctr20;
		}
		_ctr20:
		{
#line 180 "cfg.rl"
			fas.strv_st = p; fas.v_strlen = 0; }
		
#line 1429 "cfg.cpp"
		
		goto _st213;
		_ctr272:
		{
#line 181 "cfg.rl"
			
			fas.v_strlen = p - fas.strv_st;
			if (fas.v_strlen <= INT_MAX) {
				ssize_t snl = snprintf(fas.v_str, sizeof fas.v_str,
				"%.*s", (int)fas.v_strlen, fas.strv_st);
				if (snl < 0 || (size_t)snl >= sizeof fas.v_str) {
					log_line("error parsing line %zu in crontab; too long?", fas.linenum);
					std::exit(EXIT_FAILURE);
				}
			}
		}
		
#line 1447 "cfg.cpp"
		
		{
#line 232 "cfg.rl"
			fas.wm->chroot_ = std::string(fas.v_str, fas.v_strlen); }
		
#line 1453 "cfg.cpp"
		
		goto _st213;
		_st213:
		if ( p == eof )
			goto _out213;
		p+= 1;
		st_case_213:
		if ( p == pe && p != eof )
			goto _out213;
		if ( p == eof ) {
			goto _ctr272;}
		else {
			switch( ( (*( p))) ) {
				case 0: {
					goto _st0;
				}
				case 10: {
					goto _st0;
				}
			}
			goto _st213;
		}
		_ctr21:
		{
#line 180 "cfg.rl"
			fas.strv_st = p; fas.v_strlen = 0; }
		
#line 1481 "cfg.cpp"
		
		goto _st214;
		_ctr274:
		{
#line 181 "cfg.rl"
			
			fas.v_strlen = p - fas.strv_st;
			if (fas.v_strlen <= INT_MAX) {
				ssize_t snl = snprintf(fas.v_str, sizeof fas.v_str,
				"%.*s", (int)fas.v_strlen, fas.strv_st);
				if (snl < 0 || (size_t)snl >= sizeof fas.v_str) {
					log_line("error parsing line %zu in crontab; too long?", fas.linenum);
					std::exit(EXIT_FAILURE);
				}
			}
		}
		
#line 1499 "cfg.cpp"
		
		{
#line 232 "cfg.rl"
			fas.wm->chroot_ = std::string(fas.v_str, fas.v_strlen); }
		
#line 1505 "cfg.cpp"
		
		goto _st214;
		_st214:
		if ( p == eof )
			goto _out214;
		p+= 1;
		st_case_214:
		if ( p == pe && p != eof )
			goto _out214;
		if ( p == eof ) {
			goto _ctr274;}
		else {
			switch( ( (*( p))) ) {
				case 0: {
					goto _st0;
				}
				case 9: {
					goto _ctr21;
				}
				case 10: {
					goto _st0;
				}
				case 32: {
					goto _ctr21;
				}
			}
			goto _ctr20;
		}
		_st10:
		if ( p == eof )
			goto _out10;
		p+= 1;
		st_case_10:
		if ( p == pe && p != eof )
			goto _out10;
		if ( p == eof ) {
			goto _st10;}
		else {
			switch( ( (*( p))) ) {
				case 77: {
					goto _st11;
				}
				case 109: {
					goto _st11;
				}
			}
			goto _st0;
		}
		_st11:
		if ( p == eof )
			goto _out11;
		p+= 1;
		st_case_11:
		if ( p == pe && p != eof )
			goto _out11;
		if ( p == eof ) {
			goto _st11;}
		else {
			switch( ( (*( p))) ) {
				case 77: {
					goto _st12;
				}
				case 109: {
					goto _st12;
				}
			}
			goto _st0;
		}
		_st12:
		if ( p == eof )
			goto _out12;
		p+= 1;
		st_case_12:
		if ( p == pe && p != eof )
			goto _out12;
		if ( p == eof ) {
			goto _st12;}
		else {
			switch( ( (*( p))) ) {
				case 65: {
					goto _st13;
				}
				case 97: {
					goto _st13;
				}
			}
			goto _st0;
		}
		_st13:
		if ( p == eof )
			goto _out13;
		p+= 1;
		st_case_13:
		if ( p == pe && p != eof )
			goto _out13;
		if ( p == eof ) {
			goto _st13;}
		else {
			switch( ( (*( p))) ) {
				case 78: {
					goto _st14;
				}
				case 110: {
					goto _st14;
				}
			}
			goto _st0;
		}
		_st14:
		if ( p == eof )
			goto _out14;
		p+= 1;
		st_case_14:
		if ( p == pe && p != eof )
			goto _out14;
		if ( p == eof ) {
			goto _st14;}
		else {
			switch( ( (*( p))) ) {
				case 68: {
					goto _st15;
				}
				case 100: {
					goto _st15;
				}
			}
			goto _st0;
		}
		_st15:
		if ( p == eof )
			goto _out15;
		p+= 1;
		st_case_15:
		if ( p == pe && p != eof )
			goto _out15;
		if ( p == eof ) {
			goto _st15;}
		else {
			switch( ( (*( p))) ) {
				case 9: {
					goto _st15;
				}
				case 32: {
					goto _st15;
				}
				case 61: {
					goto _st16;
				}
			}
			goto _st0;
		}
		_st16:
		if ( p == eof )
			goto _out16;
		p+= 1;
		st_case_16:
		if ( p == pe && p != eof )
			goto _out16;
		if ( p == eof ) {
			goto _st16;}
		else {
			switch( ( (*( p))) ) {
				case 0: {
					goto _st0;
				}
				case 9: {
					goto _ctr29;
				}
				case 10: {
					goto _st0;
				}
				case 32: {
					goto _ctr29;
				}
			}
			goto _ctr28;
		}
		_ctr28:
		{
#line 180 "cfg.rl"
			fas.strv_st = p; fas.v_strlen = 0; }
		
#line 1688 "cfg.cpp"
		
		goto _st215;
		_ctr275:
		{
#line 181 "cfg.rl"
			
			fas.v_strlen = p - fas.strv_st;
			if (fas.v_strlen <= INT_MAX) {
				ssize_t snl = snprintf(fas.v_str, sizeof fas.v_str,
				"%.*s", (int)fas.v_strlen, fas.strv_st);
				if (snl < 0 || (size_t)snl >= sizeof fas.v_str) {
					log_line("error parsing line %zu in crontab; too long?", fas.linenum);
					std::exit(EXIT_FAILURE);
				}
			}
		}
		
#line 1706 "cfg.cpp"
		
		{
#line 233 "cfg.rl"
			parse_command_key(fas); }
		
#line 1712 "cfg.cpp"
		
		goto _st215;
		_st215:
		if ( p == eof )
			goto _out215;
		p+= 1;
		st_case_215:
		if ( p == pe && p != eof )
			goto _out215;
		if ( p == eof ) {
			goto _ctr275;}
		else {
			switch( ( (*( p))) ) {
				case 0: {
					goto _st0;
				}
				case 10: {
					goto _st0;
				}
			}
			goto _st215;
		}
		_ctr29:
		{
#line 180 "cfg.rl"
			fas.strv_st = p; fas.v_strlen = 0; }
		
#line 1740 "cfg.cpp"
		
		goto _st216;
		_ctr277:
		{
#line 181 "cfg.rl"
			
			fas.v_strlen = p - fas.strv_st;
			if (fas.v_strlen <= INT_MAX) {
				ssize_t snl = snprintf(fas.v_str, sizeof fas.v_str,
				"%.*s", (int)fas.v_strlen, fas.strv_st);
				if (snl < 0 || (size_t)snl >= sizeof fas.v_str) {
					log_line("error parsing line %zu in crontab; too long?", fas.linenum);
					std::exit(EXIT_FAILURE);
				}
			}
		}
		
#line 1758 "cfg.cpp"
		
		{
#line 233 "cfg.rl"
			parse_command_key(fas); }
		
#line 1764 "cfg.cpp"
		
		goto _st216;
		_st216:
		if ( p == eof )
			goto _out216;
		p+= 1;
		st_case_216:
		if ( p == pe && p != eof )
			goto _out216;
		if ( p == eof ) {
			goto _ctr277;}
		else {
			switch( ( (*( p))) ) {
				case 0: {
					goto _st0;
				}
				case 9: {
					goto _ctr29;
				}
				case 10: {
					goto _st0;
				}
				case 32: {
					goto _ctr29;
				}
			}
			goto _ctr28;
		}
		_st17:
		if ( p == eof )
			goto _out17;
		p+= 1;
		st_case_17:
		if ( p == pe && p != eof )
			goto _out17;
		if ( p == eof ) {
			goto _st17;}
		else {
			switch( ( (*( p))) ) {
				case 69: {
					goto _st18;
				}
				case 101: {
					goto _st18;
				}
			}
			goto _st0;
		}
		_st18:
		if ( p == eof )
			goto _out18;
		p+= 1;
		st_case_18:
		if ( p == pe && p != eof )
			goto _out18;
		if ( p == eof ) {
			goto _st18;}
		else {
			switch( ( (*( p))) ) {
				case 66: {
					goto _st19;
				}
				case 98: {
					goto _st19;
				}
			}
			goto _st0;
		}
		_st19:
		if ( p == eof )
			goto _out19;
		p+= 1;
		st_case_19:
		if ( p == pe && p != eof )
			goto _out19;
		if ( p == eof ) {
			goto _st19;}
		else {
			switch( ( (*( p))) ) {
				case 79: {
					goto _st20;
				}
				case 111: {
					goto _st20;
				}
			}
			goto _st0;
		}
		_st20:
		if ( p == eof )
			goto _out20;
		p+= 1;
		st_case_20:
		if ( p == pe && p != eof )
			goto _out20;
		if ( p == eof ) {
			goto _st20;}
		else {
			switch( ( (*( p))) ) {
				case 85: {
					goto _st21;
				}
				case 117: {
					goto _st21;
				}
			}
			goto _st0;
		}
		_st21:
		if ( p == eof )
			goto _out21;
		p+= 1;
		st_case_21:
		if ( p == pe && p != eof )
			goto _out21;
		if ( p == eof ) {
			goto _st21;}
		else {
			switch( ( (*( p))) ) {
				case 78: {
					goto _st22;
				}
				case 110: {
					goto _st22;
				}
			}
			goto _st0;
		}
		_st22:
		if ( p == eof )
			goto _out22;
		p+= 1;
		st_case_22:
		if ( p == pe && p != eof )
			goto _out22;
		if ( p == eof ) {
			goto _st22;}
		else {
			switch( ( (*( p))) ) {
				case 67: {
					goto _st23;
				}
				case 99: {
					goto _st23;
				}
			}
			goto _st0;
		}
		_st23:
		if ( p == eof )
			goto _out23;
		p+= 1;
		st_case_23:
		if ( p == pe && p != eof )
			goto _out23;
		if ( p == eof ) {
			goto _st23;}
		else {
			switch( ( (*( p))) ) {
				case 69: {
					goto _st24;
				}
				case 101: {
					goto _st24;
				}
			}
			goto _st0;
		}
		_st24:
		if ( p == eof )
			goto _out24;
		p+= 1;
		st_case_24:
		if ( p == pe && p != eof )
			goto _out24;
		if ( p == eof ) {
			goto _st24;}
		else {
			if ( ( (*( p))) == 95 ) {
				goto _st25;
			}
			goto _st0;
		}
		_st25:
		if ( p == eof )
			goto _out25;
		p+= 1;
		st_case_25:
		if ( p == pe && p != eof )
			goto _out25;
		if ( p == eof ) {
			goto _st25;}
		else {
			switch( ( (*( p))) ) {
				case 70: {
					goto _st26;
				}
				case 82: {
					goto _st31;
				}
				case 102: {
					goto _st26;
				}
				case 114: {
					goto _st31;
				}
			}
			goto _st0;
		}
		_st26:
		if ( p == eof )
			goto _out26;
		p+= 1;
		st_case_26:
		if ( p == pe && p != eof )
			goto _out26;
		if ( p == eof ) {
			goto _st26;}
		else {
			switch( ( (*( p))) ) {
				case 65: {
					goto _st27;
				}
				case 97: {
					goto _st27;
				}
			}
			goto _st0;
		}
		_st27:
		if ( p == eof )
			goto _out27;
		p+= 1;
		st_case_27:
		if ( p == pe && p != eof )
			goto _out27;
		if ( p == eof ) {
			goto _st27;}
		else {
			switch( ( (*( p))) ) {
				case 76: {
					goto _st28;
				}
				case 108: {
					goto _st28;
				}
			}
			goto _st0;
		}
		_st28:
		if ( p == eof )
			goto _out28;
		p+= 1;
		st_case_28:
		if ( p == pe && p != eof )
			goto _out28;
		if ( p == eof ) {
			goto _st28;}
		else {
			switch( ( (*( p))) ) {
				case 76: {
					goto _st29;
				}
				case 108: {
					goto _st29;
				}
			}
			goto _st0;
		}
		_st29:
		if ( p == eof )
			goto _out29;
		p+= 1;
		st_case_29:
		if ( p == pe && p != eof )
			goto _out29;
		if ( p == eof ) {
			goto _st29;}
		else {
			switch( ( (*( p))) ) {
				case 9: {
					goto _st29;
				}
				case 32: {
					goto _st29;
				}
				case 61: {
					goto _st30;
				}
			}
			goto _st0;
		}
		_st30:
		if ( p == eof )
			goto _out30;
		p+= 1;
		st_case_30:
		if ( p == pe && p != eof )
			goto _out30;
		if ( p == eof ) {
			goto _st30;}
		else {
			switch( ( (*( p))) ) {
				case 9: {
					goto _st30;
				}
				case 32: {
					goto _st30;
				}
			}
			if ( 48 <= ( (*( p))) && ( (*( p))) <= 57 ) {
				goto _ctr44;
			}
			goto _st0;
		}
		_ctr44:
		{
#line 160 "cfg.rl"
			
			fas.intv_st = p;
			fas.v_int = fas.v_int2 = 0;
			fas.intv2_exist = false;
		}
		
#line 2089 "cfg.cpp"
		
		goto _st217;
		_ctr278:
		{
#line 165 "cfg.rl"
			
			if (auto t = nk::from_string<int>(fas.intv_st, p - fas.intv_st)) fas.v_int = *t; else {
				log_line("invalid value where integer is expected on line %zu", fas.linenum);
				std::exit(EXIT_FAILURE);
			}
		}
		
#line 2102 "cfg.cpp"
		
		{
#line 277 "cfg.rl"
			
			fas.wm->debounce_rise_ms_ = 0;
			fas.wm->debounce_fall_ms_ = fas.v_int;
		}
		
#line 2111 "cfg.cpp"
		
		goto _st217;
		_st217:
		if ( p == eof )
			goto _out217;
		p+= 1;
		st_case_217:
		if ( p == pe && p != eof )
			goto _out217;
		if ( p == eof ) {
			goto _ctr278;}
		else {
			if ( 48 <= ( (*( p))) && ( (*( p))) <= 57 ) {
				goto _st217;
			}
			goto _st0;
		}
		_st31:
		if ( p == eof )
			goto _out31;
		p+= 1;
		st_case_31:
		if ( p == pe && p != eof )
			goto _out31;
		if ( p == eof ) {
			goto _st31;}
		else {
			switch( ( (*( p))) ) {
				case 73: {
					goto _st32;
				}
				case 105: {
					goto _st32;
				}
			}
			goto _st0;
		}
		_st32:
		if ( p == eof )
			goto _out32;
		p+= 1;
		st_case_32:
		if ( p == pe && p != eof )
			goto _out32;
		if ( p == eof ) {
			goto _st32;}
		else {
			switch( ( (*( p))) ) {
				case 83: {
					goto _st33;
				}
				case 115: {
					goto _st33;
				}
			}
			goto _st0;
		}
		_st33:
		if ( p == eof )
			goto _out33;
		p+= 1;
		st_case_33:
		if ( p == pe && p != eof )
			goto _out33;
		if ( p == eof ) {
			goto _st33;}
		else {
			switch( ( (*( p))) ) {
				case 69: {
					goto _st34;
				}
				case 101: {
					goto _st34;
				}
			}
			goto _st0;
		}
		_st34:
		if ( p == eof )
			goto _out34;
		p+= 1;
		st_case_34:
		if ( p == pe && p != eof )
			goto _out34;
		if ( p == eof ) {
			goto _st34;}
		else {
			switch( ( (*( p))) ) {
				case 9: {
					goto _st34;
				}
				case 32: {
					goto _st34;
				}
				case 61: {
					goto _st35;
				}
			}
			goto _st0;
		}
		_st35:
		if ( p == eof )
			goto _out35;
		p+= 1;
		st_case_35:
		if ( p == pe && p != eof )
			goto _out35;
		if ( p == eof ) {
			goto _st35;}
		else {
			switch( ( (*( p))) ) {
				case 9: {
					goto _st35;
				}
				case 32: {
					goto _st35;
				}
			}
			if ( 48 <= ( (*( p))) && ( (*( p))) <= 57 ) {
				goto _ctr49;
			}
			goto _st0;
		}
		_ctr49:
		{
#line 160 "cfg.rl"
			
			fas.intv_st = p;
			fas.v_int = fas.v_int2 = 0;
			fas.intv2_exist = false;
		}
		
#line 2244 "cfg.cpp"
		
		goto _st218;
		_ctr280:
		{
#line 165 "cfg.rl"
			
			if (auto t = nk::from_string<int>(fas.intv_st, p - fas.intv_st)) fas.v_int = *t; else {
				log_line("invalid value where integer is expected on line %zu", fas.linenum);
				std::exit(EXIT_FAILURE);
			}
		}
		
#line 2257 "cfg.cpp"
		
		{
#line 273 "cfg.rl"
			
			fas.wm->debounce_rise_ms_ = fas.v_int;
			fas.wm->debounce_fall_ms_ = 0;
		}
		
#line 2266 "cfg.cpp"
		
		goto _st218;
		_st218:
		if ( p == eof )
			goto _out218;
		p+= 1;
		st_case_218:
		if ( p == pe && p != eof )
			goto _out218;
		if ( p == eof ) {
			goto _ctr280;}
		else {
			if ( 48 <= ( (*( p))) && ( (*( p))) <= 57 ) {
				goto _st218;
			}
			goto _st0;
		}
		_st36:
		if ( p == eof )
			goto _out36;
		p+= 1;
		st_case_36:
		if ( p == pe && p != eof )
			goto _out36;
		if ( p == eof ) {
			goto _st36;}
		else {
			switch( ( (*( p))) ) {
				case 65: {
					goto _st37;
				}
				case 67: {
					goto _st46;
				}
				case 68: {
					goto _st64;
				}
				case 77: {
					goto _st72;
				}
				case 79: {
					goto _st85;
				}
				case 97: {
					goto _st37;
				}
				case 99: {
					goto _st46;
				}
				case 100: {
					goto _st64;
				}
				case 109: {
					goto _st72;
				}
				case 111: {
					goto _st85;
				}
			}
			goto _st0;
		}
		_st37:
		if ( p == eof )
			goto _out37;
		p+= 1;
		st_case_37:
		if ( p == pe && p != eof )
			goto _out37;
		if ( p == eof ) {
			goto _st37;}
		else {
			switch( ( (*( p))) ) {
				case 67: {
					goto _st38;
				}
				case 84: {
					goto _st42;
				}
				case 99: {
					goto _st38;
				}
				case 116: {
					goto _st42;
				}
			}
			goto _st0;
		}
		_st38:
		if ( p == eof )
			goto _out38;
		p+= 1;
		st_case_38:
		if ( p == pe && p != eof )
			goto _out38;
		if ( p == eof ) {
			goto _st38;}
		else {
			switch( ( (*( p))) ) {
				case 67: {
					goto _st39;
				}
				case 99: {
					goto _st39;
				}
			}
			goto _st0;
		}
		_st39:
		if ( p == eof )
			goto _out39;
		p+= 1;
		st_case_39:
		if ( p == pe && p != eof )
			goto _out39;
		if ( p == eof ) {
			goto _st39;}
		else {
			switch( ( (*( p))) ) {
				case 69: {
					goto _st40;
				}
				case 101: {
					goto _st40;
				}
			}
			goto _st0;
		}
		_st40:
		if ( p == eof )
			goto _out40;
		p+= 1;
		st_case_40:
		if ( p == pe && p != eof )
			goto _out40;
		if ( p == eof ) {
			goto _st40;}
		else {
			switch( ( (*( p))) ) {
				case 83: {
					goto _st41;
				}
				case 115: {
					goto _st41;
				}
			}
			goto _st0;
		}
		_st41:
		if ( p == eof )
			goto _out41;
		p+= 1;
		st_case_41:
		if ( p == pe && p != eof )
			goto _out41;
		if ( p == eof ) {
			goto _st41;}
		else {
			switch( ( (*( p))) ) {
				case 83: {
					goto _st219;
				}
				case 115: {
					goto _st219;
				}
			}
			goto _st0;
		}
		_ctr282:
		{
#line 244 "cfg.rl"
			fas.wm->eventflags_ |= IN_ACCESS; }
		
#line 2439 "cfg.cpp"
		
		goto _st219;
		_st219:
		if ( p == eof )
			goto _out219;
		p+= 1;
		st_case_219:
		if ( p == pe && p != eof )
			goto _out219;
		if ( p == eof ) {
			goto _ctr282;}
		else {
			goto _st0;
		}
		_st42:
		if ( p == eof )
			goto _out42;
		p+= 1;
		st_case_42:
		if ( p == pe && p != eof )
			goto _out42;
		if ( p == eof ) {
			goto _st42;}
		else {
			switch( ( (*( p))) ) {
				case 84: {
					goto _st43;
				}
				case 116: {
					goto _st43;
				}
			}
			goto _st0;
		}
		_st43:
		if ( p == eof )
			goto _out43;
		p+= 1;
		st_case_43:
		if ( p == pe && p != eof )
			goto _out43;
		if ( p == eof ) {
			goto _st43;}
		else {
			switch( ( (*( p))) ) {
				case 82: {
					goto _st44;
				}
				case 114: {
					goto _st44;
				}
			}
			goto _st0;
		}
		_st44:
		if ( p == eof )
			goto _out44;
		p+= 1;
		st_case_44:
		if ( p == pe && p != eof )
			goto _out44;
		if ( p == eof ) {
			goto _st44;}
		else {
			switch( ( (*( p))) ) {
				case 73: {
					goto _st45;
				}
				case 105: {
					goto _st45;
				}
			}
			goto _st0;
		}
		_st45:
		if ( p == eof )
			goto _out45;
		p+= 1;
		st_case_45:
		if ( p == pe && p != eof )
			goto _out45;
		if ( p == eof ) {
			goto _st45;}
		else {
			switch( ( (*( p))) ) {
				case 66: {
					goto _st220;
				}
				case 98: {
					goto _st220;
				}
			}
			goto _st0;
		}
		_ctr283:
		{
#line 245 "cfg.rl"
			fas.wm->eventflags_ |= IN_ATTRIB; }
		
#line 2539 "cfg.cpp"
		
		goto _st220;
		_st220:
		if ( p == eof )
			goto _out220;
		p+= 1;
		st_case_220:
		if ( p == pe && p != eof )
			goto _out220;
		if ( p == eof ) {
			goto _ctr283;}
		else {
			goto _st0;
		}
		_st46:
		if ( p == eof )
			goto _out46;
		p+= 1;
		st_case_46:
		if ( p == pe && p != eof )
			goto _out46;
		if ( p == eof ) {
			goto _st46;}
		else {
			switch( ( (*( p))) ) {
				case 76: {
					goto _st47;
				}
				case 82: {
					goto _st60;
				}
				case 108: {
					goto _st47;
				}
				case 114: {
					goto _st60;
				}
			}
			goto _st0;
		}
		_st47:
		if ( p == eof )
			goto _out47;
		p+= 1;
		st_case_47:
		if ( p == pe && p != eof )
			goto _out47;
		if ( p == eof ) {
			goto _st47;}
		else {
			switch( ( (*( p))) ) {
				case 79: {
					goto _st48;
				}
				case 111: {
					goto _st48;
				}
			}
			goto _st0;
		}
		_st48:
		if ( p == eof )
			goto _out48;
		p+= 1;
		st_case_48:
		if ( p == pe && p != eof )
			goto _out48;
		if ( p == eof ) {
			goto _st48;}
		else {
			switch( ( (*( p))) ) {
				case 83: {
					goto _st49;
				}
				case 115: {
					goto _st49;
				}
			}
			goto _st0;
		}
		_st49:
		if ( p == eof )
			goto _out49;
		p+= 1;
		st_case_49:
		if ( p == pe && p != eof )
			goto _out49;
		if ( p == eof ) {
			goto _st49;}
		else {
			switch( ( (*( p))) ) {
				case 69: {
					goto _st221;
				}
				case 101: {
					goto _st221;
				}
			}
			goto _st0;
		}
		_ctr284:
		{
#line 248 "cfg.rl"
			fas.wm->eventflags_ |= IN_CLOSE; }
		
#line 2645 "cfg.cpp"
		
		goto _st221;
		_st221:
		if ( p == eof )
			goto _out221;
		p+= 1;
		st_case_221:
		if ( p == pe && p != eof )
			goto _out221;
		if ( p == eof ) {
			goto _ctr284;}
		else {
			switch( ( (*( p))) ) {
				case 78: {
					goto _st50;
				}
				case 87: {
					goto _st56;
				}
				case 110: {
					goto _st50;
				}
				case 119: {
					goto _st56;
				}
			}
			goto _st0;
		}
		_st50:
		if ( p == eof )
			goto _out50;
		p+= 1;
		st_case_50:
		if ( p == pe && p != eof )
			goto _out50;
		if ( p == eof ) {
			goto _st50;}
		else {
			switch( ( (*( p))) ) {
				case 79: {
					goto _st51;
				}
				case 111: {
					goto _st51;
				}
			}
			goto _st0;
		}
		_st51:
		if ( p == eof )
			goto _out51;
		p+= 1;
		st_case_51:
		if ( p == pe && p != eof )
			goto _out51;
		if ( p == eof ) {
			goto _st51;}
		else {
			switch( ( (*( p))) ) {
				case 87: {
					goto _st52;
				}
				case 119: {
					goto _st52;
				}
			}
			goto _st0;
		}
		_st52:
		if ( p == eof )
			goto _out52;
		p+= 1;
		st_case_52:
		if ( p == pe && p != eof )
			goto _out52;
		if ( p == eof ) {
			goto _st52;}
		else {
			switch( ( (*( p))) ) {
				case 82: {
					goto _st53;
				}
				case 114: {
					goto _st53;
				}
			}
			goto _st0;
		}
		_st53:
		if ( p == eof )
			goto _out53;
		p+= 1;
		st_case_53:
		if ( p == pe && p != eof )
			goto _out53;
		if ( p == eof ) {
			goto _st53;}
		else {
			switch( ( (*( p))) ) {
				case 73: {
					goto _st54;
				}
				case 105: {
					goto _st54;
				}
			}
			goto _st0;
		}
		_st54:
		if ( p == eof )
			goto _out54;
		p+= 1;
		st_case_54:
		if ( p == pe && p != eof )
			goto _out54;
		if ( p == eof ) {
			goto _st54;}
		else {
			switch( ( (*( p))) ) {
				case 84: {
					goto _st55;
				}
				case 116: {
					goto _st55;
				}
			}
			goto _st0;
		}
		_st55:
		if ( p == eof )
			goto _out55;
		p+= 1;
		st_case_55:
		if ( p == pe && p != eof )
			goto _out55;
		if ( p == eof ) {
			goto _st55;}
		else {
			switch( ( (*( p))) ) {
				case 69: {
					goto _st222;
				}
				case 101: {
					goto _st222;
				}
			}
			goto _st0;
		}
		_ctr285:
		{
#line 247 "cfg.rl"
			fas.wm->eventflags_ |= IN_CLOSE_NOWRITE; }
		
#line 2799 "cfg.cpp"
		
		goto _st222;
		_st222:
		if ( p == eof )
			goto _out222;
		p+= 1;
		st_case_222:
		if ( p == pe && p != eof )
			goto _out222;
		if ( p == eof ) {
			goto _ctr285;}
		else {
			goto _st0;
		}
		_st56:
		if ( p == eof )
			goto _out56;
		p+= 1;
		st_case_56:
		if ( p == pe && p != eof )
			goto _out56;
		if ( p == eof ) {
			goto _st56;}
		else {
			switch( ( (*( p))) ) {
				case 82: {
					goto _st57;
				}
				case 114: {
					goto _st57;
				}
			}
			goto _st0;
		}
		_st57:
		if ( p == eof )
			goto _out57;
		p+= 1;
		st_case_57:
		if ( p == pe && p != eof )
			goto _out57;
		if ( p == eof ) {
			goto _st57;}
		else {
			switch( ( (*( p))) ) {
				case 73: {
					goto _st58;
				}
				case 105: {
					goto _st58;
				}
			}
			goto _st0;
		}
		_st58:
		if ( p == eof )
			goto _out58;
		p+= 1;
		st_case_58:
		if ( p == pe && p != eof )
			goto _out58;
		if ( p == eof ) {
			goto _st58;}
		else {
			switch( ( (*( p))) ) {
				case 84: {
					goto _st59;
				}
				case 116: {
					goto _st59;
				}
			}
			goto _st0;
		}
		_st59:
		if ( p == eof )
			goto _out59;
		p+= 1;
		st_case_59:
		if ( p == pe && p != eof )
			goto _out59;
		if ( p == eof ) {
			goto _st59;}
		else {
			switch( ( (*( p))) ) {
				case 69: {
					goto _st223;
				}
				case 101: {
					goto _st223;
				}
			}
			goto _st0;
		}
		_ctr286:
		{
#line 246 "cfg.rl"
			fas.wm->eventflags_ |= IN_CLOSE_WRITE; }
		
#line 2899 "cfg.cpp"
		
		goto _st223;
		_st223:
		if ( p == eof )
			goto _out223;
		p+= 1;
		st_case_223:
		if ( p == pe && p != eof )
			goto _out223;
		if ( p == eof ) {
			goto _ctr286;}
		else {
			goto _st0;
		}
		_st60:
		if ( p == eof )
			goto _out60;
		p+= 1;
		st_case_60:
		if ( p == pe && p != eof )
			goto _out60;
		if ( p == eof ) {
			goto _st60;}
		else {
			switch( ( (*( p))) ) {
				case 69: {
					goto _st61;
				}
				case 101: {
					goto _st61;
				}
			}
			goto _st0;
		}
		_st61:
		if ( p == eof )
			goto _out61;
		p+= 1;
		st_case_61:
		if ( p == pe && p != eof )
			goto _out61;
		if ( p == eof ) {
			goto _st61;}
		else {
			switch( ( (*( p))) ) {
				case 65: {
					goto _st62;
				}
				case 97: {
					goto _st62;
				}
			}
			goto _st0;
		}
		_st62:
		if ( p == eof )
			goto _out62;
		p+= 1;
		st_case_62:
		if ( p == pe && p != eof )
			goto _out62;
		if ( p == eof ) {
			goto _st62;}
		else {
			switch( ( (*( p))) ) {
				case 84: {
					goto _st63;
				}
				case 116: {
					goto _st63;
				}
			}
			goto _st0;
		}
		_st63:
		if ( p == eof )
			goto _out63;
		p+= 1;
		st_case_63:
		if ( p == pe && p != eof )
			goto _out63;
		if ( p == eof ) {
			goto _st63;}
		else {
			switch( ( (*( p))) ) {
				case 69: {
					goto _st224;
				}
				case 101: {
					goto _st224;
				}
			}
			goto _st0;
		}
		_ctr287:
		{
#line 249 "cfg.rl"
			fas.wm->eventflags_ |= IN_CREATE; }
		
#line 2999 "cfg.cpp"
		
		goto _st224;
		_st224:
		if ( p == eof )
			goto _out224;
		p+= 1;
		st_case_224:
		if ( p == pe && p != eof )
			goto _out224;
		if ( p == eof ) {
			goto _ctr287;}
		else {
			goto _st0;
		}
		_st64:
		if ( p == eof )
			goto _out64;
		p+= 1;
		st_case_64:
		if ( p == pe && p != eof )
			goto _out64;
		if ( p == eof ) {
			goto _st64;}
		else {
			switch( ( (*( p))) ) {
				case 69: {
					goto _st65;
				}
				case 101: {
					goto _st65;
				}
			}
			goto _st0;
		}
		_st65:
		if ( p == eof )
			goto _out65;
		p+= 1;
		st_case_65:
		if ( p == pe && p != eof )
			goto _out65;
		if ( p == eof ) {
			goto _st65;}
		else {
			switch( ( (*( p))) ) {
				case 76: {
					goto _st66;
				}
				case 108: {
					goto _st66;
				}
			}
			goto _st0;
		}
		_st66:
		if ( p == eof )
			goto _out66;
		p+= 1;
		st_case_66:
		if ( p == pe && p != eof )
			goto _out66;
		if ( p == eof ) {
			goto _st66;}
		else {
			switch( ( (*( p))) ) {
				case 69: {
					goto _st67;
				}
				case 101: {
					goto _st67;
				}
			}
			goto _st0;
		}
		_st67:
		if ( p == eof )
			goto _out67;
		p+= 1;
		st_case_67:
		if ( p == pe && p != eof )
			goto _out67;
		if ( p == eof ) {
			goto _st67;}
		else {
			switch( ( (*( p))) ) {
				case 84: {
					goto _st68;
				}
				case 116: {
					goto _st68;
				}
			}
			goto _st0;
		}
		_st68:
		if ( p == eof )
			goto _out68;
		p+= 1;
		st_case_68:
		if ( p == pe && p != eof )
			goto _out68;
		if ( p == eof ) {
			goto _st68;}
		else {
			switch( ( (*( p))) ) {
				case 69: {
					goto _st225;
				}
				case 101: {
					goto _st225;
				}
			}
			goto _st0;
		}
		_ctr288:
		{
#line 250 "cfg.rl"
			fas.wm->eventflags_ |= IN_DELETE; }
		
#line 3119 "cfg.cpp"
		
		goto _st225;
		_st225:
		if ( p == eof )
			goto _out225;
		p+= 1;
		st_case_225:
		if ( p == pe && p != eof )
			goto _out225;
		if ( p == eof ) {
			goto _ctr288;}
		else {
			switch( ( (*( p))) ) {
				case 83: {
					goto _st69;
				}
				case 115: {
					goto _st69;
				}
			}
			goto _st0;
		}
		_st69:
		if ( p == eof )
			goto _out69;
		p+= 1;
		st_case_69:
		if ( p == pe && p != eof )
			goto _out69;
		if ( p == eof ) {
			goto _st69;}
		else {
			switch( ( (*( p))) ) {
				case 69: {
					goto _st70;
				}
				case 101: {
					goto _st70;
				}
			}
			goto _st0;
		}
		_st70:
		if ( p == eof )
			goto _out70;
		p+= 1;
		st_case_70:
		if ( p == pe && p != eof )
			goto _out70;
		if ( p == eof ) {
			goto _st70;}
		else {
			switch( ( (*( p))) ) {
				case 76: {
					goto _st71;
				}
				case 108: {
					goto _st71;
				}
			}
			goto _st0;
		}
		_st71:
		if ( p == eof )
			goto _out71;
		p+= 1;
		st_case_71:
		if ( p == pe && p != eof )
			goto _out71;
		if ( p == eof ) {
			goto _st71;}
		else {
			switch( ( (*( p))) ) {
				case 70: {
					goto _st226;
				}
				case 102: {
					goto _st226;
				}
			}
			goto _st0;
		}
		_ctr289:
		{
#line 251 "cfg.rl"
			fas.wm->eventflags_ |= IN_DELETE_SELF; }
		
#line 3207 "cfg.cpp"
		
		goto _st226;
		_st226:
		if ( p == eof )
			goto _out226;
		p+= 1;
		st_case_226:
		if ( p == pe && p != eof )
			goto _out226;
		if ( p == eof ) {
			goto _ctr289;}
		else {
			goto _st0;
		}
		_st72:
		if ( p == eof )
			goto _out72;
		p+= 1;
		st_case_72:
		if ( p == pe && p != eof )
			goto _out72;
		if ( p == eof ) {
			goto _st72;}
		else {
			switch( ( (*( p))) ) {
				case 79: {
					goto _st73;
				}
				case 111: {
					goto _st73;
				}
			}
			goto _st0;
		}
		_st73:
		if ( p == eof )
			goto _out73;
		p+= 1;
		st_case_73:
		if ( p == pe && p != eof )
			goto _out73;
		if ( p == eof ) {
			goto _st73;}
		else {
			switch( ( (*( p))) ) {
				case 68: {
					goto _st74;
				}
				case 86: {
					goto _st77;
				}
				case 100: {
					goto _st74;
				}
				case 118: {
					goto _st77;
				}
			}
			goto _st0;
		}
		_st74:
		if ( p == eof )
			goto _out74;
		p+= 1;
		st_case_74:
		if ( p == pe && p != eof )
			goto _out74;
		if ( p == eof ) {
			goto _st74;}
		else {
			switch( ( (*( p))) ) {
				case 73: {
					goto _st75;
				}
				case 105: {
					goto _st75;
				}
			}
			goto _st0;
		}
		_st75:
		if ( p == eof )
			goto _out75;
		p+= 1;
		st_case_75:
		if ( p == pe && p != eof )
			goto _out75;
		if ( p == eof ) {
			goto _st75;}
		else {
			switch( ( (*( p))) ) {
				case 70: {
					goto _st76;
				}
				case 102: {
					goto _st76;
				}
			}
			goto _st0;
		}
		_st76:
		if ( p == eof )
			goto _out76;
		p+= 1;
		st_case_76:
		if ( p == pe && p != eof )
			goto _out76;
		if ( p == eof ) {
			goto _st76;}
		else {
			switch( ( (*( p))) ) {
				case 89: {
					goto _st227;
				}
				case 121: {
					goto _st227;
				}
			}
			goto _st0;
		}
		_ctr290:
		{
#line 252 "cfg.rl"
			fas.wm->eventflags_ |= IN_MODIFY; }
		
#line 3333 "cfg.cpp"
		
		goto _st227;
		_st227:
		if ( p == eof )
			goto _out227;
		p+= 1;
		st_case_227:
		if ( p == pe && p != eof )
			goto _out227;
		if ( p == eof ) {
			goto _ctr290;}
		else {
			goto _st0;
		}
		_st77:
		if ( p == eof )
			goto _out77;
		p+= 1;
		st_case_77:
		if ( p == pe && p != eof )
			goto _out77;
		if ( p == eof ) {
			goto _st77;}
		else {
			switch( ( (*( p))) ) {
				case 69: {
					goto _st228;
				}
				case 101: {
					goto _st228;
				}
			}
			goto _st0;
		}
		_ctr291:
		{
#line 257 "cfg.rl"
			fas.wm->eventflags_ |= IN_MOVE; }
		
#line 3373 "cfg.cpp"
		
		goto _st228;
		_st228:
		if ( p == eof )
			goto _out228;
		p+= 1;
		st_case_228:
		if ( p == pe && p != eof )
			goto _out228;
		if ( p == eof ) {
			goto _ctr291;}
		else {
			switch( ( (*( p))) ) {
				case 70: {
					goto _st78;
				}
				case 83: {
					goto _st81;
				}
				case 84: {
					goto _st84;
				}
				case 102: {
					goto _st78;
				}
				case 115: {
					goto _st81;
				}
				case 116: {
					goto _st84;
				}
			}
			goto _st0;
		}
		_st78:
		if ( p == eof )
			goto _out78;
		p+= 1;
		st_case_78:
		if ( p == pe && p != eof )
			goto _out78;
		if ( p == eof ) {
			goto _st78;}
		else {
			switch( ( (*( p))) ) {
				case 82: {
					goto _st79;
				}
				case 114: {
					goto _st79;
				}
			}
			goto _st0;
		}
		_st79:
		if ( p == eof )
			goto _out79;
		p+= 1;
		st_case_79:
		if ( p == pe && p != eof )
			goto _out79;
		if ( p == eof ) {
			goto _st79;}
		else {
			switch( ( (*( p))) ) {
				case 79: {
					goto _st80;
				}
				case 111: {
					goto _st80;
				}
			}
			goto _st0;
		}
		_st80:
		if ( p == eof )
			goto _out80;
		p+= 1;
		st_case_80:
		if ( p == pe && p != eof )
			goto _out80;
		if ( p == eof ) {
			goto _st80;}
		else {
			switch( ( (*( p))) ) {
				case 77: {
					goto _st229;
				}
				case 109: {
					goto _st229;
				}
			}
			goto _st0;
		}
		_ctr292:
		{
#line 254 "cfg.rl"
			fas.wm->eventflags_ |= IN_MOVED_FROM; }
		
#line 3473 "cfg.cpp"
		
		goto _st229;
		_st229:
		if ( p == eof )
			goto _out229;
		p+= 1;
		st_case_229:
		if ( p == pe && p != eof )
			goto _out229;
		if ( p == eof ) {
			goto _ctr292;}
		else {
			goto _st0;
		}
		_st81:
		if ( p == eof )
			goto _out81;
		p+= 1;
		st_case_81:
		if ( p == pe && p != eof )
			goto _out81;
		if ( p == eof ) {
			goto _st81;}
		else {
			switch( ( (*( p))) ) {
				case 69: {
					goto _st82;
				}
				case 101: {
					goto _st82;
				}
			}
			goto _st0;
		}
		_st82:
		if ( p == eof )
			goto _out82;
		p+= 1;
		st_case_82:
		if ( p == pe && p != eof )
			goto _out82;
		if ( p == eof ) {
			goto _st82;}
		else {
			switch( ( (*( p))) ) {
				case 76: {
					goto _st83;
				}
				case 108: {
					goto _st83;
				}
			}
			goto _st0;
		}
		_st83:
		if ( p == eof )
			goto _out83;
		p+= 1;
		st_case_83:
		if ( p == pe && p != eof )
			goto _out83;
		if ( p == eof ) {
			goto _st83;}
		else {
			switch( ( (*( p))) ) {
				case 70: {
					goto _st230;
				}
				case 102: {
					goto _st230;
				}
			}
			goto _st0;
		}
		_ctr293:
		{
#line 253 "cfg.rl"
			fas.wm->eventflags_ |= IN_MOVE_SELF; }
		
#line 3553 "cfg.cpp"
		
		goto _st230;
		_st230:
		if ( p == eof )
			goto _out230;
		p+= 1;
		st_case_230:
		if ( p == pe && p != eof )
			goto _out230;
		if ( p == eof ) {
			goto _ctr293;}
		else {
			goto _st0;
		}
		_st84:
		if ( p == eof )
			goto _out84;
		p+= 1;
		st_case_84:
		if ( p == pe && p != eof )
			goto _out84;
		if ( p == eof ) {
			goto _st84;}
		else {
			switch( ( (*( p))) ) {
				case 79: {
					goto _st231;
				}
				case 111: {
					goto _st231;
				}
			}
			goto _st0;
		}
		_ctr294:
		{
#line 255 "cfg.rl"
			fas.wm->eventflags_ |= IN_MOVED_TO; }
		
#line 3593 "cfg.cpp"
		
		goto _st231;
		_st231:
		if ( p == eof )
			goto _out231;
		p+= 1;
		st_case_231:
		if ( p == pe && p != eof )
			goto _out231;
		if ( p == eof ) {
			goto _ctr294;}
		else {
			goto _st0;
		}
		_st85:
		if ( p == eof )
			goto _out85;
		p+= 1;
		st_case_85:
		if ( p == pe && p != eof )
			goto _out85;
		if ( p == eof ) {
			goto _st85;}
		else {
			switch( ( (*( p))) ) {
				case 80: {
					goto _st86;
				}
				case 112: {
					goto _st86;
				}
			}
			goto _st0;
		}
		_st86:
		if ( p == eof )
			goto _out86;
		p+= 1;
		st_case_86:
		if ( p == pe && p != eof )
			goto _out86;
		if ( p == eof ) {
			goto _st86;}
		else {
			switch( ( (*( p))) ) {
				case 69: {
					goto _st87;
				}
				case 101: {
					goto _st87;
				}
			}
			goto _st0;
		}
		_st87:
		if ( p == eof )
			goto _out87;
		p+= 1;
		st_case_87:
		if ( p == pe && p != eof )
			goto _out87;
		if ( p == eof ) {
			goto _st87;}
		else {
			switch( ( (*( p))) ) {
				case 78: {
					goto _st232;
				}
				case 110: {
					goto _st232;
				}
			}
			goto _st0;
		}
		_ctr295:
		{
#line 256 "cfg.rl"
			fas.wm->eventflags_ |= IN_OPEN; }
		
#line 3673 "cfg.cpp"
		
		goto _st232;
		_st232:
		if ( p == eof )
			goto _out232;
		p+= 1;
		st_case_232:
		if ( p == pe && p != eof )
			goto _out232;
		if ( p == eof ) {
			goto _ctr295;}
		else {
			goto _st0;
		}
		_st88:
		if ( p == eof )
			goto _out88;
		p+= 1;
		st_case_88:
		if ( p == pe && p != eof )
			goto _out88;
		if ( p == eof ) {
			goto _st88;}
		else {
			switch( ( (*( p))) ) {
				case 82: {
					goto _st89;
				}
				case 114: {
					goto _st89;
				}
			}
			goto _st0;
		}
		_st89:
		if ( p == eof )
			goto _out89;
		p+= 1;
		st_case_89:
		if ( p == pe && p != eof )
			goto _out89;
		if ( p == eof ) {
			goto _st89;}
		else {
			switch( ( (*( p))) ) {
				case 79: {
					goto _st90;
				}
				case 111: {
					goto _st90;
				}
			}
			goto _st0;
		}
		_st90:
		if ( p == eof )
			goto _out90;
		p+= 1;
		st_case_90:
		if ( p == pe && p != eof )
			goto _out90;
		if ( p == eof ) {
			goto _st90;}
		else {
			switch( ( (*( p))) ) {
				case 85: {
					goto _st91;
				}
				case 117: {
					goto _st91;
				}
			}
			goto _st0;
		}
		_st91:
		if ( p == eof )
			goto _out91;
		p+= 1;
		st_case_91:
		if ( p == pe && p != eof )
			goto _out91;
		if ( p == eof ) {
			goto _st91;}
		else {
			switch( ( (*( p))) ) {
				case 80: {
					goto _st92;
				}
				case 112: {
					goto _st92;
				}
			}
			goto _st0;
		}
		_st92:
		if ( p == eof )
			goto _out92;
		p+= 1;
		st_case_92:
		if ( p == pe && p != eof )
			goto _out92;
		if ( p == eof ) {
			goto _st92;}
		else {
			switch( ( (*( p))) ) {
				case 9: {
					goto _st92;
				}
				case 32: {
					goto _st92;
				}
				case 61: {
					goto _st93;
				}
			}
			goto _st0;
		}
		_st93:
		if ( p == eof )
			goto _out93;
		p+= 1;
		st_case_93:
		if ( p == pe && p != eof )
			goto _out93;
		if ( p == eof ) {
			goto _st93;}
		else {
			switch( ( (*( p))) ) {
				case 0: {
					goto _st0;
				}
				case 9: {
					goto _ctr121;
				}
				case 10: {
					goto _st0;
				}
				case 32: {
					goto _ctr121;
				}
			}
			goto _ctr120;
		}
		_ctr120:
		{
#line 180 "cfg.rl"
			fas.strv_st = p; fas.v_strlen = 0; }
		
#line 3822 "cfg.cpp"
		
		goto _st233;
		_ctr296:
		{
#line 181 "cfg.rl"
			
			fas.v_strlen = p - fas.strv_st;
			if (fas.v_strlen <= INT_MAX) {
				ssize_t snl = snprintf(fas.v_str, sizeof fas.v_str,
				"%.*s", (int)fas.v_strlen, fas.strv_st);
				if (snl < 0 || (size_t)snl >= sizeof fas.v_str) {
					log_line("error parsing line %zu in crontab; too long?", fas.linenum);
					std::exit(EXIT_FAILURE);
				}
			}
		}
		
#line 3840 "cfg.cpp"
		
		{
#line 230 "cfg.rl"
			fas.setgroupv(); }
		
#line 3846 "cfg.cpp"
		
		goto _st233;
		_st233:
		if ( p == eof )
			goto _out233;
		p+= 1;
		st_case_233:
		if ( p == pe && p != eof )
			goto _out233;
		if ( p == eof ) {
			goto _ctr296;}
		else {
			switch( ( (*( p))) ) {
				case 0: {
					goto _st0;
				}
				case 10: {
					goto _st0;
				}
			}
			goto _st233;
		}
		_ctr121:
		{
#line 180 "cfg.rl"
			fas.strv_st = p; fas.v_strlen = 0; }
		
#line 3874 "cfg.cpp"
		
		goto _st234;
		_ctr298:
		{
#line 181 "cfg.rl"
			
			fas.v_strlen = p - fas.strv_st;
			if (fas.v_strlen <= INT_MAX) {
				ssize_t snl = snprintf(fas.v_str, sizeof fas.v_str,
				"%.*s", (int)fas.v_strlen, fas.strv_st);
				if (snl < 0 || (size_t)snl >= sizeof fas.v_str) {
					log_line("error parsing line %zu in crontab; too long?", fas.linenum);
					std::exit(EXIT_FAILURE);
				}
			}
		}
		
#line 3892 "cfg.cpp"
		
		{
#line 230 "cfg.rl"
			fas.setgroupv(); }
		
#line 3898 "cfg.cpp"
		
		goto _st234;
		_st234:
		if ( p == eof )
			goto _out234;
		p+= 1;
		st_case_234:
		if ( p == pe && p != eof )
			goto _out234;
		if ( p == eof ) {
			goto _ctr298;}
		else {
			switch( ( (*( p))) ) {
				case 0: {
					goto _st0;
				}
				case 9: {
					goto _ctr121;
				}
				case 10: {
					goto _st0;
				}
				case 32: {
					goto _ctr121;
				}
			}
			goto _ctr120;
		}
		_st94:
		if ( p == eof )
			goto _out94;
		p+= 1;
		st_case_94:
		if ( p == pe && p != eof )
			goto _out94;
		if ( p == eof ) {
			goto _st94;}
		else {
			if ( ( (*( p))) == 95 ) {
				goto _st95;
			}
			goto _st0;
		}
		_st95:
		if ( p == eof )
			goto _out95;
		p+= 1;
		st_case_95:
		if ( p == pe && p != eof )
			goto _out95;
		if ( p == eof ) {
			goto _st95;}
		else {
			switch( ( (*( p))) ) {
				case 65: {
					goto _st96;
				}
				case 67: {
					goto _st100;
				}
				case 68: {
					goto _st110;
				}
				case 70: {
					goto _st116;
				}
				case 77: {
					goto _st123;
				}
				case 78: {
					goto _st141;
				}
				case 82: {
					goto _st160;
				}
				case 83: {
					goto _st178;
				}
				case 97: {
					goto _st96;
				}
				case 99: {
					goto _st100;
				}
				case 100: {
					goto _st110;
				}
				case 102: {
					goto _st116;
				}
				case 109: {
					goto _st123;
				}
				case 110: {
					goto _st141;
				}
				case 114: {
					goto _st160;
				}
				case 115: {
					goto _st178;
				}
			}
			goto _st0;
		}
		_st96:
		if ( p == eof )
			goto _out96;
		p+= 1;
		st_case_96:
		if ( p == pe && p != eof )
			goto _out96;
		if ( p == eof ) {
			goto _st96;}
		else {
			switch( ( (*( p))) ) {
				case 83: {
					goto _st97;
				}
				case 115: {
					goto _st97;
				}
			}
			goto _st0;
		}
		_st97:
		if ( p == eof )
			goto _out97;
		p+= 1;
		st_case_97:
		if ( p == pe && p != eof )
			goto _out97;
		if ( p == eof ) {
			goto _st97;}
		else {
			switch( ( (*( p))) ) {
				case 9: {
					goto _st97;
				}
				case 32: {
					goto _st97;
				}
				case 61: {
					goto _st98;
				}
			}
			goto _st0;
		}
		_st98:
		if ( p == eof )
			goto _out98;
		p+= 1;
		st_case_98:
		if ( p == pe && p != eof )
			goto _out98;
		if ( p == eof ) {
			goto _st98;}
		else {
			switch( ( (*( p))) ) {
				case 9: {
					goto _st98;
				}
				case 32: {
					goto _st98;
				}
			}
			if ( 48 <= ( (*( p))) && ( (*( p))) <= 57 ) {
				goto _ctr133;
			}
			goto _st0;
		}
		_ctr133:
		{
#line 160 "cfg.rl"
			
			fas.intv_st = p;
			fas.v_int = fas.v_int2 = 0;
			fas.intv2_exist = false;
		}
		
#line 4079 "cfg.cpp"
		
		goto _st235;
		_ctr299:
		{
#line 165 "cfg.rl"
			
			if (auto t = nk::from_string<int>(fas.intv_st, p - fas.intv_st)) fas.v_int = *t; else {
				log_line("invalid value where integer is expected on line %zu", fas.linenum);
				std::exit(EXIT_FAILURE);
			}
		}
		
#line 4092 "cfg.cpp"
		
		{
#line 198 "cfg.rl"
			fas.setlim(RLIMIT_AS); }
		
#line 4098 "cfg.cpp"
		
		goto _st235;
		_st235:
		if ( p == eof )
			goto _out235;
		p+= 1;
		st_case_235:
		if ( p == pe && p != eof )
			goto _out235;
		if ( p == eof ) {
			goto _ctr299;}
		else {
			if ( ( (*( p))) == 44 ) {
				goto _ctr300;
			}
			if ( 48 <= ( (*( p))) && ( (*( p))) <= 57 ) {
				goto _st235;
			}
			goto _st0;
		}
		_ctr300:
		{
#line 165 "cfg.rl"
			
			if (auto t = nk::from_string<int>(fas.intv_st, p - fas.intv_st)) fas.v_int = *t; else {
				log_line("invalid value where integer is expected on line %zu", fas.linenum);
				std::exit(EXIT_FAILURE);
			}
		}
		
#line 4129 "cfg.cpp"
		
		goto _st99;
		_st99:
		if ( p == eof )
			goto _out99;
		p+= 1;
		st_case_99:
		if ( p == pe && p != eof )
			goto _out99;
		if ( p == eof ) {
			goto _st99;}
		else {
			if ( 48 <= ( (*( p))) && ( (*( p))) <= 57 ) {
				goto _ctr135;
			}
			goto _st0;
		}
		_ctr135:
		{
#line 171 "cfg.rl"
			fas.intv2_st = p; }
		
#line 4152 "cfg.cpp"
		
		goto _st236;
		_ctr302:
		{
#line 172 "cfg.rl"
			
			if (auto t = nk::from_string<int>(fas.intv2_st, p - fas.intv2_st)) fas.v_int2 = *t; {
				log_line("invalid value where second integer in range is expected on line %zu", fas.linenum);
				std::exit(EXIT_FAILURE);
			}
			fas.intv2_exist = true;
		}
		
#line 4166 "cfg.cpp"
		
		{
#line 198 "cfg.rl"
			fas.setlim(RLIMIT_AS); }
		
#line 4172 "cfg.cpp"
		
		goto _st236;
		_st236:
		if ( p == eof )
			goto _out236;
		p+= 1;
		st_case_236:
		if ( p == pe && p != eof )
			goto _out236;
		if ( p == eof ) {
			goto _ctr302;}
		else {
			if ( 48 <= ( (*( p))) && ( (*( p))) <= 57 ) {
				goto _st236;
			}
			goto _st0;
		}
		_st100:
		if ( p == eof )
			goto _out100;
		p+= 1;
		st_case_100:
		if ( p == pe && p != eof )
			goto _out100;
		if ( p == eof ) {
			goto _st100;}
		else {
			switch( ( (*( p))) ) {
				case 79: {
					goto _st101;
				}
				case 80: {
					goto _st106;
				}
				case 111: {
					goto _st101;
				}
				case 112: {
					goto _st106;
				}
			}
			goto _st0;
		}
		_st101:
		if ( p == eof )
			goto _out101;
		p+= 1;
		st_case_101:
		if ( p == pe && p != eof )
			goto _out101;
		if ( p == eof ) {
			goto _st101;}
		else {
			switch( ( (*( p))) ) {
				case 82: {
					goto _st102;
				}
				case 114: {
					goto _st102;
				}
			}
			goto _st0;
		}
		_st102:
		if ( p == eof )
			goto _out102;
		p+= 1;
		st_case_102:
		if ( p == pe && p != eof )
			goto _out102;
		if ( p == eof ) {
			goto _st102;}
		else {
			switch( ( (*( p))) ) {
				case 69: {
					goto _st103;
				}
				case 101: {
					goto _st103;
				}
			}
			goto _st0;
		}
		_st103:
		if ( p == eof )
			goto _out103;
		p+= 1;
		st_case_103:
		if ( p == pe && p != eof )
			goto _out103;
		if ( p == eof ) {
			goto _st103;}
		else {
			switch( ( (*( p))) ) {
				case 9: {
					goto _st103;
				}
				case 32: {
					goto _st103;
				}
				case 61: {
					goto _st104;
				}
			}
			goto _st0;
		}
		_st104:
		if ( p == eof )
			goto _out104;
		p+= 1;
		st_case_104:
		if ( p == pe && p != eof )
			goto _out104;
		if ( p == eof ) {
			goto _st104;}
		else {
			switch( ( (*( p))) ) {
				case 9: {
					goto _st104;
				}
				case 32: {
					goto _st104;
				}
			}
			if ( 48 <= ( (*( p))) && ( (*( p))) <= 57 ) {
				goto _ctr141;
			}
			goto _st0;
		}
		_ctr141:
		{
#line 160 "cfg.rl"
			
			fas.intv_st = p;
			fas.v_int = fas.v_int2 = 0;
			fas.intv2_exist = false;
		}
		
#line 4311 "cfg.cpp"
		
		goto _st237;
		_ctr304:
		{
#line 165 "cfg.rl"
			
			if (auto t = nk::from_string<int>(fas.intv_st, p - fas.intv_st)) fas.v_int = *t; else {
				log_line("invalid value where integer is expected on line %zu", fas.linenum);
				std::exit(EXIT_FAILURE);
			}
		}
		
#line 4324 "cfg.cpp"
		
		{
#line 203 "cfg.rl"
			fas.setlim(RLIMIT_CORE); }
		
#line 4330 "cfg.cpp"
		
		goto _st237;
		_st237:
		if ( p == eof )
			goto _out237;
		p+= 1;
		st_case_237:
		if ( p == pe && p != eof )
			goto _out237;
		if ( p == eof ) {
			goto _ctr304;}
		else {
			if ( ( (*( p))) == 44 ) {
				goto _ctr305;
			}
			if ( 48 <= ( (*( p))) && ( (*( p))) <= 57 ) {
				goto _st237;
			}
			goto _st0;
		}
		_ctr305:
		{
#line 165 "cfg.rl"
			
			if (auto t = nk::from_string<int>(fas.intv_st, p - fas.intv_st)) fas.v_int = *t; else {
				log_line("invalid value where integer is expected on line %zu", fas.linenum);
				std::exit(EXIT_FAILURE);
			}
		}
		
#line 4361 "cfg.cpp"
		
		goto _st105;
		_st105:
		if ( p == eof )
			goto _out105;
		p+= 1;
		st_case_105:
		if ( p == pe && p != eof )
			goto _out105;
		if ( p == eof ) {
			goto _st105;}
		else {
			if ( 48 <= ( (*( p))) && ( (*( p))) <= 57 ) {
				goto _ctr143;
			}
			goto _st0;
		}
		_ctr143:
		{
#line 171 "cfg.rl"
			fas.intv2_st = p; }
		
#line 4384 "cfg.cpp"
		
		goto _st238;
		_ctr307:
		{
#line 172 "cfg.rl"
			
			if (auto t = nk::from_string<int>(fas.intv2_st, p - fas.intv2_st)) fas.v_int2 = *t; {
				log_line("invalid value where second integer in range is expected on line %zu", fas.linenum);
				std::exit(EXIT_FAILURE);
			}
			fas.intv2_exist = true;
		}
		
#line 4398 "cfg.cpp"
		
		{
#line 203 "cfg.rl"
			fas.setlim(RLIMIT_CORE); }
		
#line 4404 "cfg.cpp"
		
		goto _st238;
		_st238:
		if ( p == eof )
			goto _out238;
		p+= 1;
		st_case_238:
		if ( p == pe && p != eof )
			goto _out238;
		if ( p == eof ) {
			goto _ctr307;}
		else {
			if ( 48 <= ( (*( p))) && ( (*( p))) <= 57 ) {
				goto _st238;
			}
			goto _st0;
		}
		_st106:
		if ( p == eof )
			goto _out106;
		p+= 1;
		st_case_106:
		if ( p == pe && p != eof )
			goto _out106;
		if ( p == eof ) {
			goto _st106;}
		else {
			switch( ( (*( p))) ) {
				case 85: {
					goto _st107;
				}
				case 117: {
					goto _st107;
				}
			}
			goto _st0;
		}
		_st107:
		if ( p == eof )
			goto _out107;
		p+= 1;
		st_case_107:
		if ( p == pe && p != eof )
			goto _out107;
		if ( p == eof ) {
			goto _st107;}
		else {
			switch( ( (*( p))) ) {
				case 9: {
					goto _st107;
				}
				case 32: {
					goto _st107;
				}
				case 61: {
					goto _st108;
				}
			}
			goto _st0;
		}
		_st108:
		if ( p == eof )
			goto _out108;
		p+= 1;
		st_case_108:
		if ( p == pe && p != eof )
			goto _out108;
		if ( p == eof ) {
			goto _st108;}
		else {
			switch( ( (*( p))) ) {
				case 9: {
					goto _st108;
				}
				case 32: {
					goto _st108;
				}
			}
			if ( 48 <= ( (*( p))) && ( (*( p))) <= 57 ) {
				goto _ctr146;
			}
			goto _st0;
		}
		_ctr146:
		{
#line 160 "cfg.rl"
			
			fas.intv_st = p;
			fas.v_int = fas.v_int2 = 0;
			fas.intv2_exist = false;
		}
		
#line 4497 "cfg.cpp"
		
		goto _st239;
		_ctr309:
		{
#line 165 "cfg.rl"
			
			if (auto t = nk::from_string<int>(fas.intv_st, p - fas.intv_st)) fas.v_int = *t; else {
				log_line("invalid value where integer is expected on line %zu", fas.linenum);
				std::exit(EXIT_FAILURE);
			}
		}
		
#line 4510 "cfg.cpp"
		
		{
#line 207 "cfg.rl"
			fas.setlim(RLIMIT_CPU); }
		
#line 4516 "cfg.cpp"
		
		goto _st239;
		_st239:
		if ( p == eof )
			goto _out239;
		p+= 1;
		st_case_239:
		if ( p == pe && p != eof )
			goto _out239;
		if ( p == eof ) {
			goto _ctr309;}
		else {
			if ( ( (*( p))) == 44 ) {
				goto _ctr310;
			}
			if ( 48 <= ( (*( p))) && ( (*( p))) <= 57 ) {
				goto _st239;
			}
			goto _st0;
		}
		_ctr310:
		{
#line 165 "cfg.rl"
			
			if (auto t = nk::from_string<int>(fas.intv_st, p - fas.intv_st)) fas.v_int = *t; else {
				log_line("invalid value where integer is expected on line %zu", fas.linenum);
				std::exit(EXIT_FAILURE);
			}
		}
		
#line 4547 "cfg.cpp"
		
		goto _st109;
		_st109:
		if ( p == eof )
			goto _out109;
		p+= 1;
		st_case_109:
		if ( p == pe && p != eof )
			goto _out109;
		if ( p == eof ) {
			goto _st109;}
		else {
			if ( 48 <= ( (*( p))) && ( (*( p))) <= 57 ) {
				goto _ctr148;
			}
			goto _st0;
		}
		_ctr148:
		{
#line 171 "cfg.rl"
			fas.intv2_st = p; }
		
#line 4570 "cfg.cpp"
		
		goto _st240;
		_ctr312:
		{
#line 172 "cfg.rl"
			
			if (auto t = nk::from_string<int>(fas.intv2_st, p - fas.intv2_st)) fas.v_int2 = *t; {
				log_line("invalid value where second integer in range is expected on line %zu", fas.linenum);
				std::exit(EXIT_FAILURE);
			}
			fas.intv2_exist = true;
		}
		
#line 4584 "cfg.cpp"
		
		{
#line 207 "cfg.rl"
			fas.setlim(RLIMIT_CPU); }
		
#line 4590 "cfg.cpp"
		
		goto _st240;
		_st240:
		if ( p == eof )
			goto _out240;
		p+= 1;
		st_case_240:
		if ( p == pe && p != eof )
			goto _out240;
		if ( p == eof ) {
			goto _ctr312;}
		else {
			if ( 48 <= ( (*( p))) && ( (*( p))) <= 57 ) {
				goto _st240;
			}
			goto _st0;
		}
		_st110:
		if ( p == eof )
			goto _out110;
		p+= 1;
		st_case_110:
		if ( p == pe && p != eof )
			goto _out110;
		if ( p == eof ) {
			goto _st110;}
		else {
			switch( ( (*( p))) ) {
				case 65: {
					goto _st111;
				}
				case 97: {
					goto _st111;
				}
			}
			goto _st0;
		}
		_st111:
		if ( p == eof )
			goto _out111;
		p+= 1;
		st_case_111:
		if ( p == pe && p != eof )
			goto _out111;
		if ( p == eof ) {
			goto _st111;}
		else {
			switch( ( (*( p))) ) {
				case 84: {
					goto _st112;
				}
				case 116: {
					goto _st112;
				}
			}
			goto _st0;
		}
		_st112:
		if ( p == eof )
			goto _out112;
		p+= 1;
		st_case_112:
		if ( p == pe && p != eof )
			goto _out112;
		if ( p == eof ) {
			goto _st112;}
		else {
			switch( ( (*( p))) ) {
				case 65: {
					goto _st113;
				}
				case 97: {
					goto _st113;
				}
			}
			goto _st0;
		}
		_st113:
		if ( p == eof )
			goto _out113;
		p+= 1;
		st_case_113:
		if ( p == pe && p != eof )
			goto _out113;
		if ( p == eof ) {
			goto _st113;}
		else {
			switch( ( (*( p))) ) {
				case 9: {
					goto _st113;
				}
				case 32: {
					goto _st113;
				}
				case 61: {
					goto _st114;
				}
			}
			goto _st0;
		}
		_st114:
		if ( p == eof )
			goto _out114;
		p+= 1;
		st_case_114:
		if ( p == pe && p != eof )
			goto _out114;
		if ( p == eof ) {
			goto _st114;}
		else {
			switch( ( (*( p))) ) {
				case 9: {
					goto _st114;
				}
				case 32: {
					goto _st114;
				}
			}
			if ( 48 <= ( (*( p))) && ( (*( p))) <= 57 ) {
				goto _ctr153;
			}
			goto _st0;
		}
		_ctr153:
		{
#line 160 "cfg.rl"
			
			fas.intv_st = p;
			fas.v_int = fas.v_int2 = 0;
			fas.intv2_exist = false;
		}
		
#line 4723 "cfg.cpp"
		
		goto _st241;
		_ctr314:
		{
#line 165 "cfg.rl"
			
			if (auto t = nk::from_string<int>(fas.intv_st, p - fas.intv_st)) fas.v_int = *t; else {
				log_line("invalid value where integer is expected on line %zu", fas.linenum);
				std::exit(EXIT_FAILURE);
			}
		}
		
#line 4736 "cfg.cpp"
		
		{
#line 205 "cfg.rl"
			fas.setlim(RLIMIT_DATA); }
		
#line 4742 "cfg.cpp"
		
		goto _st241;
		_st241:
		if ( p == eof )
			goto _out241;
		p+= 1;
		st_case_241:
		if ( p == pe && p != eof )
			goto _out241;
		if ( p == eof ) {
			goto _ctr314;}
		else {
			if ( ( (*( p))) == 44 ) {
				goto _ctr315;
			}
			if ( 48 <= ( (*( p))) && ( (*( p))) <= 57 ) {
				goto _st241;
			}
			goto _st0;
		}
		_ctr315:
		{
#line 165 "cfg.rl"
			
			if (auto t = nk::from_string<int>(fas.intv_st, p - fas.intv_st)) fas.v_int = *t; else {
				log_line("invalid value where integer is expected on line %zu", fas.linenum);
				std::exit(EXIT_FAILURE);
			}
		}
		
#line 4773 "cfg.cpp"
		
		goto _st115;
		_st115:
		if ( p == eof )
			goto _out115;
		p+= 1;
		st_case_115:
		if ( p == pe && p != eof )
			goto _out115;
		if ( p == eof ) {
			goto _st115;}
		else {
			if ( 48 <= ( (*( p))) && ( (*( p))) <= 57 ) {
				goto _ctr155;
			}
			goto _st0;
		}
		_ctr155:
		{
#line 171 "cfg.rl"
			fas.intv2_st = p; }
		
#line 4796 "cfg.cpp"
		
		goto _st242;
		_ctr317:
		{
#line 172 "cfg.rl"
			
			if (auto t = nk::from_string<int>(fas.intv2_st, p - fas.intv2_st)) fas.v_int2 = *t; {
				log_line("invalid value where second integer in range is expected on line %zu", fas.linenum);
				std::exit(EXIT_FAILURE);
			}
			fas.intv2_exist = true;
		}
		
#line 4810 "cfg.cpp"
		
		{
#line 205 "cfg.rl"
			fas.setlim(RLIMIT_DATA); }
		
#line 4816 "cfg.cpp"
		
		goto _st242;
		_st242:
		if ( p == eof )
			goto _out242;
		p+= 1;
		st_case_242:
		if ( p == pe && p != eof )
			goto _out242;
		if ( p == eof ) {
			goto _ctr317;}
		else {
			if ( 48 <= ( (*( p))) && ( (*( p))) <= 57 ) {
				goto _st242;
			}
			goto _st0;
		}
		_st116:
		if ( p == eof )
			goto _out116;
		p+= 1;
		st_case_116:
		if ( p == pe && p != eof )
			goto _out116;
		if ( p == eof ) {
			goto _st116;}
		else {
			switch( ( (*( p))) ) {
				case 83: {
					goto _st117;
				}
				case 115: {
					goto _st117;
				}
			}
			goto _st0;
		}
		_st117:
		if ( p == eof )
			goto _out117;
		p+= 1;
		st_case_117:
		if ( p == pe && p != eof )
			goto _out117;
		if ( p == eof ) {
			goto _st117;}
		else {
			switch( ( (*( p))) ) {
				case 73: {
					goto _st118;
				}
				case 105: {
					goto _st118;
				}
			}
			goto _st0;
		}
		_st118:
		if ( p == eof )
			goto _out118;
		p+= 1;
		st_case_118:
		if ( p == pe && p != eof )
			goto _out118;
		if ( p == eof ) {
			goto _st118;}
		else {
			switch( ( (*( p))) ) {
				case 90: {
					goto _st119;
				}
				case 122: {
					goto _st119;
				}
			}
			goto _st0;
		}
		_st119:
		if ( p == eof )
			goto _out119;
		p+= 1;
		st_case_119:
		if ( p == pe && p != eof )
			goto _out119;
		if ( p == eof ) {
			goto _st119;}
		else {
			switch( ( (*( p))) ) {
				case 69: {
					goto _st120;
				}
				case 101: {
					goto _st120;
				}
			}
			goto _st0;
		}
		_st120:
		if ( p == eof )
			goto _out120;
		p+= 1;
		st_case_120:
		if ( p == pe && p != eof )
			goto _out120;
		if ( p == eof ) {
			goto _st120;}
		else {
			switch( ( (*( p))) ) {
				case 9: {
					goto _st120;
				}
				case 32: {
					goto _st120;
				}
				case 61: {
					goto _st121;
				}
			}
			goto _st0;
		}
		_st121:
		if ( p == eof )
			goto _out121;
		p+= 1;
		st_case_121:
		if ( p == pe && p != eof )
			goto _out121;
		if ( p == eof ) {
			goto _st121;}
		else {
			switch( ( (*( p))) ) {
				case 9: {
					goto _st121;
				}
				case 32: {
					goto _st121;
				}
			}
			if ( 48 <= ( (*( p))) && ( (*( p))) <= 57 ) {
				goto _ctr161;
			}
			goto _st0;
		}
		_ctr161:
		{
#line 160 "cfg.rl"
			
			fas.intv_st = p;
			fas.v_int = fas.v_int2 = 0;
			fas.intv2_exist = false;
		}
		
#line 4969 "cfg.cpp"
		
		goto _st243;
		_ctr319:
		{
#line 165 "cfg.rl"
			
			if (auto t = nk::from_string<int>(fas.intv_st, p - fas.intv_st)) fas.v_int = *t; else {
				log_line("invalid value where integer is expected on line %zu", fas.linenum);
				std::exit(EXIT_FAILURE);
			}
		}
		
#line 4982 "cfg.cpp"
		
		{
#line 206 "cfg.rl"
			fas.setlim(RLIMIT_FSIZE); }
		
#line 4988 "cfg.cpp"
		
		goto _st243;
		_st243:
		if ( p == eof )
			goto _out243;
		p+= 1;
		st_case_243:
		if ( p == pe && p != eof )
			goto _out243;
		if ( p == eof ) {
			goto _ctr319;}
		else {
			if ( ( (*( p))) == 44 ) {
				goto _ctr320;
			}
			if ( 48 <= ( (*( p))) && ( (*( p))) <= 57 ) {
				goto _st243;
			}
			goto _st0;
		}
		_ctr320:
		{
#line 165 "cfg.rl"
			
			if (auto t = nk::from_string<int>(fas.intv_st, p - fas.intv_st)) fas.v_int = *t; else {
				log_line("invalid value where integer is expected on line %zu", fas.linenum);
				std::exit(EXIT_FAILURE);
			}
		}
		
#line 5019 "cfg.cpp"
		
		goto _st122;
		_st122:
		if ( p == eof )
			goto _out122;
		p+= 1;
		st_case_122:
		if ( p == pe && p != eof )
			goto _out122;
		if ( p == eof ) {
			goto _st122;}
		else {
			if ( 48 <= ( (*( p))) && ( (*( p))) <= 57 ) {
				goto _ctr163;
			}
			goto _st0;
		}
		_ctr163:
		{
#line 171 "cfg.rl"
			fas.intv2_st = p; }
		
#line 5042 "cfg.cpp"
		
		goto _st244;
		_ctr322:
		{
#line 172 "cfg.rl"
			
			if (auto t = nk::from_string<int>(fas.intv2_st, p - fas.intv2_st)) fas.v_int2 = *t; {
				log_line("invalid value where second integer in range is expected on line %zu", fas.linenum);
				std::exit(EXIT_FAILURE);
			}
			fas.intv2_exist = true;
		}
		
#line 5056 "cfg.cpp"
		
		{
#line 206 "cfg.rl"
			fas.setlim(RLIMIT_FSIZE); }
		
#line 5062 "cfg.cpp"
		
		goto _st244;
		_st244:
		if ( p == eof )
			goto _out244;
		p+= 1;
		st_case_244:
		if ( p == pe && p != eof )
			goto _out244;
		if ( p == eof ) {
			goto _ctr322;}
		else {
			if ( 48 <= ( (*( p))) && ( (*( p))) <= 57 ) {
				goto _st244;
			}
			goto _st0;
		}
		_st123:
		if ( p == eof )
			goto _out123;
		p+= 1;
		st_case_123:
		if ( p == pe && p != eof )
			goto _out123;
		if ( p == eof ) {
			goto _st123;}
		else {
			switch( ( (*( p))) ) {
				case 69: {
					goto _st124;
				}
				case 83: {
					goto _st132;
				}
				case 101: {
					goto _st124;
				}
				case 115: {
					goto _st132;
				}
			}
			goto _st0;
		}
		_st124:
		if ( p == eof )
			goto _out124;
		p+= 1;
		st_case_124:
		if ( p == pe && p != eof )
			goto _out124;
		if ( p == eof ) {
			goto _st124;}
		else {
			switch( ( (*( p))) ) {
				case 77: {
					goto _st125;
				}
				case 109: {
					goto _st125;
				}
			}
			goto _st0;
		}
		_st125:
		if ( p == eof )
			goto _out125;
		p+= 1;
		st_case_125:
		if ( p == pe && p != eof )
			goto _out125;
		if ( p == eof ) {
			goto _st125;}
		else {
			switch( ( (*( p))) ) {
				case 76: {
					goto _st126;
				}
				case 108: {
					goto _st126;
				}
			}
			goto _st0;
		}
		_st126:
		if ( p == eof )
			goto _out126;
		p+= 1;
		st_case_126:
		if ( p == pe && p != eof )
			goto _out126;
		if ( p == eof ) {
			goto _st126;}
		else {
			switch( ( (*( p))) ) {
				case 79: {
					goto _st127;
				}
				case 111: {
					goto _st127;
				}
			}
			goto _st0;
		}
		_st127:
		if ( p == eof )
			goto _out127;
		p+= 1;
		st_case_127:
		if ( p == pe && p != eof )
			goto _out127;
		if ( p == eof ) {
			goto _st127;}
		else {
			switch( ( (*( p))) ) {
				case 67: {
					goto _st128;
				}
				case 99: {
					goto _st128;
				}
			}
			goto _st0;
		}
		_st128:
		if ( p == eof )
			goto _out128;
		p+= 1;
		st_case_128:
		if ( p == pe && p != eof )
			goto _out128;
		if ( p == eof ) {
			goto _st128;}
		else {
			switch( ( (*( p))) ) {
				case 75: {
					goto _st129;
				}
				case 107: {
					goto _st129;
				}
			}
			goto _st0;
		}
		_st129:
		if ( p == eof )
			goto _out129;
		p+= 1;
		st_case_129:
		if ( p == pe && p != eof )
			goto _out129;
		if ( p == eof ) {
			goto _st129;}
		else {
			switch( ( (*( p))) ) {
				case 9: {
					goto _st129;
				}
				case 32: {
					goto _st129;
				}
				case 61: {
					goto _st130;
				}
			}
			goto _st0;
		}
		_st130:
		if ( p == eof )
			goto _out130;
		p+= 1;
		st_case_130:
		if ( p == pe && p != eof )
			goto _out130;
		if ( p == eof ) {
			goto _st130;}
		else {
			switch( ( (*( p))) ) {
				case 9: {
					goto _st130;
				}
				case 32: {
					goto _st130;
				}
			}
			if ( 48 <= ( (*( p))) && ( (*( p))) <= 57 ) {
				goto _ctr172;
			}
			goto _st0;
		}
		_ctr172:
		{
#line 160 "cfg.rl"
			
			fas.intv_st = p;
			fas.v_int = fas.v_int2 = 0;
			fas.intv2_exist = false;
		}
		
#line 5261 "cfg.cpp"
		
		goto _st245;
		_ctr324:
		{
#line 165 "cfg.rl"
			
			if (auto t = nk::from_string<int>(fas.intv_st, p - fas.intv_st)) fas.v_int = *t; else {
				log_line("invalid value where integer is expected on line %zu", fas.linenum);
				std::exit(EXIT_FAILURE);
			}
		}
		
#line 5274 "cfg.cpp"
		
		{
#line 199 "cfg.rl"
			fas.setlim(RLIMIT_MEMLOCK); }
		
#line 5280 "cfg.cpp"
		
		goto _st245;
		_st245:
		if ( p == eof )
			goto _out245;
		p+= 1;
		st_case_245:
		if ( p == pe && p != eof )
			goto _out245;
		if ( p == eof ) {
			goto _ctr324;}
		else {
			if ( ( (*( p))) == 44 ) {
				goto _ctr325;
			}
			if ( 48 <= ( (*( p))) && ( (*( p))) <= 57 ) {
				goto _st245;
			}
			goto _st0;
		}
		_ctr325:
		{
#line 165 "cfg.rl"
			
			if (auto t = nk::from_string<int>(fas.intv_st, p - fas.intv_st)) fas.v_int = *t; else {
				log_line("invalid value where integer is expected on line %zu", fas.linenum);
				std::exit(EXIT_FAILURE);
			}
		}
		
#line 5311 "cfg.cpp"
		
		goto _st131;
		_st131:
		if ( p == eof )
			goto _out131;
		p+= 1;
		st_case_131:
		if ( p == pe && p != eof )
			goto _out131;
		if ( p == eof ) {
			goto _st131;}
		else {
			if ( 48 <= ( (*( p))) && ( (*( p))) <= 57 ) {
				goto _ctr174;
			}
			goto _st0;
		}
		_ctr174:
		{
#line 171 "cfg.rl"
			fas.intv2_st = p; }
		
#line 5334 "cfg.cpp"
		
		goto _st246;
		_ctr327:
		{
#line 172 "cfg.rl"
			
			if (auto t = nk::from_string<int>(fas.intv2_st, p - fas.intv2_st)) fas.v_int2 = *t; {
				log_line("invalid value where second integer in range is expected on line %zu", fas.linenum);
				std::exit(EXIT_FAILURE);
			}
			fas.intv2_exist = true;
		}
		
#line 5348 "cfg.cpp"
		
		{
#line 199 "cfg.rl"
			fas.setlim(RLIMIT_MEMLOCK); }
		
#line 5354 "cfg.cpp"
		
		goto _st246;
		_st246:
		if ( p == eof )
			goto _out246;
		p+= 1;
		st_case_246:
		if ( p == pe && p != eof )
			goto _out246;
		if ( p == eof ) {
			goto _ctr327;}
		else {
			if ( 48 <= ( (*( p))) && ( (*( p))) <= 57 ) {
				goto _st246;
			}
			goto _st0;
		}
		_st132:
		if ( p == eof )
			goto _out132;
		p+= 1;
		st_case_132:
		if ( p == pe && p != eof )
			goto _out132;
		if ( p == eof ) {
			goto _st132;}
		else {
			switch( ( (*( p))) ) {
				case 71: {
					goto _st133;
				}
				case 103: {
					goto _st133;
				}
			}
			goto _st0;
		}
		_st133:
		if ( p == eof )
			goto _out133;
		p+= 1;
		st_case_133:
		if ( p == pe && p != eof )
			goto _out133;
		if ( p == eof ) {
			goto _st133;}
		else {
			switch( ( (*( p))) ) {
				case 81: {
					goto _st134;
				}
				case 113: {
					goto _st134;
				}
			}
			goto _st0;
		}
		_st134:
		if ( p == eof )
			goto _out134;
		p+= 1;
		st_case_134:
		if ( p == pe && p != eof )
			goto _out134;
		if ( p == eof ) {
			goto _st134;}
		else {
			switch( ( (*( p))) ) {
				case 85: {
					goto _st135;
				}
				case 117: {
					goto _st135;
				}
			}
			goto _st0;
		}
		_st135:
		if ( p == eof )
			goto _out135;
		p+= 1;
		st_case_135:
		if ( p == pe && p != eof )
			goto _out135;
		if ( p == eof ) {
			goto _st135;}
		else {
			switch( ( (*( p))) ) {
				case 69: {
					goto _st136;
				}
				case 101: {
					goto _st136;
				}
			}
			goto _st0;
		}
		_st136:
		if ( p == eof )
			goto _out136;
		p+= 1;
		st_case_136:
		if ( p == pe && p != eof )
			goto _out136;
		if ( p == eof ) {
			goto _st136;}
		else {
			switch( ( (*( p))) ) {
				case 85: {
					goto _st137;
				}
				case 117: {
					goto _st137;
				}
			}
			goto _st0;
		}
		_st137:
		if ( p == eof )
			goto _out137;
		p+= 1;
		st_case_137:
		if ( p == pe && p != eof )
			goto _out137;
		if ( p == eof ) {
			goto _st137;}
		else {
			switch( ( (*( p))) ) {
				case 69: {
					goto _st138;
				}
				case 101: {
					goto _st138;
				}
			}
			goto _st0;
		}
		_st138:
		if ( p == eof )
			goto _out138;
		p+= 1;
		st_case_138:
		if ( p == pe && p != eof )
			goto _out138;
		if ( p == eof ) {
			goto _st138;}
		else {
			switch( ( (*( p))) ) {
				case 9: {
					goto _st138;
				}
				case 32: {
					goto _st138;
				}
				case 61: {
					goto _st139;
				}
			}
			goto _st0;
		}
		_st139:
		if ( p == eof )
			goto _out139;
		p+= 1;
		st_case_139:
		if ( p == pe && p != eof )
			goto _out139;
		if ( p == eof ) {
			goto _st139;}
		else {
			switch( ( (*( p))) ) {
				case 9: {
					goto _st139;
				}
				case 32: {
					goto _st139;
				}
			}
			if ( 48 <= ( (*( p))) && ( (*( p))) <= 57 ) {
				goto _ctr182;
			}
			goto _st0;
		}
		_ctr182:
		{
#line 160 "cfg.rl"
			
			fas.intv_st = p;
			fas.v_int = fas.v_int2 = 0;
			fas.intv2_exist = false;
		}
		
#line 5547 "cfg.cpp"
		
		goto _st247;
		_ctr329:
		{
#line 165 "cfg.rl"
			
			if (auto t = nk::from_string<int>(fas.intv_st, p - fas.intv_st)) fas.v_int = *t; else {
				log_line("invalid value where integer is expected on line %zu", fas.linenum);
				std::exit(EXIT_FAILURE);
			}
		}
		
#line 5560 "cfg.cpp"
		
		{
#line 208 "cfg.rl"
			fas.setlim(RLIMIT_MSGQUEUE); }
		
#line 5566 "cfg.cpp"
		
		goto _st247;
		_st247:
		if ( p == eof )
			goto _out247;
		p+= 1;
		st_case_247:
		if ( p == pe && p != eof )
			goto _out247;
		if ( p == eof ) {
			goto _ctr329;}
		else {
			if ( ( (*( p))) == 44 ) {
				goto _ctr330;
			}
			if ( 48 <= ( (*( p))) && ( (*( p))) <= 57 ) {
				goto _st247;
			}
			goto _st0;
		}
		_ctr330:
		{
#line 165 "cfg.rl"
			
			if (auto t = nk::from_string<int>(fas.intv_st, p - fas.intv_st)) fas.v_int = *t; else {
				log_line("invalid value where integer is expected on line %zu", fas.linenum);
				std::exit(EXIT_FAILURE);
			}
		}
		
#line 5597 "cfg.cpp"
		
		goto _st140;
		_st140:
		if ( p == eof )
			goto _out140;
		p+= 1;
		st_case_140:
		if ( p == pe && p != eof )
			goto _out140;
		if ( p == eof ) {
			goto _st140;}
		else {
			if ( 48 <= ( (*( p))) && ( (*( p))) <= 57 ) {
				goto _ctr184;
			}
			goto _st0;
		}
		_ctr184:
		{
#line 171 "cfg.rl"
			fas.intv2_st = p; }
		
#line 5620 "cfg.cpp"
		
		goto _st248;
		_ctr332:
		{
#line 172 "cfg.rl"
			
			if (auto t = nk::from_string<int>(fas.intv2_st, p - fas.intv2_st)) fas.v_int2 = *t; {
				log_line("invalid value where second integer in range is expected on line %zu", fas.linenum);
				std::exit(EXIT_FAILURE);
			}
			fas.intv2_exist = true;
		}
		
#line 5634 "cfg.cpp"
		
		{
#line 208 "cfg.rl"
			fas.setlim(RLIMIT_MSGQUEUE); }
		
#line 5640 "cfg.cpp"
		
		goto _st248;
		_st248:
		if ( p == eof )
			goto _out248;
		p+= 1;
		st_case_248:
		if ( p == pe && p != eof )
			goto _out248;
		if ( p == eof ) {
			goto _ctr332;}
		else {
			if ( 48 <= ( (*( p))) && ( (*( p))) <= 57 ) {
				goto _st248;
			}
			goto _st0;
		}
		_st141:
		if ( p == eof )
			goto _out141;
		p+= 1;
		st_case_141:
		if ( p == pe && p != eof )
			goto _out141;
		if ( p == eof ) {
			goto _st141;}
		else {
			switch( ( (*( p))) ) {
				case 73: {
					goto _st142;
				}
				case 79: {
					goto _st147;
				}
				case 80: {
					goto _st154;
				}
				case 105: {
					goto _st142;
				}
				case 111: {
					goto _st147;
				}
				case 112: {
					goto _st154;
				}
			}
			goto _st0;
		}
		_st142:
		if ( p == eof )
			goto _out142;
		p+= 1;
		st_case_142:
		if ( p == pe && p != eof )
			goto _out142;
		if ( p == eof ) {
			goto _st142;}
		else {
			switch( ( (*( p))) ) {
				case 67: {
					goto _st143;
				}
				case 99: {
					goto _st143;
				}
			}
			goto _st0;
		}
		_st143:
		if ( p == eof )
			goto _out143;
		p+= 1;
		st_case_143:
		if ( p == pe && p != eof )
			goto _out143;
		if ( p == eof ) {
			goto _st143;}
		else {
			switch( ( (*( p))) ) {
				case 69: {
					goto _st144;
				}
				case 101: {
					goto _st144;
				}
			}
			goto _st0;
		}
		_st144:
		if ( p == eof )
			goto _out144;
		p+= 1;
		st_case_144:
		if ( p == pe && p != eof )
			goto _out144;
		if ( p == eof ) {
			goto _st144;}
		else {
			switch( ( (*( p))) ) {
				case 9: {
					goto _st144;
				}
				case 32: {
					goto _st144;
				}
				case 61: {
					goto _st145;
				}
			}
			goto _st0;
		}
		_st145:
		if ( p == eof )
			goto _out145;
		p+= 1;
		st_case_145:
		if ( p == pe && p != eof )
			goto _out145;
		if ( p == eof ) {
			goto _st145;}
		else {
			switch( ( (*( p))) ) {
				case 9: {
					goto _st145;
				}
				case 32: {
					goto _st145;
				}
			}
			if ( 48 <= ( (*( p))) && ( (*( p))) <= 57 ) {
				goto _ctr191;
			}
			goto _st0;
		}
		_ctr191:
		{
#line 160 "cfg.rl"
			
			fas.intv_st = p;
			fas.v_int = fas.v_int2 = 0;
			fas.intv2_exist = false;
		}
		
#line 5785 "cfg.cpp"
		
		goto _st249;
		_ctr334:
		{
#line 165 "cfg.rl"
			
			if (auto t = nk::from_string<int>(fas.intv_st, p - fas.intv_st)) fas.v_int = *t; else {
				log_line("invalid value where integer is expected on line %zu", fas.linenum);
				std::exit(EXIT_FAILURE);
			}
		}
		
#line 5798 "cfg.cpp"
		
		{
#line 209 "cfg.rl"
			fas.setlim(RLIMIT_NICE); }
		
#line 5804 "cfg.cpp"
		
		goto _st249;
		_st249:
		if ( p == eof )
			goto _out249;
		p+= 1;
		st_case_249:
		if ( p == pe && p != eof )
			goto _out249;
		if ( p == eof ) {
			goto _ctr334;}
		else {
			if ( ( (*( p))) == 44 ) {
				goto _ctr335;
			}
			if ( 48 <= ( (*( p))) && ( (*( p))) <= 57 ) {
				goto _st249;
			}
			goto _st0;
		}
		_ctr335:
		{
#line 165 "cfg.rl"
			
			if (auto t = nk::from_string<int>(fas.intv_st, p - fas.intv_st)) fas.v_int = *t; else {
				log_line("invalid value where integer is expected on line %zu", fas.linenum);
				std::exit(EXIT_FAILURE);
			}
		}
		
#line 5835 "cfg.cpp"
		
		goto _st146;
		_st146:
		if ( p == eof )
			goto _out146;
		p+= 1;
		st_case_146:
		if ( p == pe && p != eof )
			goto _out146;
		if ( p == eof ) {
			goto _st146;}
		else {
			if ( 48 <= ( (*( p))) && ( (*( p))) <= 57 ) {
				goto _ctr193;
			}
			goto _st0;
		}
		_ctr193:
		{
#line 171 "cfg.rl"
			fas.intv2_st = p; }
		
#line 5858 "cfg.cpp"
		
		goto _st250;
		_ctr337:
		{
#line 172 "cfg.rl"
			
			if (auto t = nk::from_string<int>(fas.intv2_st, p - fas.intv2_st)) fas.v_int2 = *t; {
				log_line("invalid value where second integer in range is expected on line %zu", fas.linenum);
				std::exit(EXIT_FAILURE);
			}
			fas.intv2_exist = true;
		}
		
#line 5872 "cfg.cpp"
		
		{
#line 209 "cfg.rl"
			fas.setlim(RLIMIT_NICE); }
		
#line 5878 "cfg.cpp"
		
		goto _st250;
		_st250:
		if ( p == eof )
			goto _out250;
		p+= 1;
		st_case_250:
		if ( p == pe && p != eof )
			goto _out250;
		if ( p == eof ) {
			goto _ctr337;}
		else {
			if ( 48 <= ( (*( p))) && ( (*( p))) <= 57 ) {
				goto _st250;
			}
			goto _st0;
		}
		_st147:
		if ( p == eof )
			goto _out147;
		p+= 1;
		st_case_147:
		if ( p == pe && p != eof )
			goto _out147;
		if ( p == eof ) {
			goto _st147;}
		else {
			switch( ( (*( p))) ) {
				case 70: {
					goto _st148;
				}
				case 102: {
					goto _st148;
				}
			}
			goto _st0;
		}
		_st148:
		if ( p == eof )
			goto _out148;
		p+= 1;
		st_case_148:
		if ( p == pe && p != eof )
			goto _out148;
		if ( p == eof ) {
			goto _st148;}
		else {
			switch( ( (*( p))) ) {
				case 73: {
					goto _st149;
				}
				case 105: {
					goto _st149;
				}
			}
			goto _st0;
		}
		_st149:
		if ( p == eof )
			goto _out149;
		p+= 1;
		st_case_149:
		if ( p == pe && p != eof )
			goto _out149;
		if ( p == eof ) {
			goto _st149;}
		else {
			switch( ( (*( p))) ) {
				case 76: {
					goto _st150;
				}
				case 108: {
					goto _st150;
				}
			}
			goto _st0;
		}
		_st150:
		if ( p == eof )
			goto _out150;
		p+= 1;
		st_case_150:
		if ( p == pe && p != eof )
			goto _out150;
		if ( p == eof ) {
			goto _st150;}
		else {
			switch( ( (*( p))) ) {
				case 69: {
					goto _st151;
				}
				case 101: {
					goto _st151;
				}
			}
			goto _st0;
		}
		_st151:
		if ( p == eof )
			goto _out151;
		p+= 1;
		st_case_151:
		if ( p == pe && p != eof )
			goto _out151;
		if ( p == eof ) {
			goto _st151;}
		else {
			switch( ( (*( p))) ) {
				case 9: {
					goto _st151;
				}
				case 32: {
					goto _st151;
				}
				case 61: {
					goto _st152;
				}
			}
			goto _st0;
		}
		_st152:
		if ( p == eof )
			goto _out152;
		p+= 1;
		st_case_152:
		if ( p == pe && p != eof )
			goto _out152;
		if ( p == eof ) {
			goto _st152;}
		else {
			switch( ( (*( p))) ) {
				case 9: {
					goto _st152;
				}
				case 32: {
					goto _st152;
				}
			}
			if ( 48 <= ( (*( p))) && ( (*( p))) <= 57 ) {
				goto _ctr199;
			}
			goto _st0;
		}
		_ctr199:
		{
#line 160 "cfg.rl"
			
			fas.intv_st = p;
			fas.v_int = fas.v_int2 = 0;
			fas.intv2_exist = false;
		}
		
#line 6031 "cfg.cpp"
		
		goto _st251;
		_ctr339:
		{
#line 165 "cfg.rl"
			
			if (auto t = nk::from_string<int>(fas.intv_st, p - fas.intv_st)) fas.v_int = *t; else {
				log_line("invalid value where integer is expected on line %zu", fas.linenum);
				std::exit(EXIT_FAILURE);
			}
		}
		
#line 6044 "cfg.cpp"
		
		{
#line 200 "cfg.rl"
			fas.setlim(RLIMIT_NOFILE); }
		
#line 6050 "cfg.cpp"
		
		goto _st251;
		_st251:
		if ( p == eof )
			goto _out251;
		p+= 1;
		st_case_251:
		if ( p == pe && p != eof )
			goto _out251;
		if ( p == eof ) {
			goto _ctr339;}
		else {
			if ( ( (*( p))) == 44 ) {
				goto _ctr340;
			}
			if ( 48 <= ( (*( p))) && ( (*( p))) <= 57 ) {
				goto _st251;
			}
			goto _st0;
		}
		_ctr340:
		{
#line 165 "cfg.rl"
			
			if (auto t = nk::from_string<int>(fas.intv_st, p - fas.intv_st)) fas.v_int = *t; else {
				log_line("invalid value where integer is expected on line %zu", fas.linenum);
				std::exit(EXIT_FAILURE);
			}
		}
		
#line 6081 "cfg.cpp"
		
		goto _st153;
		_st153:
		if ( p == eof )
			goto _out153;
		p+= 1;
		st_case_153:
		if ( p == pe && p != eof )
			goto _out153;
		if ( p == eof ) {
			goto _st153;}
		else {
			if ( 48 <= ( (*( p))) && ( (*( p))) <= 57 ) {
				goto _ctr201;
			}
			goto _st0;
		}
		_ctr201:
		{
#line 171 "cfg.rl"
			fas.intv2_st = p; }
		
#line 6104 "cfg.cpp"
		
		goto _st252;
		_ctr342:
		{
#line 172 "cfg.rl"
			
			if (auto t = nk::from_string<int>(fas.intv2_st, p - fas.intv2_st)) fas.v_int2 = *t; {
				log_line("invalid value where second integer in range is expected on line %zu", fas.linenum);
				std::exit(EXIT_FAILURE);
			}
			fas.intv2_exist = true;
		}
		
#line 6118 "cfg.cpp"
		
		{
#line 200 "cfg.rl"
			fas.setlim(RLIMIT_NOFILE); }
		
#line 6124 "cfg.cpp"
		
		goto _st252;
		_st252:
		if ( p == eof )
			goto _out252;
		p+= 1;
		st_case_252:
		if ( p == pe && p != eof )
			goto _out252;
		if ( p == eof ) {
			goto _ctr342;}
		else {
			if ( 48 <= ( (*( p))) && ( (*( p))) <= 57 ) {
				goto _st252;
			}
			goto _st0;
		}
		_st154:
		if ( p == eof )
			goto _out154;
		p+= 1;
		st_case_154:
		if ( p == pe && p != eof )
			goto _out154;
		if ( p == eof ) {
			goto _st154;}
		else {
			switch( ( (*( p))) ) {
				case 82: {
					goto _st155;
				}
				case 114: {
					goto _st155;
				}
			}
			goto _st0;
		}
		_st155:
		if ( p == eof )
			goto _out155;
		p+= 1;
		st_case_155:
		if ( p == pe && p != eof )
			goto _out155;
		if ( p == eof ) {
			goto _st155;}
		else {
			switch( ( (*( p))) ) {
				case 79: {
					goto _st156;
				}
				case 111: {
					goto _st156;
				}
			}
			goto _st0;
		}
		_st156:
		if ( p == eof )
			goto _out156;
		p+= 1;
		st_case_156:
		if ( p == pe && p != eof )
			goto _out156;
		if ( p == eof ) {
			goto _st156;}
		else {
			switch( ( (*( p))) ) {
				case 67: {
					goto _st157;
				}
				case 99: {
					goto _st157;
				}
			}
			goto _st0;
		}
		_st157:
		if ( p == eof )
			goto _out157;
		p+= 1;
		st_case_157:
		if ( p == pe && p != eof )
			goto _out157;
		if ( p == eof ) {
			goto _st157;}
		else {
			switch( ( (*( p))) ) {
				case 9: {
					goto _st157;
				}
				case 32: {
					goto _st157;
				}
				case 61: {
					goto _st158;
				}
			}
			goto _st0;
		}
		_st158:
		if ( p == eof )
			goto _out158;
		p+= 1;
		st_case_158:
		if ( p == pe && p != eof )
			goto _out158;
		if ( p == eof ) {
			goto _st158;}
		else {
			switch( ( (*( p))) ) {
				case 9: {
					goto _st158;
				}
				case 32: {
					goto _st158;
				}
			}
			if ( 48 <= ( (*( p))) && ( (*( p))) <= 57 ) {
				goto _ctr206;
			}
			goto _st0;
		}
		_ctr206:
		{
#line 160 "cfg.rl"
			
			fas.intv_st = p;
			fas.v_int = fas.v_int2 = 0;
			fas.intv2_exist = false;
		}
		
#line 6257 "cfg.cpp"
		
		goto _st253;
		_ctr344:
		{
#line 165 "cfg.rl"
			
			if (auto t = nk::from_string<int>(fas.intv_st, p - fas.intv_st)) fas.v_int = *t; else {
				log_line("invalid value where integer is expected on line %zu", fas.linenum);
				std::exit(EXIT_FAILURE);
			}
		}
		
#line 6270 "cfg.cpp"
		
		{
#line 201 "cfg.rl"
			fas.setlim(RLIMIT_NPROC); }
		
#line 6276 "cfg.cpp"
		
		goto _st253;
		_st253:
		if ( p == eof )
			goto _out253;
		p+= 1;
		st_case_253:
		if ( p == pe && p != eof )
			goto _out253;
		if ( p == eof ) {
			goto _ctr344;}
		else {
			if ( ( (*( p))) == 44 ) {
				goto _ctr345;
			}
			if ( 48 <= ( (*( p))) && ( (*( p))) <= 57 ) {
				goto _st253;
			}
			goto _st0;
		}
		_ctr345:
		{
#line 165 "cfg.rl"
			
			if (auto t = nk::from_string<int>(fas.intv_st, p - fas.intv_st)) fas.v_int = *t; else {
				log_line("invalid value where integer is expected on line %zu", fas.linenum);
				std::exit(EXIT_FAILURE);
			}
		}
		
#line 6307 "cfg.cpp"
		
		goto _st159;
		_st159:
		if ( p == eof )
			goto _out159;
		p+= 1;
		st_case_159:
		if ( p == pe && p != eof )
			goto _out159;
		if ( p == eof ) {
			goto _st159;}
		else {
			if ( 48 <= ( (*( p))) && ( (*( p))) <= 57 ) {
				goto _ctr208;
			}
			goto _st0;
		}
		_ctr208:
		{
#line 171 "cfg.rl"
			fas.intv2_st = p; }
		
#line 6330 "cfg.cpp"
		
		goto _st254;
		_ctr347:
		{
#line 172 "cfg.rl"
			
			if (auto t = nk::from_string<int>(fas.intv2_st, p - fas.intv2_st)) fas.v_int2 = *t; {
				log_line("invalid value where second integer in range is expected on line %zu", fas.linenum);
				std::exit(EXIT_FAILURE);
			}
			fas.intv2_exist = true;
		}
		
#line 6344 "cfg.cpp"
		
		{
#line 201 "cfg.rl"
			fas.setlim(RLIMIT_NPROC); }
		
#line 6350 "cfg.cpp"
		
		goto _st254;
		_st254:
		if ( p == eof )
			goto _out254;
		p+= 1;
		st_case_254:
		if ( p == pe && p != eof )
			goto _out254;
		if ( p == eof ) {
			goto _ctr347;}
		else {
			if ( 48 <= ( (*( p))) && ( (*( p))) <= 57 ) {
				goto _st254;
			}
			goto _st0;
		}
		_st160:
		if ( p == eof )
			goto _out160;
		p+= 1;
		st_case_160:
		if ( p == pe && p != eof )
			goto _out160;
		if ( p == eof ) {
			goto _st160;}
		else {
			switch( ( (*( p))) ) {
				case 83: {
					goto _st161;
				}
				case 84: {
					goto _st165;
				}
				case 115: {
					goto _st161;
				}
				case 116: {
					goto _st165;
				}
			}
			goto _st0;
		}
		_st161:
		if ( p == eof )
			goto _out161;
		p+= 1;
		st_case_161:
		if ( p == pe && p != eof )
			goto _out161;
		if ( p == eof ) {
			goto _st161;}
		else {
			switch( ( (*( p))) ) {
				case 83: {
					goto _st162;
				}
				case 115: {
					goto _st162;
				}
			}
			goto _st0;
		}
		_st162:
		if ( p == eof )
			goto _out162;
		p+= 1;
		st_case_162:
		if ( p == pe && p != eof )
			goto _out162;
		if ( p == eof ) {
			goto _st162;}
		else {
			switch( ( (*( p))) ) {
				case 9: {
					goto _st162;
				}
				case 32: {
					goto _st162;
				}
				case 61: {
					goto _st163;
				}
			}
			goto _st0;
		}
		_st163:
		if ( p == eof )
			goto _out163;
		p+= 1;
		st_case_163:
		if ( p == pe && p != eof )
			goto _out163;
		if ( p == eof ) {
			goto _st163;}
		else {
			switch( ( (*( p))) ) {
				case 9: {
					goto _st163;
				}
				case 32: {
					goto _st163;
				}
			}
			if ( 48 <= ( (*( p))) && ( (*( p))) <= 57 ) {
				goto _ctr213;
			}
			goto _st0;
		}
		_ctr213:
		{
#line 160 "cfg.rl"
			
			fas.intv_st = p;
			fas.v_int = fas.v_int2 = 0;
			fas.intv2_exist = false;
		}
		
#line 6469 "cfg.cpp"
		
		goto _st255;
		_ctr349:
		{
#line 165 "cfg.rl"
			
			if (auto t = nk::from_string<int>(fas.intv_st, p - fas.intv_st)) fas.v_int = *t; else {
				log_line("invalid value where integer is expected on line %zu", fas.linenum);
				std::exit(EXIT_FAILURE);
			}
		}
		
#line 6482 "cfg.cpp"
		
		{
#line 202 "cfg.rl"
			fas.setlim(RLIMIT_RSS); }
		
#line 6488 "cfg.cpp"
		
		goto _st255;
		_st255:
		if ( p == eof )
			goto _out255;
		p+= 1;
		st_case_255:
		if ( p == pe && p != eof )
			goto _out255;
		if ( p == eof ) {
			goto _ctr349;}
		else {
			if ( ( (*( p))) == 44 ) {
				goto _ctr350;
			}
			if ( 48 <= ( (*( p))) && ( (*( p))) <= 57 ) {
				goto _st255;
			}
			goto _st0;
		}
		_ctr350:
		{
#line 165 "cfg.rl"
			
			if (auto t = nk::from_string<int>(fas.intv_st, p - fas.intv_st)) fas.v_int = *t; else {
				log_line("invalid value where integer is expected on line %zu", fas.linenum);
				std::exit(EXIT_FAILURE);
			}
		}
		
#line 6519 "cfg.cpp"
		
		goto _st164;
		_st164:
		if ( p == eof )
			goto _out164;
		p+= 1;
		st_case_164:
		if ( p == pe && p != eof )
			goto _out164;
		if ( p == eof ) {
			goto _st164;}
		else {
			if ( 48 <= ( (*( p))) && ( (*( p))) <= 57 ) {
				goto _ctr215;
			}
			goto _st0;
		}
		_ctr215:
		{
#line 171 "cfg.rl"
			fas.intv2_st = p; }
		
#line 6542 "cfg.cpp"
		
		goto _st256;
		_ctr352:
		{
#line 172 "cfg.rl"
			
			if (auto t = nk::from_string<int>(fas.intv2_st, p - fas.intv2_st)) fas.v_int2 = *t; {
				log_line("invalid value where second integer in range is expected on line %zu", fas.linenum);
				std::exit(EXIT_FAILURE);
			}
			fas.intv2_exist = true;
		}
		
#line 6556 "cfg.cpp"
		
		{
#line 202 "cfg.rl"
			fas.setlim(RLIMIT_RSS); }
		
#line 6562 "cfg.cpp"
		
		goto _st256;
		_st256:
		if ( p == eof )
			goto _out256;
		p+= 1;
		st_case_256:
		if ( p == pe && p != eof )
			goto _out256;
		if ( p == eof ) {
			goto _ctr352;}
		else {
			if ( 48 <= ( (*( p))) && ( (*( p))) <= 57 ) {
				goto _st256;
			}
			goto _st0;
		}
		_st165:
		if ( p == eof )
			goto _out165;
		p+= 1;
		st_case_165:
		if ( p == pe && p != eof )
			goto _out165;
		if ( p == eof ) {
			goto _st165;}
		else {
			switch( ( (*( p))) ) {
				case 80: {
					goto _st166;
				}
				case 84: {
					goto _st172;
				}
				case 112: {
					goto _st166;
				}
				case 116: {
					goto _st172;
				}
			}
			goto _st0;
		}
		_st166:
		if ( p == eof )
			goto _out166;
		p+= 1;
		st_case_166:
		if ( p == pe && p != eof )
			goto _out166;
		if ( p == eof ) {
			goto _st166;}
		else {
			switch( ( (*( p))) ) {
				case 82: {
					goto _st167;
				}
				case 114: {
					goto _st167;
				}
			}
			goto _st0;
		}
		_st167:
		if ( p == eof )
			goto _out167;
		p+= 1;
		st_case_167:
		if ( p == pe && p != eof )
			goto _out167;
		if ( p == eof ) {
			goto _st167;}
		else {
			switch( ( (*( p))) ) {
				case 73: {
					goto _st168;
				}
				case 105: {
					goto _st168;
				}
			}
			goto _st0;
		}
		_st168:
		if ( p == eof )
			goto _out168;
		p+= 1;
		st_case_168:
		if ( p == pe && p != eof )
			goto _out168;
		if ( p == eof ) {
			goto _st168;}
		else {
			switch( ( (*( p))) ) {
				case 79: {
					goto _st169;
				}
				case 111: {
					goto _st169;
				}
			}
			goto _st0;
		}
		_st169:
		if ( p == eof )
			goto _out169;
		p+= 1;
		st_case_169:
		if ( p == pe && p != eof )
			goto _out169;
		if ( p == eof ) {
			goto _st169;}
		else {
			switch( ( (*( p))) ) {
				case 9: {
					goto _st169;
				}
				case 32: {
					goto _st169;
				}
				case 61: {
					goto _st170;
				}
			}
			goto _st0;
		}
		_st170:
		if ( p == eof )
			goto _out170;
		p+= 1;
		st_case_170:
		if ( p == pe && p != eof )
			goto _out170;
		if ( p == eof ) {
			goto _st170;}
		else {
			switch( ( (*( p))) ) {
				case 9: {
					goto _st170;
				}
				case 32: {
					goto _st170;
				}
			}
			if ( 48 <= ( (*( p))) && ( (*( p))) <= 57 ) {
				goto _ctr222;
			}
			goto _st0;
		}
		_ctr222:
		{
#line 160 "cfg.rl"
			
			fas.intv_st = p;
			fas.v_int = fas.v_int2 = 0;
			fas.intv2_exist = false;
		}
		
#line 6721 "cfg.cpp"
		
		goto _st257;
		_ctr354:
		{
#line 165 "cfg.rl"
			
			if (auto t = nk::from_string<int>(fas.intv_st, p - fas.intv_st)) fas.v_int = *t; else {
				log_line("invalid value where integer is expected on line %zu", fas.linenum);
				std::exit(EXIT_FAILURE);
			}
		}
		
#line 6734 "cfg.cpp"
		
		{
#line 211 "cfg.rl"
			fas.setlim(RLIMIT_RTPRIO); }
		
#line 6740 "cfg.cpp"
		
		goto _st257;
		_st257:
		if ( p == eof )
			goto _out257;
		p+= 1;
		st_case_257:
		if ( p == pe && p != eof )
			goto _out257;
		if ( p == eof ) {
			goto _ctr354;}
		else {
			if ( ( (*( p))) == 44 ) {
				goto _ctr355;
			}
			if ( 48 <= ( (*( p))) && ( (*( p))) <= 57 ) {
				goto _st257;
			}
			goto _st0;
		}
		_ctr355:
		{
#line 165 "cfg.rl"
			
			if (auto t = nk::from_string<int>(fas.intv_st, p - fas.intv_st)) fas.v_int = *t; else {
				log_line("invalid value where integer is expected on line %zu", fas.linenum);
				std::exit(EXIT_FAILURE);
			}
		}
		
#line 6771 "cfg.cpp"
		
		goto _st171;
		_st171:
		if ( p == eof )
			goto _out171;
		p+= 1;
		st_case_171:
		if ( p == pe && p != eof )
			goto _out171;
		if ( p == eof ) {
			goto _st171;}
		else {
			if ( 48 <= ( (*( p))) && ( (*( p))) <= 57 ) {
				goto _ctr224;
			}
			goto _st0;
		}
		_ctr224:
		{
#line 171 "cfg.rl"
			fas.intv2_st = p; }
		
#line 6794 "cfg.cpp"
		
		goto _st258;
		_ctr357:
		{
#line 172 "cfg.rl"
			
			if (auto t = nk::from_string<int>(fas.intv2_st, p - fas.intv2_st)) fas.v_int2 = *t; {
				log_line("invalid value where second integer in range is expected on line %zu", fas.linenum);
				std::exit(EXIT_FAILURE);
			}
			fas.intv2_exist = true;
		}
		
#line 6808 "cfg.cpp"
		
		{
#line 211 "cfg.rl"
			fas.setlim(RLIMIT_RTPRIO); }
		
#line 6814 "cfg.cpp"
		
		goto _st258;
		_st258:
		if ( p == eof )
			goto _out258;
		p+= 1;
		st_case_258:
		if ( p == pe && p != eof )
			goto _out258;
		if ( p == eof ) {
			goto _ctr357;}
		else {
			if ( 48 <= ( (*( p))) && ( (*( p))) <= 57 ) {
				goto _st258;
			}
			goto _st0;
		}
		_st172:
		if ( p == eof )
			goto _out172;
		p+= 1;
		st_case_172:
		if ( p == pe && p != eof )
			goto _out172;
		if ( p == eof ) {
			goto _st172;}
		else {
			switch( ( (*( p))) ) {
				case 73: {
					goto _st173;
				}
				case 105: {
					goto _st173;
				}
			}
			goto _st0;
		}
		_st173:
		if ( p == eof )
			goto _out173;
		p+= 1;
		st_case_173:
		if ( p == pe && p != eof )
			goto _out173;
		if ( p == eof ) {
			goto _st173;}
		else {
			switch( ( (*( p))) ) {
				case 77: {
					goto _st174;
				}
				case 109: {
					goto _st174;
				}
			}
			goto _st0;
		}
		_st174:
		if ( p == eof )
			goto _out174;
		p+= 1;
		st_case_174:
		if ( p == pe && p != eof )
			goto _out174;
		if ( p == eof ) {
			goto _st174;}
		else {
			switch( ( (*( p))) ) {
				case 69: {
					goto _st175;
				}
				case 101: {
					goto _st175;
				}
			}
			goto _st0;
		}
		_st175:
		if ( p == eof )
			goto _out175;
		p+= 1;
		st_case_175:
		if ( p == pe && p != eof )
			goto _out175;
		if ( p == eof ) {
			goto _st175;}
		else {
			switch( ( (*( p))) ) {
				case 9: {
					goto _st175;
				}
				case 32: {
					goto _st175;
				}
				case 61: {
					goto _st176;
				}
			}
			goto _st0;
		}
		_st176:
		if ( p == eof )
			goto _out176;
		p+= 1;
		st_case_176:
		if ( p == pe && p != eof )
			goto _out176;
		if ( p == eof ) {
			goto _st176;}
		else {
			switch( ( (*( p))) ) {
				case 9: {
					goto _st176;
				}
				case 32: {
					goto _st176;
				}
			}
			if ( 48 <= ( (*( p))) && ( (*( p))) <= 57 ) {
				goto _ctr229;
			}
			goto _st0;
		}
		_ctr229:
		{
#line 160 "cfg.rl"
			
			fas.intv_st = p;
			fas.v_int = fas.v_int2 = 0;
			fas.intv2_exist = false;
		}
		
#line 6947 "cfg.cpp"
		
		goto _st259;
		_ctr359:
		{
#line 165 "cfg.rl"
			
			if (auto t = nk::from_string<int>(fas.intv_st, p - fas.intv_st)) fas.v_int = *t; else {
				log_line("invalid value where integer is expected on line %zu", fas.linenum);
				std::exit(EXIT_FAILURE);
			}
		}
		
#line 6960 "cfg.cpp"
		
		{
#line 210 "cfg.rl"
			fas.setlim(RLIMIT_RTTIME); }
		
#line 6966 "cfg.cpp"
		
		goto _st259;
		_st259:
		if ( p == eof )
			goto _out259;
		p+= 1;
		st_case_259:
		if ( p == pe && p != eof )
			goto _out259;
		if ( p == eof ) {
			goto _ctr359;}
		else {
			if ( ( (*( p))) == 44 ) {
				goto _ctr360;
			}
			if ( 48 <= ( (*( p))) && ( (*( p))) <= 57 ) {
				goto _st259;
			}
			goto _st0;
		}
		_ctr360:
		{
#line 165 "cfg.rl"
			
			if (auto t = nk::from_string<int>(fas.intv_st, p - fas.intv_st)) fas.v_int = *t; else {
				log_line("invalid value where integer is expected on line %zu", fas.linenum);
				std::exit(EXIT_FAILURE);
			}
		}
		
#line 6997 "cfg.cpp"
		
		goto _st177;
		_st177:
		if ( p == eof )
			goto _out177;
		p+= 1;
		st_case_177:
		if ( p == pe && p != eof )
			goto _out177;
		if ( p == eof ) {
			goto _st177;}
		else {
			if ( 48 <= ( (*( p))) && ( (*( p))) <= 57 ) {
				goto _ctr231;
			}
			goto _st0;
		}
		_ctr231:
		{
#line 171 "cfg.rl"
			fas.intv2_st = p; }
		
#line 7020 "cfg.cpp"
		
		goto _st260;
		_ctr362:
		{
#line 172 "cfg.rl"
			
			if (auto t = nk::from_string<int>(fas.intv2_st, p - fas.intv2_st)) fas.v_int2 = *t; {
				log_line("invalid value where second integer in range is expected on line %zu", fas.linenum);
				std::exit(EXIT_FAILURE);
			}
			fas.intv2_exist = true;
		}
		
#line 7034 "cfg.cpp"
		
		{
#line 210 "cfg.rl"
			fas.setlim(RLIMIT_RTTIME); }
		
#line 7040 "cfg.cpp"
		
		goto _st260;
		_st260:
		if ( p == eof )
			goto _out260;
		p+= 1;
		st_case_260:
		if ( p == pe && p != eof )
			goto _out260;
		if ( p == eof ) {
			goto _ctr362;}
		else {
			if ( 48 <= ( (*( p))) && ( (*( p))) <= 57 ) {
				goto _st260;
			}
			goto _st0;
		}
		_st178:
		if ( p == eof )
			goto _out178;
		p+= 1;
		st_case_178:
		if ( p == pe && p != eof )
			goto _out178;
		if ( p == eof ) {
			goto _st178;}
		else {
			switch( ( (*( p))) ) {
				case 73: {
					goto _st179;
				}
				case 84: {
					goto _st190;
				}
				case 105: {
					goto _st179;
				}
				case 116: {
					goto _st190;
				}
			}
			goto _st0;
		}
		_st179:
		if ( p == eof )
			goto _out179;
		p+= 1;
		st_case_179:
		if ( p == pe && p != eof )
			goto _out179;
		if ( p == eof ) {
			goto _st179;}
		else {
			switch( ( (*( p))) ) {
				case 71: {
					goto _st180;
				}
				case 103: {
					goto _st180;
				}
			}
			goto _st0;
		}
		_st180:
		if ( p == eof )
			goto _out180;
		p+= 1;
		st_case_180:
		if ( p == pe && p != eof )
			goto _out180;
		if ( p == eof ) {
			goto _st180;}
		else {
			switch( ( (*( p))) ) {
				case 80: {
					goto _st181;
				}
				case 112: {
					goto _st181;
				}
			}
			goto _st0;
		}
		_st181:
		if ( p == eof )
			goto _out181;
		p+= 1;
		st_case_181:
		if ( p == pe && p != eof )
			goto _out181;
		if ( p == eof ) {
			goto _st181;}
		else {
			switch( ( (*( p))) ) {
				case 69: {
					goto _st182;
				}
				case 101: {
					goto _st182;
				}
			}
			goto _st0;
		}
		_st182:
		if ( p == eof )
			goto _out182;
		p+= 1;
		st_case_182:
		if ( p == pe && p != eof )
			goto _out182;
		if ( p == eof ) {
			goto _st182;}
		else {
			switch( ( (*( p))) ) {
				case 78: {
					goto _st183;
				}
				case 110: {
					goto _st183;
				}
			}
			goto _st0;
		}
		_st183:
		if ( p == eof )
			goto _out183;
		p+= 1;
		st_case_183:
		if ( p == pe && p != eof )
			goto _out183;
		if ( p == eof ) {
			goto _st183;}
		else {
			switch( ( (*( p))) ) {
				case 68: {
					goto _st184;
				}
				case 100: {
					goto _st184;
				}
			}
			goto _st0;
		}
		_st184:
		if ( p == eof )
			goto _out184;
		p+= 1;
		st_case_184:
		if ( p == pe && p != eof )
			goto _out184;
		if ( p == eof ) {
			goto _st184;}
		else {
			switch( ( (*( p))) ) {
				case 73: {
					goto _st185;
				}
				case 105: {
					goto _st185;
				}
			}
			goto _st0;
		}
		_st185:
		if ( p == eof )
			goto _out185;
		p+= 1;
		st_case_185:
		if ( p == pe && p != eof )
			goto _out185;
		if ( p == eof ) {
			goto _st185;}
		else {
			switch( ( (*( p))) ) {
				case 78: {
					goto _st186;
				}
				case 110: {
					goto _st186;
				}
			}
			goto _st0;
		}
		_st186:
		if ( p == eof )
			goto _out186;
		p+= 1;
		st_case_186:
		if ( p == pe && p != eof )
			goto _out186;
		if ( p == eof ) {
			goto _st186;}
		else {
			switch( ( (*( p))) ) {
				case 71: {
					goto _st187;
				}
				case 103: {
					goto _st187;
				}
			}
			goto _st0;
		}
		_st187:
		if ( p == eof )
			goto _out187;
		p+= 1;
		st_case_187:
		if ( p == pe && p != eof )
			goto _out187;
		if ( p == eof ) {
			goto _st187;}
		else {
			switch( ( (*( p))) ) {
				case 9: {
					goto _st187;
				}
				case 32: {
					goto _st187;
				}
				case 61: {
					goto _st188;
				}
			}
			goto _st0;
		}
		_st188:
		if ( p == eof )
			goto _out188;
		p+= 1;
		st_case_188:
		if ( p == pe && p != eof )
			goto _out188;
		if ( p == eof ) {
			goto _st188;}
		else {
			switch( ( (*( p))) ) {
				case 9: {
					goto _st188;
				}
				case 32: {
					goto _st188;
				}
			}
			if ( 48 <= ( (*( p))) && ( (*( p))) <= 57 ) {
				goto _ctr243;
			}
			goto _st0;
		}
		_ctr243:
		{
#line 160 "cfg.rl"
			
			fas.intv_st = p;
			fas.v_int = fas.v_int2 = 0;
			fas.intv2_exist = false;
		}
		
#line 7299 "cfg.cpp"
		
		goto _st261;
		_ctr364:
		{
#line 165 "cfg.rl"
			
			if (auto t = nk::from_string<int>(fas.intv_st, p - fas.intv_st)) fas.v_int = *t; else {
				log_line("invalid value where integer is expected on line %zu", fas.linenum);
				std::exit(EXIT_FAILURE);
			}
		}
		
#line 7312 "cfg.cpp"
		
		{
#line 212 "cfg.rl"
			fas.setlim(RLIMIT_SIGPENDING); }
		
#line 7318 "cfg.cpp"
		
		goto _st261;
		_st261:
		if ( p == eof )
			goto _out261;
		p+= 1;
		st_case_261:
		if ( p == pe && p != eof )
			goto _out261;
		if ( p == eof ) {
			goto _ctr364;}
		else {
			if ( ( (*( p))) == 44 ) {
				goto _ctr365;
			}
			if ( 48 <= ( (*( p))) && ( (*( p))) <= 57 ) {
				goto _st261;
			}
			goto _st0;
		}
		_ctr365:
		{
#line 165 "cfg.rl"
			
			if (auto t = nk::from_string<int>(fas.intv_st, p - fas.intv_st)) fas.v_int = *t; else {
				log_line("invalid value where integer is expected on line %zu", fas.linenum);
				std::exit(EXIT_FAILURE);
			}
		}
		
#line 7349 "cfg.cpp"
		
		goto _st189;
		_st189:
		if ( p == eof )
			goto _out189;
		p+= 1;
		st_case_189:
		if ( p == pe && p != eof )
			goto _out189;
		if ( p == eof ) {
			goto _st189;}
		else {
			if ( 48 <= ( (*( p))) && ( (*( p))) <= 57 ) {
				goto _ctr245;
			}
			goto _st0;
		}
		_ctr245:
		{
#line 171 "cfg.rl"
			fas.intv2_st = p; }
		
#line 7372 "cfg.cpp"
		
		goto _st262;
		_ctr367:
		{
#line 172 "cfg.rl"
			
			if (auto t = nk::from_string<int>(fas.intv2_st, p - fas.intv2_st)) fas.v_int2 = *t; {
				log_line("invalid value where second integer in range is expected on line %zu", fas.linenum);
				std::exit(EXIT_FAILURE);
			}
			fas.intv2_exist = true;
		}
		
#line 7386 "cfg.cpp"
		
		{
#line 212 "cfg.rl"
			fas.setlim(RLIMIT_SIGPENDING); }
		
#line 7392 "cfg.cpp"
		
		goto _st262;
		_st262:
		if ( p == eof )
			goto _out262;
		p+= 1;
		st_case_262:
		if ( p == pe && p != eof )
			goto _out262;
		if ( p == eof ) {
			goto _ctr367;}
		else {
			if ( 48 <= ( (*( p))) && ( (*( p))) <= 57 ) {
				goto _st262;
			}
			goto _st0;
		}
		_st190:
		if ( p == eof )
			goto _out190;
		p+= 1;
		st_case_190:
		if ( p == pe && p != eof )
			goto _out190;
		if ( p == eof ) {
			goto _st190;}
		else {
			switch( ( (*( p))) ) {
				case 65: {
					goto _st191;
				}
				case 97: {
					goto _st191;
				}
			}
			goto _st0;
		}
		_st191:
		if ( p == eof )
			goto _out191;
		p+= 1;
		st_case_191:
		if ( p == pe && p != eof )
			goto _out191;
		if ( p == eof ) {
			goto _st191;}
		else {
			switch( ( (*( p))) ) {
				case 67: {
					goto _st192;
				}
				case 99: {
					goto _st192;
				}
			}
			goto _st0;
		}
		_st192:
		if ( p == eof )
			goto _out192;
		p+= 1;
		st_case_192:
		if ( p == pe && p != eof )
			goto _out192;
		if ( p == eof ) {
			goto _st192;}
		else {
			switch( ( (*( p))) ) {
				case 75: {
					goto _st193;
				}
				case 107: {
					goto _st193;
				}
			}
			goto _st0;
		}
		_st193:
		if ( p == eof )
			goto _out193;
		p+= 1;
		st_case_193:
		if ( p == pe && p != eof )
			goto _out193;
		if ( p == eof ) {
			goto _st193;}
		else {
			switch( ( (*( p))) ) {
				case 9: {
					goto _st193;
				}
				case 32: {
					goto _st193;
				}
				case 61: {
					goto _st194;
				}
			}
			goto _st0;
		}
		_st194:
		if ( p == eof )
			goto _out194;
		p+= 1;
		st_case_194:
		if ( p == pe && p != eof )
			goto _out194;
		if ( p == eof ) {
			goto _st194;}
		else {
			switch( ( (*( p))) ) {
				case 9: {
					goto _st194;
				}
				case 32: {
					goto _st194;
				}
			}
			if ( 48 <= ( (*( p))) && ( (*( p))) <= 57 ) {
				goto _ctr250;
			}
			goto _st0;
		}
		_ctr250:
		{
#line 160 "cfg.rl"
			
			fas.intv_st = p;
			fas.v_int = fas.v_int2 = 0;
			fas.intv2_exist = false;
		}
		
#line 7525 "cfg.cpp"
		
		goto _st263;
		_ctr369:
		{
#line 165 "cfg.rl"
			
			if (auto t = nk::from_string<int>(fas.intv_st, p - fas.intv_st)) fas.v_int = *t; else {
				log_line("invalid value where integer is expected on line %zu", fas.linenum);
				std::exit(EXIT_FAILURE);
			}
		}
		
#line 7538 "cfg.cpp"
		
		{
#line 204 "cfg.rl"
			fas.setlim(RLIMIT_STACK); }
		
#line 7544 "cfg.cpp"
		
		goto _st263;
		_st263:
		if ( p == eof )
			goto _out263;
		p+= 1;
		st_case_263:
		if ( p == pe && p != eof )
			goto _out263;
		if ( p == eof ) {
			goto _ctr369;}
		else {
			if ( ( (*( p))) == 44 ) {
				goto _ctr370;
			}
			if ( 48 <= ( (*( p))) && ( (*( p))) <= 57 ) {
				goto _st263;
			}
			goto _st0;
		}
		_ctr370:
		{
#line 165 "cfg.rl"
			
			if (auto t = nk::from_string<int>(fas.intv_st, p - fas.intv_st)) fas.v_int = *t; else {
				log_line("invalid value where integer is expected on line %zu", fas.linenum);
				std::exit(EXIT_FAILURE);
			}
		}
		
#line 7575 "cfg.cpp"
		
		goto _st195;
		_st195:
		if ( p == eof )
			goto _out195;
		p+= 1;
		st_case_195:
		if ( p == pe && p != eof )
			goto _out195;
		if ( p == eof ) {
			goto _st195;}
		else {
			if ( 48 <= ( (*( p))) && ( (*( p))) <= 57 ) {
				goto _ctr252;
			}
			goto _st0;
		}
		_ctr252:
		{
#line 171 "cfg.rl"
			fas.intv2_st = p; }
		
#line 7598 "cfg.cpp"
		
		goto _st264;
		_ctr372:
		{
#line 172 "cfg.rl"
			
			if (auto t = nk::from_string<int>(fas.intv2_st, p - fas.intv2_st)) fas.v_int2 = *t; {
				log_line("invalid value where second integer in range is expected on line %zu", fas.linenum);
				std::exit(EXIT_FAILURE);
			}
			fas.intv2_exist = true;
		}
		
#line 7612 "cfg.cpp"
		
		{
#line 204 "cfg.rl"
			fas.setlim(RLIMIT_STACK); }
		
#line 7618 "cfg.cpp"
		
		goto _st264;
		_st264:
		if ( p == eof )
			goto _out264;
		p+= 1;
		st_case_264:
		if ( p == pe && p != eof )
			goto _out264;
		if ( p == eof ) {
			goto _ctr372;}
		else {
			if ( 48 <= ( (*( p))) && ( (*( p))) <= 57 ) {
				goto _st264;
			}
			goto _st0;
		}
		_st196:
		if ( p == eof )
			goto _out196;
		p+= 1;
		st_case_196:
		if ( p == pe && p != eof )
			goto _out196;
		if ( p == eof ) {
			goto _st196;}
		else {
			switch( ( (*( p))) ) {
				case 65: {
					goto _st197;
				}
				case 97: {
					goto _st197;
				}
			}
			goto _st0;
		}
		_st197:
		if ( p == eof )
			goto _out197;
		p+= 1;
		st_case_197:
		if ( p == pe && p != eof )
			goto _out197;
		if ( p == eof ) {
			goto _st197;}
		else {
			switch( ( (*( p))) ) {
				case 84: {
					goto _st198;
				}
				case 116: {
					goto _st198;
				}
			}
			goto _st0;
		}
		_st198:
		if ( p == eof )
			goto _out198;
		p+= 1;
		st_case_198:
		if ( p == pe && p != eof )
			goto _out198;
		if ( p == eof ) {
			goto _st198;}
		else {
			switch( ( (*( p))) ) {
				case 72: {
					goto _st199;
				}
				case 104: {
					goto _st199;
				}
			}
			goto _st0;
		}
		_st199:
		if ( p == eof )
			goto _out199;
		p+= 1;
		st_case_199:
		if ( p == pe && p != eof )
			goto _out199;
		if ( p == eof ) {
			goto _st199;}
		else {
			switch( ( (*( p))) ) {
				case 9: {
					goto _st199;
				}
				case 32: {
					goto _st199;
				}
				case 61: {
					goto _st200;
				}
			}
			goto _st0;
		}
		_st200:
		if ( p == eof )
			goto _out200;
		p+= 1;
		st_case_200:
		if ( p == pe && p != eof )
			goto _out200;
		if ( p == eof ) {
			goto _st200;}
		else {
			switch( ( (*( p))) ) {
				case 0: {
					goto _st0;
				}
				case 9: {
					goto _ctr258;
				}
				case 10: {
					goto _st0;
				}
				case 32: {
					goto _ctr258;
				}
			}
			goto _ctr257;
		}
		_ctr257:
		{
#line 180 "cfg.rl"
			fas.strv_st = p; fas.v_strlen = 0; }
		
#line 7750 "cfg.cpp"
		
		goto _st265;
		_ctr374:
		{
#line 181 "cfg.rl"
			
			fas.v_strlen = p - fas.strv_st;
			if (fas.v_strlen <= INT_MAX) {
				ssize_t snl = snprintf(fas.v_str, sizeof fas.v_str,
				"%.*s", (int)fas.v_strlen, fas.strv_st);
				if (snl < 0 || (size_t)snl >= sizeof fas.v_str) {
					log_line("error parsing line %zu in crontab; too long?", fas.linenum);
					std::exit(EXIT_FAILURE);
				}
			}
		}
		
#line 7768 "cfg.cpp"
		
		{
#line 235 "cfg.rl"
			fas.wm->path_ = std::string(fas.v_str, fas.v_strlen); }
		
#line 7774 "cfg.cpp"
		
		goto _st265;
		_st265:
		if ( p == eof )
			goto _out265;
		p+= 1;
		st_case_265:
		if ( p == pe && p != eof )
			goto _out265;
		if ( p == eof ) {
			goto _ctr374;}
		else {
			switch( ( (*( p))) ) {
				case 0: {
					goto _st0;
				}
				case 10: {
					goto _st0;
				}
			}
			goto _st265;
		}
		_ctr258:
		{
#line 180 "cfg.rl"
			fas.strv_st = p; fas.v_strlen = 0; }
		
#line 7802 "cfg.cpp"
		
		goto _st266;
		_ctr376:
		{
#line 181 "cfg.rl"
			
			fas.v_strlen = p - fas.strv_st;
			if (fas.v_strlen <= INT_MAX) {
				ssize_t snl = snprintf(fas.v_str, sizeof fas.v_str,
				"%.*s", (int)fas.v_strlen, fas.strv_st);
				if (snl < 0 || (size_t)snl >= sizeof fas.v_str) {
					log_line("error parsing line %zu in crontab; too long?", fas.linenum);
					std::exit(EXIT_FAILURE);
				}
			}
		}
		
#line 7820 "cfg.cpp"
		
		{
#line 235 "cfg.rl"
			fas.wm->path_ = std::string(fas.v_str, fas.v_strlen); }
		
#line 7826 "cfg.cpp"
		
		goto _st266;
		_st266:
		if ( p == eof )
			goto _out266;
		p+= 1;
		st_case_266:
		if ( p == pe && p != eof )
			goto _out266;
		if ( p == eof ) {
			goto _ctr376;}
		else {
			switch( ( (*( p))) ) {
				case 0: {
					goto _st0;
				}
				case 9: {
					goto _ctr258;
				}
				case 10: {
					goto _st0;
				}
				case 32: {
					goto _ctr258;
				}
			}
			goto _ctr257;
		}
		_st201:
		if ( p == eof )
			goto _out201;
		p+= 1;
		st_case_201:
		if ( p == pe && p != eof )
			goto _out201;
		if ( p == eof ) {
			goto _st201;}
		else {
			switch( ( (*( p))) ) {
				case 83: {
					goto _st202;
				}
				case 115: {
					goto _st202;
				}
			}
			goto _st0;
		}
		_st202:
		if ( p == eof )
			goto _out202;
		p+= 1;
		st_case_202:
		if ( p == pe && p != eof )
			goto _out202;
		if ( p == eof ) {
			goto _st202;}
		else {
			switch( ( (*( p))) ) {
				case 69: {
					goto _st203;
				}
				case 101: {
					goto _st203;
				}
			}
			goto _st0;
		}
		_st203:
		if ( p == eof )
			goto _out203;
		p+= 1;
		st_case_203:
		if ( p == pe && p != eof )
			goto _out203;
		if ( p == eof ) {
			goto _st203;}
		else {
			switch( ( (*( p))) ) {
				case 82: {
					goto _st204;
				}
				case 114: {
					goto _st204;
				}
			}
			goto _st0;
		}
		_st204:
		if ( p == eof )
			goto _out204;
		p+= 1;
		st_case_204:
		if ( p == pe && p != eof )
			goto _out204;
		if ( p == eof ) {
			goto _st204;}
		else {
			switch( ( (*( p))) ) {
				case 9: {
					goto _st204;
				}
				case 32: {
					goto _st204;
				}
				case 61: {
					goto _st205;
				}
			}
			goto _st0;
		}
		_st205:
		if ( p == eof )
			goto _out205;
		p+= 1;
		st_case_205:
		if ( p == pe && p != eof )
			goto _out205;
		if ( p == eof ) {
			goto _st205;}
		else {
			switch( ( (*( p))) ) {
				case 0: {
					goto _st0;
				}
				case 9: {
					goto _ctr264;
				}
				case 10: {
					goto _st0;
				}
				case 32: {
					goto _ctr264;
				}
			}
			goto _ctr263;
		}
		_ctr263:
		{
#line 180 "cfg.rl"
			fas.strv_st = p; fas.v_strlen = 0; }
		
#line 7969 "cfg.cpp"
		
		goto _st267;
		_ctr377:
		{
#line 181 "cfg.rl"
			
			fas.v_strlen = p - fas.strv_st;
			if (fas.v_strlen <= INT_MAX) {
				ssize_t snl = snprintf(fas.v_str, sizeof fas.v_str,
				"%.*s", (int)fas.v_strlen, fas.strv_st);
				if (snl < 0 || (size_t)snl >= sizeof fas.v_str) {
					log_line("error parsing line %zu in crontab; too long?", fas.linenum);
					std::exit(EXIT_FAILURE);
				}
			}
		}
		
#line 7987 "cfg.cpp"
		
		{
#line 231 "cfg.rl"
			fas.setuserv(); }
		
#line 7993 "cfg.cpp"
		
		goto _st267;
		_st267:
		if ( p == eof )
			goto _out267;
		p+= 1;
		st_case_267:
		if ( p == pe && p != eof )
			goto _out267;
		if ( p == eof ) {
			goto _ctr377;}
		else {
			switch( ( (*( p))) ) {
				case 0: {
					goto _st0;
				}
				case 10: {
					goto _st0;
				}
			}
			goto _st267;
		}
		_ctr264:
		{
#line 180 "cfg.rl"
			fas.strv_st = p; fas.v_strlen = 0; }
		
#line 8021 "cfg.cpp"
		
		goto _st268;
		_ctr379:
		{
#line 181 "cfg.rl"
			
			fas.v_strlen = p - fas.strv_st;
			if (fas.v_strlen <= INT_MAX) {
				ssize_t snl = snprintf(fas.v_str, sizeof fas.v_str,
				"%.*s", (int)fas.v_strlen, fas.strv_st);
				if (snl < 0 || (size_t)snl >= sizeof fas.v_str) {
					log_line("error parsing line %zu in crontab; too long?", fas.linenum);
					std::exit(EXIT_FAILURE);
				}
			}
		}
		
#line 8039 "cfg.cpp"
		
		{
#line 231 "cfg.rl"
			fas.setuserv(); }
		
#line 8045 "cfg.cpp"
		
		goto _st268;
		_st268:
		if ( p == eof )
			goto _out268;
		p+= 1;
		st_case_268:
		if ( p == pe && p != eof )
			goto _out268;
		if ( p == eof ) {
			goto _ctr379;}
		else {
			switch( ( (*( p))) ) {
				case 0: {
					goto _st0;
				}
				case 9: {
					goto _ctr264;
				}
				case 10: {
					goto _st0;
				}
				case 32: {
					goto _ctr264;
				}
			}
			goto _ctr263;
		}
		_st206:
		if ( p == eof )
			goto _out206;
		p+= 1;
		st_case_206:
		if ( p == pe && p != eof )
			goto _out206;
		if ( p == eof ) {
			goto _st206;}
		else {
			switch( ( (*( p))) ) {
				case 65: {
					goto _st207;
				}
				case 97: {
					goto _st207;
				}
			}
			goto _st0;
		}
		_st207:
		if ( p == eof )
			goto _out207;
		p+= 1;
		st_case_207:
		if ( p == pe && p != eof )
			goto _out207;
		if ( p == eof ) {
			goto _st207;}
		else {
			switch( ( (*( p))) ) {
				case 84: {
					goto _st208;
				}
				case 116: {
					goto _st208;
				}
			}
			goto _st0;
		}
		_st208:
		if ( p == eof )
			goto _out208;
		p+= 1;
		st_case_208:
		if ( p == pe && p != eof )
			goto _out208;
		if ( p == eof ) {
			goto _st208;}
		else {
			switch( ( (*( p))) ) {
				case 67: {
					goto _st209;
				}
				case 99: {
					goto _st209;
				}
			}
			goto _st0;
		}
		_st209:
		if ( p == eof )
			goto _out209;
		p+= 1;
		st_case_209:
		if ( p == pe && p != eof )
			goto _out209;
		if ( p == eof ) {
			goto _st209;}
		else {
			switch( ( (*( p))) ) {
				case 72: {
					goto _st210;
				}
				case 104: {
					goto _st210;
				}
			}
			goto _st0;
		}
		_st210:
		if ( p == eof )
			goto _out210;
		p+= 1;
		st_case_210:
		if ( p == pe && p != eof )
			goto _out210;
		if ( p == eof ) {
			goto _st210;}
		else {
			switch( ( (*( p))) ) {
				case 9: {
					goto _st210;
				}
				case 32: {
					goto _st210;
				}
				case 61: {
					goto _st211;
				}
			}
			goto _st0;
		}
		_st211:
		if ( p == eof )
			goto _out211;
		p+= 1;
		st_case_211:
		if ( p == pe && p != eof )
			goto _out211;
		if ( p == eof ) {
			goto _st211;}
		else {
			switch( ( (*( p))) ) {
				case 0: {
					goto _st0;
				}
				case 9: {
					goto _ctr271;
				}
				case 10: {
					goto _st0;
				}
				case 32: {
					goto _ctr271;
				}
			}
			goto _ctr270;
		}
		_ctr270:
		{
#line 180 "cfg.rl"
			fas.strv_st = p; fas.v_strlen = 0; }
		
#line 8208 "cfg.cpp"
		
		goto _st269;
		_ctr380:
		{
#line 181 "cfg.rl"
			
			fas.v_strlen = p - fas.strv_st;
			if (fas.v_strlen <= INT_MAX) {
				ssize_t snl = snprintf(fas.v_str, sizeof fas.v_str,
				"%.*s", (int)fas.v_strlen, fas.strv_st);
				if (snl < 0 || (size_t)snl >= sizeof fas.v_str) {
					log_line("error parsing line %zu in crontab; too long?", fas.linenum);
					std::exit(EXIT_FAILURE);
				}
			}
		}
		
#line 8226 "cfg.cpp"
		
		{
#line 234 "cfg.rl"
			fas.wm->filepath_ = std::string(fas.v_str, fas.v_strlen); }
		
#line 8232 "cfg.cpp"
		
		goto _st269;
		_st269:
		if ( p == eof )
			goto _out269;
		p+= 1;
		st_case_269:
		if ( p == pe && p != eof )
			goto _out269;
		if ( p == eof ) {
			goto _ctr380;}
		else {
			switch( ( (*( p))) ) {
				case 0: {
					goto _st0;
				}
				case 10: {
					goto _st0;
				}
			}
			goto _st269;
		}
		_ctr271:
		{
#line 180 "cfg.rl"
			fas.strv_st = p; fas.v_strlen = 0; }
		
#line 8260 "cfg.cpp"
		
		goto _st270;
		_ctr382:
		{
#line 181 "cfg.rl"
			
			fas.v_strlen = p - fas.strv_st;
			if (fas.v_strlen <= INT_MAX) {
				ssize_t snl = snprintf(fas.v_str, sizeof fas.v_str,
				"%.*s", (int)fas.v_strlen, fas.strv_st);
				if (snl < 0 || (size_t)snl >= sizeof fas.v_str) {
					log_line("error parsing line %zu in crontab; too long?", fas.linenum);
					std::exit(EXIT_FAILURE);
				}
			}
		}
		
#line 8278 "cfg.cpp"
		
		{
#line 234 "cfg.rl"
			fas.wm->filepath_ = std::string(fas.v_str, fas.v_strlen); }
		
#line 8284 "cfg.cpp"
		
		goto _st270;
		_st270:
		if ( p == eof )
			goto _out270;
		p+= 1;
		st_case_270:
		if ( p == pe && p != eof )
			goto _out270;
		if ( p == eof ) {
			goto _ctr382;}
		else {
			switch( ( (*( p))) ) {
				case 0: {
					goto _st0;
				}
				case 9: {
					goto _ctr271;
				}
				case 10: {
					goto _st0;
				}
				case 32: {
					goto _ctr271;
				}
			}
			goto _ctr270;
		}
		_out1: fas.cs = 1; goto _out; 
		_out0: fas.cs = 0; goto _out; 
		_out2: fas.cs = 2; goto _out; 
		_out212: fas.cs = 212; goto _out; 
		_out3: fas.cs = 3; goto _out; 
		_out4: fas.cs = 4; goto _out; 
		_out5: fas.cs = 5; goto _out; 
		_out6: fas.cs = 6; goto _out; 
		_out7: fas.cs = 7; goto _out; 
		_out8: fas.cs = 8; goto _out; 
		_out9: fas.cs = 9; goto _out; 
		_out213: fas.cs = 213; goto _out; 
		_out214: fas.cs = 214; goto _out; 
		_out10: fas.cs = 10; goto _out; 
		_out11: fas.cs = 11; goto _out; 
		_out12: fas.cs = 12; goto _out; 
		_out13: fas.cs = 13; goto _out; 
		_out14: fas.cs = 14; goto _out; 
		_out15: fas.cs = 15; goto _out; 
		_out16: fas.cs = 16; goto _out; 
		_out215: fas.cs = 215; goto _out; 
		_out216: fas.cs = 216; goto _out; 
		_out17: fas.cs = 17; goto _out; 
		_out18: fas.cs = 18; goto _out; 
		_out19: fas.cs = 19; goto _out; 
		_out20: fas.cs = 20; goto _out; 
		_out21: fas.cs = 21; goto _out; 
		_out22: fas.cs = 22; goto _out; 
		_out23: fas.cs = 23; goto _out; 
		_out24: fas.cs = 24; goto _out; 
		_out25: fas.cs = 25; goto _out; 
		_out26: fas.cs = 26; goto _out; 
		_out27: fas.cs = 27; goto _out; 
		_out28: fas.cs = 28; goto _out; 
		_out29: fas.cs = 29; goto _out; 
		_out30: fas.cs = 30; goto _out; 
		_out217: fas.cs = 217; goto _out; 
		_out31: fas.cs = 31; goto _out; 
		_out32: fas.cs = 32; goto _out; 
		_out33: fas.cs = 33; goto _out; 
		_out34: fas.cs = 34; goto _out; 
		_out35: fas.cs = 35; goto _out; 
		_out218: fas.cs = 218; goto _out; 
		_out36: fas.cs = 36; goto _out; 
		_out37: fas.cs = 37; goto _out; 
		_out38: fas.cs = 38; goto _out; 
		_out39: fas.cs = 39; goto _out; 
		_out40: fas.cs = 40; goto _out; 
		_out41: fas.cs = 41; goto _out; 
		_out219: fas.cs = 219; goto _out; 
		_out42: fas.cs = 42; goto _out; 
		_out43: fas.cs = 43; goto _out; 
		_out44: fas.cs = 44; goto _out; 
		_out45: fas.cs = 45; goto _out; 
		_out220: fas.cs = 220; goto _out; 
		_out46: fas.cs = 46; goto _out; 
		_out47: fas.cs = 47; goto _out; 
		_out48: fas.cs = 48; goto _out; 
		_out49: fas.cs = 49; goto _out; 
		_out221: fas.cs = 221; goto _out; 
		_out50: fas.cs = 50; goto _out; 
		_out51: fas.cs = 51; goto _out; 
		_out52: fas.cs = 52; goto _out; 
		_out53: fas.cs = 53; goto _out; 
		_out54: fas.cs = 54; goto _out; 
		_out55: fas.cs = 55; goto _out; 
		_out222: fas.cs = 222; goto _out; 
		_out56: fas.cs = 56; goto _out; 
		_out57: fas.cs = 57; goto _out; 
		_out58: fas.cs = 58; goto _out; 
		_out59: fas.cs = 59; goto _out; 
		_out223: fas.cs = 223; goto _out; 
		_out60: fas.cs = 60; goto _out; 
		_out61: fas.cs = 61; goto _out; 
		_out62: fas.cs = 62; goto _out; 
		_out63: fas.cs = 63; goto _out; 
		_out224: fas.cs = 224; goto _out; 
		_out64: fas.cs = 64; goto _out; 
		_out65: fas.cs = 65; goto _out; 
		_out66: fas.cs = 66; goto _out; 
		_out67: fas.cs = 67; goto _out; 
		_out68: fas.cs = 68; goto _out; 
		_out225: fas.cs = 225; goto _out; 
		_out69: fas.cs = 69; goto _out; 
		_out70: fas.cs = 70; goto _out; 
		_out71: fas.cs = 71; goto _out; 
		_out226: fas.cs = 226; goto _out; 
		_out72: fas.cs = 72; goto _out; 
		_out73: fas.cs = 73; goto _out; 
		_out74: fas.cs = 74; goto _out; 
		_out75: fas.cs = 75; goto _out; 
		_out76: fas.cs = 76; goto _out; 
		_out227: fas.cs = 227; goto _out; 
		_out77: fas.cs = 77; goto _out; 
		_out228: fas.cs = 228; goto _out; 
		_out78: fas.cs = 78; goto _out; 
		_out79: fas.cs = 79; goto _out; 
		_out80: fas.cs = 80; goto _out; 
		_out229: fas.cs = 229; goto _out; 
		_out81: fas.cs = 81; goto _out; 
		_out82: fas.cs = 82; goto _out; 
		_out83: fas.cs = 83; goto _out; 
		_out230: fas.cs = 230; goto _out; 
		_out84: fas.cs = 84; goto _out; 
		_out231: fas.cs = 231; goto _out; 
		_out85: fas.cs = 85; goto _out; 
		_out86: fas.cs = 86; goto _out; 
		_out87: fas.cs = 87; goto _out; 
		_out232: fas.cs = 232; goto _out; 
		_out88: fas.cs = 88; goto _out; 
		_out89: fas.cs = 89; goto _out; 
		_out90: fas.cs = 90; goto _out; 
		_out91: fas.cs = 91; goto _out; 
		_out92: fas.cs = 92; goto _out; 
		_out93: fas.cs = 93; goto _out; 
		_out233: fas.cs = 233; goto _out; 
		_out234: fas.cs = 234; goto _out; 
		_out94: fas.cs = 94; goto _out; 
		_out95: fas.cs = 95; goto _out; 
		_out96: fas.cs = 96; goto _out; 
		_out97: fas.cs = 97; goto _out; 
		_out98: fas.cs = 98; goto _out; 
		_out235: fas.cs = 235; goto _out; 
		_out99: fas.cs = 99; goto _out; 
		_out236: fas.cs = 236; goto _out; 
		_out100: fas.cs = 100; goto _out; 
		_out101: fas.cs = 101; goto _out; 
		_out102: fas.cs = 102; goto _out; 
		_out103: fas.cs = 103; goto _out; 
		_out104: fas.cs = 104; goto _out; 
		_out237: fas.cs = 237; goto _out; 
		_out105: fas.cs = 105; goto _out; 
		_out238: fas.cs = 238; goto _out; 
		_out106: fas.cs = 106; goto _out; 
		_out107: fas.cs = 107; goto _out; 
		_out108: fas.cs = 108; goto _out; 
		_out239: fas.cs = 239; goto _out; 
		_out109: fas.cs = 109; goto _out; 
		_out240: fas.cs = 240; goto _out; 
		_out110: fas.cs = 110; goto _out; 
		_out111: fas.cs = 111; goto _out; 
		_out112: fas.cs = 112; goto _out; 
		_out113: fas.cs = 113; goto _out; 
		_out114: fas.cs = 114; goto _out; 
		_out241: fas.cs = 241; goto _out; 
		_out115: fas.cs = 115; goto _out; 
		_out242: fas.cs = 242; goto _out; 
		_out116: fas.cs = 116; goto _out; 
		_out117: fas.cs = 117; goto _out; 
		_out118: fas.cs = 118; goto _out; 
		_out119: fas.cs = 119; goto _out; 
		_out120: fas.cs = 120; goto _out; 
		_out121: fas.cs = 121; goto _out; 
		_out243: fas.cs = 243; goto _out; 
		_out122: fas.cs = 122; goto _out; 
		_out244: fas.cs = 244; goto _out; 
		_out123: fas.cs = 123; goto _out; 
		_out124: fas.cs = 124; goto _out; 
		_out125: fas.cs = 125; goto _out; 
		_out126: fas.cs = 126; goto _out; 
		_out127: fas.cs = 127; goto _out; 
		_out128: fas.cs = 128; goto _out; 
		_out129: fas.cs = 129; goto _out; 
		_out130: fas.cs = 130; goto _out; 
		_out245: fas.cs = 245; goto _out; 
		_out131: fas.cs = 131; goto _out; 
		_out246: fas.cs = 246; goto _out; 
		_out132: fas.cs = 132; goto _out; 
		_out133: fas.cs = 133; goto _out; 
		_out134: fas.cs = 134; goto _out; 
		_out135: fas.cs = 135; goto _out; 
		_out136: fas.cs = 136; goto _out; 
		_out137: fas.cs = 137; goto _out; 
		_out138: fas.cs = 138; goto _out; 
		_out139: fas.cs = 139; goto _out; 
		_out247: fas.cs = 247; goto _out; 
		_out140: fas.cs = 140; goto _out; 
		_out248: fas.cs = 248; goto _out; 
		_out141: fas.cs = 141; goto _out; 
		_out142: fas.cs = 142; goto _out; 
		_out143: fas.cs = 143; goto _out; 
		_out144: fas.cs = 144; goto _out; 
		_out145: fas.cs = 145; goto _out; 
		_out249: fas.cs = 249; goto _out; 
		_out146: fas.cs = 146; goto _out; 
		_out250: fas.cs = 250; goto _out; 
		_out147: fas.cs = 147; goto _out; 
		_out148: fas.cs = 148; goto _out; 
		_out149: fas.cs = 149; goto _out; 
		_out150: fas.cs = 150; goto _out; 
		_out151: fas.cs = 151; goto _out; 
		_out152: fas.cs = 152; goto _out; 
		_out251: fas.cs = 251; goto _out; 
		_out153: fas.cs = 153; goto _out; 
		_out252: fas.cs = 252; goto _out; 
		_out154: fas.cs = 154; goto _out; 
		_out155: fas.cs = 155; goto _out; 
		_out156: fas.cs = 156; goto _out; 
		_out157: fas.cs = 157; goto _out; 
		_out158: fas.cs = 158; goto _out; 
		_out253: fas.cs = 253; goto _out; 
		_out159: fas.cs = 159; goto _out; 
		_out254: fas.cs = 254; goto _out; 
		_out160: fas.cs = 160; goto _out; 
		_out161: fas.cs = 161; goto _out; 
		_out162: fas.cs = 162; goto _out; 
		_out163: fas.cs = 163; goto _out; 
		_out255: fas.cs = 255; goto _out; 
		_out164: fas.cs = 164; goto _out; 
		_out256: fas.cs = 256; goto _out; 
		_out165: fas.cs = 165; goto _out; 
		_out166: fas.cs = 166; goto _out; 
		_out167: fas.cs = 167; goto _out; 
		_out168: fas.cs = 168; goto _out; 
		_out169: fas.cs = 169; goto _out; 
		_out170: fas.cs = 170; goto _out; 
		_out257: fas.cs = 257; goto _out; 
		_out171: fas.cs = 171; goto _out; 
		_out258: fas.cs = 258; goto _out; 
		_out172: fas.cs = 172; goto _out; 
		_out173: fas.cs = 173; goto _out; 
		_out174: fas.cs = 174; goto _out; 
		_out175: fas.cs = 175; goto _out; 
		_out176: fas.cs = 176; goto _out; 
		_out259: fas.cs = 259; goto _out; 
		_out177: fas.cs = 177; goto _out; 
		_out260: fas.cs = 260; goto _out; 
		_out178: fas.cs = 178; goto _out; 
		_out179: fas.cs = 179; goto _out; 
		_out180: fas.cs = 180; goto _out; 
		_out181: fas.cs = 181; goto _out; 
		_out182: fas.cs = 182; goto _out; 
		_out183: fas.cs = 183; goto _out; 
		_out184: fas.cs = 184; goto _out; 
		_out185: fas.cs = 185; goto _out; 
		_out186: fas.cs = 186; goto _out; 
		_out187: fas.cs = 187; goto _out; 
		_out188: fas.cs = 188; goto _out; 
		_out261: fas.cs = 261; goto _out; 
		_out189: fas.cs = 189; goto _out; 
		_out262: fas.cs = 262; goto _out; 
		_out190: fas.cs = 190; goto _out; 
		_out191: fas.cs = 191; goto _out; 
		_out192: fas.cs = 192; goto _out; 
		_out193: fas.cs = 193; goto _out; 
		_out194: fas.cs = 194; goto _out; 
		_out263: fas.cs = 263; goto _out; 
		_out195: fas.cs = 195; goto _out; 
		_out264: fas.cs = 264; goto _out; 
		_out196: fas.cs = 196; goto _out; 
		_out197: fas.cs = 197; goto _out; 
		_out198: fas.cs = 198; goto _out; 
		_out199: fas.cs = 199; goto _out; 
		_out200: fas.cs = 200; goto _out; 
		_out265: fas.cs = 265; goto _out; 
		_out266: fas.cs = 266; goto _out; 
		_out201: fas.cs = 201; goto _out; 
		_out202: fas.cs = 202; goto _out; 
		_out203: fas.cs = 203; goto _out; 
		_out204: fas.cs = 204; goto _out; 
		_out205: fas.cs = 205; goto _out; 
		_out267: fas.cs = 267; goto _out; 
		_out268: fas.cs = 268; goto _out; 
		_out206: fas.cs = 206; goto _out; 
		_out207: fas.cs = 207; goto _out; 
		_out208: fas.cs = 208; goto _out; 
		_out209: fas.cs = 209; goto _out; 
		_out210: fas.cs = 210; goto _out; 
		_out211: fas.cs = 211; goto _out; 
		_out269: fas.cs = 269; goto _out; 
		_out270: fas.cs = 270; goto _out; 
		_out: {}
	}
	
#line 308 "cfg.rl"
	
	
	if (fas.cs == factions_error)
		return -1;
	if (fas.cs >= factions_first_final)
		return 1;
	return 0;
}

void parse_config(inotify &inot, const std::string &path)
{
	char buf[MAX_LINE];
	struct ParseCfgState fas(inot);
	
	auto f = fopen(path.c_str(), "r");
	if (!f) {
		log_line("%s: failed to open file: '%s'", __func__, path.c_str());
		std::exit(EXIT_FAILURE);
	}
	SCOPE_EXIT{ fclose(f); };
	while (!feof(f)) {
		if (!fgets(buf, sizeof buf, f)) {
			if (!feof(f))
				log_line("%s: io error fetching line of '%s'", __func__, path.c_str());
			break;
		}
		auto llen = strlen(buf);
		if (llen == 0)
			continue;
		if (buf[llen-1] == '\n')
			buf[--llen] = 0;
		++fas.linenum;
		if (do_parse_config(fas, buf, llen) < 0) {
			log_line("%s: do_parse_config(%s) failed at line %zu", __func__, path.c_str(), fas.linenum);
			std::exit(EXIT_FAILURE);
		}
	}
}

