#include "CommandOptions.h"
#include <iostream>
#include "assert.h"

CommandOptions::CommandOptions() {
  // default options...
  needBoolOption("v", "verbose", false);
  needBoolOption("h", "help", false);
}

CommandOptions::~CommandOptions() {
}

template <typename T>
bool addOption(map<string, T>& map, 
               const string& name, const T& value) {
  // cerr << "Adding option for: " << name << ", value: " << value << endl;
  if(map.find(name)==map.end()) {
    map.insert(make_pair<string, T>(name, value));
  } else {
    map[name] = value; 
  }
  return true;
}

template <typename T>
bool addOptionAndDesc(map<string, T>& map, map<string, string>& descs, 
               const string& name, const string& desc, const T& value) {
  bool ret_val = addOption(map, name, value);
  descs.insert(make_pair<string, string>(name, desc));
  return ret_val;
}

template <typename T>
T getOption(const map<string, T>& vals, const string& name) {
  typename map<string, T>::const_iterator it;
  it = vals.find(name);
  if(it==vals.end()) {
    assert(0);
  }
  return it->second;;
}

bool CommandOptions::parseOptions(int argc, char* argv[]) {
  // check arguments
  char** args = argv;
  m_tag = *argv; // process executable name as well
  args = argv + 1;
  while (argc-- > 2) { 
    bool cont_flag = false;
    for(map<string, bool>::const_iterator bit=m_bool_options.begin(); 
        bit!=m_bool_options.end(); ++bit) {
      string tag = string("-")+(bit->first);
      if(!strcmp(*args,tag.c_str())) {
        // cerr << "Identified: bool " << tag << endl;
        addOption(m_bool_options, bit->first, !getBoolOption(bit->first));
        // toggle the default
        cont_flag = true;
        break;
      }
    }
    if(cont_flag) {
      args++;
      continue;
    }
    for(map<string, int>::const_iterator iit=m_int_options.begin(); 
        iit!=m_int_options.end(); ++iit) {
      string tag = string("-")+(iit->first);
      if(!strcmp(*args,tag.c_str())) {
        // cerr << "Identified: int option " << tag << endl;
        argc--; args++;
        addOption(m_int_options, iit->first, atoi(*args));
        cont_flag = true;
        break;
      }
    }
    if(cont_flag) {
      args++;
      continue;
    }
    for(map<string, size_t>::const_iterator uit=m_uint_options.begin(); 
        uit!=m_uint_options.end(); ++uit) {
      string tag = string("-")+(uit->first);
      if(!strcmp(*args,tag.c_str())) {
        // cerr << "Identified: uint option " << tag << endl;
        argc--; args++;
        addOption(m_uint_options, uit->first, (size_t)(atoi(*args)));
        cont_flag = true;
        break;
      }
    }
    if(cont_flag) {
      args++;
      continue;
    }
    for(map<string, string>::const_iterator sit=m_string_options.begin(); 
        sit!=m_string_options.end(); ++sit) {
      string tag = string("-")+(sit->first);
      if(!strcmp(*args,tag.c_str())) {
        // cerr << "Identified: string option " << tag << endl;
        argc--; args++;
        addOption(m_string_options, sit->first, string(*args));
        cont_flag = true;
        break;
      }
    }
    if(cont_flag) {
      args++;
      continue;
    }
    // cerr << "ERROR: unrecognized option " << *args << endl;
    // return false;
  }
  // if (argc != 0) {
  //  usage();
  //  exit(-1);
  //}
  // cerr << "Done parsing options" << endl;
  if(getBoolOption("h")) {
    usage(); 
    exit(-1);
  }
  return true;
}

bool CommandOptions::getVerboseFlag() const {
  return getBoolOption("v");
}

template<typename T>
void assertNewOption(map<string, T>& map, 
                     map<string, string>& descs, 
                     const string& name) {
  if(descs.find(name)!=descs.end() ||
     (map.find(name)!=map.end())) {
     assert(!"option already taken");
  }
}

bool CommandOptions::needBoolOption(const string& name, const string& desc, bool default_value) {
  assertNewOption(m_bool_options, m_descs, name);
  return addOptionAndDesc(m_bool_options, m_descs, name, desc, default_value);
}

bool CommandOptions::needStringOption(const string& name, const string& desc, const string& default_value) {
  assertNewOption(m_string_options, m_descs, name);
  return addOptionAndDesc(m_string_options, m_descs, name, desc, default_value);
}

bool CommandOptions::needUintOption(const string& name, const string& desc, size_t default_value) {
  assertNewOption(m_uint_options, m_descs, name);
  return addOptionAndDesc(m_uint_options, m_descs, name, desc, default_value);
}

bool CommandOptions::needIntOption(const string& name, const string& desc, int default_value) {
  assertNewOption(m_int_options, m_descs, name);
  return addOptionAndDesc(m_int_options, m_descs, name, desc, default_value);
}

bool CommandOptions::getBoolOption(const string& name) const {
  return getOption(m_bool_options,name);
}

int CommandOptions::getIntOption(const string& name) const {
  return getOption(m_int_options,name);
}

size_t CommandOptions::getUintOption(const string& name) const {
  return getOption(m_uint_options,name);
}

string CommandOptions::getStringOption(const string& name) const {
  return getOption(m_string_options,name);
}

void CommandOptions::usage() const {
  cerr << m_tag << " [<options>] <director>" << endl << endl;
  cerr << "OPTIONS:\n";
  map<string, string>::const_iterator it;
  for(it=m_descs.begin(); it!=m_descs.end(); ++it) {
    cerr << "    -" << it->first << "       ::" << it->second << endl;
  }
}
