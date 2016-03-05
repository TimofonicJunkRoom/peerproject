#ifndef _RAR_CMDDATA_
#define _RAR_CMDDATA_


#define DefaultStoreList L"7z;ace;arj;bz2;cab;gz;jpeg;jpg;lha;lzh;mp3;rar;taz;tgz;xz;z;zip"

enum RAR_CMD_LIST_MODE {RCLM_AUTO,RCLM_REJECT_LISTS,RCLM_ACCEPT_LISTS};

class CommandData:public RAROptions
{
  private:
    void ProcessSwitchesString(const wchar *Str);
    void ProcessSwitch(const wchar *Switch);
    void BadSwitch(const wchar *Switch);
    uint GetExclAttr(const wchar *Str);

    bool FileLists;
    bool NoMoreSwitches;
    RAR_CMD_LIST_MODE ListMode;
    bool BareOutput;
  public:
    CommandData();
    void Init();

    void ParseCommandLine(bool Preprocess,int argc, char *argv[]);
    void ParseArg(wchar *ArgW);
    void ParseDone();
    void ParseEnvVar();
    void ReadConfig();
    void PreprocessArg(const wchar *Arg);
    void OutTitle();
    void OutHelp(RAR_EXIT ExitCode);
    bool IsSwitch(int Ch);
    bool ExclCheck(const wchar *CheckName,bool Dir,bool CheckFullPath,bool CheckInclList);
    static bool CheckArgs(StringList *Args,bool Dir,const wchar *CheckName,bool CheckFullPath,int MatchMode);
    bool ExclDirByAttr(uint FileAttr);
    bool TimeCheck(RarTime &ft);
    bool SizeCheck(int64 Size);
    bool AnyFiltersActive();
    int IsProcessFile(FileHeader &FileHead,bool *ExactMatch=NULL,int MatchType=MATCH_WILDSUBPATH,
                      wchar *MatchedArg=NULL,uint MatchedArgSize=0);
    void ProcessCommand();
    void AddArcName(const wchar *Name);
    bool GetArcName(wchar *Name,int MaxSize);
    bool CheckWinSize();

    int GetRecoverySize(const wchar *Str,int DefSize);

#ifndef SFX_MODULE
    void ReportWrongSwitches(RARFORMAT Format);
#endif

    wchar Command[NM+16];

    wchar ArcName[NM];

    StringList FileArgs;
    StringList ExclArgs;
    StringList InclArgs;
    StringList ArcNames;
    StringList StoreArgs;
};

#endif
