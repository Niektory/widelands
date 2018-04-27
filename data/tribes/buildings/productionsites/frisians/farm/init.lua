dirname = path.dirname (__file__)

tribes:new_productionsite_type {
   msgctxt = "frisians_building",
   name = "frisians_farm",
   -- TRANSLATORS: This is a building name used in lists of buildings
   descname = pgettext ("frisians_building", "Farm"),
   helptext_script = dirname .. "helptexts.lua",
   icon = dirname .. "menu.png",
   size = "big",

   buildcost = {
      brick = 3,
      granite = 1,
      log = 2,
      thatch_reed = 3
   },
   return_on_dismantle = {
      brick = 2,
      granite = 1,
      log = 1,
      thatch_reed = 1
   },

   animations = {
      idle = {
         pictures = path.list_files (dirname .. "idle_??.png"),
         hotspot = {105, 132},
         fps = 10,
      },
      working = {
         pictures = path.list_files (dirname .. "working_???.png"),
         hotspot = {105, 132},
         fps = 15,
      },
      unoccupied = {
         pictures = path.list_files (dirname .. "unoccupied_?.png"),
         hotspot = {105, 108},
      },
   },

   aihints = {
      space_consumer = true,
      prohibited_till = 220,
      supports_production_of = { "honey" }
   },

   working_positions = {
      frisians_farmer = 1
   },

   outputs = {
      "barley"
   },

   programs = {
      work = {
         -- TRANSLATORS: Completed/Skipped/Did not start working because ...
         descname = _"working",
         actions = {
            "call=plant_barley",
            "call=harvest_barley",
            "return=skipped"
         }
      },
      plant_barley = {
         -- TRANSLATORS: Completed/Skipped/Did not start planting barley because ...
         descname = _"planting barley",
         actions = {
            "sleep=18000",
            "worker=plant"
         }
      },
      harvest_barley = {
         -- TRANSLATORS: Completed/Skipped/Did not start harvesting barley because ...
         descname = _"harvesting barley",
         actions = {
            "sleep=8000",
            "worker=harvest",
            "animate=working 40000",
            "produce=barley" --produces 2 barley per field
         }
      },
   },
   out_of_resource_notification = {
      -- Translators: Short for "Out of ..." for a resource
      title = _"No Fields",
      heading = _"Out of Fields",
      message = pgettext ("frisians_building", "The farmer working at this farm has no cleared soil to plant his seeds."),
      productivity_threshold = 30
   },
}
