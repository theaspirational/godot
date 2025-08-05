extends KinematicBody2D
class_name Player
func get_class(): return "Player"
func _to_string()->String: return '[%s:%s]' % [get_class(), get_instance_id()]
"""
Rules for 'Player' declared in res://rules/Player.clp
"""

export(String) var neighbor setget set_neighbor
export(bool) var can_rotate = true setget set_can_rotate


func set_neighbor(new):
	neighbor = new
	CLIPS.set_slot(to_string(),'neighbor', neighbor)
	CLIPS.run(1)


func set_can_rotate(new):
	can_rotate = new
	CLIPS.set_slot(to_string(),'can_rotate', can_rotate)
	CLIPS.run(1)


func _init() -> void:
	CLIPS.make_instance(self)


func _notification(what):
	match what:
		NOTIFICATION_PREDELETE:
			CLIPS.del_instance(self)


# Called when the node enters the scene tree for the first time.
func _ready() -> void:
	position = get_viewport().size/2


# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(_delta: float) -> void:
	position = get_global_mouse_position()

