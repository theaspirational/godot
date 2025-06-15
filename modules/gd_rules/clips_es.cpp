/**************************************************************************/
/*  clips_es.cpp                                                          */
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

#include "clips_es.h"
#ifdef STOP
#undef STOP
#endif
#include "core/string/print_string.h"
#include "factory.h"

Node *ingame_singletone;

//*****************************
// external CLIPS functions
// for <task-pattern> - look in the singletone/CLIPS.gd file at _match_task(t) function
//*****************************

//(. <task-pattern> <arguments>) - adds task to godot flow array -
//	- executes on iddle frame after prev. task, has no meaningful return value
void godot_step_function(void *env, DATA_OBJECT_PTR returnValuePtr) {
	Variant value;
	String gd_funcname("add_step");
	Array args_arr;
	int argnum = EnvRtnArgCount(env);

	for (int i = 1; i <= argnum; i++) {
		EnvRtnUnknown(env, i, returnValuePtr);
		args_arr.push_back(data_object_to_variant(env, returnValuePtr));
	};

	value = ingame_singletone->call(gd_funcname, args_arr);
	variant_to_data_object(env, value, returnValuePtr, FALSE);
	return;
}

// (..
//	-> <task-pattern> <arguments>
//	-> <task-pattern> <arguments>
//	-> ..	) - flow pipeline godot function - adds tasks(steps) to godot flow array
// - executes on iddle frame "step by step", returns TRUE
void godot_steps_function(void *env, DATA_OBJECT_PTR returnValuePtr) {
	Variant value;
	String gd_funcname1("add_step");
	String gd_funcname2("add_steps");
	Array final_arr;
	Array temp_arr;
	int argnum = EnvRtnArgCount(env);

	if (!EnvArgTypeCheck(env, "..", 1, SYMBOL, returnValuePtr) && strcmp(DOPToString(returnValuePtr), "->") != 0) {
		print_line("clips_es:godot_steps_function: First argument must be separator '->'");
		SetpType(returnValuePtr, SYMBOL);
		SetpValue(returnValuePtr, EnvFalseSymbol(env));
		return;
	}

	for (int i = 2; i <= argnum; i++) {
		bool is_separator;
		EnvRtnUnknown(env, i, returnValuePtr);
		is_separator = GetpType(returnValuePtr) == SYMBOL && strcmp(DOPToString(returnValuePtr), "->") == 0;

		if (is_separator) {
			final_arr.append(ingame_singletone->call(gd_funcname1, temp_arr.duplicate()));
			temp_arr.clear();
		} else {
			temp_arr.push_back(data_object_to_variant(env, returnValuePtr));
		}
	}

	final_arr.append(ingame_singletone->call(gd_funcname1, temp_arr.duplicate()));
	value = ingame_singletone->call(gd_funcname2, final_arr);

	variant_to_data_object(env, value, returnValuePtr, FALSE);
	return;
}

// (now ?<gd-obj> <gd-obj_func_name> <arg1> <arg2> <argN> ...) - instant godot function -
// - executes and returns value immediately
void godot_dofast_function(void *env, DATA_OBJECT_PTR returnValuePtr) {
	String gd_funcname;
	Array gd_arguments;
	int argnum = EnvRtnArgCount(env);
	Variant value;
	Object *inst;

	EnvRtnUnknown(env, 1, returnValuePtr);

	if (
			GetpType(returnValuePtr) == INSTANCE_NAME ||
			GetpType(returnValuePtr) == INSTANCE_ADDRESS) {
		inst = data_object_to_variant(env, returnValuePtr);
	} else {
		print_line("clips_es:godot_dofast_function: First argument in 'now' func must be of type INSTANCE_NAME");
		SetpType(returnValuePtr, SYMBOL);
		SetpValue(returnValuePtr, EnvFalseSymbol(env));
		return;
	}

	if (!EnvArgTypeCheck(env, "now", 2, SYMBOL, returnValuePtr)) {
		print_line("clips_es:godot_retval_function: Second argument in 'now' func (gd_funcname) must be of type SYMBOL");
		SetpType(returnValuePtr, SYMBOL);
		SetpValue(returnValuePtr, EnvFalseSymbol(env));
		return;
	}

	gd_funcname = DOPToString(returnValuePtr);

	for (int i = 3; i <= argnum; i++) {
		EnvRtnUnknown(env, i, returnValuePtr);
		gd_arguments.push_back(data_object_to_variant(env, returnValuePtr));
	}
	// print_line("==> Doing now from clips:");
	// print_line(inst->to_string()+" "+gd_funcname+" "+ingame_singletone->call("arr_to_str", gd_arguments));
	value = inst->callv(gd_funcname, gd_arguments);

	variant_to_data_object(env, value, returnValuePtr, FALSE);
	return;
}

// (.now <task-pattern> <arguments>) - instant  godot function
// - executes and returns value immediately
void godot_do_function(void *env, DATA_OBJECT_PTR returnValuePtr) {
	Variant value;
	String gd_funcname("do");
	Array args_arr;
	int argnum = EnvRtnArgCount(env);

	for (int i = 1; i <= argnum; i++) {
		EnvRtnUnknown(env, i, returnValuePtr);
		args_arr.push_back(data_object_to_variant(env, returnValuePtr));
	};

	value = ingame_singletone->call(gd_funcname, args_arr);
	variant_to_data_object(env, value, returnValuePtr, FALSE);
	return;
}

//(..now
//	-> <task-pattern> <arguments>
//	-> <task-pattern> <arguments>
//	-> ..	) - instant pipeline godot function - executes and returns !last value immediately
void godot_dopipe_function(void *env, DATA_OBJECT_PTR returnValuePtr) {
	Variant value;
	String gd_funcname("dopipe");
	Array final_arr;
	Array temp_arr;
	int argnum = EnvRtnArgCount(env);

	if (!EnvArgTypeCheck(env, "..now", 1, SYMBOL, returnValuePtr) && strcmp(DOPToString(returnValuePtr), "->") != 0) {
		print_line("clips_es:godot_dopipe_function: First argument in '..now' func must be separator '->'");
		SetpType(returnValuePtr, SYMBOL);
		SetpValue(returnValuePtr, EnvFalseSymbol(env));
		return;
	}

	for (int i = 2; i <= argnum; i++) {
		bool is_separator;
		EnvRtnUnknown(env, i, returnValuePtr);
		is_separator = GetpType(returnValuePtr) == SYMBOL && strcmp(DOPToString(returnValuePtr), "->") == 0;

		if (is_separator) {
			final_arr.append(temp_arr.duplicate());
			temp_arr.clear();
		} else {
			temp_arr.push_back(data_object_to_variant(env, returnValuePtr));
		}
	}

	final_arr.append(temp_arr.duplicate());
	value = ingame_singletone->call(gd_funcname, final_arr);

	variant_to_data_object(env, value, returnValuePtr, FALSE);
	return;
}

//*****************************
// CLIPS_ENV:: functions
//*****************************
CLIPS_ENV::CLIPS_ENV() {
	env = CreateEnvironment();
	r = ROUTER();
	r.env = env;
	r.add_to_environment();
}

Variant CLIPS_ENV::get_class_slots(String defclassName, int inheritFlag) {
	DATA_OBJECT temp;
	EnvClassSlots(env, EnvFindDefclass(env, defclassName.ascii().get_data()), &temp, inheritFlag);
	return data_object_to_variant(env, &temp);
}

Variant CLIPS_ENV::get_defclasses() {
	DATA_OBJECT temp;
	EnvGetDefclassList(env, &temp, NULL);
	return data_object_to_variant(env, &temp);
}

void CLIPS_ENV::set_ingame_node(Variant node) {
	ingame_singletone = node;
	return;
}

void CLIPS_ENV::make_instance(String makeCommand) {
	EnvMakeInstance(env, makeCommand.ascii().get_data());
}

// This function is equivalent to DeleteInstance except
// that it uses message-passing instead of directly deleting the instance(s).
int CLIPS_ENV::unmake_instance(String instanceName) {
	instanceName = instanceName.lstrip("[").rstrip("]");
	void *instancePtr = EnvFindInstance(env, NULL, instanceName.ascii().get_data(), FALSE);
	return EnvUnmakeInstance(env, instancePtr);
}

int CLIPS_ENV::delete_instance(String instanceName) {
	instanceName = instanceName.lstrip("[").rstrip("]");
	void *instancePtr = EnvFindInstance(env, NULL, instanceName.ascii().get_data(), FALSE);
	return EnvDeleteInstance(env, instancePtr);
}

int CLIPS_ENV::set_slot(String instanceName, String slotName, Variant inputValue, bool as_symbols) {
	instanceName = instanceName.lstrip("[").rstrip("]");
	void *instancePtr = EnvFindInstance(env, NULL, instanceName.ascii().get_data(), FALSE);
	DATA_OBJECT *inputDataObject = variant_to_data_object(env, inputValue, NULL, as_symbols);
	return EnvDirectPutSlot(env, instancePtr, slotName.ascii().get_data(), inputDataObject);
}

void CLIPS_ENV::assert_string(String string) {
	EnvAssertString(env, string.ascii().get_data());
}

int CLIPS_ENV::build(String constructString) {
	return EnvBuild(env, constructString.ascii().get_data());
}

int CLIPS_ENV::load(String filename) {
	return EnvLoad(env, filename.ascii().get_data());
}

int CLIPS_ENV::run(int runlimit) {
	return EnvRun(env, runlimit);
}

void CLIPS_ENV::reset() {
	return EnvReset(env);
}

void CLIPS_ENV::clear() {
	return EnvClear(env);
}

Variant CLIPS_ENV::eval(String expressionString) {
	DATA_OBJECT temp;
	EnvEval(env, expressionString.ascii().get_data(), &temp);
	return data_object_to_variant(env, &temp);
}

void CLIPS_ENV::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_ingame_node"),
			&CLIPS_ENV::set_ingame_node);
	ClassDB::bind_method(D_METHOD("build"),
			&CLIPS_ENV::build);
	ClassDB::bind_method(D_METHOD("set_slot"),
			&CLIPS_ENV::set_slot);
	ClassDB::bind_method(D_METHOD("get_class_slots"),
			&CLIPS_ENV::get_class_slots);
	ClassDB::bind_method(D_METHOD("get_defclasses"),
			&CLIPS_ENV::get_defclasses);
	ClassDB::bind_method(D_METHOD("assert_string"),
			&CLIPS_ENV::assert_string);
	ClassDB::bind_method(D_METHOD("make_instance"),
			&CLIPS_ENV::make_instance);
	ClassDB::bind_method(D_METHOD("unmake_instance"),
			&CLIPS_ENV::unmake_instance);
	ClassDB::bind_method(D_METHOD("delete_instance"),
			&CLIPS_ENV::delete_instance);
	ClassDB::bind_method(D_METHOD("eval"),
			&CLIPS_ENV::eval);
	ClassDB::bind_method(D_METHOD("load"),
			&CLIPS_ENV::load);
	ClassDB::bind_method(D_METHOD("clear"),
			&CLIPS_ENV::clear);
	ClassDB::bind_method(D_METHOD("reset"),
			&CLIPS_ENV::reset);
	ClassDB::bind_method(D_METHOD("run"),
			&CLIPS_ENV::run);
}
