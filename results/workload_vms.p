set terminal pdfcairo enhanced monochrome linewidth 1 font ",7" solid 
set output "workload_vms.pdf"
set boxwidth 0.75 absolute
set style fill solid 1.00 border -1
set style data histograms
set style histogram 
set title "workload of VMs during each phase in mdp"
set xlabel "phase number"
set ylabel ""
#set key 4, 13000

set yr [0:1]
rows = "`awk 'END {print NR}' < workload_input.txt`"
columns = "`awk '{if(NR == 1) print NF}' < workload_input.txt`"
plot  newhistogram, "workload_input.txt" using 1 t "VM-1" fs pattern 1, '' using 2 t "VM-2" fs pattern 2, '' using 3 t "VM-3" fs pattern 3, ''  using 4 t "VM-4" fs pattern 4,  '' using 5 t "VM-5" fs pattern 5, '' using 6 t "VM-6" fs pattern 6
