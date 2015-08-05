# retain
Small POSIX program that specifically purges timestamped file/directory sets
based on a retention scheme

This assumes that daily backups (or some similar backup scheme) is already
taking place.  This program will then take those backups and purge them in a
pattern that matches a retention scheme.

Timestamps may be stored in any format that strptime will understand.

A specified timestamp format will match all files in the directory that match
the timestamp, and ignore all others.

Time intervals are taken in a cron-like manner, but options are passed to
flags.  The format is usually `[count]{:[specifier{,specifier...}]}{/every}`, where
`every` is 1 by default.
For example, if you want weekly backups on sundays and wednesdays, skipping 2
(So in order, you'd save sunday, skip wednesday, skip sunday, save
wednesday,etc), retaining 7 copies, you'd just specify the -w flag as
`-w '7:0,3/3'`

In order to remain consintent, the `every` is judged as an extension of the
Unix epoch, and times are judged based on the latest timestamp in the
directory, not prestent time.  Gaps are accounted for, so if you have 7 weekly
backups, but you have a 3-month gap between 3 and the other 4 of them, all 7
will be kept. Similarly, if there aren't enough backups, the rest will "bunch
up" at the end, so if you have a set of dailies going back 3 weeks, but specify
retention of 7 weeklies, you'll get 3 weeklies and the other 4 oldest backups
in the set.  This is primarily done to ease filesystem use prediction and to
keep backups near the beginning of an active scheme from being so thin (and
also for things like yearlies, if you wanted to keep a january, but had set it
up on February, it will keep a February until the next junuary backup comes
around).

Each retention specifier is handled individually, and may be used more than
once.  This means that multiple specifiers may (intentionally) have overlap.

The retention specifiers are as follows:

daily:      -d  specifier is not used
weekday:    -w  specifier is day of week, as in crontab, 0=Sunday. default = 0
monthday:   -m  specifier is day of month. default = 1
yearmonth:  -y  specifier is month of year, starting with 1. default = 1

examples:

### A week of dailies
-d 7

### Every other day for a month
-d 15/2

### Every sunday for a month
-w 4:0

### Every other Saturday for two months
-w 4:6/2

### Two times a month, at the beginning and middle, for a year
-m 24:1,15

### A yearly backup on january that lasts 10 years
-y 10:1
