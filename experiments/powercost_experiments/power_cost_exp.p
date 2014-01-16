set terminal pdfcairo enhanced monochrome linewidth 1 font ",7" solid 
set output "util_mdp.pdf"

DX=0.01; DY=0.01;

set style fill solid 1.00 border -1
set style data histograms
set style histogram rowstacked
unset ytics
set yrange [0:1.0]
unset ylabel
unset xtics
rows = "`awk 'END {print NR}' < mdp_policy150.txt`"
columns = "`awk '{if(NR == 1) print NF}' < mdp_policy150.txt`"
pms = columns-1
phases = rows/pms
NX = phases
NY = 2
set origin DX, DY
set bmargin DX; set tmargin DX; set lmargin DY; set rmargin DY
set key box
set key at screen 1, 0.95



set multiplot layout 2,phases rowsfirst scale 1, 1 title "Power-cost 150 vs 500 in MDP"

i=1
set label 1 "PowerCost 150" at screen 0.02, 0.9
plot  newhistogram , "mdp_policy150.txt" every::(i-1)*pms::i*pms-1 using 2 t "VM-0" fs pattern 1, '' every::(i-1)*pms::i*pms-1 using 3 t "VM-1" fs pattern 2, '' every::(i-1)*pms::i*pms-1 using 4 t "VM-2" fs pattern 3,  '' every::(i-1)*pms::i*pms-1 using 5 t "VM-3" fs pattern 4,  '' every::(i-1)*pms::i*pms-1 using 6 t "VM-4" fs pattern 5, '' every::(i-1)*pms::i*pms-1 using 7 t "VM-5" fs pattern 6 

#unset key
#unset ytics
#unset ylabel
unset label 1

do for[i=2:phases] {
plot  newhistogram , "mdp_policy150.txt" every::(i-1)*pms::i*pms-1 using 2 t "VM-0" fs pattern 1, '' every::(i-1)*pms::i*pms-1 using 3 t "VM-1" fs pattern 2, '' every::(i-1)*pms::i*pms-1 using 4 t "VM-2" fs pattern 3,  '' every::(i-1)*pms::i*pms-1 using 5 t "VM-3" fs pattern 4,  '' every::(i-1)*pms::i*pms-1 using 6 t "VM-4" fs pattern 5, '' every::(i-1)*pms::i*pms-1 using 7 t "VM-5" fs pattern 6 
}


i=1
#set ytics 0.1
set label 1 "PowerCost 500" at screen 0.02, 0.43
plot  newhistogram, "mdp_policy500.txt" every::(i-1)*pms::i*pms-1 using 2 t "VM-0" fs pattern 1 , '' every::(i-1)*pms::i*pms-1 using 3 t "VM-1" fs pattern 2, '' every::(i-1)*pms::i*pms-1 using 4 t "VM-2" fs pattern 3,  '' every::(i-1)*pms::i*pms-1 using 5 t "VM-3" fs pattern 4,  '' every::(i-1)*pms::i*pms-1 using 6 t "VM-4" fs pattern 5, '' every::(i-1)*pms::i*pms-1 using 7 t "VM-5" fs pattern 6

#unset ytics
unset label 1
do for [i=2:phases] {
plot  newhistogram , "mdp_policy500.txt" every::(i-1)*pms::i*pms-1 using 2 t "VM-0" fs pattern 1 , '' every::(i-1)*pms::i*pms-1 using 3 t "VM-1" fs pattern 2, '' every::(i-1)*pms::i*pms-1 using 4 t "VM-2" fs pattern 3,  '' every::(i-1)*pms::i*pms-1 using 5 t "VM-3" fs pattern 4,  '' every::(i-1)*pms::i*pms-1 using 6 t "VM-4" fs pattern 5, '' every::(i-1)*pms::i*pms-1 using 7 t "VM-5" fs pattern 6 
}

unset multiplot

