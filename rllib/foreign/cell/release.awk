BEGIN {FS = "."}
{a = $3}
{print $1"."$2"."++a}
