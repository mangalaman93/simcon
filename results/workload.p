set terminal pdf linewidth 1 enhanced color font ",7" dashed dl 1.5
set output "workload_mdp.pdf"
set   autoscale                        # scale axes automatically
unset log                              # remove any log-scaling
unset label                            # remove any previous labels
set xtic auto                          # set xtics automatically
set ytic auto                          # set ytics automatically
set title "workload of PMs during each phase in mdp"
set xlabel "phase number"
set ylabel ""
#set key 4, 13000

set yr [0:1.4]
rows = "`awk 'END {print NR}' < mdp_util.txt`"
set xr [0:rows/2]
columns = "`awk '{if(NR == 1) print NF}' < mdp_util.txt`"
plot for [i=2:columns] "mdp_util.txt" using 1:i title "PM".(i-1) with steps
columns = "`awk '{if(NR == 1) print NF}' < khanna_util.txt`"

set output "workload_khanna.pdf"
set title "workload of PMs during each phase in khanna"
#set term x1
plot for [i=2:columns] "khanna_util.txt" using 1:i title "PM".(i-1) with steps

set output "workload_astar.pdf"
set title "workload of PMs during each phase in astar"
#set term x1
plot for [i=2:columns] "astar_util.txt" using 1:i title "PM".(i-1) with steps
