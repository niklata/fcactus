#include "fcactus.hpp"
#include <cstring>
#include <climits>
#include <fstream>
extern "C" {
#include "nk/privilege.h"
}
#include <boost/algorithm/string/replace.hpp>

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
            fmt::print(stderr, "ignoring invalid job at line {}\n", linenum);
            wm.reset();
            return;
        }
        inw.add(std::move(wm));
        wm.reset();
    }

    void setgroupv()
    {
        if (nk_gidbyname(v_str, &wm->group_)) {
            fmt::print(stderr, "{}: nonexistent group specified at line {}\n",
                       __func__, linenum);
            std::exit(EXIT_FAILURE);
        }
    }

    void setuserv()
    {
        if (nk_uidgidbyname(v_str, &wm->user_, &wm->group_)) {
            fmt::print(stderr, "{}: nonexistent user specified at line {}\n",
                       __func__, linenum);
            std::exit(EXIT_FAILURE);
        }
    }

    void setlim(int type)
    {
        struct rlimit rli;
        rli.rlim_cur = v_int == 0 ? RLIM_INFINITY : v_int;
        rli.rlim_max = v_int2 == 0 ? RLIM_INFINITY : v_int2;

        if (!wm->limits_)
            wm->limits_ = std::make_unique<rlimits>();

        switch (type) {
        case RLIMIT_CPU: wm->limits_->cpu = rli; break;
        case RLIMIT_FSIZE: wm->limits_->fsize = rli; break;
        case RLIMIT_DATA: wm->limits_->data = rli; break;
        case RLIMIT_STACK: wm->limits_->stack = rli; break;
        case RLIMIT_CORE: wm->limits_->core = rli; break;
        case RLIMIT_RSS: wm->limits_->rss = rli; break;
        case RLIMIT_NPROC: wm->limits_->nproc = rli; break;
        case RLIMIT_NOFILE: wm->limits_->nofile = rli; break;
        case RLIMIT_MEMLOCK: wm->limits_->memlock = rli; break;
    #ifndef BSD
        case RLIMIT_AS: wm->limits_->as = rli; break;
        case RLIMIT_MSGQUEUE: wm->limits_->msgqueue = rli; break;
        case RLIMIT_NICE: wm->limits_->nice = rli; break;
        case RLIMIT_RTTIME: wm->limits_->rttime = rli; break;
        case RLIMIT_RTPRIO: wm->limits_->rtprio = rli; break;
        case RLIMIT_SIGPENDING: wm->limits_->sigpending = rli; break;
    #endif /* BSD */
        default: fmt::print(stderr, "{}: Bad RLIMIT_type specified.\n", __func__);
                 std::exit(EXIT_FAILURE);
        }
    }

};

struct pckm {
    pckm() : st(nullptr), cs(0) {}
    char *st;
    int cs;
};

%%{
    machine parse_cmd_key_m;
    access pckm.;

    action St { pckm.st = p; }
    action CmdEn {
        fas.wm->cmd_ = std::string(pckm.st, p - pckm.st);
        boost::algorithm::replace_all(fas.wm->cmd_, "\\ ", " ");
        boost::algorithm::replace_all(fas.wm->cmd_, "\\\\", "\\");
    }
    action ArgEn { fas.wm->args_ = std::string(pckm.st, p - pckm.st); }

    sptab = [ \t];
    cmdstr = ([^\0 \t] | '\\\\' | '\\ ')+;
    cmd = sptab* (cmdstr > St % CmdEn);
    args = sptab+ ([^\0])* > St % ArgEn;
    main := cmd args?;
}%%

%% write data;

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
        fmt::print(stderr, "Duplicate 'command' value at line {}\n",
                   fas.linenum);
        std::exit(EXIT_FAILURE);
    }

    %% write init;
    %% write exec;

    if (pckm.cs == parse_cmd_key_m_error) {
        fas.cmdret = -1;
        fmt::print(stderr, "Malformed 'command' value at line {}\n",
                   fas.linenum);
        std::exit(EXIT_FAILURE);
    } else if (pckm.cs >= parse_cmd_key_m_first_final)
        fas.cmdret = 1;
    else {
        fas.cmdret = -2;
        fmt::print(stderr, "Incomplete 'command' value at line {}\n",
                   fas.linenum);
        std::exit(EXIT_FAILURE);
    }
}

%%{
    machine factions;
    access fas.;

    spc = [ \t];
    eqsep = spc* '=' spc*;

    action IntValSt {
        fas.intv_st = p;
        fas.v_int = fas.v_int2 = 0;
        fas.intv2_exist = false;
    }
    action IntValEn { fas.v_int = atoi(fas.intv_st); }
    action IntVal2St { fas.intv2_st = p; }
    action IntVal2En {
        fas.v_int2 = atoi(fas.intv2_st);
        fas.intv2_exist = true;
    }

    action StrValSt { fas.strv_st = p; fas.v_strlen = 0; }
    action StrValEn {
        fas.v_strlen = p - fas.strv_st;
        if (fas.v_strlen <= INT_MAX) {
            ssize_t snl = snprintf(fas.v_str, sizeof fas.v_str,
                                   "%.*s", (int)fas.v_strlen, fas.strv_st);
            if (snl < 0 || (size_t)snl >= sizeof fas.v_str) {
                fmt::print(stderr, "error parsing line {} in crontab; too long?\n",
                           fas.linenum);
                std::exit(EXIT_FAILURE);
            }
        }
    }

    intrangeval = (digit+ > IntValSt % IntValEn)
                  (',' (digit+ > IntVal2St % IntVal2En))?;
    stringval = ([^\0\n]+ > StrValSt % StrValEn);

    action LimAsEn { fas.setlim(RLIMIT_AS); }
    action LimMemlockEn { fas.setlim(RLIMIT_MEMLOCK); }
    action LimNofileEn { fas.setlim(RLIMIT_NOFILE); }
    action LimNprocEn { fas.setlim(RLIMIT_NPROC); }
    action LimRssEn { fas.setlim(RLIMIT_RSS); }
    action LimCoreEn { fas.setlim(RLIMIT_CORE); }
    action LimStackEn { fas.setlim(RLIMIT_STACK); }
    action LimDataEn { fas.setlim(RLIMIT_DATA); }
    action LimFsizeEn { fas.setlim(RLIMIT_FSIZE); }
    action LimCpuEn { fas.setlim(RLIMIT_CPU); }
    action LimMsgQueueEn { fas.setlim(RLIMIT_MSGQUEUE); }
    action LimNiceEn { fas.setlim(RLIMIT_NICE); }
    action LimRtTimeEn { fas.setlim(RLIMIT_RTTIME); }
    action LimRtPrioEn { fas.setlim(RLIMIT_RTPRIO); }
    action LimSigPendingEn { fas.setlim(RLIMIT_SIGPENDING); }

    lim_as = 'l_as'i eqsep intrangeval % LimAsEn;
    lim_memlock = 'l_memlock'i eqsep intrangeval % LimMemlockEn;
    lim_nofile = 'l_nofile'i eqsep intrangeval % LimNofileEn;
    lim_nproc = 'l_nproc'i eqsep intrangeval % LimNprocEn;
    lim_rss = 'l_rss'i eqsep intrangeval % LimRssEn;
    lim_core = 'l_core'i eqsep intrangeval % LimCoreEn;
    lim_stack = 'l_stack'i eqsep intrangeval % LimStackEn;
    lim_data = 'l_data'i eqsep intrangeval % LimDataEn;
    lim_fsize = 'l_fsize'i eqsep intrangeval % LimFsizeEn;
    lim_cpu = 'l_cpu'i eqsep intrangeval % LimCpuEn;
    lim_msgqueue = 'l_msgqueue'i eqsep intrangeval % LimMsgQueueEn;
    lim_nice = 'l_nice'i eqsep intrangeval % LimNiceEn;
    lim_rttime = 'l_rttime'i eqsep intrangeval % LimRtTimeEn;
    lim_rtprio = 'l_rtprio'i eqsep intrangeval % LimRtPrioEn;
    lim_sigpending = 'l_sigpending'i eqsep intrangeval % LimSigPendingEn;

    action GroupEn { fas.setgroupv(); }
    action UserEn { fas.setuserv(); }
    action ChrootEn { fas.wm->chroot_ = std::string(fas.v_str, fas.v_strlen); }
    action CommandEn { parse_command_key(fas); }
    action WatchEn { fas.wm->filepath_ = std::string(fas.v_str, fas.v_strlen); }

    group = 'group'i eqsep stringval % GroupEn;
    user = 'user'i eqsep stringval % UserEn;
    chroot = 'chroot'i eqsep stringval % ChrootEn;
    command = 'command'i eqsep stringval % CommandEn;
    watch = 'watch'i eqsep stringval % WatchEn;

    action FAccessEn { fas.wm->eventflags_ |= IN_ACCESS; }
    action FAttribEn { fas.wm->eventflags_ |= IN_ATTRIB; }
    action FCloseWriteEn { fas.wm->eventflags_ |= IN_CLOSE_WRITE; }
    action FCloseNoWriteEn { fas.wm->eventflags_ |= IN_CLOSE_NOWRITE; }
    action FCloseEn { fas.wm->eventflags_ |= IN_CLOSE; }
    action FCreateEn { fas.wm->eventflags_ |= IN_CREATE; }
    action FDeleteEn { fas.wm->eventflags_ |= IN_DELETE; }
    action FDeleteSelfEn { fas.wm->eventflags_ |= IN_DELETE_SELF; }
    action FModifyEn { fas.wm->eventflags_ |= IN_MODIFY; }
    action FMoveSelfEn { fas.wm->eventflags_ |= IN_MOVE_SELF; }
    action FMoveFromEn { fas.wm->eventflags_ |= IN_MOVED_FROM; }
    action FMoveToEn { fas.wm->eventflags_ |= IN_MOVED_TO; }
    action FOpenEn { fas.wm->eventflags_ |= IN_OPEN; }
    action FMoveEn { fas.wm->eventflags_ |= IN_MOVE; }
    faccess = 'faccess'i % FAccessEn;
    fattrib = 'fattrib'i % FAttribEn;
    fclosewrite = 'fclosewrite'i % FCloseWriteEn;
    fclosenowrite = 'fclosenowrite'i % FCloseNoWriteEn;
    fclose = 'fclose'i % FCloseEn;
    fcreate = 'fcreate'i % FCreateEn;
    fdelete = 'fdelete'i % FDeleteEn;
    fdeleteself = 'fdeleteself'i % FDeleteSelfEn;
    fmodify = 'fmodify'i % FModifyEn;
    fmoveself = 'fmoveself'i % FMoveSelfEn;
    fmovefrom = 'fmovefrom'i % FMoveFromEn;
    fmoveto = 'fmoveto'i % FMoveToEn;
    fopen = 'fopen'i % FOpenEn;
    fmove = 'fmove'i % FMoveEn;

    cmds = watch | command | chroot | user | group |
           faccess | fattrib | fclosewrite | fclosenowrite | fclose | fcreate |
           fdelete | fdeleteself | fmodify | fmoveself | fmovefrom | fmoveto |
           fopen | fmove |
           lim_cpu | lim_fsize | lim_data | lim_stack |
           lim_core | lim_rss | lim_nproc | lim_nofile | lim_memlock | lim_as |
           lim_msgqueue | lim_nice | lim_rttime | lim_rtprio | lim_sigpending;

    action CreateJob { fas.finish_job(); fas.create_job(); }

    job_terminator = (';;' > CreateJob);

    main := job_terminator | cmds;
}%%

%% write data;

static int do_parse_config(ParseCfgState &fas, const std::string &l)
{
    const char *p = l.c_str();
    const char *pe = p + l.size();
    const char *eof = pe;

    %% write init;
    %% write exec;

    if (fas.cs == factions_error)
        return -1;
    if (fas.cs >= factions_first_final)
        return 1;
    return 0;
}

void parse_config(inotify &inot, const std::string &path)
{
    struct ParseCfgState fas(inot);

    std::string l;
    std::ifstream f(path, std::ifstream::in);
    if (f.fail() || f.bad() || f.eof()) {
        fmt::print(stderr, "{}: failed to open file: '{}'\n", __func__, path);
        std::exit(EXIT_FAILURE);
    }
    while (1) {
        std::getline(f, l);
        ++fas.linenum;
        if (f.eof())
            break;
        else if (f.bad() || f.fail()) {
            fmt::print(stderr, "{}: io error fetching line of '{}'\n",
                       __func__, path);
            std::exit(EXIT_FAILURE);
        }
        if (l.empty())
            continue;
        auto r = do_parse_config(fas, l);
        if (r < 0) {
            fmt::print(stderr, "{}: do_parse_config({}) failed at line {}\n",
                       __func__, path, fas.linenum);
            std::exit(EXIT_FAILURE);
        }
    }
}

