#pragma once
#include <ctime>
#include <iostream>
#include <string>

using namespace std;

/* Class: Timer
 *     Simple utility for printing timing messages to profile our code.
 *     Use by instantiating on the stack within the scope of the function
 *     trying to profile.
 */
class Timer {
public:
  Timer(string tag=string(""), bool print=true):m_tag(tag), m_print(print) { 
    m_start = clock(); 
  } 
  ~Timer() { 
     if(m_print) { 
       cerr << "Time taken in " << m_tag << ": " 
            << get_time_in_sec()<< " seconds\n";
     }
  }
  double get_time_in_sec() {  
    return (clock()-m_start)/double(CLOCKS_PER_SEC); 
  }   
private:
  string m_tag;
  clock_t m_start;
  bool m_print;
};
