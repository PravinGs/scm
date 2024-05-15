#ifndef IDS_SERVICE_IMPL_HPP
#define IDS_SERVICE_IMPL_HPP

#include "util/Common.hpp"
#include "ids/IdsEntity.hpp"
#include "ids/IdsService.hpp"
#include "ids/IdsRepository.hpp"
#include "util/ConfigService.hpp"
#include "rootkit/RootCheckService.hpp"

class IdsServiceImpl : public IdsService
{
public:
    IdsServiceImpl();
    int start(AnalysisEntity &entity);
    int rootkitAnalysis();
    
    int start(config_table_type &config_table)
    {
        return SCM::SUCCESS;
    }

    virtual ~IdsServiceImpl() {}

private:
    void readConfigServiceFiles(const string &decoder_path, const string &rules_path);

    int isValidSyslog(const size_t size);

    LogEvent decodeLog(const string &log, const string &format);

    void addDecoderCache(const string &decoder);

    string formatSyslog(const string &log, const string &format);

    int regexMatch(const string &log, const string &pattern, string &match);

    int pcreMatch(const string &input, const string &pattern, string &match, size_t &position);

    /*
        read the rules one by one
        checking if any rules in the memory, which is expired if it is remove it,
        checking any rules is processing state wait for logs
        Checking and do the regex matching for the logs
        Checking the read syslog size.
        Checking any rule which have child id, that might be identified before.
        Checking the timeframe or frequency is available for the rule if it is, do the given.
        And during those checkings if rule matched add the rule id to the Id'd rules.
    */

    void match(LogEvent &logInfo);

    void match(LogEvent &logInfo, AConfigService &ruleInfo);

    void match(LogEvent &logInfo, std::unordered_map<int, AConfigService> &ruleSet);

    int analyseFile(const string &file);

private:
    int isRuleFound(const int ruleId);
    void addMatchedRule(const IdRule &rule, const string &log);
    string decodeGroup(LogEvent &logEvent);

private:
    ConfigService configService;
    IdsRepository db;
    vector<PRule> processing_rules;
    vector<int> processed_rules;
    vector<IdRule> matched_rules;
    vector<string> decoder_cache;
    bool is_valid_ConfigService;
    RootCheck r_check;
};

#endif // IDS_SERVICE_IMPL_HPP