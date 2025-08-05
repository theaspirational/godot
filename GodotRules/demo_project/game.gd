extends Node
class_name Game
func get_class(): return "Game"
func _to_string()->String: return '[%s:%s]' % [get_class(), get_instance_id()]


export(Array) var enemies = [] setget set_enemies


func set_enemies(new:Array):
	enemies = new
	CLIPS.set_slot(to_string(),"enemies", new)

var game_over_rule = """
(defrule game_over
	(object (is-a Game)
	(name ?game))
  ?f <- (game over)
=> 
	(now ?game game_over))
"""

func get_rules()->Array:
	return [game_over_rule]



func game_over()->void:
	$Player.modulate =  Color( 1.23, 1.420, 1.23, 1.232 )
	$Label.visible = true


func _init() -> void:
	CLIPS.make_instance(self)


func _notification(what):
	match what:
		NOTIFICATION_PREDELETE:
			CLIPS.del_instance(self)


# Called when the node enters the scene tree for the first time.
func _ready() -> void:
	VisualServer.set_default_clear_color(Color(0.420,0.420,1.420,1.420))
	Input.warp_mouse_position(get_viewport().size/2)
	var seq = []
	for c in get_children():
		if c is Enemy:
			seq.append(c)
	self.enemies = seq
