set terminal pdfcairo enhanced monochrome linewidth 1 font ",7" solid 
set output "util_mdp.pdf"
set boxwidth 0.75 absolute
set style fill solid 1.00 border -1
set style data histograms
set style histogram rowstacked
set ytics 0.1
set yrange [0:1.0]
set ylabel "Utilization" 
set key box
set key out horiz bot center

rows = "`awk 'END {print NR}' < mdp_policy.txt`"
columns = "`awk '{if(NR == 1) print NF}' < mdp_policy.txt`"
pms = columns-1
phases = rows/pms

do for[i=1:phases] {
set output "mdp_util".i.".pdf"
plot  newhistogram "PHASE".i, "mdp_policy.txt" every::(i-1)*pms::i*pms-1 using 2 t "VM-0" fs pattern 1, '' every::(i-1)*pms::i*pms-1 using 3 t "VM-1" fs pattern 2, '' every::(i-1)*pms::i*pms-1 using 4 t "VM-2" fs pattern 3,  '' every::(i-1)*pms::i*pms-1 using 5 t "VM-3" fs pattern 4,  '' every::(i-1)*pms::i*pms-1 using 6 t "VM-4" fs pattern 5, '' every::(i-1)*pms::i*pms-1 using 7:xtic(1) t "VM-5" fs pattern 6 
}
set output "util_khanna.pdf"
do for [i=1:phases] {
set output "util_khanna".i.".pdf"
plot  newhistogram "PHASE".i, "khanna_policy.txt" every::(i-1)*pms::i*pms-1 using 2 t "VM-0" fs pattern 1 , '' every::(i-1)*pms::i*pms-1 using 3 t "VM-1" fs pattern 2, '' every::(i-1)*pms::i*pms-1 using 4 t "VM-2" fs pattern 3,  '' every::(i-1)*pms::i*pms-1 using 5 t "VM-3" fs pattern 4,  '' every::(i-1)*pms::i*pms-1 using 6 t "VM-4" fs pattern 5, '' every::(i-1)*pms::i*pms-1 using 7:xtic(1) t "VM-5" fs pattern 6 
}