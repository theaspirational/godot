/**************************************************************************/
/*  clips_es.h                                                            */
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

#pragma once

#include "Router.h"
#include "clips_core/clips.h"
#ifdef STOP
#undef STOP
#endif
#include "core/object/ref_counted.h"
#include "core/string/print_string.h"
#include "scene/main/node.h"

extern void godot_step_function(void *, DATA_OBJECT_PTR);
extern void godot_steps_function(void *, DATA_OBJECT_PTR);
extern void godot_dofast_function(void *, DATA_OBJECT_PTR);
extern void godot_do_function(void *, DATA_OBJECT_PTR);
extern void godot_dopipe_function(void *, DATA_OBJECT_PTR);

class CLIPS_ENV : public RefCounted {
	GDCLASS(CLIPS_ENV, RefCounted);

protected:
	static void _bind_methods();

private:
	void *env;
	ROUTER r;

public:
	CLIPS_ENV();
	//~CLIPS_ENV();

	Variant get_class_slots(String, int);
	Variant get_defclasses();
	void set_ingame_node(Variant);
	void make_instance(String);
	int unmake_instance(String);
	int delete_instance(String);
	int set_slot(String, String, Variant, bool);
	void assert_string(String);
	int build(String);
	int load(String);
	int run(int);
	void reset();
	void clear();
	Variant eval(String);
};
