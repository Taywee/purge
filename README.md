# retain
Small console program that specifically sorts and selects timestamped
file/directory sets based on a retention scheme

This assumes that daily backups (or some similar backup scheme) are already
taking place.  You feed this program the list of file or directory names (on
standard input, newline-separated), a time formatter, and a list of retention
rules, and it will output which ones should be deleted.

Timestamps may be stored in any format that strptime will understand.

Inputs that do not match the timestamp format at the beginning of their strings
will not be considered, and will not be output for either a keep or a delete
output.

If filenames do not have timestamps, you may extract the timestamp from the
mtime (or anywhere else you may have it), and emplace it at the front of the
string, separated with a slash from the filename, specifying this behavior with
-s.

Timestamp format is usually `[count]{:[specifier]}{/every}`, where `every` is 1
by default.
For example, if you want weekly backups on Sundays, skipping 2 (So you'd have
Sunday, skip 2 Sundays, etc), retaining 7 copies, you'd just specify the -w
flag as `-w '7:0/3'` or even just `-w 7:3`.
The specifier matches its cron counterpart.


In order to remain consistent, the `every` is judged as an extension of the
Unix epoch, and times are judged based on the latest timestamp in the
directory, not present time.  Gaps are accounted for, so if you have 7 weekly
backups, but you have a 3-month gap between 3 and the other 4 of them, all 7
will be kept. Similarly, if there aren't enough backups, the rest will "bunch
up" at the end, so if you have a set of dailies going back 3 weeks, but specify
retention of 7 weeklies, you'll get 3 weeklies and the other 4 oldest backups
in the set.  This is primarily done to ease filesystem use prediction and to
keep backups near the beginning of an active scheme from being so thin (and
also for things like yearlies, if you wanted to keep a January, but had set it
up on February, it will keep a February until the next January backup comes
around).

Remember that the count is a literal count, not a period.  If you specify 1
yearly backup for January, it means to keep 1 yearly January backup (the most
recent January), not to retain at least 1 backup that is at least a year old.
Come January, you will only have 1 yearly backup from your present month.

The timestamps are strict, even when they are not supplied enough information.
This means that if you have a pattern of just year and month, the day of month
will be assumed to be 1, and hour, minute, and second will all be 0.  If you
specify a monthday match of the 2nd, it will not match any such timestamps.

Each retention specifier is handled individually, and may be used more than
once.  This means that multiple specifiers may (intentionally) have overlap.

-r and -k are flags for "remove" and "keep".  Remove is the default
functionality, outputting files that are to be removed.  Specifying both simply
sets the behavior to the last-specified flag.

The retention specifiers are as follows:

daily:      -d  specifier is not used
weekday:    -w  specifier is day of week, as in crontab, 0=Sunday. Default = 0
monthday:   -m  specifier is day of month. Default = 1.
yearmonth:  -y  specifier is month of year, starting with 1. default = 1.

examples:

### A week of dailies
-d 7

### Every other day for a month
-d 15/2

### Every Sunday for a month
-w 4:0

### Every other Saturday for two months
-w 4:6/2

### Two times a month, at the beginning and middle, for a year
-m 24:1,15

### A yearly backup on January that lasts 10 years
-y 10:1
