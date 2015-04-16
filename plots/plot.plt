set datafile separator ","
set term png enhanced size 1000,800

set grid
set yrange [0:80]

set xlabel "Number of cycles N"
set ylabel "Time [s]"

# 1      2           3           4             5         6           7           8        9
# n,real_parent,real_children,sys_parent,sys_children,user_parent,user_children,counter,global

set output "sys_parent.png"
set title "System time - Parent process" font ",20"

plot "fork.csv" u 1:4 with linespoints title "fork",\
"vfork.csv" u 1:4 with linespoints title "vfork",\
"fork_clone.csv" u 1:4 with linespoints title "fork clone",\
"vfork_clone.csv" u 1:4 with linespoints title "vfork clone"

set output "sys_children.png"
set title "System time - Child processes" font ",20"

plot "fork.csv" u 1:5 with linespoints title "fork",\
"vfork.csv" u 1:5 with linespoints title "vfork",\
"fork_clone.csv" u 1:5 with linespoints title "fork clone",\
"vfork_clone.csv" u 1:5 with linespoints title "vfork clone"

set output "sys_sum.png"
set title "System time - Parent + Children" font ",20"

plot "fork.csv" u 1:($4 + $5) with linespoints title "fork",\
"vfork.csv" u 1:($4 + $5) with linespoints title "vfork",\
"fork_clone.csv" u 1:($4 + $5) with linespoints title "fork clone",\
"vfork_clone.csv" u 1:($4 + $5) with linespoints title "vfork clone"

#


set output "usr_parent.png"
set title "User time - Parent process" font ",20"

set yrange [0:2]

plot "fork.csv" u 1:6 with linespoints title "fork",\
"vfork.csv" u 1:6 with linespoints title "vfork",\
"fork_clone.csv" u 1:6 with linespoints title "fork clone",\
"vfork_clone.csv" u 1:6 with linespoints title "vfork clone"

set yrange [0:0.2]


set output "usr_children.png"
set title "User time - Child processes" font ",20"

plot "fork.csv" u 1:7 with linespoints title "fork",\
"vfork.csv" u 1:7 with linespoints title "vfork",\
"fork_clone.csv" u 1:7 with linespoints title "fork clone",\
"vfork_clone.csv" u 1:7 with linespoints title "vfork clone"

set yrange [0:2]


set output "usr_sum.png"
set title "User time - Parent + Children" font ",20"

plot "fork.csv" u 1:($6 + $7) with linespoints title "fork",\
"vfork.csv" u 1:($6 + $7) with linespoints title "vfork",\
"fork_clone.csv" u 1:($6 + $7) with linespoints title "fork clone",\
"vfork_clone.csv" u 1:($6 + $7) with linespoints title "vfork clone"
set yrange [0:80]

#


set output "usr_sys_parent.png"
set title "User + system time - Parent process" font ",20"

plot "fork.csv" u 1:($4 + $6) with linespoints title "fork",\
"vfork.csv" u 1:($4 + $6) with linespoints title "vfork",\
"fork_clone.csv" u 1:($4 + $6) with linespoints title "fork clone",\
"vfork_clone.csv" u 1:($4 + $6) with linespoints title "vfork clone"

set output "usr_sys_children.png"
set title "User + system time - Child processes" font ",20"

plot "fork.csv" u 1:($5 + $7) with linespoints title "fork",\
"vfork.csv" u 1:($5 + $7) with linespoints title "vfork",\
"fork_clone.csv" u 1:($5 + $7) with linespoints title "fork clone",\
"vfork_clone.csv" u 1:($5 + $7) with linespoints title "vfork clone"

set output "usr_sys_sum.png"
set title "User + system time - Parent + Children" font ",20"

plot "fork.csv" u 1:($4 + $6 + $5 + $7) with linespoints title "fork",\
"vfork.csv" u 1:($4 + $6 + $5 + $7) with linespoints title "vfork",\
"fork_clone.csv" u 1:($4 + $6 + $5 + $7) with linespoints title "fork clone",\
"vfork_clone.csv" u 1:($4 + $6 + $5 + $7) with linespoints title "vfork clone"

#


set output "real_parent.png"
set title "Real time - Parent process" font ",20"

plot "fork.csv" u 1:2 with linespoints title "fork",\
"vfork.csv" u 1:2 with linespoints title "vfork",\
"fork_clone.csv" u 1:2 with linespoints title "fork clone",\
"vfork_clone.csv" u 1:2 with linespoints title "vfork clone"

set output "real_children.png"
set title "Real time - Child processes" font ",20"

plot "fork.csv" u 1:3 with linespoints title "fork",\
"vfork.csv" u 1:3 with linespoints title "vfork",\
"fork_clone.csv" u 1:3 with linespoints title "fork clone",\
"vfork_clone.csv" u 1:3 with linespoints title "vfork clone"

set output "real_sum.png"
set title "Real time - Parent + Children" font ",20"

plot "fork.csv" u 1:($2 + $3) with linespoints title "fork",\
"vfork.csv" u 1:($2 + $3) with linespoints title "vfork",\
"fork_clone.csv" u 1:($2 + $3) with linespoints title "fork clone",\
"vfork_clone.csv" u 1:($2 + $3) with linespoints title "vfork clone"

#



