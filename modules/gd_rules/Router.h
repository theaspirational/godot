#ifndef CLIPS_ROUTER_H
#define CLIPS_ROUTER_H

#include <string>
#include "core/print_string.h"
#include "core/ustring.h"

// ROUTER class serves for printing clips messages to external shells (e.g Godot debug)
class ROUTER {

public:
  static String message;
  void *env;
  const char *name = "godot_console";
  int  priority = 20;

  static bool hasEnding (std::string const &fullString, std::string const &ending);

  int add_to_environment();
  static int query_function(void *, const char *);
  static int print_function(void *, const char *, const char *);

};

#endif // CLIPS_ROUTER_H
