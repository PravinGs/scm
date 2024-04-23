#ifndef CONFIG_HPP
#define CONFIG_HPP

#pragma once

#include "util/common.hpp"
#include "ids/ids_entity.hpp"

class config
{

public:
    int read_decoder_config(const string &path, std::unordered_map<string, decoder> &table)
    {
        // agent_utils::write_log("config: read_decoder_config: reading " + path, DEBUG);
        if (!std::filesystem::is_regular_file(path))
        {
            LOG_ERROR(Audit::INVALID_FILE + path);
            return Audit::FAILED;
            // agent_utils::write_log("config: read_decoder_config: expected a file " + path, FATAL);
        }
        return read_decoder(path, table);
    }
    int read_xml_rule_config(const string path, std::unordered_map<string, std::unordered_map<int, aconfig>> &table)
    {
        // agent_utils::write_log("config: read_xml_rule_config: reading " + path, DEBUG);
        int result = Audit::SUCCESS;
        int isFile = 0;
        if (os::is_dir_exist(path) && std::filesystem::is_regular_file(path))
        {
            isFile = 1;
        }

        if (isFile)
        {
            result = read_aconfig(path, table);
        }
        else
        {
            string current_file = path;
            vector<string> files;
            if (current_file[current_file.size() - 1] == '/')
            {
                current_file = current_file.substr(0, current_file.find_last_of('/'));
            }
            result = os::get_regular_files(current_file, files);

            if (result == Audit::FAILED)
                return Audit::FAILED;

            if (files.size() == 0)
            {
                LOG_ERROR(Audit::INVALID_PATH + path);
                // ent_utils::write_log("config: read_xml_rule_config: " + INVALID_PATH + path, FAILED);
                return Audit::FAILED;
            }
            for (string file : files)
            {
                result = read_aconfig(file, table);
            }
        }
        return result;
    }
    int read_ini_config_file(const string &config_file, config_table_type &config_table)
    {
        int result = Audit::SUCCESS;
        fstream file(config_file, std::ios::in | std::ios::binary);
        string line, current_section;
        int index = 1;

        if (!file)
        {
            LOG_ERROR(Audit::INVALID_FILE, config_file);
            return Audit::FAILED;
        }

        while (std::getline(file, line))
        {
            line = common::trim(line);

            if (line.empty() || line[0] == ';')
            {
                continue;
            }
            else if (line[0] == '[' && line[line.size() - 1] == ']')
            {
                current_section = common::trim(line.substr(1, line.size() - 2));
            }
            else if (line[0] == '[' && line[line.size() - 1] != ']')
            {
                string line_number = std::to_string(index);
                LOG_ERROR(Audit::INVALID_FILE, config_file, " line_number : ", line_number);
                result = Audit::FAILED;
                break;
            }
            else
            {
                size_t delimiter = line.find('=');
                if (delimiter != string::npos)
                {
                    string key = common::trim(line.substr(0, delimiter));
                    if (!validate_ini_format(key))
                    {
                        string line_number = std::to_string(index);
                        LOG_ERROR(Audit::INVALID_FILE, config_file, line_number);
                        result = Audit::FAILED;
                        break;
                    }
                    string value = common::trim(line.substr(delimiter + 1));
                    validate_ini_format(value);
                    config_table[current_section][key] = value;
                }
            }
            index++;
        }
        return result;
    }

    vector<string> to_vector(const string &columns, const char sep)
    {
        vector<string> names;
        if (columns.find(sep) == string::npos)
        {
            DEBUG("Seperator not found in string: " , std::to_string(sep));
            names.push_back(common::trim(columns));
            return names;
        }
        std::stringstream iss(columns);
        string name;
        while (std::getline(iss, name, sep))
        {
            names.push_back(common::trim(name));
        }
        return names;
    }

public: // private
    bool validate_ini_format(string &line)
    {
        size_t delimiter = line.find(';');
        bool result = true;
        if (delimiter != string::npos)
        {
            string content = common::trim(line.substr(0, delimiter));
            line = content;
            result = false;
        }
        return result;
    }

    int to_int(const string &number)
    {
        if (number.empty())
            return -1;
        int digit;
        try
        {
            digit = stoi(number);
        }
        catch (exception &e)
        {
            digit = -1;
            //LOG_ERROR(e.what());
        }
        return digit;
    }

    int read_decoder(const string &file_name, std::unordered_map<string, decoder> &table)
    {
        /*
            for (const auto& d: decoders)
            {
                decoder s = d.second;
                cout << "<decoder name=" << d.first << " >\n";
                cout << "    <parent> " << s.parent << " </parent>\n";
                cout << "    <program_name_pcre2> " << s.program_name_pcre2 << " </program_name_pcre2>\n";
                cout << "    <pcre2> " << s.pcre2 << " </pcre2>\n";
                cout << "    <prematch_pcre2> " << s.prematch_pcre2 << " </prematch_pcre2>\n";
                cout << "    <order> " << s.order << " </order>\n";
                cout << "   <prematch_offset> " << s.prematch_offset << " </prematch_offset>\n";
                cout << "   <pcre2_offset> " << s.pcre2_offset << " </pcre2_offset>\n";
            }
        */
        if (!os::is_file_exist(file_name))
        {
            LOG_ERROR(Audit::FILE_ERROR, file_name);
            return Audit::FAILED;
        }
        pugi::xml_parse_result result = doc.load_file(file_name.c_str());
        int index = 0;
        if (!result)
        {
            string error = result.description();
            LOG_ERROR("Invalid XML file: " + file_name);
            // agent_utils::write_log("config: read_decoder: " + FILE_ERROR + file_name, FAILED);
            return Audit::FAILED;
        }
        pugi::xml_node root = doc.child("root");
        for (pugi::xml_node group_node = root.child("decoder"); group_node; group_node = group_node.next_sibling("decoder"))
        {
            index++;
            decoder d;
            std::string current_section = group_node.attribute("name").value();
            if (!current_section.empty())
            {
                d.decode = current_section;
            }
            current_section = group_node.child_value("parent");
            if (!current_section.empty())
            {
                d.parent = current_section;
            }
            current_section = group_node.child_value("program_name_pcre2");
            if (!current_section.empty())
            {
                d.program_name_pcre2 = current_section;
            }
            // if (current_section == "^kernel")
            // {
            //     //std::cout << d.decode << "\n";
            // }
            current_section = group_node.child_value("pcre2");
            if (!current_section.empty())
            {
                d.pcre2 = current_section;
            }
            current_section = group_node.child("pcre2").attribute("offset").value();
            if (!current_section.empty())
            {
                d.pcre2_offset = current_section;
            }
            current_section = group_node.child_value("prematch_pcre2");
            if (!current_section.empty())
            {
                d.prematch_pcre2 = current_section;
            }
            current_section = group_node.child("prematch_pcre2").attribute("offset").value();
            if (!current_section.empty())
            {
                d.prematch_offset = current_section;
            }
            current_section = group_node.child_value("order");
            if (!current_section.empty())
            {
                d.order = current_section;
            }
            current_section = group_node.child_value("fts");
            if (!current_section.empty())
            {
                d.fts = current_section;
            }
            if (table.find(d.decode) != table.end())
            {
                table.at(d.decode).update(d);
            }
            else
            {
                table[d.decode] = d;
            }
        }
        LOG("XML parsing success");
        // agent_utils::write_log("config: read_decoder: XML parsing success for " + file_name, DEBUG);
        return Audit::SUCCESS;
    }

    void extract_rule_attributes(pugi::xml_node &root, std::unordered_map<string, std::unordered_map<int, aconfig>> &table)
    {
        for (pugi::xml_node group_node = root; group_node; group_node = group_node.next_sibling("group"))
        {
            string current_section = group_node.attribute("name").value();

            for (pugi::xml_node rule_node = group_node.child("rule"); rule_node; rule_node = rule_node.next_sibling("rule"))
            {
                int digit;
                string str;
                aconfig rule;
                if (!current_section.empty())
                {
                    rule.group = current_section; /*Need clarity about this group assignation*/
                }
                digit = to_int(rule_node.attribute("id").value());
                if (digit != -1)
                {
                    rule.id = digit;
                }
                digit = -1;

                digit = to_int(rule_node.attribute("level").value());
                if (digit != -1)
                {
                    rule.level = digit;
                }
                digit = -1;
                digit = to_int(rule_node.attribute("frequency").value());
                if (digit != -1)
                {
                    rule.frequency = digit;
                }
                digit = -1;
                digit = to_int(rule_node.attribute("timeframe").value());
                if (digit != -1)
                {
                    rule.timeframe = digit;
                }
                digit = -1;
                digit = to_int(rule_node.attribute("ignore").value());
                if (digit != -1)
                {
                    rule.ignore = digit;
                }
                digit = -1;

                digit = to_int(rule_node.child_value("if_matched_sid"));
                // cout << "Match Id: " << digit;
                if (digit != -1)
                {
                    rule.if_matched_id = digit;
                }
                digit = -1;

                digit = to_int(rule_node.child_value("if_sid"));
                if (digit != -1)
                {
                    rule.if_sid = digit;
                }
                digit = -1;
                digit = to_int(rule_node.child_value("same_source_ip"));
                if (digit != -1)
                {
                    rule.same_source_ip = 1;
                }
                digit = -1;
                str = rule_node.child_value("status_pcre2");
                if (!str.empty())
                {
                    rule.status_pcre2 = str;
                }

                str = rule_node.child_value("id_pcre2");
                if (!str.empty())
                {
                    rule.id_pcre2 = str;
                }

                str = rule_node.child_value("extra_data_pcre2");
                if (!str.empty())
                {
                    rule.extra_data_pcre2 = str;
                }

                // Iterate through the child nodes of <rule> to find <pcre2> elements

                /*str = rule_node.child_value("pcre2");
                if (!str.empty())
                {
                    rule.pcre2 = str;
                }*/

                str = rule_node.child_value("program_name_pcre2");
                if (!str.empty())
                {
                    rule.program_name_pcre2 = str;
                }

                str = rule_node.child_value("scrip"); /*Not sure */
                if (!str.empty())
                {
                    rule.script = str;
                }

                str = rule_node.child_value("group");
                if (!str.empty())
                {
                    rule.group = str;
                }

                str = rule_node.child_value("description");
                if (!str.empty())
                {
                    rule.description = str;
                }

                str = rule_node.child_value("decoded_as");
                if (!str.empty())
                {
                    rule.decoded_as = str;
                }

                str = rule_node.child_value("category"); /*check*/
                if (!str.empty())
                {
                    rule.categories = str;
                }

                str = rule_node.child_value("action");
                if (!str.empty())
                {
                    rule.action = str;
                }

                str = rule_node.child_value("options");
                if (!str.empty())
                {
                    rule.options = str;
                }

                str = rule_node.child_value("url_pcre2");
                if (!str.empty())
                {
                    rule.url_pcre2 = str;
                }

                str = rule_node.child_value("compiled_rule");
                if (!str.empty())
                {
                    rule.compiled_rule = str;
                }

                str = rule_node.child_value("hostname_pcre2");
                if (!str.empty())
                {

                    rule.hostname_pcre2 = str;
                }
                for (pugi::xml_node pcre2_node = rule_node.child("pcre2"); pcre2_node; pcre2_node = pcre2_node.next_sibling("pcre2"))
                {
                    string s = pcre2_node.text().as_string();
                    if (!s.empty())
                    {
                        rule.pcre2.push_back(s); // Store each <pcre2> value in the vector
                    }
                }

                table[current_section][rule.id] = rule;
            }
        }
    }

    int read_aconfig(const string& file_name, std::unordered_map<string, std::unordered_map<int, aconfig>> &table)
    {
        if (!os::is_file_exist(file_name))
        {
            LOG_ERROR(Audit::FILE_ERROR + file_name);
            return Audit::FAILED;
        }
        pugi::xml_parse_result result = doc.load_file(file_name.c_str());

        if (!result)
        {
            string error = result.description();
            LOG_ERROR("Invalid XML file: "+ file_name);
            // agent_utils::write_log("config: read_aconfig: " + FILE_ERROR + file_name, FAILED);
            return Audit::FAILED;
        }

        pugi::xml_node root = doc.child("root");
        if (root)
        {
            for (pugi::xml_node group_node = root.child("group"); group_node; group_node = group_node.next_sibling("group"))
            {
                extract_rule_attributes(group_node, table);
            }
        }
        else
        {
            root = doc.child("group");
            extract_rule_attributes(root, table);
        }
        // LOG("XML parsing success");
        // agent_utils::write_log("config: read_aconfig: XML parsing success for " + file_name, DEBUG);
        return Audit::SUCCESS;
    }

private:
    pugi::xml_document doc;
};

#endif // !CONFIG_HPP