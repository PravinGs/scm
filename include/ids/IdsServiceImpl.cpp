#include "ids/IdsServiceImpl.hpp"

const string AFTER_PARENT = "after_parent";
const string AFTER_PREMATCH = "after_prematch";
const string AFTER_PCRE2 = "after_pcre2";

IdsServiceImpl::IdsServiceImpl() : is_valid_ConfigService(false) {}

int IdsServiceImpl::rootkitAnalysis()
{
    Json::Value json = r_check.start();
    if (!json.empty())
    {
        return db.saveRootAnalysisReport(json);
    }
    return SCM::FAILED;
}

void IdsServiceImpl::readConfigServiceFiles(const string &decoder_path, const string &rule_path)
{
    is_valid_ConfigService = db.readConfigServiceFiles(decoder_path, rule_path);
}

void extract_network_log(LogEvent &log_info)
{
    /* IN= OUT=lo SRC=127.0.0.1 DST=127.0.0.1 LEN=52 Tos=0x00 PREC=0x00
    TTL=64 ID=38860 DF PROTO=TCP SPT=8888 DPT=55764 WINDOW=512 RES=0x00 ACK URGP=0 */

    string temp = log_info.log;
    string src_ip = temp.substr(temp.find("SRC") + 4);
    src_ip = src_ip.substr(0, src_ip.find_first_of(' '));
    string dest_ip = temp.substr(temp.find("DST") + 4);
    dest_ip = dest_ip.substr(0, dest_ip.find_first_of(' '));
    string proto = temp.substr(temp.find("PROTO") + 6);
    proto = proto.substr(0, proto.find_first_of(' '));

    log_info.src_ip = src_ip;
    log_info.dest_ip = dest_ip;
    log_info.proto = proto;

    return;
}

int IdsServiceImpl::isValidSyslog(size_t size)
{
    return (size <= 1024) ? SCM::SUCCESS : SCM::FAILED;
}

string IdsServiceImpl::decodeGroup(LogEvent &log_info)
{
    string group;
    std::unordered_map<string, decoder> decoder_list = db.getDecoderList(); // think abou the optimization
    for (const auto &d : decoder_list)
    {
        decoder p = d.second;
        int match = 0;
        string match_data;
        string after_match;
        size_t position = 0;

        if (!p.program_name_pcre2.empty())
        {
            match = pcreMatch(log_info.program, p.program_name_pcre2, match_data, position);
            after_match = log_info.program.substr(position);
            if (match == 1 && position > 0)
            {
                group = (!p.parent.empty()) ? p.parent : p.decode;
                addDecoderCache(group);
                break;
            }
        }

        if (!p.prematch_pcre2.empty())
        {
            string input = (!p.prematch_offset.empty() && p.prematch_offset == AFTER_PARENT) ? log_info.message : log_info.log;

            match = pcreMatch(input, p.prematch_pcre2, match_data, position);
            after_match = input.substr(position);
            if (match == 1 && position > 0)
            {
                group = (!p.parent.empty()) ? p.parent : p.decode;
                addDecoderCache(group);
                break;
            }
        }

        if (!p.pcre2.empty())
        {
            string input;

            if (p.pcre2_offset == AFTER_PARENT)
            {
                input = log_info.message;
            }
            else if (p.pcre2_offset == AFTER_PREMATCH)
            {
                input = (match_data.empty()) ? log_info.log : match_data; /*Need to update*/
            }
            else
            {
                input = log_info.log;
            }
            match = pcreMatch(input, p.pcre2, match_data, position);
            after_match = input.substr(position);
            if (match == 1 && position > 0)
            {
                group = (!p.parent.empty()) ? p.parent : p.decode;
                addDecoderCache(group);
                break;
            }
        }
    }
    return group;
}

void IdsServiceImpl::addDecoderCache(const string &decoder)
{
    if (decoder_cache.size() >= SCM::MAX_CACHE_SIZE)
    {
        decoder_cache.insert(decoder_cache.begin() + 0, decoder);
    }
    else
    {
        decoder_cache.push_back(decoder);
    }
}

LogEvent IdsServiceImpl::decodeLog(const string &log, const string &format)
{
    LogEvent log_info;
    string timestamp, user, program, message, group;
    const string &logToParse = (std::count(log.begin(), log.end(), '|') >= 2) ? log : formatSyslog(log, format);
    if (logToParse.empty())
        return log_info;
    std::istringstream ss(logToParse);
    std::getline(ss, timestamp, '|');
    std::getline(ss, user, '|');
    std::getline(ss, program, '|');
    std::getline(ss, message, '|');
    log_info.log = log;
    log_info.format = (strcmp(format.c_str(), "auth") == 0) ? "pam" : format;
    log_info.size = log.size();
    log_info.timestamp = timestamp;
    log_info.user = user;
    log_info.program = (format == "dpkg") ? format : program;
    log_info.message = message;
    log_info.decoded = decodeGroup(log_info); // Need to invoke the decoder group function

    if (log_info.log.find("SRC=") != string::npos)
    {
        extract_network_log(log_info);
    }
    return log_info;
}

string IdsServiceImpl::formatSyslog(const string &log, const string &format)
{
    string format_log, token, formatted_time;
    int index = 0;
    string current_time = log.substr(0, 15);
    std::stringstream stream;
    if (format == "dpkg")
    {
        string temp;
        format_log += log.substr(0, 19);
        format_log += "|" + os::host_name;
        format_log += "|" + format;
        temp = log.substr(20);
        format_log += "|" + temp;
        return format_log;
    }
    else
    {
        if (Common::convertSysLogToUtcFormat(current_time, formatted_time) == SCM::FAILED)
        {
            return "";
        }
        stream << log.substr(16);
    }
    format_log += formatted_time;
    while (std::getline(stream, token, ' ') && index < 3)
    {
        if (index == 2)
        {
            string message = token;
            while (std::getline(stream, token, ' '))
            {
                message += ' ' + token;
            }
            format_log += '|' + message;
            index = 4;
            continue;
        }
        format_log += '|' + token;
        index++;
    }
    return format_log;
}

int IdsServiceImpl::regexMatch(const string &log, const string &pattern, string &match)
{
    std::regex r(pattern);
    std::smatch matches;
    int result = regex_search(log, matches, r);
    for (const auto &s : matches)
    {
        match += s.str();
    }
    return (result && matches.size() > 0) ? 1 : 0;
}

int IdsServiceImpl::pcreMatch(const string &input, const string &pattern, string &match, size_t &position)
{
    int errorcode = -1;
    PCRE2_SIZE erroroffset;

    // Compile the pattern
    pcre2_code *re = pcre2_compile(
        reinterpret_cast<PCRE2_SPTR8>(pattern.c_str()), // Pattern string
        PCRE2_ZERO_TERMINATED,                          // Length of pattern
        PCRE2_CASELESS,                                 // Compile options (case-insensitive)
        &errorcode,                                     // Error code
        &erroroffset,                                   // Error offset
        nullptr);                                       // Compile context

    if (re == nullptr)
    {
        PCRE2_UCHAR buffer[256];
        pcre2_get_error_message(errorcode, buffer, sizeof(buffer));
        std::cerr << "PCRE2 compilation failed at offset " << erroroffset << ": " << buffer << "\n";
        return SCM::FAILED;
    }

    // Match the input against the pattern
    pcre2_match_data *match_data = pcre2_match_data_create_from_pattern(re, nullptr);
    int rc = pcre2_match(
        re,                                           // Compiled pattern
        reinterpret_cast<PCRE2_SPTR8>(input.c_str()), // Input string
        PCRE2_ZERO_TERMINATED,                        // Length of input
        0,                                            // Start offset
        0,                                            // Match options
        match_data,                                   // Match data
        nullptr);                                     // Match context

    if (rc >= 0)
    {
        PCRE2_SIZE *ovector = pcre2_get_ovector_pointer(match_data);

        // Extract the matched substring
        const char *matched_string_start = reinterpret_cast<const char *>(input.c_str()) + ovector[0];
        PCRE2_SIZE matched_string_length = ovector[1] - ovector[0];

        std::string matched_str(matched_string_start, matched_string_length);

        match = matched_str;

        // Calculate the position of the last matched value
        PCRE2_SIZE last_match_start = ovector[2 * (rc - 1)];                              // Start offset of the last match
        PCRE2_SIZE last_match_length = ovector[2 * (rc - 1) + 1] - ovector[2 * (rc - 1)]; // Length of the last match

        // Calculate the end position of the last matched value
        PCRE2_SIZE last_match_end = last_match_start + last_match_length;
        position = reinterpret_cast<size_t>(last_match_end);
    }
    pcre2_code_free(re);
    pcre2_match_data_free(match_data);
    return rc;
}

int IdsServiceImpl::isRuleFound(const int ruleId)
{
    for (const auto &rule : matched_rules)
    {
        if (rule.id == ruleId)
            return SCM::SUCCESS;
    }
    return SCM::FAILED;
}

void IdsServiceImpl::addMatchedRule(const IdRule &rule, const string &log)
{
    int result = isRuleFound(rule.id);
    if (result == SCM::SUCCESS)
        return;
    if (matched_rules.size() >= SCM::MAX_CACHE_SIZE)
    {
        matched_rules.insert(matched_rules.begin() + 0, rule);
    }
    else
    {
        matched_rules.push_back(rule);
    }
    DEBUG("rule id: " + std::to_string(rule.id) + " -> " + log);
    return;
}

void IdsServiceImpl::match(LogEvent &log_info, std::unordered_map<int, AConfigService> &ruleSet)
{
    for (const auto &r : matched_rules)
    {
        AConfigService rule = db.getAConfigServiceRuleByGroupAndId(r.group, r.id);
        match(log_info, rule);
        if (log_info.is_matched == 1)
            break;
    }
    for (auto &r : ruleSet)
    {
        AConfigService rule = r.second;
        match(log_info, rule);
        if (log_info.is_matched == 1)
            break;
    }
    return;
}

void IdsServiceImpl::match(LogEvent &log_info, AConfigService &rule_info)
{
    bool is_parent_rule_matching = false;
    string match_data;
    size_t position;
    IdRule matched_rule = {rule_info.group, rule_info.id};
    PRule pRule;
    {
        std::set<int> processed_rules_id;
        for (int i : this->processed_rules)
        {
            processed_rules_id.insert(i);
        }
        processing_rules.erase(
            std::remove_if(
                processing_rules.begin(),
                processing_rules.end(),
                [&processed_rules_id](const PRule &s)
                {
                    // Return true if the PRule.id is in processed_rules_id
                    return processed_rules_id.find(s.id) != processed_rules_id.end();
                }),
            processing_rules.end());
    }
    
    if (!rule_info.decoded_as.empty() && rule_info.decoded_as == log_info.decoded)
    {
        log_info.is_matched = 1;
        log_info.rule_id = rule_info.id;
        log_info.group = rule_info.group;
        is_parent_rule_matching = true;
        addMatchedRule(matched_rule, log_info.log);
        return;
    }

    if (!rule_info.regex.empty()) /* Checking the regex patterns if exists in the rule */
    {
        int result = regexMatch(log_info.log, rule_info.regex, match_data);
        if (result == 1)
        {
            log_info.is_matched = 1;
            log_info.rule_id = rule_info.id;
            log_info.group = rule_info.group;
            is_parent_rule_matching = true;
            addMatchedRule(matched_rule, log_info.log);
            return;
        }
    }

    if (rule_info.pcre2.size() > 0)
    {
        int result = SCM::SUCCESS;
        for (const string &pattern : rule_info.pcre2)
        {
            result = pcreMatch(log_info.log, pattern, match_data, position);
            if (result > 0 && !match_data.empty())
            {
                log_info.is_matched = 1;
                log_info.rule_id = rule_info.id;
                log_info.group = rule_info.group;
                is_parent_rule_matching = true;
                addMatchedRule(matched_rule, log_info.log);
                return;
            }
        }
        if (result > 0 && result > 0 && !match_data.empty())
        {
            return;
        }
    }
    if (!rule_info.program_name_pcre2.empty())
    {
        int result = pcreMatch(log_info.program, rule_info.program_name_pcre2, match_data, position);
        if (result > 0 && !match_data.empty())
        {
            log_info.is_matched = 1;
            log_info.rule_id = rule_info.id;
            log_info.group = rule_info.group;
            is_parent_rule_matching = true;
            addMatchedRule(matched_rule, log_info.log);
            return;
        }
    }

    if (rule_info.max_log_size > 0) /* Validating the syslog size */
    {
        int result = isValidSyslog(log_info.size);
        if (result == SCM::FAILED)
        {
            log_info.is_matched = 1;
            log_info.rule_id = rule_info.id;
            log_info.group = rule_info.group;
            addMatchedRule(matched_rule, log_info.log);
            return;
        }
    }

    if (rule_info.frequency > 0 && rule_info.timeframe > 0) /*Check if the rule has time based matcher, if it is mark this rule as the processing rule.*/
    {
        pRule.id = rule_info.id;
        pRule.start = Common::stringToTime(log_info.timestamp);
        pRule.end = pRule.start + rule_info.timeframe;
        pRule.frequency = rule_info.frequency;
        pRule.time = rule_info.timeframe;
        if (rule_info.same_source_ip == 1) /* Check at the XML pasing. */
        {
            pRule.same_source_ip = 1;
        }

        if (rule_info.same_id == 1)
        {
            pRule.same_id = 1;
        }
    }
    if (rule_info.if_matched_id > 0) /* If matched_sid enabled apply the child rules alert value to this rule. */
    {
        pRule.if_mid = rule_info.if_matched_id;
        if (isRuleFound(rule_info.if_matched_id) == SCM::SUCCESS)
        {
            pRule.d_frequency++;
            log_info.is_matched = 1;
            log_info.rule_id = rule_info.id;
            log_info.group = rule_info.group;
            addMatchedRule(matched_rule, log_info.log);
            return;
        }
    }

    else if (rule_info.if_matched_id > 0 && rule_info.same_id == 1)
    {
        pRule.if_mid = rule_info.if_matched_id;
        for (PRule p : processing_rules)
        {
            if (isRuleFound(p.if_mid) == SCM::SUCCESS)
            {
                p.d_frequency++;
            }
        }
    }

    if (rule_info.same_source_ip == 1 && !log_info.src_ip.empty()) /* Is is firewall related log. Then check any rules are being in processing state.*/
    {
        for (PRule p : processing_rules) /*Processing rule has the src_ip, that frequency will be monitored for the given timeframe in the rule.*/
        {
            if (strcmp(p.src_ip.c_str(), log_info.src_ip.c_str()) == 0 && p.same_source_ip == 1) /*src_ip matching, if it matches remove increase the helper_varible.*/
            {
                p.d_frequency++;
            }
        }
    }

    if (rule_info.if_sid > 0 && rule_info.same_id == 1) /*If the child id of this rule exists, then you can set child id to the log*/
    {
        pRule.if_sid = rule_info.if_sid;
        for (PRule p : processing_rules)
        {
            if (isRuleFound(p.if_sid == SCM::SUCCESS))
            {
                p.d_frequency++;
            }
        }
    }

    if (is_parent_rule_matching)
    {
        log_info.rule_id = rule_info.id;

    } /* If the parent, child matched, priority to parent. */

    if (pRule.id > 0)
    {
        processing_rules.push_back(pRule);
    }

    if (processing_rules.size() > 0) /* Checking if rules are about to expire or not. */
    {
        for (int i = 0; i < (int)processing_rules.size(); i++)
        {
            PRule pRule = processing_rules[i];
            if ((pRule.end < Common::stringToTime(log_info.timestamp) && pRule.d_frequency != pRule.frequency))
            {
                processed_rules.push_back(i); /* The rule expired */
            }

            else if ((pRule.end < Common::stringToTime(log_info.timestamp) && pRule.frequency == pRule.d_frequency) || pRule.frequency <= pRule.d_frequency)
            {
                /*The rule ID'd and have to alerted*/
                log_info.is_matched = 1;
                log_info.rule_id = pRule.id;
                log_info.group = pRule.group;
                matched_rule.group = pRule.group;
                matched_rule.id = pRule.id;
                addMatchedRule(matched_rule, log_info.log);
                processed_rules.push_back(i);
                return;
            }
        }
    }
    return;
}

void IdsServiceImpl::match(LogEvent &log_info)
{
    std::unordered_map<string, std::unordered_map<int, AConfigService>> rules = db.getAllAConfigServiceRules();
    if (log_info.format.empty() || rules.find(log_info.group) == rules.end())
    {
        for (auto &r : rules)
        {
            match(log_info, r.second);
            if (log_info.is_matched == 1)
                break;
        }
    }
    else
    {
        match(log_info, rules.at(log_info.group));
    }
}

string get_log_file_format(const string& file)
{
    string format;
    if (file.find("dpkg") != string::npos)
    {
        format = "dpkg";
    }
    else if (file.find("auth") != string::npos)
    {
        format = "auth";
    }
    else
    {
        format = "syslog";
    }
    return format;
}

int IdsServiceImpl::analyseFile(const string &file)
{
    vector<LogEvent> matched_logs;
    fstream fp(file, std::ios::in);
    if (!fp)
    {
        LOG_ERROR(SCM::FILE_ERROR + file);
        return SCM::FAILED;
    }
    string format = get_log_file_format(file);
    DEBUG("log analysis started for " + file);
    string line;
    while (std::getline(fp, line))
    {
        if (line.empty())
        {
            continue;
        }
        LogEvent log_info = decodeLog(line, format);
        match(log_info);
        if (log_info.is_matched == 1)
        {
            matched_logs.push_back(log_info);
        }
    }
    fp.close();
    DEBUG("total matched logs" + std::to_string(matched_logs.size()));
    return db.save(matched_logs);
}

int IdsServiceImpl::start(AnalysisEntity &entity)
{
    int result = SCM::SUCCESS;
    vector<string> files;
    readConfigServiceFiles(entity.decoder_path, entity.rules_path);

    if (!is_valid_ConfigService)
    {
        return SCM::FAILED;
    }

    int is_file = (os::isFileExist(entity.log_path) && std::filesystem::is_regular_file(entity.log_path)) ? 1 : 0;

    if (is_file)
    {
        result = analyseFile(entity.log_path);
    }
    else
    {
        string current_file = entity.log_path;
        if (current_file[current_file.size() - 1] == '/')
        {
            current_file = current_file.substr(0, current_file.find_last_of('/'));
        }
        result = os::getRegularFiles(current_file, files);
        if (result == SCM::FAILED)
        {
            return SCM::FAILED;
        }
        if (files.size() == 0)
        {
            LOG_ERROR(SCM::INVALID_PATH + entity.log_path);
            return SCM::FAILED;
        }
        for (const string &file : files)
        {
            result = analyseFile(file);
        }
    }
    return result;
}
// ids_impl_service.cpp