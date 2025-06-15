CLIPS Expert System module for Godot
====================================

A C++ module for rule based game development in Godot Engine.

### Is part of GodotRules project

To make game development in Godot even easier we have packaged well-known
[CLIPS](http://www.clipsrules.net/) (expert system shell and rule-engine
developed in NASA) as a Godot module.

It unlocks the enormous potential of rapid gamed design and development for all
platforms.

How To Install
--------------

It is distributed as a
[GDExtension](https://docs.godotengine.org/en/stable/tutorials/scripting/gdextension/what_is_gdextension.html)
library for Godot 4.5+. Build it using the `gdextension_build` folder and
include the generated binary in your project to use the extension.

Example build command:

```
scons --directory=modules/gd_rules/gdextension_build
```

Usage
-----

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
# Creating new CLIPS environment
var Environment := CLIPS_ENV.new()
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

###  
