$ ! build pvserver components for OpenVMS
$
$ @[.rllib.lib]vms_build_libs.com
$ @[.rllib.rlsvg]vms_build.com
$ @[.pvserver]vms_build_libs.com
$ @[.pvserver]vms_build_demo.com
