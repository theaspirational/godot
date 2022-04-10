
#include "Router.h"
#include "clips_core/clips.h"

#include "core/print_string.h"
#include "core/ustring.h"


String ROUTER::message  = "";


bool ROUTER::hasEnding (std::string const &fullString, std::string const &ending) {
    if (fullString.length() >= ending.length()) {
        return (0 == fullString.compare (fullString.length() - ending.length(), ending.length(), ending));
    } else {
        return false;
    }
}


int ROUTER::add_to_environment(){
  return EnvAddRouter(env, name, priority,
     query_function,
     print_function,
     NULL,
     NULL,
     NULL);}


int ROUTER::query_function(void * env, const char * name){
  return TRUE;}


int ROUTER::print_function(void * env,  const char *name, const char *m){
  if (strcmp(m, "\n") == 0)
      {
      print_line(message);
      message = "";}
  else
  { message += m;
    if (hasEnding(m, "\n"))
        {print_line(message);} };
  return 0;}

