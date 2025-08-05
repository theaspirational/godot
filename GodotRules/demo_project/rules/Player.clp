
(defrule rotate_player
	
	(object (is-a Enemy)
	(name ?enemy)
	(neighbor ?player))

	(object (is-a Player)
	(name ?player)
	(can_rotate TRUE)
	(neighbor ?enemy))
	
	=> 

	(now ?player set can_rotate FALSE)
	(bind ?scale (/ (random 56 150) 100.0))
	(..
	-> ?player apply_scale (create$ Vector2 ?scale ?scale)
	-> ?player rotate (/ (random 20 60) 100.0)
	))
	

(defrule let_player_grow_to_max_size

	(object (is-a Game)
		(name ?game)
		(enemies $?enems&:(= 1 (length ?enems))))
	
	=> 
	(bind ?player (now ?game find_node Player))
	(now ?player set scale (create$ Vector2 10 10))
	(now ?player set can_rotate TRUE)
	(assert (game over))
	)
