/*******************************************************************\

Module:

Author: Muhammad Osama, 2021

\*******************************************************************/

#include <string>
#include <cstring>

void parse_configuration(std::ifstream& configfile);
void read_configuration(const std::string& name);

inline void eatSpaces(std::string &line)
{
  const size_t len = line.size();
  size_t i = 0;
  while(i < len && isspace(line[i]))
    i++;
  if(i)
    line.erase(0, i);
}

inline void eatCharsInclusive(std::string &line, const char &prefix)
{
  const size_t len = line.size();
  size_t i = 0;
  while(i < len && line[i] != prefix)
    i++;
  if(i)
    line.erase(0, i + 1);
}

inline bool strhas(const char *in, const char *ref)
{
  size_t count = 0, realchars = 0;
  const char *tmpin = in;
  while(*tmpin)
  {
    if(isspace(*tmpin++))
      break;
    realchars++;
  }
  const size_t reflen = strlen(ref);
  while(*in)
  {
    if(ref[count] != *in)
      count = 0;
    else
      count++;
    in++;
    if(realchars == reflen && count == reflen)
      return true;
  }
  return false;
}

inline bool isEnabled(const char *val)
{
  return strhas(val, "off") ? false : true;
}