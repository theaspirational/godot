extends KinematicBody2D
class_name Enemy
func get_class(): return "Enemy"
func _to_string()->String: return '[%s:%s]' % [get_class(), get_instance_id()]
"""
Rules for 'Enemy' declared in this script (as an example)
by implementing `get_rules()->Array` method
"""

onready var player = get_parent().get_node("Player")

export(int) var speed = 100 setget set_speed
export(NodePath) var neighbor setget set_neighbor


func set_speed(new):
	speed = new
	CLIPS.set_slot(to_string(),'speed', speed)


func set_neighbor(new):
	#if neighbor is Object and neighbor == new: return
	neighbor = new
	if neighbor == player:
		player.neighbor = self
	CLIPS.set_slot(to_string(),'neighbor', neighbor)
	CLIPS.run(1)


func _init() -> void:
	CLIPS.make_instance(self)


func _notification(what):
	match what:
		NOTIFICATION_PREDELETE:
			CLIPS.del_instance(self)


var grow_rule = """ 
(defrule eat_and_grow

	(object (is-a Enemy)
	(name ?enemy1)
	(speed ?speed)
	(neighbor ?enemy2))

	(object (is-a Enemy)
	(name ?enemy2)
	(neighbor ?enemy1))

	(object (is-a Game)
	(name ?game)
	(enemies $?enemies&:(member$ ?enemy2 ?enemies)))
	
=>

	(now ?game set enemies (delete-member$ ?enemies ?enemy2))
	(now ?game remove_child ?enemy2)
	
	(bind ?scale (/ (random 60 100) 100.0))
	(..
	-> ?enemy1 set speed (+ 200 ?speed)
	-> ?enemy1 apply_scale (create$ Vector2 1.5 1.5)
	)
	(assert (enemy_grows ?enemy1))
)
"""

var rotate_rule = """
(defrule rotate_enemy_after_grow
	?f <- (enemy_grows ?enemy)
	
	(object (is-a Enemy)
	(name ?enemy))
	
	=>
	(retract ?f)
	(bind ?rads (/ (random 60 200) 100.0))
	(. ?enemy rotate ?rads)
)
"""


func get_rules()->Array:
	return [grow_rule, rotate_rule]


# Called every frame. 'delta' is the elapsed time since the previous frame.
func _physics_process(delta):
	var dir = (player.global_position - global_position).normalized()
	var collision = move_and_collide(dir * speed * delta)
	if collision:
		self.neighbor = collision.collider

