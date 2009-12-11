$ ! build pvserver components for OpenVMS
$ set noon
$
$ set def [.rllib]
$ create/dir [.obj]
$ set def [.lib]
$ @vms_build_libs.com
$ set def [-.-]
$ set def [.rllib.rlsvg]
$ @vms_build.com
$ set def [-.-]
$ create/dir [.obj]
$ set def [.pvserver]
$ @vms_build_libs.com
$ @vms_build_demo.com
$ set def [-]
