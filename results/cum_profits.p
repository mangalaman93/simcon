set   autoscale                        # scale axes automatically
unset log                              # remove any log-scaling
unset label                            # remove any previous labels
set xtic auto                          # set xtics automatically
set ytic auto                          # set ytics automatically
set title "cumulative profit for MDP and Khanna"
rows = "`awk 'END {print NR}' < khanna_cum_profits.txt`"
set xlabel "phase number"
set ylabel "profit"
#set key 4, 13000
set xr [0:rows]
set yr [0:500]
set terminal pdf linewidth 1 enhanced color font ",7" dashed dl 1.5
set output "cumProfit.pdf"
plot    "khanna_cum_profits.txt" using 1:2 title 'profit-khanna' with linespoints , \
"mdp_cum_profits.txt" using 1:2 title 'profit-mdp' with linespoints , \
"astar_cum_profits.txt" using 1:2 title 'profit-A*' with linespoints