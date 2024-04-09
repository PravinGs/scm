#ifndef IDS_SERVICE_IMPL_HPP
#define IDS_SERVICE_IMPL_HPP
 
#include "util/common.hpp"
#include "ids/ids_entity.hpp"
#include "ids/ids_service.hpp"
#include "ids/ids_repository.hpp"
#include "util/config.hpp"
 
class ids_service_impl : public ids_service
{
public:
    ids_service_impl();
 
    void read_config_files(const string& decoder_path, const string& rules_path);
 
    int is_valid_syslog(const size_t size);
 
    log_event decode_log(const string& log, const string& format);
 
    void add_decoder_cache(const string& decoder);
 
    string format_syslog(const string& log, const string& format);
 
    int regex_match(const string& log, const string& pattern, string& match);
 
    int pcre_match(const string& input, const string& pattern, string& match, size_t& position);
 
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
 
    void match(log_event& logInfo);
 
    void match(log_event& logInfo, aconfig& ruleInfo);
 
    void match(log_event& logInfo, std::unordered_map<int, aconfig>& ruleSet);
 
    int analyse_file(const string& file);
 
    int start(analysis_entity& entity);
 
    virtual ~ids_service_impl() {}
 
private:
    int is_rule_found(const int ruleId);
    void add_matched_rule(const id_rule& rule, const string& log);
    string decode_group(log_event& logEvent);
private:
    config config_s;
    ids_repository db;
    vector<p_rule> processing_rules;
    vector<int> processed_rules;
    vector<id_rule> matched_rules;
    vector<string> decoder_cache;
    bool is_valid_config;
};
 
#endif
//ids_impl_Service.hpp