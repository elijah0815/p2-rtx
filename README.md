# p2-rtx

Commandline args:
`-xo_disable_map_conf` :: disable loading of map specific `.conf` files (remix variables)
`-xo_disable_all_culling` :: disables culling completely (same as enabling `r_novis`)

Console commands:
`xo_debug_toggle_node_vis` :: Toggle debug visualization of bsp leaf using the remix api
`xo_mapsettings_update` :: Reload the map_settings.ini file + map.conf
`xo_vars_parse_options` :: Re-parse the rtx.conf file
`xo_vars_reset_all_options` :: Reset all options to the rtx.conf level
`xo_vars_clear_transitions` :: Clear all ongoing transitions