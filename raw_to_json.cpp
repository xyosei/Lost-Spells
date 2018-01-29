#include <map>
#include <vector>
#include <set>
#include <string>
#include <iostream>
#include <fstream>
#include <fstream>
#include <cstdlib>
#include <algorithm>

typedef std::map<std::string, std::map<std::string, std::string> > spell_t;
typedef std::map<std::string, std::string> tmp_t;

std::string ReplaceAll(std::string str, const std::string& from, const std::string& to) {
        size_t start_pos = 0;
        while((start_pos = str.find(from, start_pos)) != std::string::npos) {
                str.replace(start_pos, from.length(), to);
                start_pos += to.length(); // Handles case where 'to' is a substring of 'from'
        }
        return str;
}
std::string lower( std::string str ){
        for ( unsigned int i = 0; i < str.size(); i++)
                str[i] = tolower(str[i]);
        return str;
}

bool regular_print(spell_t &spells){
        for ( spell_t::iterator s_itr = spells.begin(); s_itr != spells.end(); s_itr++ ) {
                std::cout << s_itr->first << '\n';
                for ( tmp_t::iterator t_itr = s_itr->second.begin(); t_itr != s_itr->second.end(); t_itr++ ) {
                        std::cout << "  " << t_itr->first << ": " << t_itr->second << '\n';
                }
        }
        return true;
}

bool json_print(spell_t &spells){

        //Abjur", "Conj", "Div", "Ench", "Evoc", "Illus", "Necro", "Trans"


        tmp_t shortened_names = { std::make_pair("conjuration", "Conj"),
                                  std::make_pair("illusion", "Illus"),
                                  std::make_pair("evocation", "Evoc"),
                                  std::make_pair("transmutation", "Trans"),
                                  std::make_pair("necromancy", "Necro"),
                                  std::make_pair("enchantment", "Ench"),
                                  std::make_pair("abjuration", "Abjur"),
                                  std::make_pair("divination", "Div") };
        std::vector<std::string> valid_names = { "range", "duration", "description","components" };

        std::ofstream myfile ("tmp.js");

        myfile << "var iFileName = \"Lost spells-Starter-Spells.js\";\nRequiredSheetVersion(12.999);\nSourceList[\"TBoLS\"] = {\n          name : \"The Book of Lost Spells\",\n          abbreviation : \"TBoLS\",\n          group : \"The Book of Lost Spells\",\n};\n";

        for ( spell_t::iterator s_itr = spells.begin(); s_itr != spells.end(); s_itr++ ) {
                myfile << "SpellsList[\"" << lower(s_itr->first) << "\"] = {" << '\n';
                myfile << "          name : \"" << s_itr->first << "\",\n";
                myfile << "          source : [\"TBoLS\",0],\n";
                for ( tmp_t::iterator t_itr = s_itr->second.begin(); t_itr != s_itr->second.end(); t_itr++ ) {
                        if ( t_itr->first == "classes") {
                                myfile << "          classes : [\"";
                                std::string tmp = t_itr->second;
                                tmp = ReplaceAll(lower(tmp), ", ", "\",\"");
                                myfile << tmp << "\"],\n";
                        }
                        else if ( t_itr->first == "level" )
                                myfile << "          " << lower(t_itr->first) << " : " << t_itr->second << ",\n";
                        else if ( t_itr->first == "ritual" )
                                myfile << "          " << lower(t_itr->first) << " : " << t_itr->second << ",\n";
                        else if ( lower(t_itr->first) == "components" )
                                myfile << "          " << lower(t_itr->first) << " : \"" << ReplaceAll(t_itr->second, " ", "") << "\",\n";
                        else if ( t_itr->first == "school" )
                                myfile << "          " << lower(t_itr->first) << " : \"" << shortened_names[lower(t_itr->second)] << "\"\n";
                        else if ( lower(t_itr->first) == "area of effect" )
                                myfile << "          " << "range : \"" << t_itr->second << "\",\n";
                        else if ( lower(t_itr->first) == "saving throw" )
                                myfile << "          " << "save : \"" << t_itr->second << "\",\n";
                        else if ( lower(t_itr->first) == "casting time" )
                                myfile << "          " << "time : \"" << t_itr->second << "\",\n";
                        else if ( lower(t_itr->first) == "compmaterial" )
                                myfile << "          " << "compMaterial : \"" << t_itr->second << "\",\n";
                        else if ( std::find(valid_names.begin(), valid_names.end(), lower(t_itr->first)) != valid_names.end() )
                                myfile << "          " << lower(t_itr->first) << " : \"" << t_itr->second << "\",\n";
                        else
                                std::cout << t_itr->first << "\n";
                }
                myfile << "};" << '\n';
        }
        myfile.close();
        return true;
}

int main(int argc, char const *argv[]) {


        std::set<std::string> spell_names;
        std::string line, spell_name = "", description;
        spell_t spells;
        tmp_t tmp_map;

        std::ifstream spell_names_file("spell_names.txt", std::ios::in);
        if (spell_names_file.is_open()) {
                while (std::getline(spell_names_file, line))
                { spell_names.insert(line); }

                spell_names_file.close();
        }

        //for( std::set<std::string>::iterator itr = spell_names.begin(); itr != spell_names.end(); itr++ ) std::cout << *itr << '\n';

        std::ifstream spells_file("raw_spells.txt", std::ios::in);
        if (spells_file.is_open()) {
                while (std::getline(spells_file, line)) {
                        if( spell_names.find( line.substr(0, line.size())) != spell_names.end() ) {
                                if (spell_name != "") {
                                        tmp_map["description"] = description;
                                        spells.insert( std::pair<std::string,tmp_t>(spell_name, tmp_map));
                                        tmp_map.clear();
                                        description = "";
                                }
                                spell_name = line;
                                std::getline(spells_file, line);

                                if ( line.find("cantrip") != std::string::npos ) {
                                        tmp_map["school"] = line.substr(0, line.find(" "));
                                        tmp_map["level"] = "0";
                                } else {
                                        tmp_map["level"] = line[0];
                                        if ( line.find("ritual") != std::string::npos ) {
                                                tmp_map["ritual"] = "true";
                                                tmp_map["school"] = line.substr(line.find(" ") + 1, line.find("(") - line.find(" ")  - 2);
                                        } else
                                                tmp_map["school"] = line.substr(line.find(" ") + 1, 999999 );
                                }

                                std::getline(spells_file, line);
                                tmp_map["classes"] = line;
                        } else if ( line.find(":") != std::string::npos ) {

                                if ( line.substr( 0, line.find(":") ) == "Components" &&  line.find("(") != std::string::npos ) {
                                        tmp_map["compMaterial"] = line.substr( line.find("(") + 1, line.find(")") - line.find("(") - 1 );
                                        tmp_map["components"] = line.substr( line.find(":") + 2,line.find("(") - line.find(":") - 3);
                                } else
                                        tmp_map[line.substr(0, line.find(":"))] = line.substr( line.find(":") + 2, 999999 );

                        } else {
                                if( description == "" )
                                        description = line;
                                else
                                        description = description + " " + line;
                        }
                }
                spells_file.close();
        }
        //std::cout << spells.size() << '\n';

        json_print(spells);





        return 0;
}
