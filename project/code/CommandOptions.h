#pragma once

#include <string>
#include <map>
using namespace std;

/* Class: CommandOptions
 *   Single class that various apps (tune/test/train) can use to get command line
 *   options. Works with existing command line parsing options code.
 */
class CommandOptions {
public:
  CommandOptions();
  ~CommandOptions();
  bool parseOptions(int argc, char* argv[]); 
  void addBoolOption(const string& name, bool value);
  void addUintOption(const string& name, size_t value);
  void addIntOption(const string& name, int value);
  void addStringOption(const string& name, const string& value);
  void addDoubleOption(const string& name, double value);

  bool needBoolOption(const string& name, const string& desc, bool default_value);
  bool needUintOption(const string& name, const string& desc, size_t default_value);
  bool needIntOption(const string& name, const string& desc, int default_value);
  bool needStringOption(const string& name, const string& desc, const string& default_value);
  bool needDoubleOption(const string& name, const string& desc, double default_value);

  bool getBoolOption(const string& name) const;
  int getIntOption(const string& name) const;
  size_t getUintOption(const string& name) const;
  double getDoubleOption(const string& name) const;
  string getStringOption(const string& name) const;
  
  void usage() const;
  bool getVerboseFlag() const;
private:
  string m_tag;
  map<string, bool> m_bool_options;
  map<string, size_t> m_uint_options;
  map<string, int> m_int_options;
  map<string, string> m_string_options;
  map<string, double> m_double_options;
  map<string, string> m_descs;
};
 
