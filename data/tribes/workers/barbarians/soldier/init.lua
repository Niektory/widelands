dirname = path.dirname(__file__)

animations = {
   idle = {
      pictures = path.list_files(dirname .. "idle_??.png"),
      hotspot = { 16, 31 },
      fps = 5
   },
   atk_ok_e = {
      pictures = path.list_files(dirname .. "atk_ok_e_??.png"),
      hotspot = { 30, 36 },
      fps = 10
   },
   atk_fail_e = {
      pictures = path.list_files(dirname .. "atk_fail_e_??.png"),
      hotspot = { 30, 36 },
      fps = 10
   },
   eva_ok_e = {
      pictures = path.list_files(dirname .. "eva_ok_e_??.png"),
      hotspot = { 18, 34 },
      fps = 20
   },
   eva_fail_e = {
      pictures = path.list_files(dirname .. "eva_fail_e_??.png"),
      hotspot = { 30, 36 },
      fps = 10
   },
   atk_ok_w = {
      pictures = path.list_files(dirname .. "atk_ok_w_??.png"),
      hotspot = { 30, 36 },
      fps = 10
   },
   atk_fail_w = {
      pictures = path.list_files(dirname .. "atk_fail_w_??.png"),
      hotspot = { 30, 36 },
      fps = 10
   },
   eva_ok_w = {
      pictures = path.list_files(dirname .. "eva_ok_w_??.png"),
      hotspot = { 18, 34 },
      fps = 20
   },
   eva_fail_w = {
      pictures = path.list_files(dirname .. "eva_fail_w_??.png"),
      hotspot = { 30, 36 },
      fps = 10
   },
   die_w = {
      pictures = path.list_files(dirname .. "die_??.png"),
      hotspot = { 16, 31 },
      fps = 20
   },
   die_e = {
      pictures = path.list_files(dirname .. "die_??.png"),
      hotspot = { 16, 31 },
      fps = 20
   },
}
add_walking_animations(animations, "walk", dirname, "walk", {16, 31}, 10)


tribes:new_soldier_type {
   msgctxt = "barbarians_worker",
   name = "barbarians_soldier",
   -- TRANSLATORS: This is a worker name used in lists of workers
   descname = pgettext("barbarians_worker", "Soldier"),
   helptext_script = dirname .. "helptexts.lua",
   icon = dirname .. "menu.png",
   vision_range = 2,

   animations = animations,

   default_target_quantity = 10,

   -- Battle attributes - initial values and per level increase
   health = {
      max_level = 3,
      base = 13000,
      increase_per_level = 3000,
      pictures = path.list_files(dirname .. "health_level?.png"),
   },
   attack = {
      max_level = 5,
      base = 1200,
      maximum = 1600,
      increase_per_level = 850,
      pictures = path.list_files(dirname .. "attack_level?.png"),
   },
   defense = {
      max_level = 0,
      base = 3,
      increase_per_level = 4,
      pictures = path.list_files(dirname .. "defense_level?.png"),
   },
   evade = {
      max_level = 2,
      base = 25,
      increase_per_level = 15,
      pictures = path.list_files(dirname .. "evade_level?.png"),
   },

   -- Random animations for battle
   attack_success_w = {
      atk_ok_w = {
         min_health = 0,
         min_attack = 0,
         min_defense = 0,
         min_evade = 0,
         max_health = 3,
         max_attack = 5,
         max_defense = 0,
         max_evade = 2,
      },
   },
   attack_success_e = {
      atk_ok_e = {
         min_health = 0,
         min_attack = 0,
         min_defense = 0,
         min_evade = 0,
         max_health = 3,
         max_attack = 5,
         max_defense = 0,
         max_evade = 2,
      },
   },
   attack_failure_w = {
      atk_fail_w = {
         min_health = 0,
         min_attack = 0,
         min_defense = 0,
         min_evade = 0,
         max_health = 3,
         max_attack = 5,
         max_defense = 0,
         max_evade = 2,
      },
   },
   attack_failure_e = {
      atk_fail_e = {
         min_health = 0,
         min_attack = 0,
         min_defense = 0,
         min_evade = 0,
         max_health = 3,
         max_attack = 5,
         max_defense = 0,
         max_evade = 2,
      },
   },
   evade_success_w = {
      eva_ok_w = {
         min_health = 0,
         min_attack = 0,
         min_defense = 0,
         min_evade = 0,
         max_health = 3,
         max_attack = 5,
         max_defense = 0,
         max_evade = 2,
      },
   },
   evade_success_e = {
      eva_ok_e = {
         min_health = 0,
         min_attack = 0,
         min_defense = 0,
         min_evade = 0,
         max_health = 3,
         max_attack = 5,
         max_defense = 0,
         max_evade = 2,
      },
   },
   evade_failure_w = {
      eva_fail_w = {
         min_health = 0,
         min_attack = 0,
         min_defense = 0,
         min_evade = 0,
         max_health = 3,
         max_attack = 5,
         max_defense = 0,
         max_evade = 2,
      },
   },
   evade_failure_e = {
      eva_fail_e = {
         min_health = 0,
         min_attack = 0,
         min_defense = 0,
         min_evade = 0,
         max_health = 3,
         max_attack = 5,
         max_defense = 0,
         max_evade = 2,
      },
   },
   die_w = {
      die_w = {
         min_health = 0,
         min_attack = 0,
         min_defense = 0,
         min_evade = 0,
         max_health = 3,
         max_attack = 5,
         max_defense = 0,
         max_evade = 2,
      },
   },
   die_e = {
      die_e = {
         min_health = 0,
         min_attack = 0,
         min_defense = 0,
         min_evade = 0,
         max_health = 3,
         max_attack = 5,
         max_defense = 0,
         max_evade = 2,
      },
   },
   idle = {
      idle = {
         min_health = 0,
         min_attack = 0,
         min_defense = 0,
         min_evade = 0,
         max_health = 3,
         max_attack = 5,
         max_defense = 0,
         max_evade = 2,
      },
   },
   walk_se = {
      walk_se = {
         min_health = 0,
         min_attack = 0,
         min_defense = 0,
         min_evade = 0,
         max_health = 3,
         max_attack = 5,
         max_defense = 0,
         max_evade = 2,
      },
   },
   walk_ne = {
      walk_ne = {
         min_health = 0,
         min_attack = 0,
         min_defense = 0,
         min_evade = 0,
         max_health = 3,
         max_attack = 5,
         max_defense = 0,
         max_evade = 2,
      },
   },
   walk_e = {
      walk_e = {
         min_health = 0,
         min_attack = 0,
         min_defense = 0,
         min_evade = 0,
         max_health = 3,
         max_attack = 5,
         max_defense = 0,
         max_evade = 2,
      },
   },
   walk_sw = {
      walk_sw = {
         min_health = 0,
         min_attack = 0,
         min_defense = 0,
         min_evade = 0,
         max_health = 3,
         max_attack = 5,
         max_defense = 0,
         max_evade = 2,
      },
   },
   walk_nw = {
      walk_nw = {
         min_health = 0,
         min_attack = 0,
         min_defense = 0,
         min_evade = 0,
         max_health = 3,
         max_attack = 5,
         max_defense = 0,
         max_evade = 2,
      },
   },
   walk_w = {
      walk_w = {
         min_health = 0,
         min_attack = 0,
         min_defense = 0,
         min_evade = 0,
         max_health = 3,
         max_attack = 5,
         max_defense = 0,
         max_evade = 2,
      },
   },
}
