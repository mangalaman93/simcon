set terminal pdf linewidth 1 enhanced color font ",7" dashed dl 1.5
set output "workload.pdf"
set   autoscale                        # scale axes automatically
unset log                              # remove any log-scaling
unset label                            # remove any previous labels
set xtic auto                          # set xtics automatically
set ytic auto                          # set ytics automatically
set title "workload of PMs during each phase in mdp"
set xlabel "phase number"
set ylabel ""
#set key 4, 13000

set yr [0:1]
rows = "`awk 'END {print NR}' < input_workload.txt`"
set xr [0:rows/2]
columns = "`awk '{if(NR == 1) print NF}' < input_workload.txt`"
plot for [i=1:columns] "mdp_util.txt" using i title "VM".(i-1) with steps



