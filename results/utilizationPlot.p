set terminal pdfcairo enhanced monochrome linewidth 1 font ",7" solid 
set output "util.pdf"
set boxwidth 0.75 absolute
set style fill solid 1.00 border -1
set style data histograms
set style histogram rowstacked
set ytics 0.1
set yrange [0:1.0]
set ylabel "Utilization" 
set xlabel "Physical Machine"
set key box
set key out horiz bot center

rows = "`awk 'END {print NR}' < mdp_policy.txt`"
columns = "`awk '{if(NR == 1) print NF}' < mdp_policy.txt`"
phases = rows/(columns-1)

plot for [i=1:phases] newhistogram , 'mdp_policy.txt' using 2 t "VM-1" fs pattern 1, '' using 3 t "VM-2" fs pattern 2, '' using 4 t "VM-3" fs pattern 3,  '' using 5 t "VM-4" fs pattern 4,  '' using 6 t "VM-5" fs pattern 5, '' using 7:xtic(1) t "VM-6" fs pattern 6 

plot for [i=1:phases] newhistogram , 'khanna_policy.txt' using 2 t "VM-1" fs pattern 1, '' using 3 t "VM-2" fs pattern 2, '' using 4 t "VM-3" fs pattern 3,  '' using 5 t "VM-4" fs pattern 4,  '' using 6 t "VM-5" fs pattern 5, '' using 7:xtic(1) t "VM-6" fs pattern 6 
