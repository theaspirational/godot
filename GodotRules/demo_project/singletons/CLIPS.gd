extends Node
"""
GodotRules 
Singletone script
for more information read USAGE.md file
"""

# Create CLIPS Environment (in C++ module)
# to execute any CLIPS shell command call:
# env.eval("(command_name arg1 arg2)") 
var env := CLIPS_ENV.new()


# Streaming protocol:
# once some rule fires, 
# tasks from the RHS of the rule are added to the flow array,
# then they are executed one after another in _process() func.
var flow : Array = [] 
var flow_state: GDScriptFunctionState # state of the latest executed task


func _init() -> void:
	process_priority = -100
	env.set_ingame_node(self)
# CLIPS provides several commands to help you debug programs. 
# There are a number of things you can watch. 
# These include the following,
# which are described in more detail in the CLIPS Reference Manual. 
#	env.eval("(watch all)")
#	env.eval("(watch compilations)")
#	env.eval("(watch messages)")
#	env.eval("(watch message-handlers)")
#	env.eval("(watch methods)")
#	env.eval("(watch facts)")
	env.eval("(watch instances)")
#	env.eval("(watch slots)")
	env.eval("(watch rules)")
#	env.eval("(watch activations)")
#	env.eval("(watch statistics)")
	print("==> CLIPS is ready")


func _process(_delta: float) -> void:
	if flow.empty(): return
	if flow_state && flow_state.is_valid(): return
	flow_state = next_step()


func next_step():
	var step = flow[0]
	flow.remove(0)
	return _match_task(step)


func _match_task(task:Array):
	var temp
	prints("Godot start doing task:", task)
	match task:
		["pass"], [1]:
			pass
		['flow', 'clear']:
			flow.clear()
		['print', ..]:
			print_from_clips(task.slice(1,task.size()))
		['sleep', var time]:
			yield(get_tree().create_timer(float(time)), "timeout")
		['wait', var inst, var signl]:
			yield(inst,signl)
		['set', var inst, var slot, var val]:
			inst.set(slot, val)
		[var inst, var func_name]:
			temp = inst.call(func_name)
		[var inst, var func_name, ..]:
			temp = inst.callv(func_name, task.slice(2,task.size()))
		_:
			printerr("WARNING: Task doesn't match:")
	if temp is GDScriptFunctionState:
		yield(temp,"completed")


#****************************************
# Exported functions (are called from Environment created in C++ module)
# Do not change the names of funcs below
#****************************************
# (. <task-pattern> <argumets>) adds task to the flow
func add_step(step: Array):
	flow.push_back(step)
	return "pass"


# (.. -> <task-pattern1> <argumets>
#	  -> <task-pattern2> <argumets> 
#	  -> (.. -> <task-pattern3> <argumets>)
#	  -> <task-patternN> <argumets>) recursively adds all tasks to the flow
func add_steps(_steps: Array):
	return 1


# (.now <task-pattern> <argumets>...) instant pipeline function 
func do(task: Array):
	return _match_task(task)


# (..now -> <task-pattern> <argumets>...) instant pipeline function 
func dopipe(todo: Array):
	var temp
	for task in todo:
		temp = _match_task(task)
	return temp
#****************************************


#*************************
#* DEFCLASSES *
#*************************
func build_class(obj:Object) -> void:
	var cls_name = obj.get_class()
	var parent_cls = "USER"
	var s: String = "(defclass %s (is-a %s)" % [cls_name, parent_cls]
	s += "(slot inst_id (override-message inst_id-put))"
	
	for property in get_export_properties(obj):
		if property.type in [TYPE_NIL, TYPE_BOOL, TYPE_INT, TYPE_REAL, TYPE_STRING, TYPE_NODE_PATH, TYPE_OBJECT]:
			s += " (slot {slot_name} (override-message {slot_name}-put))".format({"slot_name":property.name})
		elif property.type in [TYPE_ARRAY, TYPE_VECTOR2]:
			s += " (multislot {slot_name} (override-message {slot_name}-put))".format({"slot_name":property.name})
	s += ")"
# warning-ignore:return_value_discarded
	env.build(s)
	
# warning-ignore:return_value_discarded
	env.build("(defmessage-handler {class} inst_id-put (?value) (bind ?self:inst_id (now (instance-name ?self) get_instance_id)))".format({"class":cls_name}))
	for slot in get_class_slots(cls_name):
		if slot != "inst_id":
# warning-ignore:return_value_discarded
			env.build("(defmessage-handler {class} {slot}-put (?value) (bind ?self:{slot} (now (instance-name ?self) get {slot})))".format({"class":cls_name, "slot":slot}))


func build_rules(obj:Object) -> void:
	var cls_name = obj.get_class()
	prints("==>", "Start loading", cls_name, "rules")
	
	if obj.has_method("get_rules"):
		for rule in obj.get_rules():
# warning-ignore:return_value_discarded
			env.build(rule)
	
	if exists_rule_file(cls_name):
# warning-ignore:return_value_discarded
		load_rules(cls_name)


# Alternative to build_rules
# at first copies .clp file to `user://` path
# than loads rules from `res://rules/`
# Returns:
# 0 if the file couldn't be opened, -1 if the file was
# opened but an error occurred while loading constructs,
# and 1 if the file was opened and no errors occured
# while loading.
func load_rules(filename: String) -> int:
	var res_filepath := "res://rules/"+filename+".clp"
	var user_filepath := ProjectSettings.globalize_path("user://"+filename+".clp") 
	var dir = Directory.new()
	var file = File.new()
	if !file.file_exists(user_filepath) or file.get_md5(res_filepath) != file.get_md5(user_filepath):
		dir.copy(res_filepath, user_filepath)
		print("==> New %s rules has been loaded" % filename)
	return env.load(user_filepath)


func get_export_properties(obj:Object) -> Array:
	var ret :=[]
	for property in obj.get_property_list():
		if property.usage == 8199:
			ret.append(property)
	return ret


func exists_rule_file(filename: String) -> bool:
	var res_filepath := "res://rules/"+filename+".clp"
	var file = File.new()
	return file.file_exists(res_filepath)

	
func get_class_slots(cls_name:String, include_inherited=1) -> Array:
	return env.get_class_slots(cls_name, include_inherited)


func make_instance(obj:Object) -> void:
	if not "MAIN::"+obj.get_class() in env.get_defclasses():
		build_class(obj)
		build_rules(obj)

	var s: String = "(%s of %s" % [obj.to_string(), obj.get_class()]
	for slot in get_class_slots(obj.get_class()):
		s += "(%s value)" % slot
	s += ")"
	env.make_instance(s) 


func del_instance(inst) -> void:
	if inst is Object:
		inst = inst.to_string()
# warning-ignore:return_value_discarded
	env.delete_instance(inst)


func set_slot(inst_name: String , slot: String, value, as_symbols:=true) -> void:
	# env.set_slot() RETURNS : 1 if set successful, 0 otherwise
# warning-ignore:return_value_discarded
	env.set_slot(inst_name, slot, value, as_symbols)


func assert_fact(fact:String):
	env.assert_string(fact)


func eval(expr:String):
	env.eval(expr)


# `run_limit` arg is an integer which defines how many rules should fire 
# If `run_limit` is a negative integer, rules will fire until the agenda is empty.
func run(run_limit:int=-1):
	return env.run(run_limit)


func clear():
	print("...clearing CLIPS")
	flow_state = null
	flow.clear()
	env.clear()


func reset():
	print("...reseting CLIPS")
	flow_state = null
	flow.clear()
	env.reset()


#*************************
#* HELPERS *
#*************************
func print_from_clips(strings:Array):
	var complete = ""
	for i in strings:
		complete += str(i)+" "
	print("==> Printing from CLIPS:")
	print(complete)


func print_clp_instance(inst):
	if inst is Object:
		inst = inst.to_string()
	print("==> Printing from CLIPS:")
	env.eval("(send %s print)" % inst)


func arr_to_str(arr:Array) -> String:
	return convert(arr, TYPE_STRING)
