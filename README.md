# retain
Small console program that specifically sorts and selects timestamped
file/directory sets based on a retention scheme

This assumes that daily backups (or some similar backup scheme) are already
taking place.  You feed this program the list of file or directory names (on
standard input, newline-separated), a time formatter, and a list of retention
rules, and it will output which ones should be deleted.

Timestamps may be described in any format that strptime will understand.

Inputs that do not match the timestamp format at the beginning of their strings
will not be considered, and will not be output for either a keep or a delete
output.

If filenames do not have timestamps, you may extract the timestamp from the
mtime (or anywhere else you may have it), and emplace it at the front of the
string, separated with a slash from the filename, specifying this behavior with
-s.

Selector format is usually `[count]{:[specifier]}{/every}`, where `every` is 1
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

The timestamps are loose, but assumptions are made when not supplied enough
information.  This means that if you have a pattern of just year and month, the
day of month will be assumed to be 1, and hour, minute, and second will all be
0.  If you specify a monthday match of the 2nd, it will not match any such
timestamps.  If you specify a monthday of 1 but have hourly backups, it
will only match the first such matching file.

Each retention specifier is handled individually, and may be used more than
once.  This means that multiple specifiers may (intentionally) have overlap.

-r and -k are flags for "remove" and "keep".  Remove is the default
functionality, outputting files that are to be removed.  Specifying both simply
sets the behavior to the last-specified flag.

Time formatter is specified with -f.

The retention specifiers are as follows:

daily:      -d  specifier is not used.  Will match first backup of its day, ignoring seconds, minutes, and hours.
weekday:    -w  specifier is day of week, as in crontab, 0=Sunday. Default = 0. Will match first backup of its day, ignoring seconds, minutes, and hours.
monthday:   -m  specifier is day of month. Default = 1. Will match first backup of its day, ignoring seconds, minutes, and hours.
yearmonth:  -y  specifier is month of year, starting with 1. default = 1. Will match first backup of its month, ignoring days, seconds, minutes, and hours.

examples:

### A week of dailies
retain -d 7

### Every other day for a month
retain -d 15/2

### Every Sunday for a month
retain -w 4:0

### Every other Saturday for two months
retain -w 4:6/2

### Two times a month, at the beginning and middle, for a year
retain -m 12:1 -m 12:15

### A yearly backup on January that lasts 10 years
retain -y 10:1

# Author
retain is Copyright (C) 2015 Taylor C. Richberger

This program is free software: you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free Software
Foundation, either version 3 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE.  See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with
this program.  If not, see <http://www.gnu.org/licenses/>.
