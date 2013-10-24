set terminal pdf linewidth 1 enhanced color font ",7" dashed dl 1.5
set output "workload.pdf"
set   autoscale                        # scale axes automatically
unset log                              # remove any log-scaling
unset label                            # remove any previous labels
set xtic 1                          # set xtics automatically
set ytic auto                          # set ytics automatically
set title "workload of PMs during each phase in mdp"
set xlabel "phase number"
set ylabel ""
#set key 4, 13000
set xr [0:10]
set yr [0:1.2]

call "col_counter.p" "mdp_util.txt"
print col_count
#set term x11 0
plot for [i=2:col_count] "mdp_util.txt" using 1:i title "PM".(i-1) with steps
call "col_counter.p" "khanna_util.txt"
print col_count 
set multiplot
set title "workload of PMs during each phase in khanna"
#set term x1
plot for [i=2:col_count] "khanna_util.txt" using 1:i title "PM".(i-1) with steps
unset multiplot
set output
