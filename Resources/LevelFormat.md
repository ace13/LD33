Level
=====

Definitions
-----------

`Vec2<uint>` Size
`Vec2<uint>` Start
`Vec2<uint>` Goal

Level data
----------

Size.y lines of Size.x `char` with:
`w` - Water
`m` - Mountain
`g` - Grass
`f` - Forest

Wave definitions
----------------

`uint` Count

Count lines with:
`uint` Number of enemies
`float` Strength
`float` Spawn rate (seconds per enemy)
`string` Name