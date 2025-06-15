/**************************************************************************/
/*  Router.cpp                                                            */
/**************************************************************************/
/*                         This file is part of:                          */
/*                             GODOT ENGINE                               */
/*                        https://godotengine.org                         */
/**************************************************************************/
/* Copyright (c) 2014-present Godot Engine contributors (see AUTHORS.md). */
/* Copyright (c) 2007-2014 Juan Linietsky, Ariel Manzur.                  */
/*                                                                        */
/* Permission is hereby granted, free of charge, to any person obtaining  */
/* a copy of this software and associated documentation files (the        */
/* "Software"), to deal in the Software without restriction, including    */
/* without limitation the rights to use, copy, modify, merge, publish,    */
/* distribute, sublicense, and/or sell copies of the Software, and to     */
/* permit persons to whom the Software is furnished to do so, subject to  */
/* the following conditions:                                              */
/*                                                                        */
/* The above copyright notice and this permission notice shall be         */
/* included in all copies or substantial portions of the Software.        */
/*                                                                        */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,        */
/* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF     */
/* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. */
/* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY   */
/* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,   */
/* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE      */
/* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                 */
/**************************************************************************/

#include "Router.h"
#include "clips_core/clips.h"
#ifdef STOP
#undef STOP
#endif

#ifdef GDEXTENSION
#include <godot_cpp/core/defs.hpp>
#include <godot_cpp/variant/string.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
using namespace godot;
#else
#include "core/string/print_string.h"
#include "core/string/ustring.h"
#endif

String ROUTER::message = "";

bool ROUTER::hasEnding(std::string const &fullString, std::string const &ending) {
	if (fullString.length() >= ending.length()) {
		return (0 == fullString.compare(fullString.length() - ending.length(), ending.length(), ending));
	} else {
		return false;
	}
}

int ROUTER::add_to_environment() {
	return EnvAddRouter(env, name, priority,
			query_function,
			print_function,
			NULL,
			NULL,
			NULL);
}

int ROUTER::query_function(void *env, const char *name) {
	return TRUE;
}

int ROUTER::print_function(void *env, const char *name, const char *m) {
	if (strcmp(m, "\n") == 0) {
#ifdef GDEXTENSION
		UtilityFunctions::print(message);
#else
		print_line(message);
#endif
		message = "";
	} else {
		message += m;
		if (hasEnding(m, "\n")) {
#ifdef GDEXTENSION
			UtilityFunctions::print(message);
#else
			print_line(message);
#endif
		}
	};
	return 0;
}
