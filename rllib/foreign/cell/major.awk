BEGIN {FS = "."}
{a = $2}
{b = 0}
{print $1 "." ++a "." b}
