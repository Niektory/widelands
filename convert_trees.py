#!/usr/bin/env python3

import os, pathlib, subprocess

input_dir = "data/world/immovables/trees"
output_dir = "trees/converted"
# tree_dir = "mushroom_dark"
# tree_name = "mushroom_dark_wasteland_old"

for tree_dir in os.listdir(input_dir):
    if tree_dir.startswith("deadtree") or tree_dir.startswith("fallentree"):
        continue
    print(tree_dir + "/init.lua")
    with open(input_dir + "/" + tree_dir + "/init.lua") as init_lua:
        for line in init_lua:
            if 'name = "' not in line:
                continue
            tree_name = line.split('"')[1]
            print(tree_name)
            tree_maturity = tree_name.split("_")[-1]

            pathlib.Path(output_dir + "/" + tree_dir).mkdir(parents=True, exist_ok=True)
            result = subprocess.run(["./wl_create_spritesheet",
                tree_name, "idle", output_dir + "/" + tree_dir],
                capture_output=True)
            with open(output_dir + "/" + tree_dir + "/" + tree_maturity + "_stdout.log", "wb") \
                      as stdout_log:
                stdout_log.write(result.stdout)
            os.rename(output_dir + "/" + tree_dir + "/idle_1.png",
                      output_dir + "/" + tree_dir + "/" + tree_maturity + "_1.png")

