dirname = path.dirname(__file__)

world:new_immovable_type{
   name = "standing_stone6",
   descname = _ "Standing Stone",
   -- category = "standing_stones",
   size = "none",
   attributes = {},
   programs = {},
   animations = {
      idle = {
         pictures = { dirname .. "idle.png" },
         player_color_masks = {},
         hotspot = { 7, 63 },
      },
   }
}
